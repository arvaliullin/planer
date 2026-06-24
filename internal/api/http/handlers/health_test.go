package handlers

import (
	"context"
	"encoding/json"
	"errors"
	"net/http"
	"net/http/httptest"
	"testing"

	"github.com/arvaliullin/planer/internal/api/http/dto"
	"github.com/arvaliullin/planer/internal/core/ports/mocks"
	"github.com/stretchr/testify/require"
	"go.uber.org/mock/gomock"
)

func TestHealth_Handle_NoDeps(t *testing.T) {
	t.Parallel()
	h := &Health{}
	req := httptest.NewRequest(http.MethodGet, "/health", nil)
	rec := httptest.NewRecorder()
	h.Handle(rec, req)
	require.Equal(t, http.StatusOK, rec.Code)
	var body dto.HealthResponse
	require.NoError(t, json.Unmarshal(rec.Body.Bytes(), &body))
	require.Nil(t, body.Postgres)
	require.Nil(t, body.Minio)
}

func TestHealth_UnhealthyMinio(t *testing.T) {
	t.Parallel()
	ctrl := gomock.NewController(t)
	pinger := mocks.NewMockPinger(ctrl)
	pinger.EXPECT().Ping(gomock.Any()).Return(errors.New("connection refused"))

	h := &Health{Minio: pinger}
	req := httptest.NewRequest(http.MethodGet, "/health", nil)
	rec := httptest.NewRecorder()
	h.Handle(rec, req)
	require.Equal(t, http.StatusServiceUnavailable, rec.Code)
	var body dto.HealthResponse
	require.NoError(t, json.Unmarshal(rec.Body.Bytes(), &body))
	require.NotNil(t, body.Minio)
	require.False(t, body.Minio.OK)
}

func TestHealth_HealthyMinio(t *testing.T) {
	t.Parallel()
	ctrl := gomock.NewController(t)
	pinger := mocks.NewMockPinger(ctrl)
	pinger.EXPECT().Ping(gomock.Any()).Return(nil)

	h := &Health{Minio: pinger}
	req := httptest.NewRequest(http.MethodGet, "/health", nil)
	rec := httptest.NewRecorder()
	h.Handle(rec, req)
	require.Equal(t, http.StatusOK, rec.Code)
	var body dto.HealthResponse
	require.NoError(t, json.Unmarshal(rec.Body.Bytes(), &body))
	require.NotNil(t, body.Minio)
	require.True(t, body.Minio.OK)
}

func TestHealth_ContextCancel(t *testing.T) {
	t.Parallel()
	ctrl := gomock.NewController(t)
	pinger := mocks.NewMockPinger(ctrl)
	pinger.EXPECT().Ping(gomock.Any()).Return(nil)

	h := &Health{Minio: pinger}
	ctx, cancel := context.WithCancel(context.Background())
	cancel()
	req := httptest.NewRequest(http.MethodGet, "/health", nil).WithContext(ctx)
	rec := httptest.NewRecorder()
	h.Handle(rec, req)
	require.Equal(t, http.StatusOK, rec.Code)
}
