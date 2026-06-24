package config

import (
	"time"

	"github.com/caarlos0/env/v11"
)

type minioConfig struct {
	Endpoint  string `env:"MINIO_ENDPOINT,required"`
	AccessKey string `env:"MINIO_ACCESS_KEY,required"`
	SecretKey string `env:"MINIO_SECRET_KEY,required"`
	Secure    bool   `env:"MINIO_SECURE" envDefault:"false"`
	Bucket    string `env:"MINIO_BUCKET" envDefault:"planer"`
}

// Server настройки HTTP-сервиса planerd.
type Server struct {
	HTTPAddr        string        `env:"HTTP_ADDR" envDefault:":8080"`
	DatabaseURI     string        `env:"DATABASE_URI,required"`
	ShutdownTimeout time.Duration `env:"SHUTDOWN_TIMEOUT" envDefault:"10s"`
	LogLevel        string        `env:"LOG_LEVEL" envDefault:"info"`
	Minio           minioConfig
}

// LoadServer читает переменные окружения в Server.
func LoadServer() (*Server, error) {
	var c Server
	if err := env.Parse(&c); err != nil {
		return nil, err
	}
	return &c, nil
}
