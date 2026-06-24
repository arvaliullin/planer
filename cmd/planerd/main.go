package main

import (
	"context"
	"log/slog"
	"os"
	"os/signal"
	"syscall"

	"github.com/arvaliullin/planer/internal/app"

	_ "github.com/arvaliullin/planer/docs" // swagger
)

func main() {
	log := slog.New(slog.NewJSONHandler(os.Stdout, nil)).With("service", "planerd")

	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()

	a, err := app.NewPlanerd(ctx)
	if err != nil {
		log.Error("planerd init", "error", err)
		os.Exit(1)
	}

	if err := a.Run(ctx); err != nil {
		a.Logger().Error("planerd run", "error", err)
		os.Exit(1)
	}
}
