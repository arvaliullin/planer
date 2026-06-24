#include "data/api/client/mock/MockPlanerApiClient.hpp"

#include "core/TokenStorage.hpp"
#include "domain/entities/Enums.hpp"

#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QTimer>
#include <QUuid>

namespace {

constexpr int kMinDelayMs = 50;
constexpr int kMaxDelayMs = 150;

QString newId() { return QUuid::createUuid().toString(QUuid::WithoutBraces); }

QDateTime nowUtc() { return QDateTime::currentDateTimeUtc(); }

bool isTerminalStatus(TaskStatus status) {
  return status == TaskStatus::Done || status == TaskStatus::Cancelled;
}

} // namespace

MockPlanerApiClient::MockPlanerApiClient(QObject *parent) : QObject(parent) {
  seedData();
}

void MockPlanerApiClient::dispatch(std::function<void()> fn) {
  const int delay = QRandomGenerator::global()->bounded(kMinDelayMs, kMaxDelayMs);
  QTimer::singleShot(delay, this, std::move(fn));
}

QString MockPlanerApiClient::currentUserId() const {
  const QString token = TokenStorage::token();
  if (token.isEmpty()) {
    return {};
  }
  if (tokens_.contains(token)) {
    return tokens_.value(token);
  }
  const QString storedUserId = TokenStorage::userId();
  if (!storedUserId.isEmpty()) {
    tokens_[token] = storedUserId;
    return storedUserId;
  }
  return {};
}

bool MockPlanerApiClient::requireAuth(ErrorHandler onError) {
  if (currentUserId().isEmpty()) {
    onError(ApiError::unauthorized());
    return false;
  }
  return true;
}

ProjectRole MockPlanerApiClient::roleForUser(const QString &projectId,
                                             const QString &userId) const {
  for (const auto &membership : memberships_) {
    if (membership.projectId == projectId && membership.userId == userId) {
      return membership.role;
    }
  }
  return ProjectRole::Viewer;
}

bool MockPlanerApiClient::hasProjectAccess(const QString &projectId,
                                           const QString &userId) const {
  for (const auto &m : memberships_) {
    if (m.projectId == projectId && m.userId == userId) {
      return true;
    }
  }
  return false;
}

bool MockPlanerApiClient::canEditTasks(const QString &projectId,
                                       const QString &userId) const {
  const ProjectRole role = roleForUser(projectId, userId);
  return role == ProjectRole::Admin || role == ProjectRole::Member;
}

bool MockPlanerApiClient::canAdminProject(const QString &projectId,
                                          const QString &userId) const {
  return roleForUser(projectId, userId) == ProjectRole::Admin;
}

QString MockPlanerApiClient::projectKeyForId(const QString &projectId) const {
  return projects_.value(projectId).key;
}

Task MockPlanerApiClient::buildTaskPublic(const Task &stored) const {
  Task task = stored;
  task.key = QStringLiteral("%1-%2").arg(projectKeyForId(stored.projectId),
                                          QString::number(stored.number));
  return task;
}

AuthResult MockPlanerApiClient::makeAuthResult(const QString &userId) {
  const QString token = QStringLiteral("mock-token-%1").arg(newId());
  tokens_[token] = userId;
  TokenStorage::setSession(token, userId);
  return {token, usersById_.value(userId).user};
}

bool MockPlanerApiClient::isValidProjectKey(const QString &key) const {
  static const QRegularExpression pattern(QStringLiteral("^[A-Z]{2,10}$"));
  return pattern.match(key).hasMatch();
}

bool MockPlanerApiClient::wouldCreateCycle(const QString &blockerId,
                                           const QString &blockedId) const {
  QSet<QString> visited;
  QList<QString> queue;
  queue.append(blockedId);

  while (!queue.isEmpty()) {
    const QString current = queue.takeFirst();
    if (current == blockerId) {
      return true;
    }
    if (visited.contains(current)) {
      continue;
    }
    visited.insert(current);

    for (const auto &dep : dependencies_) {
      if (dep.blockerTaskId == current) {
        queue.append(dep.blockedTaskId);
      }
    }
  }
  return false;
}

void MockPlanerApiClient::seedData() {
  const QString demoId = QStringLiteral("550e8400-e29b-41d4-a716-446655440000");
  const QString devId = QStringLiteral("550e8400-e29b-41d4-a716-446655440001");
  const QString planProjectId =
      QStringLiteral("660e8400-e29b-41d4-a716-446655440001");
  const QString devProjectId =
      QStringLiteral("660e8400-e29b-41d4-a716-446655440002");

  usersById_[demoId] = {User{demoId, QStringLiteral("demo@planer.local"),
                              QStringLiteral("Иван Иванов"),
                              QDateTime(QDate(2026, 6, 24), QTime(12, 0),
                                        Qt::UTC)},
                        QStringLiteral("secret123")};
  usersByEmail_[QStringLiteral("demo@planer.local")] = demoId;

  usersById_[devId] = {User{devId, QStringLiteral("dev@example.com"),
                             QStringLiteral("Алексей Петров"),
                             QDateTime(QDate(2026, 6, 20), QTime(10, 0),
                                       Qt::UTC)},
                       QStringLiteral("secret123")};
  usersByEmail_[QStringLiteral("dev@example.com")] = devId;

  projects_[planProjectId] =
      Project{planProjectId, QStringLiteral("PLAN"),
              QStringLiteral("Planer MVP"),
              QStringLiteral("Первый релиз desktop-клиента"), demoId,
              ProjectRole::Admin,
              QDateTime(QDate(2026, 6, 24), QTime(12, 0), Qt::UTC)};
  projects_[devProjectId] =
      Project{devProjectId, QStringLiteral("DEV"),
              QStringLiteral("Developer Tools"),
              QStringLiteral("Инструменты разработки"), demoId,
              ProjectRole::Admin,
              QDateTime(QDate(2026, 6, 22), QTime(9, 0), Qt::UTC)};
  usedProjectKeys_.insert(QStringLiteral("PLAN"));
  usedProjectKeys_.insert(QStringLiteral("DEV"));

  memberships_.append({planProjectId, demoId, ProjectRole::Admin});
  memberships_.append({planProjectId, devId, ProjectRole::Member});
  memberships_.append({devProjectId, demoId, ProjectRole::Admin});

  taskCounters_[planProjectId] = 0;
  taskCounters_[devProjectId] = 0;

  auto addTask = [&](const QString &projectId, const QString &id,
                     int number, const QString &title, TaskStatus status,
                     TaskPriority priority, const QString &assigneeId,
                     const QDate &start, const QDate &due) {
    Task task;
    task.id = id;
    task.projectId = projectId;
    task.number = number;
    task.title = title;
    task.description = QString();
    task.type = TaskType::Task;
    task.status = status;
    task.priority = priority;
    if (!assigneeId.isEmpty()) {
      const User user = usersById_.value(assigneeId).user;
      task.assignee = UserRef{user.id, user.displayName};
    }
    const User demoUser = usersById_.value(demoId).user;
    task.reporter = UserRef{demoUser.id, demoUser.displayName};
    task.startDate = start;
    task.dueDate = due;
    task.createdAt = nowUtc();
    task.updatedAt = task.createdAt;
    tasks_[id] = task;
    taskCounters_[projectId] = qMax(taskCounters_[projectId], number);
  };

  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440002"),
          1, QStringLiteral("Настроить CI"), TaskStatus::InProgress,
          TaskPriority::High, demoId, QDate(2026, 6, 1), QDate(2026, 6, 15));
  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440003"),
          2, QStringLiteral("Реализовать Kanban"), TaskStatus::Todo,
          TaskPriority::Normal, devId, QDate(2026, 6, 10), QDate(2026, 6, 25));
  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440004"),
          3, QStringLiteral("Диаграмма Ганта"), TaskStatus::Backlog,
          TaskPriority::Normal, QString(), QDate(), QDate());
  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440005"),
          4, QStringLiteral("Auth flow"), TaskStatus::Done, TaskPriority::High,
          demoId, QDate(2026, 5, 20), QDate(2026, 6, 5));
  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440006"),
          5, QStringLiteral("Исправить drag на Gantt"), TaskStatus::Review,
          TaskPriority::Critical, devId, QDate(2026, 6, 12), QDate(2026, 6, 18));
  addTask(planProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440007"),
          6, QStringLiteral("Документация API"), TaskStatus::Todo,
          TaskPriority::Low, demoId, QDate(), QDate(2026, 6, 30));

  addTask(devProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440008"),
          1, QStringLiteral("CLI утилита"), TaskStatus::InProgress,
          TaskPriority::Normal, demoId, QDate(2026, 6, 5), QDate(2026, 6, 20));
  addTask(devProjectId, QStringLiteral("770e8400-e29b-41d4-a716-446655440009"),
          2, QStringLiteral("Lint rules"), TaskStatus::Backlog,
          TaskPriority::Low, QString(), QDate(), QDate());

  const QString depId = QStringLiteral("880e8400-e29b-41d4-a716-446655440003");
  dependencies_[depId] = TaskDependency{
      depId, QStringLiteral("770e8400-e29b-41d4-a716-446655440002"),
      QStringLiteral("PLAN-1"),
      QStringLiteral("770e8400-e29b-41d4-a716-446655440003"),
      QStringLiteral("PLAN-2")};

  const QString attId = QStringLiteral("990e8400-e29b-41d4-a716-446655440005");
  attachments_[attId] = Attachment{
      attId, QStringLiteral("770e8400-e29b-41d4-a716-446655440002"),
      QStringLiteral("screenshot.png"), QStringLiteral("image/png"), 102400,
      nowUtc()};
}

void MockPlanerApiClient::checkHealth(SuccessHandler<HealthStatus> onSuccess,
                                      ErrorHandler onError) {
  Q_UNUSED(onError);
  dispatch([onSuccess = std::move(onSuccess)]() {
    HealthStatus status;
    status.postgres = ComponentHealth{true, {}};
    status.minio = ComponentHealth{true, {}};
    onSuccess(status);
  });
}

void MockPlanerApiClient::registerUser(const RegisterRequest &request,
                                       SuccessHandler<AuthResult> onSuccess,
                                       ErrorHandler onError) {
  dispatch([this, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (request.email.trimmed().isEmpty() ||
        request.password.trimmed().isEmpty()) {
      onError(ApiError::badRequest(QStringLiteral("Заполните все поля")));
      return;
    }
    if (request.password.size() < 8) {
      onError(ApiError::validation(
          QStringLiteral("Пароль должен содержать минимум 8 символов")));
      return;
    }
    if (usersByEmail_.contains(request.email.toLower())) {
      onError(ApiError::conflict(QStringLiteral("Email уже занят")));
      return;
    }

    const QString id = newId();
    User user{id, request.email.toLower(), request.displayName.trimmed(),
              nowUtc()};
    usersById_[id] = {user, request.password};
    usersByEmail_[user.email] = id;
    onSuccess(makeAuthResult(id));
  });
}

void MockPlanerApiClient::login(const LoginRequest &request,
                                SuccessHandler<AuthResult> onSuccess,
                                ErrorHandler onError) {
  dispatch([this, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    const QString email = request.email.toLower();
    const QString userId = usersByEmail_.value(email);
    if (userId.isEmpty() ||
        usersById_.value(userId).password != request.password) {
      onError({ApiErrorKind::Unauthorized,
               QStringLiteral("Неверная пара email/пароль")});
      return;
    }
    onSuccess(makeAuthResult(userId));
  });
}

void MockPlanerApiClient::getMe(SuccessHandler<User> onSuccess,
                                ErrorHandler onError) {
  dispatch([this, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    onSuccess(usersById_.value(currentUserId()).user);
  });
}

void MockPlanerApiClient::listProjects(SuccessHandler<Paginated<Project>> onSuccess,
                                       ErrorHandler onError) {
  dispatch([this, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    QList<Project> items;
    for (const auto &project : projects_) {
      const ProjectRole role = roleForUser(project.id, userId);
      bool member = false;
      for (const auto &m : memberships_) {
        if (m.projectId == project.id && m.userId == userId) {
          member = true;
          break;
        }
      }
      if (!member) {
        continue;
      }
      Project copy = project;
      copy.role = role;
      items.append(copy);
    }
    std::sort(items.begin(), items.end(),
              [](const Project &a, const Project &b) {
                return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
              });
    onSuccess({items, items.size()});
  });
}

void MockPlanerApiClient::createProject(const CreateProjectRequest &request,
                                        SuccessHandler<Project> onSuccess,
                                        ErrorHandler onError) {
  dispatch([this, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString key = request.key.trimmed().toUpper();
    if (!isValidProjectKey(key)) {
      onError(ApiError::validation(
          QStringLiteral("Ключ проекта: 2-10 заглавных лatinских букв")));
      return;
    }
    if (usedProjectKeys_.contains(key)) {
      onError(ApiError::conflict(QStringLiteral("Ключ проекта уже занят")));
      return;
    }
    if (request.name.trimmed().isEmpty()) {
      onError(ApiError::badRequest(QStringLiteral("Укажите название проекта")));
      return;
    }

    const QString userId = currentUserId();
    const QString id = newId();
    Project project{id, key, request.name.trimmed(), request.description.trimmed(),
                    userId, ProjectRole::Admin, nowUtc()};
    projects_[id] = project;
    usedProjectKeys_.insert(key);
    memberships_.append({id, userId, ProjectRole::Admin});
    taskCounters_[id] = 0;
    onSuccess(project);
  });
}

void MockPlanerApiClient::getProject(const QString &projectId,
                                     SuccessHandler<Project> onSuccess,
                                     ErrorHandler onError) {
  dispatch([this, projectId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    bool member = false;
    for (const auto &m : memberships_) {
      if (m.projectId == projectId && m.userId == userId) {
        member = true;
        break;
      }
    }
    if (!member) {
      onError(ApiError::forbidden());
      return;
    }
    Project copy = projects_.value(projectId);
    copy.role = roleForUser(projectId, userId);
    onSuccess(copy);
  });
}

void MockPlanerApiClient::updateProject(const QString &projectId,
                                        const UpdateProjectRequest &request,
                                        SuccessHandler<Project> onSuccess,
                                        ErrorHandler onError) {
  dispatch([this, projectId, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!canAdminProject(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    Project &project = projects_[projectId];
    if (!request.name.isEmpty()) {
      project.name = request.name;
    }
    project.description = request.description;
    project.role = roleForUser(projectId, userId);
    onSuccess(project);
  });
}

void MockPlanerApiClient::deleteProject(const QString &projectId,
                                        SuccessVoid onSuccess,
                                        ErrorHandler onError) {
  dispatch([this, projectId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!canAdminProject(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    usedProjectKeys_.remove(projects_.value(projectId).key);
    projects_.remove(projectId);
    memberships_.erase(
        std::remove_if(memberships_.begin(), memberships_.end(),
                       [&](const ProjectMembership &m) {
                         return m.projectId == projectId;
                       }),
        memberships_.end());

    QSet<QString> taskIds;
    for (auto it = tasks_.begin(); it != tasks_.end(); ++it) {
      if (it->projectId == projectId) {
        taskIds.insert(it.key());
      }
    }
    for (const QString &taskId : taskIds) {
      tasks_.remove(taskId);
    }
    taskCounters_.remove(projectId);

    QList<QString> depIds;
    for (auto it = dependencies_.begin(); it != dependencies_.end(); ++it) {
      if (taskIds.contains(it->blockerTaskId) ||
          taskIds.contains(it->blockedTaskId)) {
        depIds.append(it.key());
      }
    }
    for (const QString &depId : depIds) {
      dependencies_.remove(depId);
    }

    onSuccess();
  });
}

void MockPlanerApiClient::listMembers(
    const QString &projectId, SuccessHandler<Paginated<ProjectMember>> onSuccess,
    ErrorHandler onError) {
  dispatch([this, projectId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!hasProjectAccess(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    QList<ProjectMember> items;
    for (const auto &m : memberships_) {
      if (m.projectId != projectId) {
        continue;
      }
      const User user = usersById_.value(m.userId).user;
      items.append({user.id, user.email, user.displayName, m.role});
    }
    onSuccess({items, items.size()});
  });
}

void MockPlanerApiClient::addMember(const QString &projectId,
                                    const AddMemberRequest &request,
                                    SuccessVoid onSuccess, ErrorHandler onError) {
  dispatch([this, projectId, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!canAdminProject(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    const QString newUserId = usersByEmail_.value(request.email.toLower());
    if (newUserId.isEmpty()) {
      onError(ApiError::notFound(QStringLiteral("Пользователь")));
      return;
    }
    for (const auto &m : memberships_) {
      if (m.projectId == projectId && m.userId == newUserId) {
        onError(ApiError::conflict(
            QStringLiteral("Пользователь уже участник проекта")));
        return;
      }
    }
    memberships_.append({projectId, newUserId, request.role});
    onSuccess();
  });
}

void MockPlanerApiClient::removeMember(const QString &projectId,
                                       const QString &userId,
                                       SuccessVoid onSuccess,
                                       ErrorHandler onError) {
  dispatch([this, projectId, userId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString current = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!canAdminProject(projectId, current)) {
      onError(ApiError::forbidden());
      return;
    }
    if (projects_.value(projectId).ownerId == userId) {
      onError(ApiError::validation(
          QStringLiteral("Нельзя удалить владельца проекта")));
      return;
    }
    const int removed = memberships_.removeIf(
        [&](const ProjectMembership &m) {
          return m.projectId == projectId && m.userId == userId;
        });
    if (removed == 0) {
      onError(ApiError::notFound(QStringLiteral("Участник")));
      return;
    }
    onSuccess();
  });
}

void MockPlanerApiClient::listTasks(const QString &projectId,
                                    SuccessHandler<Paginated<Task>> onSuccess,
                                    ErrorHandler onError) {
  dispatch([this, projectId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!hasProjectAccess(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    QList<Task> items;
    for (const auto &task : tasks_) {
      if (task.projectId != projectId) {
        continue;
      }
      items.append(buildTaskPublic(task));
    }
    std::sort(items.begin(), items.end(), [](const Task &a, const Task &b) {
      const QDate aDue = a.dueDate.value_or(QDate());
      const QDate bDue = b.dueDate.value_or(QDate());
      if (aDue.isValid() && bDue.isValid() && aDue != bDue) {
        return aDue < bDue;
      }
      if (aDue.isValid() != bDue.isValid()) {
        return aDue.isValid();
      }
      return a.createdAt > b.createdAt;
    });
    onSuccess({items, items.size()});
  });
}

void MockPlanerApiClient::createTask(const QString &projectId,
                                     const CreateTaskRequest &request,
                                     SuccessHandler<Task> onSuccess,
                                     ErrorHandler onError) {
  dispatch([this, projectId, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!canEditTasks(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    if (request.title.trimmed().isEmpty()) {
      onError(ApiError::badRequest(QStringLiteral("Укажите заголовок задачи")));
      return;
    }
    if (request.startDate && request.dueDate &&
        request.startDate.value() > request.dueDate.value()) {
      onError(ApiError::validation(
          QStringLiteral("Дата начала не может быть позже даты окончания")));
      return;
    }

    Task task;
    task.id = newId();
    task.projectId = projectId;
    task.number = ++taskCounters_[projectId];
    task.title = request.title.trimmed();
    task.description = request.description;
    task.type = request.type;
    task.status = TaskStatus::Backlog;
    task.priority = request.priority;
    if (request.assigneeId) {
      const User user = usersById_.value(request.assigneeId.value()).user;
      if (user.id.isEmpty()) {
        onError(ApiError::notFound(QStringLiteral("Исполнитель")));
        return;
      }
      task.assignee = UserRef{user.id, user.displayName};
    }
    const User reporter = usersById_.value(userId).user;
    task.reporter = UserRef{reporter.id, reporter.displayName};
    task.startDate = request.startDate;
    task.dueDate = request.dueDate;
    task.createdAt = nowUtc();
    task.updatedAt = task.createdAt;
    tasks_[task.id] = task;
    onSuccess(buildTaskPublic(task));
  });
}

void MockPlanerApiClient::getTask(const QString &taskId,
                                  SuccessHandler<Task> onSuccess,
                                  ErrorHandler onError) {
  dispatch([this, taskId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task stored = tasks_.value(taskId);
    if (!hasProjectAccess(stored.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    onSuccess(buildTaskPublic(stored));
  });
}

void MockPlanerApiClient::updateTask(const QString &taskId,
                                     const UpdateTaskRequest &request,
                                     SuccessHandler<Task> onSuccess,
                                     ErrorHandler onError) {
  dispatch([this, taskId, request, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    Task &task = tasks_[taskId];
    if (!canEditTasks(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    if (request.title) {
      task.title = request.title.value();
    }
    if (request.description) {
      task.description = request.description.value();
    }
    if (request.type) {
      task.type = request.type.value();
    }
    if (request.status) {
      task.status = request.status.value();
    }
    if (request.priority) {
      task.priority = request.priority.value();
    }
    if (request.clearAssignee && *request.clearAssignee) {
      task.assignee.reset();
    } else if (request.assigneeId) {
      const User user = usersById_.value(request.assigneeId.value()).user;
      if (user.id.isEmpty()) {
        onError(ApiError::notFound(QStringLiteral("Исполнитель")));
        return;
      }
      task.assignee = UserRef{user.id, user.displayName};
    }
    if (request.clearStartDate && *request.clearStartDate) {
      task.startDate.reset();
    } else if (request.startDate) {
      task.startDate = request.startDate;
    }
    if (request.clearDueDate && *request.clearDueDate) {
      task.dueDate.reset();
    } else if (request.dueDate) {
      task.dueDate = request.dueDate;
    }

    if (task.startDate && task.dueDate &&
        task.startDate.value() > task.dueDate.value()) {
      onError(ApiError::validation(
          QStringLiteral("Дата начала не может быть позже даты окончания")));
      return;
    }

    task.updatedAt = nowUtc();
    onSuccess(buildTaskPublic(task));
  });
}

void MockPlanerApiClient::deleteTask(const QString &taskId, SuccessVoid onSuccess,
                                     ErrorHandler onError) {
  dispatch([this, taskId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!canEditTasks(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    tasks_.remove(taskId);
    QList<QString> depIds;
    for (auto it = dependencies_.begin(); it != dependencies_.end(); ++it) {
      if (it->blockerTaskId == taskId || it->blockedTaskId == taskId) {
        depIds.append(it.key());
      }
    }
    for (const QString &depId : depIds) {
      dependencies_.remove(depId);
    }
    QList<QString> attIds;
    for (auto it = attachments_.begin(); it != attachments_.end(); ++it) {
      if (it->taskId == taskId) {
        attIds.append(it.key());
      }
    }
    for (const QString &attId : attIds) {
      attachments_.remove(attId);
    }
    onSuccess();
  });
}

void MockPlanerApiClient::listDependencies(
    const QString &taskId, SuccessHandler<Paginated<TaskDependency>> onSuccess,
    ErrorHandler onError) {
  dispatch([this, taskId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!hasProjectAccess(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    QList<TaskDependency> items;
    for (const auto &dep : dependencies_) {
      if (dep.blockerTaskId == taskId || dep.blockedTaskId == taskId) {
        TaskDependency copy = dep;
        copy.blockerKey = buildTaskPublic(tasks_.value(dep.blockerTaskId)).key;
        copy.blockedKey = buildTaskPublic(tasks_.value(dep.blockedTaskId)).key;
        items.append(copy);
      }
    }
    onSuccess({items, items.size()});
  });
}

void MockPlanerApiClient::createDependency(const QString &blockerTaskId,
                                           const QString &blockedTaskId,
                                           SuccessHandler<TaskDependency> onSuccess,
                                           ErrorHandler onError) {
  dispatch([this, blockerTaskId, blockedTaskId,
            onSuccess = std::move(onSuccess), onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(blockerTaskId) || !tasks_.contains(blockedTaskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task blocker = tasks_.value(blockerTaskId);
    const Task blocked = tasks_.value(blockedTaskId);
    if (blocker.projectId != blocked.projectId) {
      onError(ApiError::validation(
          QStringLiteral("Зависимость возможна только между задачами одного проекта")));
      return;
    }
    if (!canEditTasks(blocker.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    if (blockerTaskId == blockedTaskId) {
      onError(ApiError::validation(
          QStringLiteral("Задача не может блокировать саму себя")));
      return;
    }
    for (const auto &dep : dependencies_) {
      if (dep.blockerTaskId == blockerTaskId &&
          dep.blockedTaskId == blockedTaskId) {
        onError(ApiError::conflict(QStringLiteral("Зависимость уже существует")));
        return;
      }
    }
    if (wouldCreateCycle(blockerTaskId, blockedTaskId)) {
      onError(ApiError::validation(
          QStringLiteral("Зависимость создаёт цикл в графе задач")));
      return;
    }

    const QString id = newId();
    TaskDependency dep{id, blockerTaskId, buildTaskPublic(blocker).key,
                       blockedTaskId, buildTaskPublic(blocked).key};
    dependencies_[id] = dep;
    onSuccess(dep);
  });
}

void MockPlanerApiClient::deleteDependency(const QString &taskId,
                                           const QString &dependencyId,
                                           SuccessVoid onSuccess,
                                           ErrorHandler onError) {
  dispatch([this, taskId, dependencyId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    if (!dependencies_.contains(dependencyId)) {
      onError(ApiError::notFound(QStringLiteral("Зависимость")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!canEditTasks(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    dependencies_.remove(dependencyId);
    onSuccess();
  });
}

void MockPlanerApiClient::getGantt(const QString &projectId,
                                   SuccessHandler<GanttData> onSuccess,
                                   ErrorHandler onError) {
  dispatch([this, projectId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!projects_.contains(projectId)) {
      onError(ApiError::notFound(QStringLiteral("Проект")));
      return;
    }
    if (!hasProjectAccess(projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    GanttData data;
    data.projectId = projectId;
    for (const auto &task : tasks_) {
      if (task.projectId != projectId) {
        continue;
      }
      GanttTask ganttTask;
      ganttTask.id = task.id;
      ganttTask.key = buildTaskPublic(task).key;
      ganttTask.title = task.title;
      ganttTask.status = task.status;
      ganttTask.assignee = task.assignee;
      ganttTask.startDate = task.startDate;
      ganttTask.dueDate = task.dueDate;
      ganttTask.progress = ganttProgressForStatus(task.status);
      data.tasks.append(ganttTask);
    }
    for (const auto &dep : dependencies_) {
      const Task blocker = tasks_.value(dep.blockerTaskId);
      const Task blocked = tasks_.value(dep.blockedTaskId);
      if (blocker.projectId == projectId && blocked.projectId == projectId) {
        data.dependencies.append(
            GanttDependency{dep.id, dep.blockerTaskId, dep.blockedTaskId});
      }
    }
    onSuccess(data);
  });
}

void MockPlanerApiClient::listAttachments(
    const QString &taskId, SuccessHandler<Paginated<Attachment>> onSuccess,
    ErrorHandler onError) {
  dispatch([this, taskId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!hasProjectAccess(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }

    QList<Attachment> items;
    for (const auto &attachment : attachments_) {
      if (attachment.taskId == taskId) {
        items.append(attachment);
      }
    }
    onSuccess({items, items.size()});
  });
}

void MockPlanerApiClient::uploadAttachment(const QString &taskId,
                                           const QString &fileName,
                                           const QString &mimeType,
                                           qint64 sizeBytes,
                                           SuccessHandler<Attachment> onSuccess,
                                           ErrorHandler onError) {
  dispatch([this, taskId, fileName, mimeType, sizeBytes,
            onSuccess = std::move(onSuccess), onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!canEditTasks(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    if (sizeBytes > 10 * 1024 * 1024) {
      onError({ApiErrorKind::PayloadTooLarge,
               QStringLiteral("Файл превышает максимальный размер")});
      return;
    }
    const bool allowed = mimeType.startsWith(QStringLiteral("image/")) ||
                         mimeType == QLatin1String("application/pdf") ||
                         mimeType == QLatin1String("text/plain");
    if (!allowed) {
      onError({ApiErrorKind::UnsupportedMedia,
               QStringLiteral("Недопустимый тип файла")});
      return;
    }

    const QString id = newId();
    Attachment attachment{id, taskId, fileName, mimeType, sizeBytes, nowUtc()};
    attachments_[id] = attachment;
    onSuccess(attachment);
  });
}

void MockPlanerApiClient::deleteAttachment(const QString &taskId,
                                           const QString &attachmentId,
                                           SuccessVoid onSuccess,
                                           ErrorHandler onError) {
  dispatch([this, taskId, attachmentId, onSuccess = std::move(onSuccess),
            onError = std::move(onError)]() {
    if (!requireAuth(onError)) {
      return;
    }
    const QString userId = currentUserId();
    if (!tasks_.contains(taskId)) {
      onError(ApiError::notFound(QStringLiteral("Задача")));
      return;
    }
    const Task task = tasks_.value(taskId);
    if (!canEditTasks(task.projectId, userId)) {
      onError(ApiError::forbidden());
      return;
    }
    if (!attachments_.contains(attachmentId) ||
        attachments_.value(attachmentId).taskId != taskId) {
      onError(ApiError::notFound(QStringLiteral("Вложение")));
      return;
    }
    attachments_.remove(attachmentId);
    onSuccess();
  });
}
