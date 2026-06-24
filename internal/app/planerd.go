package app

import (
	"context"
	"fmt"
	"log/slog"
	"net/http"
	"os"
	"time"

	httpserver "github.com/arvaliullin/planer/internal/api/http"
	"github.com/arvaliullin/planer/internal/api/http/handlers"
	"github.com/arvaliullin/planer/internal/config"
	"github.com/arvaliullin/planer/internal/repository/minio"
	"github.com/arvaliullin/planer/internal/repository/postgres"
)

const (
	httpReadHeaderTimeout = 10 * time.Second
	httpReadWriteTimeout  = 60 * time.Second
)

// Planerd HTTP-сервис planer.
type Planerd struct {
	cfg *config.Server
	log *slog.Logger
	db  *postgres.DB
	srv *http.Server
}

// NewPlanerd собирает зависимости и HTTP-сервер.
func NewPlanerd(ctx context.Context) (*Planerd, error) {
	cfg, err := config.LoadServer()
	if err != nil {
		return nil, fmt.Errorf("config: %w", err)
	}

	log := newLogger(cfg.LogLevel)
	log.Info("planerd configuration loaded",
		"http_addr", cfg.HTTPAddr,
		"minio_endpoint", cfg.Minio.Endpoint,
		"minio_bucket", cfg.Minio.Bucket,
		"minio_secure", cfg.Minio.Secure,
		"shutdown_timeout", cfg.ShutdownTimeout.String(),
	)

	if err = postgres.RunMigrations(ctx, cfg.DatabaseURI); err != nil {
		return nil, fmt.Errorf("postgres migrations: %w", err)
	}

	db, err := postgres.New(ctx, cfg.DatabaseURI)
	if err != nil {
		return nil, fmt.Errorf("postgres: %w", err)
	}

	st, err := minio.New(ctx, cfg.Minio.Endpoint, cfg.Minio.AccessKey, cfg.Minio.SecretKey, cfg.Minio.Secure, cfg.Minio.Bucket)
	if err != nil {
		db.Close()
		return nil, fmt.Errorf("minio: %w", err)
	}

	health := &handlers.Health{
		DB:    db.Pool,
		Minio: st,
	}

	srv := &http.Server{
		Addr: cfg.HTTPAddr,
		Handler: httpserver.NewRouter(httpserver.Deps{
			Log:    log,
			Health: health,
		}),
		ReadHeaderTimeout: httpReadHeaderTimeout,
		ReadTimeout:       httpReadWriteTimeout,
		WriteTimeout:      httpReadWriteTimeout,
	}

	return &Planerd{
		cfg: cfg,
		log: log,
		db:  db,
		srv: srv,
	}, nil
}

// Logger возвращает логгер приложения.
func (a *Planerd) Logger() *slog.Logger {
	return a.log
}

// Run слушает сигналы, останавливает HTTP и закрывает ресурсы.
func (a *Planerd) Run(ctx context.Context) error {
	go func() {
		a.log.Info("listen", "addr", a.srv.Addr)
		if err := a.srv.ListenAndServe(); err != nil && err != http.ErrServerClosed {
			a.log.Error("http", "error", err)
		}
	}()

	<-ctx.Done()
	a.log.Info("shutting down planerd")

	shutdownCtx, cancel := context.WithTimeout(context.Background(), a.cfg.ShutdownTimeout)
	defer cancel()
	if err := a.srv.Shutdown(shutdownCtx); err != nil {
		a.log.Error("http shutdown", "error", err)
	}

	a.db.Close()
	return nil
}

func newLogger(level string) *slog.Logger {
	var lvl slog.Level
	if err := lvl.UnmarshalText([]byte(level)); err != nil {
		lvl = slog.LevelInfo
	}
	return slog.New(slog.NewJSONHandler(os.Stdout, &slog.HandlerOptions{Level: lvl})).With("service", "planerd")
}
