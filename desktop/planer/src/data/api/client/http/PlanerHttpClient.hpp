#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "data/api/HttpClient.hpp"

// PlanerHttpClient — HTTP-реализация IPlanerApiClient (каркас для будущей интеграции).
class PlanerHttpClient : public IPlanerApiClient {
public:
  PlanerHttpClient(HttpClient &httpClient, QString apiBaseUrl);

  void checkHealth(SuccessHandler<HealthStatus> onSuccess,
                   ErrorHandler onError) override;

  void registerUser(const RegisterRequest &request,
                    SuccessHandler<AuthResult> onSuccess,
                    ErrorHandler onError) override;
  void login(const LoginRequest &request, SuccessHandler<AuthResult> onSuccess,
             ErrorHandler onError) override;
  void getMe(SuccessHandler<User> onSuccess, ErrorHandler onError) override;

  void listProjects(SuccessHandler<Paginated<Project>> onSuccess,
                    ErrorHandler onError) override;
  void createProject(const CreateProjectRequest &request,
                     SuccessHandler<Project> onSuccess,
                     ErrorHandler onError) override;
  void getProject(const QString &projectId, SuccessHandler<Project> onSuccess,
                  ErrorHandler onError) override;
  void updateProject(const QString &projectId,
                     const UpdateProjectRequest &request,
                     SuccessHandler<Project> onSuccess,
                     ErrorHandler onError) override;
  void deleteProject(const QString &projectId, SuccessVoid onSuccess,
                     ErrorHandler onError) override;

  void listMembers(const QString &projectId,
                   SuccessHandler<Paginated<ProjectMember>> onSuccess,
                   ErrorHandler onError) override;
  void addMember(const QString &projectId, const AddMemberRequest &request,
                 SuccessVoid onSuccess, ErrorHandler onError) override;
  void removeMember(const QString &projectId, const QString &userId,
                    SuccessVoid onSuccess, ErrorHandler onError) override;

  void listTasks(const QString &projectId,
                 SuccessHandler<Paginated<Task>> onSuccess,
                 ErrorHandler onError) override;
  void createTask(const QString &projectId, const CreateTaskRequest &request,
                  SuccessHandler<Task> onSuccess,
                  ErrorHandler onError) override;
  void getTask(const QString &taskId, SuccessHandler<Task> onSuccess,
               ErrorHandler onError) override;
  void updateTask(const QString &taskId, const UpdateTaskRequest &request,
                  SuccessHandler<Task> onSuccess,
                  ErrorHandler onError) override;
  void deleteTask(const QString &taskId, SuccessVoid onSuccess,
                  ErrorHandler onError) override;

  void listDependencies(const QString &taskId,
                        SuccessHandler<Paginated<TaskDependency>> onSuccess,
                        ErrorHandler onError) override;
  void createDependency(const QString &blockerTaskId,
                        const QString &blockedTaskId,
                        SuccessHandler<TaskDependency> onSuccess,
                        ErrorHandler onError) override;
  void deleteDependency(const QString &taskId, const QString &dependencyId,
                        SuccessVoid onSuccess, ErrorHandler onError) override;

  void getGantt(const QString &projectId, SuccessHandler<GanttData> onSuccess,
                ErrorHandler onError) override;

  void listAttachments(const QString &taskId,
                       SuccessHandler<Paginated<Attachment>> onSuccess,
                       ErrorHandler onError) override;
  void uploadAttachment(const QString &taskId, const QString &fileName,
                        const QString &mimeType, qint64 sizeBytes,
                        SuccessHandler<Attachment> onSuccess,
                        ErrorHandler onError) override;
  void deleteAttachment(const QString &taskId, const QString &attachmentId,
                        SuccessVoid onSuccess, ErrorHandler onError) override;

private:
  void notImplemented(ErrorHandler onError) const;

  HttpClient &httpClient_;
  QString apiBaseUrl_;
};
