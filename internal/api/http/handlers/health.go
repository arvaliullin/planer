package handlers

import (
	"context"
	"net/http"
	"time"

	"github.com/arvaliullin/planer/internal/api/http/dto"
	"github.com/arvaliullin/planer/internal/core/ports"
	"github.com/jackc/pgx/v5/pgxpool"
)

const healthCheckTimeout = 3 * time.Second

// Health обработчик GET /health.
type Health struct {
	DB    *pgxpool.Pool
	Minio ports.Pinger
}

// Handle проверяет PostgreSQL и MinIO.
// @Summary Проверка готовности
// @Description Статус подключения к postgres и minio.
// @Tags health
// @Produce json
// @Success 200 {object} dto.HealthResponse
// @Failure 503 {object} dto.HealthResponse
// @Router /health [get]
func (h *Health) Handle(w http.ResponseWriter, r *http.Request) {
	ctx, cancel := context.WithTimeout(r.Context(), healthCheckTimeout)
	defer cancel()
	out := dto.HealthResponse{}
	if h.DB != nil {
		st := &dto.ComponentStatus{OK: true}
		if err := h.DB.Ping(ctx); err != nil {
			st.OK = false
			st.Error = err.Error()
		}
		out.Postgres = st
	}
	if h.Minio != nil {
		st := &dto.ComponentStatus{OK: true}
		if err := h.Minio.Ping(ctx); err != nil {
			st.OK = false
			st.Error = err.Error()
		}
		out.Minio = st
	}
	writeJSON(w, healthHTTPStatus(out), out)
}

func healthHTTPStatus(out dto.HealthResponse) int {
	for _, st := range []*dto.ComponentStatus{out.Postgres, out.Minio} {
		if st != nil && !st.OK {
			return http.StatusServiceUnavailable
		}
	}
	return http.StatusOK
}
