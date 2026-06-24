package httpserver

import (
	"log/slog"
	"net/http"

	"github.com/arvaliullin/planer/internal/api/http/handlers"
	"github.com/arvaliullin/planer/internal/api/http/middleware"
	"github.com/go-chi/chi/v5"
	httpSwagger "github.com/swaggo/http-swagger/v2"
)

// @title Planer API
// @version 1.0
// @description HTTP API сервиса planer
// @host localhost:8080
// @BasePath /

// Deps зависимости HTTP API.
type Deps struct {
	Log    *slog.Logger
	Health *handlers.Health
}

// NewRouter собирает chi mux.
func NewRouter(d Deps) http.Handler {
	r := chi.NewRouter()
	r.Use(middleware.RequestLogger(d.Log))
	r.Get("/health", d.Health.Handle)
	r.Get("/swagger/*", httpSwagger.Handler(
		httpSwagger.URL("/swagger/doc.json"),
	))
	return r
}
