#include "data/api/client/http/PlanerHttpClient.hpp"

#include "data/api/client/dto/HealthDto.hpp"
#include "data/api/client/mappers/HealthMapper.hpp"

#include <QUrl>

PlanerHttpClient::PlanerHttpClient(HttpClient &httpClient, QString apiBaseUrl)
    : httpClient_(httpClient), apiBaseUrl_(std::move(apiBaseUrl)) {}

void PlanerHttpClient::notImplemented(ErrorHandler onError) const {
  onError(ApiError::internal(QStringLiteral("HTTP client not implemented yet")));
}

void PlanerHttpClient::checkHealth(SuccessHandler<HealthStatus> onSuccess,
                                   ErrorHandler onError) {
  const QUrl url(apiBaseUrl_ + QStringLiteral("/health"));
  httpClient_.get(
      url,
      [onSuccess = std::move(onSuccess)](const QJsonObject &object) {
        onSuccess(HealthMapper::toDomain(HealthDto::fromJson(object)));
      },
      [onError = std::move(onError)](const QString &message) {
        onError({ApiErrorKind::Network, message});
      });
}

void PlanerHttpClient::registerUser(const RegisterRequest &,
                                    SuccessHandler<AuthResult>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::login(const LoginRequest &, SuccessHandler<AuthResult>,
                             ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::getMe(SuccessHandler<User>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::listProjects(SuccessHandler<Paginated<Project>>,
                                    ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::createProject(const CreateProjectRequest &,
                                     SuccessHandler<Project>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::getProject(const QString &, SuccessHandler<Project>,
                                  ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::updateProject(const QString &, const UpdateProjectRequest &,
                                     SuccessHandler<Project>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::deleteProject(const QString &, SuccessVoid,
                                     ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::listMembers(const QString &,
                                   SuccessHandler<Paginated<ProjectMember>>,
                                   ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::addMember(const QString &, const AddMemberRequest &,
                                 SuccessVoid, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::removeMember(const QString &, const QString &, SuccessVoid,
                                    ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::listTasks(const QString &, SuccessHandler<Paginated<Task>>,
                                 ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::createTask(const QString &, const CreateTaskRequest &,
                                  SuccessHandler<Task>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::getTask(const QString &, SuccessHandler<Task>,
                               ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::updateTask(const QString &, const UpdateTaskRequest &,
                                  SuccessHandler<Task>, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::deleteTask(const QString &, SuccessVoid,
                                  ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::listDependencies(const QString &,
                                        SuccessHandler<Paginated<TaskDependency>>,
                                        ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::createDependency(const QString &, const QString &,
                                        SuccessHandler<TaskDependency>,
                                        ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::deleteDependency(const QString &, const QString &,
                                        SuccessVoid, ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::getGantt(const QString &, SuccessHandler<GanttData>,
                                ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::listAttachments(const QString &,
                                       SuccessHandler<Paginated<Attachment>>,
                                       ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::uploadAttachment(const QString &, const QString &,
                                        const QString &, qint64,
                                        SuccessHandler<Attachment>,
                                        ErrorHandler onError) {
  notImplemented(std::move(onError));
}

void PlanerHttpClient::deleteAttachment(const QString &, const QString &,
                                        SuccessVoid, ErrorHandler onError) {
  notImplemented(std::move(onError));
}
