package ports

import "context"

//go:generate mockgen -source=pinger.go -destination=mocks/pinger_mock.go -package=mocks

// Pinger описывает зависимость, доступность которой можно проверить.
type Pinger interface {
	Ping(ctx context.Context) error
}
