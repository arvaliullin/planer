# Техническое задание

## Planer - система календарного планирования проектов

---

### Общие требования

Система представляет собой HTTP API (сервис `planerd`) и desktop-клиент (Qt6/QML) со следующими требованиями к бизнес-логике:

* регистрация, аутентификация и авторизация пользователей;
* управление проектами: создание, редактирование, удаление, участники с ролями;
* формирование набора задач внутри проекта с ключами вида `PLAN-42`;
* назначение исполнителей, приоритетов, типов и статусов задач;
* календарное планирование: даты начала и окончания задач;
* зависимости между задачами ("блокирует") для построения диаграммы Ганта;
* представления задач: список, Kanban-доска, диаграмма Ганта по проекту;
* вложения к задачам (хранение в MinIO).

Planer - минимальный аналог YouTrack/Jira для командной работы над проектами. В MVP: проекты, задачи, исполнители, сроки, Gantt.

**Вне scope MVP** (не реализуется в первой версии):

* спринты, эпики, time tracking;
* automation rules, webhooks, email-уведомления;
* полнотекстовый поиск, сохранённые фильтры;
* кастомные workflow для каждого проекта;
* web-клиент (только desktop + REST API);
* auto-scheduling и critical path на диаграмме Ганта.

### Абстрактная схема взаимодействия с системой

Ниже представлена абстрактная бизнес-логика взаимодействия пользователя с системой:

1. Пользователь регистрируется или входит в desktop-клиент Planer.
2. Создаёт проект (или получает доступ как участник существующего проекта).
3. Добавляет участников проекта и назначает им роли (`admin`, `member`, `viewer`).
4. Создаёт задачи в проекте: заголовок, описание, тип, приоритет, исполнитель, сроки.
5. Управляет задачами на Kanban-доске: перетаскивание между колонками меняет статус.
6. Открывает диаграмму Ганта проекта: видит задачи на временной шкале, меняет даты drag-and-drop.
7. Добавляет зависимость "задача A блокирует задачу B" - на Gantt отображается стрелка; auto-scheduling не выполняется.

Примечания:

- desktop-клиент не хранит бизнес-данные локально (кроме JWT и настроек); данные хранятся в `planerd`;
- все мутации выполняются через REST API; offline-режим в MVP не предусмотрен.

### Доменная модель

```mermaid
erDiagram
    users ||--o{ project_members : participates
    projects ||--o{ project_members : has
    projects ||--o{ tasks : contains
    users ||--o{ tasks : assigns
    users ||--o{ tasks : reports
    tasks ||--o{ task_dependencies : blocks
    tasks ||--o{ attachments : has
```

#### Сущности

**users** - пользователи системы.

| Поле | Тип | Описание |
|------|-----|----------|
| `id` | UUID | первичный ключ |
| `email` | string | уникальный email |
| `display_name` | string | отображаемое имя |
| `password_hash` | string | хеш пароля (bcrypt или аналог) |
| `created_at` | timestamp | RFC3339 |

**projects** - проекты.

| Поле | Тип | Описание |
|------|-----|----------|
| `id` | UUID | первичный ключ |
| `key` | string | короткий ключ проекта (`PLAN`), 2-10 uppercase букв, уникален |
| `name` | string | название |
| `description` | string | описание (может быть пустым) |
| `owner_id` | UUID | создатель проекта |
| `created_at` | timestamp | RFC3339 |

**project_members** - участники проекта.

| Поле | Тип | Описание |
|------|-----|----------|
| `project_id` | UUID | проект |
| `user_id` | UUID | пользователь |
| `role` | enum | `admin`, `member`, `viewer` |

**tasks** - задачи проекта.

| Поле | Тип | Описание |
|------|-----|----------|
| `id` | UUID | первичный ключ |
| `project_id` | UUID | проект |
| `number` | int | порядковый номер в проекте (auto-increment) |
| `title` | string | заголовок |
| `description` | string | описание |
| `type` | enum | `task`, `bug` |
| `status` | enum | см. workflow ниже |
| `priority` | enum | `low`, `normal`, `high`, `critical` |
| `assignee_id` | UUID? | исполнитель |
| `reporter_id` | UUID | автор задачи |
| `start_date` | date? | дата начала (`YYYY-MM-DD`) |
| `due_date` | date? | дата окончания (`YYYY-MM-DD`) |
| `created_at` | timestamp | RFC3339 |
| `updated_at` | timestamp | RFC3339 |

Ключ задачи в UI и API: `{project.key}-{number}`, например `PLAN-42`.

**task_dependencies** - зависимости между задачами (finish-to-start).

| Поле | Тип | Описание |
|------|-----|----------|
| `id` | UUID | первичный ключ |
| `blocker_task_id` | UUID | задача-блокер (должна завершиться раньше) |
| `blocked_task_id` | UUID | заблокированная задача |

**attachments** - вложения к задаче.

| Поле | Тип | Описание |
|------|-----|----------|
| `id` | UUID | первичный ключ |
| `task_id` | UUID | задача |
| `file_name` | string | имя файла |
| `mime_type` | string | MIME-тип |
| `size_bytes` | int64 | размер |
| `s3_key` | string | ключ объекта в MinIO |
| `uploaded_at` | timestamp | RFC3339 |

#### Workflow статусов (фиксированный для MVP)

`backlog` -> `todo` -> `in_progress` -> `review` -> `done`

Дополнительный терминальный статус: `cancelled`.

Переходы между статусами в MVP свободные (без проверки workflow-графа).

#### Роли участников проекта

| Роль | Права |
|------|-------|
| `admin` | CRUD проекта, управление участниками, CRUD задач и вложений |
| `member` | CRUD задач и вложений, чтение проекта |
| `viewer` | только чтение проекта, задач, Gantt, вложений |

### Сводное HTTP API

Сервис `planerd` должен предоставлять следующие HTTP-хендлеры.

Префикс версии API: `/api/v1`.

Аутентификация: заголовок `Authorization: Bearer <jwt>` (кроме `/health`, `/api/v1/auth/*`, `/swagger/*`).

* `GET /health` - проверка готовности сервиса;
* `POST /api/v1/auth/register` - регистрация пользователя;
* `POST /api/v1/auth/login` - аутентификация пользователя;
* `GET /api/v1/users/me` - текущий пользователь;
* `GET /api/v1/projects` - список проектов текущего пользователя;
* `POST /api/v1/projects` - создание проекта;
* `GET /api/v1/projects/{projectID}` - получение проекта;
* `PATCH /api/v1/projects/{projectID}` - обновление проекта;
* `DELETE /api/v1/projects/{projectID}` - удаление проекта;
* `GET /api/v1/projects/{projectID}/members` - список участников;
* `POST /api/v1/projects/{projectID}/members` - добавление участника;
* `DELETE /api/v1/projects/{projectID}/members/{userID}` - удаление участника;
* `GET /api/v1/projects/{projectID}/tasks` - список задач проекта;
* `POST /api/v1/projects/{projectID}/tasks` - создание задачи;
* `GET /api/v1/tasks/{taskID}` - получение задачи;
* `PATCH /api/v1/tasks/{taskID}` - обновление задачи;
* `DELETE /api/v1/tasks/{taskID}` - удаление задачи;
* `GET /api/v1/tasks/{taskID}/dependencies` - список зависимостей задачи;
* `POST /api/v1/tasks/{taskID}/dependencies` - создание зависимости;
* `DELETE /api/v1/tasks/{taskID}/dependencies/{dependencyID}` - удаление зависимости;
* `GET /api/v1/projects/{projectID}/gantt` - данные для диаграммы Ганта;
* `GET /api/v1/tasks/{taskID}/attachments` - список вложений;
* `POST /api/v1/tasks/{taskID}/attachments` - загрузка вложения;
* `DELETE /api/v1/tasks/{taskID}/attachments/{attachmentID}` - удаление вложения;
* `GET /api/v1/tasks/{taskID}/attachments/{attachmentID}/download` - скачивание вложения.

### Общие ограничения и требования

* основное хранилище данных - PostgreSQL;
* вложения - MinIO (S3-compatible);
* структура таблиц остаётся на усмотрение разработчика;
* формат хранения паролей и чувствительных данных - на усмотрение разработчика (bcrypt);
* timestamps в JSON API - RFC3339;
* календарные даты задач (`start_date`, `due_date`) - ISO date `YYYY-MM-DD`;
* пагинация списков: query-параметры `limit` (default 50, max 100) и `offset` (default 0);
* формат paginated-ответа: `{ "items": [...], "total": N }`;
* `project.key` - от 2 до 10 символов, только латинские заглавные буквы (`A-Z`), уникален глобально;
* `task.key` формируется как `{project.key}-{number}`; `number` уникален в рамках проекта;
* пользователь видит только проекты, в которых он участник;
* зависимость не может создавать цикл в графе задач (ответ `422`);
* зависимость возможна только между задачами одного проекта;
* desktop-клиент взаимодействует только с `planerd` по HTTPS/HTTP REST;
* клиент может поддерживать HTTP-запросы/ответы со сжатием данных (gzip).

#### **Регистрация пользователя**

Хендлер: `POST /api/v1/auth/register`.

Регистрация производится по паре email/пароль. Email должен быть уникальным.
После успешной регистрации возвращается JWT-токен (автоматическая аутентификация).

Формат запроса:

```
POST /api/v1/auth/register HTTP/1.1
Content-Type: application/json
...

{
    "email": "user@example.com",
    "password": "secret123",
    "display_name": "Иван Иванов"
}
```

Возможные коды ответа:

- `201` - пользователь успешно зарегистрирован и аутентифицирован.

  Формат ответа:

    ```
    201 Created HTTP/1.1
    Content-Type: application/json
    ...

    {
        "token": "eyJhbGciOiJIUzI1NiIs...",
        "user": {
            "id": "550e8400-e29b-41d4-a716-446655440000",
            "email": "user@example.com",
            "display_name": "Иван Иванов",
            "created_at": "2026-06-24T12:00:00Z"
        }
    }
    ```

- `400` - неверный формат запроса;
- `409` - email уже занят;
- `422` - пароль не соответствует требованиям (минимум 8 символов);
- `500` - внутренняя ошибка сервера.

#### **Аутентификация пользователя**

Хендлер: `POST /api/v1/auth/login`.

Аутентификация производится по паре email/пароль.

Формат запроса:

```
POST /api/v1/auth/login HTTP/1.1
Content-Type: application/json
...

{
    "email": "user@example.com",
    "password": "secret123"
}
```

Возможные коды ответа:

- `200` - пользователь успешно аутентифицирован.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "token": "eyJhbGciOiJIUzI1NiIs...",
        "user": {
            "id": "550e8400-e29b-41d4-a716-446655440000",
            "email": "user@example.com",
            "display_name": "Иван Иванов",
            "created_at": "2026-06-24T12:00:00Z"
        }
    }
    ```

- `400` - неверный формат запроса;
- `401` - неверная пара email/пароль;
- `500` - внутренняя ошибка сервера.

#### **Текущий пользователь**

Хендлер: `GET /api/v1/users/me`.

Хендлер доступен только аутентифицированным пользователям.

Формат запроса:

```
GET /api/v1/users/me HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "id": "550e8400-e29b-41d4-a716-446655440000",
        "email": "user@example.com",
        "display_name": "Иван Иванов",
        "created_at": "2026-06-24T12:00:00Z"
    }
    ```

- `401` - пользователь не аутентифицирован;
- `500` - внутренняя ошибка сервера.

#### **Список проектов**

Хендлер: `GET /api/v1/projects`.

Возвращает проекты, в которых текущий пользователь является участником.
Сортировка по `name` ascending.

Формат запроса:

```
GET /api/v1/projects?limit=50&offset=0 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "items": [
            {
                "id": "660e8400-e29b-41d4-a716-446655440001",
                "key": "PLAN",
                "name": "Planer MVP",
                "description": "Первый релиз",
                "owner_id": "550e8400-e29b-41d4-a716-446655440000",
                "role": "admin",
                "created_at": "2026-06-24T12:00:00Z"
            }
        ],
        "total": 1
    }
    ```

- `401` - пользователь не аутентифицирован;
- `500` - внутренняя ошибка сервера.

#### **Создание проекта**

Хендлер: `POST /api/v1/projects`.

Создатель автоматически становится участником с ролью `admin`.

Формат запроса:

```
POST /api/v1/projects HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "key": "PLAN",
    "name": "Planer MVP",
    "description": "Первый релиз"
}
```

Возможные коды ответа:

- `201` - проект создан.

  Формат ответа:

    ```
    201 Created HTTP/1.1
    Content-Type: application/json
    ...

    {
        "id": "660e8400-e29b-41d4-a716-446655440001",
        "key": "PLAN",
        "name": "Planer MVP",
        "description": "Первый релиз",
        "owner_id": "550e8400-e29b-41d4-a716-446655440000",
        "created_at": "2026-06-24T12:00:00Z"
    }
    ```

- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `409` - ключ проекта уже занят;
- `422` - ключ не соответствует формату;
- `500` - внутренняя ошибка сервера.

#### **Получение проекта**

Хендлер: `GET /api/v1/projects/{projectID}`.

Доступен участникам проекта.

Формат запроса:

```
GET /api/v1/projects/660e8400-e29b-41d4-a716-446655440001 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса (тело аналогично созданию + поле `role` текущего пользователя);
- `401` - пользователь не аутентифицирован;
- `403` - нет доступа к проекту;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Обновление проекта**

Хендлер: `PATCH /api/v1/projects/{projectID}`.

Доступен участникам с ролью `admin`. Поле `key` изменять нельзя.

Формат запроса:

```
PATCH /api/v1/projects/660e8400-e29b-41d4-a716-446655440001 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "name": "Planer v1",
    "description": "Обновлённое описание"
}
```

Возможные коды ответа:

- `200` - проект обновлён;
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Удаление проекта**

Хендлер: `DELETE /api/v1/projects/{projectID}`.

Доступен участникам с ролью `admin`. Каскадно удаляет задачи, зависимости и вложения.

Формат запроса:

```
DELETE /api/v1/projects/660e8400-e29b-41d4-a716-446655440001 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `204` - проект удалён;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Список участников проекта**

Хендлер: `GET /api/v1/projects/{projectID}/members`.

Доступен участникам проекта.

Формат запроса:

```
GET /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/members HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "items": [
            {
                "user_id": "550e8400-e29b-41d4-a716-446655440000",
                "email": "user@example.com",
                "display_name": "Иван Иванов",
                "role": "admin"
            }
        ],
        "total": 1
    }
    ```

- `401` - пользователь не аутентифицирован;
- `403` - нет доступа к проекту;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Добавление участника проекта**

Хендлер: `POST /api/v1/projects/{projectID}/members`.

Доступен участникам с ролью `admin`.

Формат запроса:

```
POST /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/members HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "email": "dev@example.com",
    "role": "member"
}
```

Возможные коды ответа:

- `201` - участник добавлен;
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - проект или пользователь с указанным email не найден;
- `409` - пользователь уже участник проекта;
- `422` - недопустимая роль;
- `500` - внутренняя ошибка сервера.

#### **Удаление участника проекта**

Хендлер: `DELETE /api/v1/projects/{projectID}/members/{userID}`.

Доступен участникам с ролью `admin`. Владелец проекта (`owner_id`) не может быть удалён.

Формат запроса:

```
DELETE /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/members/550e8400-e29b-41d4-a716-446655440000 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `204` - участник удалён;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - проект или участник не найден;
- `422` - нельзя удалить владельца проекта;
- `500` - внутренняя ошибка сервера.

#### **Список задач проекта**

Хендлер: `GET /api/v1/projects/{projectID}/tasks`.

Доступен участникам проекта. Поддерживает фильтры:

* `status` - один или несколько статусов (comma-separated);
* `assignee_id` - UUID исполнителя;
* `type` - `task` или `bug`;
* `sort` - `due_date`, `created_at`, `priority` (default `created_at`);
* `order` - `asc` или `desc` (default `desc`).

Формат запроса:

```
GET /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/tasks?status=todo,in_progress&limit=50&offset=0 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "items": [
            {
                "id": "770e8400-e29b-41d4-a716-446655440002",
                "key": "PLAN-1",
                "project_id": "660e8400-e29b-41d4-a716-446655440001",
                "number": 1,
                "title": "Настроить CI",
                "description": "",
                "type": "task",
                "status": "in_progress",
                "priority": "high",
                "assignee": {
                    "id": "550e8400-e29b-41d4-a716-446655440000",
                    "display_name": "Иван Иванов"
                },
                "reporter": {
                    "id": "550e8400-e29b-41d4-a716-446655440000",
                    "display_name": "Иван Иванов"
                },
                "start_date": "2026-06-01",
                "due_date": "2026-06-15",
                "created_at": "2026-06-24T12:00:00Z",
                "updated_at": "2026-06-24T14:30:00Z"
            }
        ],
        "total": 1
    }
    ```

- `401` - пользователь не аутентифицирован;
- `403` - нет доступа к проекту;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Создание задачи**

Хендлер: `POST /api/v1/projects/{projectID}/tasks`.

Доступен участникам с ролями `admin` и `member`. `reporter_id` устанавливается автоматически из JWT.

Формат запроса:

```
POST /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/tasks HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "title": "Настроить CI",
    "description": "GitHub Actions для planerd",
    "type": "task",
    "priority": "high",
    "assignee_id": "550e8400-e29b-41d4-a716-446655440000",
    "start_date": "2026-06-01",
    "due_date": "2026-06-15"
}
```

Поля `type`, `priority`, `assignee_id`, `start_date`, `due_date`, `description` опциональны.
Defaults: `type=task`, `priority=normal`, `status=backlog`.

Возможные коды ответа:

- `201` - задача создана (тело как в списке задач);
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - проект или исполнитель не найден;
- `422` - `start_date` позже `due_date`;
- `500` - внутренняя ошибка сервера.

#### **Получение задачи**

Хендлер: `GET /api/v1/tasks/{taskID}`.

Доступен участникам проекта задачи.

Формат запроса:

```
GET /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса;
- `401` - пользователь не аутентифицирован;
- `403` - нет доступа;
- `404` - задача не найдена;
- `500` - внутренняя ошибка сервера.

#### **Обновление задачи**

Хендлер: `PATCH /api/v1/tasks/{taskID}`.

Доступен участникам с ролями `admin` и `member`. Частичное обновление: передаются только изменяемые поля.

Формат запроса:

```
PATCH /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "status": "in_progress",
    "start_date": "2026-06-05",
    "due_date": "2026-06-20"
}
```

Возможные коды ответа:

- `200` - задача обновлена;
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - задача или исполнитель не найдены;
- `422` - недопустимый статус/приоритет/тип; `start_date` позже `due_date`;
- `500` - внутренняя ошибка сервера.

#### **Удаление задачи**

Хендлер: `DELETE /api/v1/tasks/{taskID}`.

Доступен участникам с ролями `admin` и `member`. Каскадно удаляет зависимости и вложения.

Формат запроса:

```
DELETE /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `204` - задача удалена;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - задача не найдена;
- `500` - внутренняя ошибка сервера.

#### **Список зависимостей задачи**

Хендлер: `GET /api/v1/tasks/{taskID}/dependencies`.

Возвращает зависимости, где задача выступает блокером или заблокированной.

Формат запроса:

```
GET /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/dependencies HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "items": [
            {
                "id": "880e8400-e29b-41d4-a716-446655440003",
                "blocker_task_id": "770e8400-e29b-41d4-a716-446655440002",
                "blocker_key": "PLAN-1",
                "blocked_task_id": "770e8400-e29b-41d4-a716-446655440004",
                "blocked_key": "PLAN-2"
            }
        ],
        "total": 1
    }
    ```

- `401` - пользователь не аутентифицирован;
- `403` - нет доступа;
- `404` - задача не найдена;
- `500` - внутренняя ошибка сервера.

#### **Создание зависимости**

Хендлер: `POST /api/v1/tasks/{taskID}/dependencies`.

`{taskID}` - идентификатор задачи-блокера. Доступен участникам с ролями `admin` и `member`.

Формат запроса:

```
POST /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/dependencies HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: application/json
...

{
    "blocked_task_id": "770e8400-e29b-41d4-a716-446655440004"
}
```

Возможные коды ответа:

- `201` - зависимость создана;
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - одна из задач не найдена;
- `409` - зависимость уже существует;
- `422` - задачи из разных проектов; задача блокирует сама себя; создание цикла;
- `500` - внутренняя ошибка сервера.

#### **Удаление зависимости**

Хендлер: `DELETE /api/v1/tasks/{taskID}/dependencies/{dependencyID}`.

Доступен участникам с ролями `admin` и `member`.

Формат запроса:

```
DELETE /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/dependencies/880e8400-e29b-41d4-a716-446655440003 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `204` - зависимость удалена;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - задача или зависимость не найдены;
- `500` - внутренняя ошибка сервера.

#### **Данные для диаграммы Ганта**

Хендлер: `GET /api/v1/projects/{projectID}/gantt`.

Эндпоинт возвращает все задачи проекта с датами и все зависимости одним запросом.
Доступен участникам проекта.

Формат запроса:

```
GET /api/v1/projects/660e8400-e29b-41d4-a716-446655440001/gantt HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "project_id": "660e8400-e29b-41d4-a716-446655440001",
        "tasks": [
            {
                "id": "770e8400-e29b-41d4-a716-446655440002",
                "key": "PLAN-1",
                "title": "Настроить CI",
                "status": "in_progress",
                "assignee": {
                    "id": "550e8400-e29b-41d4-a716-446655440000",
                    "display_name": "Иван Иванов"
                },
                "start_date": "2026-06-01",
                "due_date": "2026-06-15",
                "progress": 50
            }
        ],
        "dependencies": [
            {
                "id": "880e8400-e29b-41d4-a716-446655440003",
                "blocker_id": "770e8400-e29b-41d4-a716-446655440002",
                "blocked_id": "770e8400-e29b-41d4-a716-446655440004"
            }
        ]
    }
    ```

  Поле `progress` вычисляется на сервере из статуса задачи:

  | Статус | progress |
  |--------|----------|
  | `backlog`, `todo` | 0 |
  | `in_progress` | 50 |
  | `review` | 75 |
  | `done` | 100 |
  | `cancelled` | 0 |

- `401` - пользователь не аутентифицирован;
- `403` - нет доступа к проекту;
- `404` - проект не найден;
- `500` - внутренняя ошибка сервера.

#### **Список вложений задачи**

Хендлер: `GET /api/v1/tasks/{taskID}/attachments`.

Доступен участникам проекта.

Формат запроса:

```
GET /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/attachments HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса.

  Формат ответа:

    ```
    200 OK HTTP/1.1
    Content-Type: application/json
    ...

    {
        "items": [
            {
                "id": "990e8400-e29b-41d4-a716-446655440005",
                "file_name": "screenshot.png",
                "mime_type": "image/png",
                "size_bytes": 102400,
                "uploaded_at": "2026-06-24T15:00:00Z"
            }
        ],
        "total": 1
    }
    ```

- `401` - пользователь не аутентифицирован;
- `403` - нет доступа;
- `404` - задача не найдена;
- `500` - внутренняя ошибка сервера.

#### **Загрузка вложения**

Хендлер: `POST /api/v1/tasks/{taskID}/attachments`.

Доступен участникам с ролями `admin` и `member`.
Формат: `multipart/form-data`, поле `file`.

Формат запроса:

```
POST /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/attachments HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
Content-Type: multipart/form-data; boundary=...
...

--boundary
Content-Disposition: form-data; name="file"; filename="screenshot.png"
Content-Type: image/png

<binary>
--boundary--
```

Возможные коды ответа:

- `201` - вложение загружено (тело как элемент списка вложений);
- `400` - неверный формат запроса;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - задача не найдена;
- `413` - файл превышает `MAX_UPLOAD_BYTES`;
- `415` - MIME-тип не в whitelist (`image/*`, `application/pdf`, `text/plain`);
- `500` - внутренняя ошибка сервера.

#### **Удаление вложения**

Хендлер: `DELETE /api/v1/tasks/{taskID}/attachments/{attachmentID}`.

Доступен участникам с ролями `admin` и `member`. Удаляет объект из MinIO.

Формат запроса:

```
DELETE /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/attachments/990e8400-e29b-41d4-a716-446655440005 HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `204` - вложение удалено;
- `401` - пользователь не аутентифицирован;
- `403` - недостаточно прав;
- `404` - задача или вложение не найдены;
- `500` - внутренняя ошибка сервера.

#### **Скачивание вложения**

Хендлер: `GET /api/v1/tasks/{taskID}/attachments/{attachmentID}/download`.

Доступен участникам проекта. Возвращает бинарное содержимое файла.

Формат запроса:

```
GET /api/v1/tasks/770e8400-e29b-41d4-a716-446655440002/attachments/990e8400-e29b-41d4-a716-446655440005/download HTTP/1.1
Authorization: Bearer eyJhbGciOiJIUzI1NiIs...
```

Возможные коды ответа:

- `200` - успешная обработка запроса (`Content-Type` и `Content-Disposition: attachment` из метаданных);
- `401` - пользователь не аутентифицирован;
- `403` - нет доступа;
- `404` - задача или вложение не найдены;
- `500` - внутренняя ошибка сервера.

### Требования к Desktop UI

Desktop-клиент расположен в `desktop/planer/` (Qt6, QML). Клиент - основной пользовательский интерфейс Planer; все данные получаются из `planerd` по REST API.

#### Общий layout

Трёхколоночный master-detail (как в YouTrack/Jira):

| Зона | Ширина | Содержимое |
|------|--------|------------|
| Левая панель | 240px | список проектов, поиск, кнопка "+ Проект" |
| Центральная область | flex | вкладки: **Список** \| **Kanban** \| **Gantt** |
| Правая панель | 320px | детали выбранной задачи (inline edit) |

Правая панель скрывается, если задача не выбрана. На экранах login/register layout не используется.

#### Экраны MVP

1. **Login / Register** - форма email + password; после успеха сохранить JWT локально (QSettings).
2. **Project list** (левая панель) - проекты пользователя; клик переключает активный проект.
3. **Task list** - таблица: `key`, `title`, `status`, `assignee`, `due_date`; сортировка по `due_date`; клик открывает detail panel.
4. **Kanban board** - колонки по статусам workflow; drag-and-drop карточки -> `PATCH /tasks/{id}` со сменой `status`; UI обновляется сразу, при ошибке откатывается.
5. **Gantt view** - см. раздел ниже.
6. **Task detail panel** - редактирование `title`, `description`, `type`, `priority`, `status`, `assignee`, `start_date`, `due_date`; список зависимостей; список вложений с upload/delete.

#### UX-правила

* **Пустые экраны** - при отсутствии проектов/задач показывать пояснение и кнопку ("Создайте первую задачу").
* **Loading** - skeleton/spinner при загрузке списков; блокировка повторных submit на формах.
* **Ошибки** - toast/snackbar с текстом ошибки API; сетевые ошибки - "Не удалось связаться с сервером".
* **Горячие клавиши**: `Ctrl+N` - создать задачу в текущем проекте; `Ctrl+K` - командная палитра (заглушка: dialog "В разработке").
* **Навигация** - `Esc` закрывает detail panel; `Enter` в списке открывает задачу.
* **Стиль** - Qt Quick Controls (Material); тёмная тема - вне scope MVP, но не hardcode цвета (использовать palette/theme).

#### Связь с backend

* HTTP-клиент: `QNetworkAccessManager` или thin C++ wrapper (`ApiClient`).
* Base URL из env `PLANER_API_URL` или QSettings (default `http://localhost:8080`).
* JWT хранится в QSettings; добавляется в заголовок `Authorization: Bearer ...`.
* При ответе `401` - redirect на Login, очистка токена.

### Диаграмма Ганта

Функциональные требования к представлению Gantt в desktop-клиенте.

#### Временная шкала

* Ось X - календарные дни; переключатель масштаба: **Неделя** / **Месяц**.
* Вертикальная линия "сегодня".
* Горизонтальный scroll для навигации по времени.

#### Полоски задач

* Полоска от `start_date` до `due_date` включительно.
* Если задан только `due_date` - однодневная полоска в эту дату.
* Если даты не заданы - задача отображается в секции **Unscheduled** над chart (как в YouTrack).
* Цвет полоски по статусу:
  * `done` - зелёный;
  * `in_progress`, `review` - синий;
  * `todo`, `backlog` - серый;
  * просроченная (`due_date` < сегодня и статус не `done`/`cancelled`) - красная обводка.

#### Зависимости

* Стрелки от blocker к blocked task (finish-to-start).
* Клик по стрелке - подсветить обе задачи в detail panel.

#### Взаимодействие

* Drag полоски - изменение `start_date` и `due_date` (сохранение длительности).
* Drag левого/правого края - изменение только `start_date` / `due_date`.
* После drag - `PATCH /api/v1/tasks/{taskID}`; UI обновляется сразу, при ошибке откатывается.
* Двойной клик по полоске - открыть detail panel.

#### Ограничения MVP

* Без auto-scheduling при изменении зависимостей.
* Без critical path и resource leveling.
* `progress` отображается заливкой полоски (значение из API), не редактируется вручную.

### Конфигурирование сервиса planerd

Сервис должен поддерживать конфигурирование переменными окружения:

| Переменная | Описание | Default |
|------------|----------|---------|
| `HTTP_ADDR` | адрес и порт HTTP-сервера | `:8080` |
| `DATABASE_URI` | URI PostgreSQL | - (required) |
| `MINIO_ENDPOINT` | endpoint MinIO | - (required) |
| `MINIO_ACCESS_KEY` | access key MinIO | - (required) |
| `MINIO_SECRET_KEY` | secret key MinIO | - (required) |
| `MINIO_SECURE` | HTTPS для MinIO | `false` |
| `MINIO_BUCKET` | bucket для вложений | `planer` |
| `JWT_SECRET` | секрет подписи JWT | - (required) |
| `JWT_TTL` | время жизни токена | `24h` |
| `MAX_UPLOAD_BYTES` | максимальный размер вложения | `10485760` (10 MiB) |
| `SHUTDOWN_TIMEOUT` | таймаут graceful shutdown | `10s` |
| `LOG_LEVEL` | уровень логирования | `info` |

### Конфигурирование desktop-клиента

| Источник | Переменная | Описание | Default |
|----------|------------|----------|---------|
| env / QSettings | `PLANER_API_URL` | base URL planerd | `http://localhost:8080` |

Приоритет: переменная окружения -> QSettings -> default.
