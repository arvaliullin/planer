package postgres

import (
	"context"
	"database/sql"
	"fmt"

	_ "github.com/arvaliullin/planer/migrations"
	"github.com/jackc/pgx/v5/pgxpool"
	_ "github.com/jackc/pgx/v5/stdlib"
	"github.com/pressly/goose/v3"
)

// DB обёртка над pgx pool.
type DB struct {
	Pool *pgxpool.Pool
}

// RunMigrations применяет миграции goose; каталог migrations ищется относительно рабочей директории процесса.
func RunMigrations(ctx context.Context, dsn string) error {
	db, err := sql.Open("pgx", dsn)
	if err != nil {
		return err
	}
	defer db.Close()

	if err := goose.SetDialect("postgres"); err != nil {
		return err
	}
	return goose.UpContext(ctx, db, "migrations")
}

// New создаёт пул соединений pgx и проверяет доступность базы через Ping.
func New(ctx context.Context, dsn string) (*DB, error) {
	pool, err := pgxpool.New(ctx, dsn)
	if err != nil {
		return nil, fmt.Errorf("pool: %w", err)
	}
	if err = pool.Ping(ctx); err != nil {
		pool.Close()
		return nil, fmt.Errorf("ping: %w", err)
	}
	return &DB{Pool: pool}, nil
}

// Close освобождает pool.
func (d *DB) Close() {
	if d.Pool != nil {
		d.Pool.Close()
	}
}
