.DEFAULT_GOAL := help

.PHONY: help
help: ## Показать список команд
	@echo "Доступные команды:"
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  %-22s %s\n", $$1, $$2}'

.PHONY: install-deps
install-deps: ## Установить инструменты разработки
	- go install go.uber.org/mock/mockgen@v0.6.0
	- go install github.com/pressly/goose/v3/cmd/goose@latest
	- go install github.com/swaggo/swag/cmd/swag@latest
	- go install github.com/golangci/golangci-lint/v2/cmd/golangci-lint@latest

.PHONY: generate
generate: ## Сгенерировать моки и документацию Swagger
	go generate ./...
	swag init -g router.go -d internal/api/http,internal/api/http/handlers,internal/api/http/dto -o docs --parseInternal

.PHONY: fmt
fmt: ## Форматировать код
	- go fmt ./...

.PHONY: up
up: ## Запустить полный стек в Docker Compose
	- docker compose -f deployments/docker-compose.yaml up --build -d

.PHONY: down
down: ## Остановить стек Docker Compose
	- docker compose -f deployments/docker-compose.yaml down -v

.PHONY: logs
logs: ## Показать логи Docker Compose
	- docker compose -f deployments/docker-compose.yaml logs -f

.PHONY: ps
ps: ## Показать статус контейнеров Docker Compose
	- docker compose -f deployments/docker-compose.yaml ps

.PHONY: up-deps
up-deps: ## Запустить только postgres и minio
	- docker compose -f deployments/docker-compose.yaml up -d postgres minio

DATABASE_URI ?= postgres://planer:planer@localhost:5432/planer?sslmode=disable
MINIO_ENDPOINT ?= localhost:9000
MINIO_ACCESS_KEY ?= minioadmin
MINIO_SECRET_KEY ?= minioadmin
MINIO_BUCKET ?= planer

.PHONY: run
run: ## Запустить planerd локально
	DATABASE_URI=$(DATABASE_URI) \
	MINIO_ENDPOINT=$(MINIO_ENDPOINT) \
	MINIO_ACCESS_KEY=$(MINIO_ACCESS_KEY) \
	MINIO_SECRET_KEY=$(MINIO_SECRET_KEY) \
	MINIO_BUCKET=$(MINIO_BUCKET) \
	go run ./cmd/planerd

.PHONY: build
build: ## Собрать бинарник planerd
	go build -o bin/planerd ./cmd/planerd

.PHONY: test
test: ## Запустить тесты
	go test ./... -count=1

.PHONY: lint
lint: ## Запустить golangci-lint
	golangci-lint run ./...

.PHONY: migration
migration: ## Создать goose-миграцию (name=<name>)
	@test -n "$(name)" || (echo "usage: make migration name=<name>" && exit 1)
	goose -dir migrations create $(name) go
