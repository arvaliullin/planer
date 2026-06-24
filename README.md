# planer

Монорепозиторий: Qt desktop-клиент и Go backend-сервис.

## Backend (planerd)

HTTP-сервис с проверкой готовности (`/health`), Swagger UI и подключением к PostgreSQL и MinIO.

### Быстрый старт

```bash
make install-deps
make generate   # опционально: перегенерировать Swagger
make up         # postgres + minio + planerd в Docker
```

- Health: http://localhost:8080/health
- Swagger: http://localhost:8080/swagger/index.html
- MinIO Console: http://localhost:9001 (minioadmin / minioadmin)

### Локальная разработка

```bash
make up-deps      # только postgres и minio
make run          # planerd на хосте
```

Переменные окружения по умолчанию заданы в `Makefile` (`DATABASE_URI`, `MINIO_*`).

### Полезные команды

| Команда | Описание |
|---------|----------|
| `make help` | Список всех команд |
| `make build` | Собрать `bin/planerd` |
| `make test` | Запустить тесты |
| `make down` | Остановить Docker Compose |
| `make migration name=<name>` | Создать goose-миграцию |

## Desktop

Qt6/QML приложение в [`desktop/planer/`](desktop/planer/). Архитектура: domain / data / presentation.

### Сборка

Требуется Qt 6.2+ (Quick, QuickControls2, Network).

```bash
cd desktop/planer
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/appplaner
```

### Настройки

| Переменная | Описание | Default |
|------------|----------|---------|
| `PLANER_API_URL` | base URL planerd | `http://localhost:8080` |

Приоритет: env -> QSettings -> default.

Для проверки backend: запустить `make run`, открыть приложение, на экране **Настройки** нажать **Проверить сервер**.
