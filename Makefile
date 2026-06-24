.DEFAULT_GOAL := help

# Desktop (Qt): make использует build/make, Qt Creator - build/Desktop-Debug
DESKTOP_DIR ?= desktop/planer
DESKTOP_BUILD ?= $(DESKTOP_DIR)/build/make
DESKTOP_QTC_BUILD ?= $(DESKTOP_DIR)/build/Desktop-Debug
CMAKE_BUILD_TYPE ?= Debug
QT_PREFIX ?= $(shell qmake6 -query QT_INSTALL_PREFIX 2>/dev/null)
CMAKE ?= cmake
CMAKE_CONFIGURE = $(CMAKE) -S $(DESKTOP_DIR) -B $(DESKTOP_BUILD) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)
ifneq ($(QT_PREFIX),)
CMAKE_CONFIGURE += -DCMAKE_PREFIX_PATH=$(QT_PREFIX)
endif

.PHONY: help
help: ## Показать список команд
	@echo "Доступные команды:"
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  %-22s %s\n", $$1, $$2}'

.PHONY: install-deps
install-deps: ## Установить инструменты разработки (Go)
	- go install go.uber.org/mock/mockgen@v0.6.0
	- go install github.com/pressly/goose/v3/cmd/goose@latest
	- go install github.com/swaggo/swag/cmd/swag@latest
	- go install github.com/golangci/golangci-lint/v2/cmd/golangci-lint@latest

.PHONY: generate
generate: ## Сгенерировать моки и документацию Swagger
	go generate ./...
	swag init -g router.go -d internal/api/http,internal/api/http/handlers,internal/api/http/dto -o docs --parseInternal

.PHONY: fmt
fmt: ## Форматировать Go-код
	- go fmt ./...

.PHONY: fmt-desktop
fmt-desktop: ## Форматировать C++ desktop (clang-format)
	@command -v clang-format >/dev/null 2>&1 || (echo "clang-format не найден" && exit 1)
	find $(DESKTOP_DIR)/src $(DESKTOP_DIR)/main.cpp -type f \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i {} +

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
PLANER_API_URL ?= http://localhost:8080

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

.PHONY: build-all
build-all: build desktop-build ## Собрать backend и desktop

.PHONY: test
test: ## Запустить Go-тесты
	go test ./... -count=1

.PHONY: lint
lint: ## Запустить golangci-lint
	golangci-lint run ./...

.PHONY: migration
migration: ## Создать goose-миграцию (name=<name>)
	@test -n "$(name)" || (echo "usage: make migration name=<name>" && exit 1)
	goose -dir migrations create $(name) go

.PHONY: desktop-config
desktop-config: ## Сконфигурировать desktop (cmake, build/make)
	$(CMAKE_CONFIGURE)

.PHONY: desktop-build
desktop-build: ## Собрать desktop-клиент (build/make)
	@test -f $(DESKTOP_BUILD)/Makefile || $(MAKE) desktop-config
	$(CMAKE) --build $(DESKTOP_BUILD) --parallel

.PHONY: desktop-run
desktop-run: desktop-build ## Запустить desktop-клиент (сборка через make)
	PLANER_API_URL=$(PLANER_API_URL) $(DESKTOP_BUILD)/appplaner

.PHONY: desktop-qtc-run
desktop-qtc-run: ## Запустить desktop из Qt Creator (build/Desktop-Debug)
	@test -x $(DESKTOP_QTC_BUILD)/appplaner || (echo "Сначала собери проект в Qt Creator (Desktop-Debug)" && exit 1)
	PLANER_API_URL=$(PLANER_API_URL) $(DESKTOP_QTC_BUILD)/appplaner

.PHONY: desktop-clean
desktop-clean: ## Удалить каталог сборки build/make
	rm -rf $(DESKTOP_BUILD)

.PHONY: desktop-rebuild
desktop-rebuild: desktop-clean desktop-build ## Пересобрать desktop с нуля (build/make)

.PHONY: dev
dev: up-deps ## Backend + подсказка: make run и make desktop-qtc-run
	@echo "Зависимости запущены. В отдельных терминалах:"
	@echo "  make run"
	@echo "  make desktop-qtc-run   # или make desktop-run, или Run в Qt Creator"
