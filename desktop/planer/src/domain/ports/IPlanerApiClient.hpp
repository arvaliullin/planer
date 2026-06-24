#pragma once

#include "domain/entities/AuthResult.hpp"
#include "domain/entities/HealthStatus.hpp"
#include "domain/entities/Paginated.hpp"
#include "domain/entities/Project.hpp"
#include "domain/entities/Task.hpp"
#include "domain/entities/User.hpp"

#include "core/ApiError.hpp"

#include <functional>

class IPlanerApiClient {
public:
  using SuccessVoid = std::function<void()>;
  template <typename T> using SuccessHandler = std::function<void(T)>;
  using ErrorHandler = std::function<void(ApiError)>;

  virtual ~IPlanerApiClient() = default;

  virtual void checkHealth(SuccessHandler<HealthStatus> onSuccess,
                           ErrorHandler onError) = 0;

  virtual void registerUser(const RegisterRequest &request,
                            SuccessHandler<AuthResult> onSuccess,
                            ErrorHandler onError) = 0;
  virtual void login(const LoginRequest &request,
                     SuccessHandler<AuthResult> onSuccess,
                     ErrorHandler onError) = 0;
  virtual void getMe(SuccessHandler<User> onSuccess, ErrorHandler onError) = 0;

  virtual void listProjects(SuccessHandler<Paginated<Project>> onSuccess,
                            ErrorHandler onError) = 0;
  virtual void createProject(const CreateProjectRequest &request,
                             SuccessHandler<Project> onSuccess,
                             ErrorHandler onError) = 0;
  virtual void getProject(const QString &projectId,
                          SuccessHandler<Project> onSuccess,
                          ErrorHandler onError) = 0;
  virtual void updateProject(const QString &projectId,
                             const UpdateProjectRequest &request,
                             SuccessHandler<Project> onSuccess,
                             ErrorHandler onError) = 0;
  virtual void deleteProject(const QString &projectId, SuccessVoid onSuccess,
                             ErrorHandler onError) = 0;

  virtual void listMembers(const QString &projectId,
                           SuccessHandler<Paginated<ProjectMember>> onSuccess,
                           ErrorHandler onError) = 0;
  virtual void addMember(const QString &projectId,
                         const AddMemberRequest &request,
                         SuccessVoid onSuccess, ErrorHandler onError) = 0;
  virtual void removeMember(const QString &projectId, const QString &userId,
                            SuccessVoid onSuccess, ErrorHandler onError) = 0;

  virtual void listTasks(const QString &projectId,
                         SuccessHandler<Paginated<Task>> onSuccess,
                         ErrorHandler onError) = 0;
  virtual void createTask(const QString &projectId,
                          const CreateTaskRequest &request,
                          SuccessHandler<Task> onSuccess,
                          ErrorHandler onError) = 0;
  virtual void getTask(const QString &taskId, SuccessHandler<Task> onSuccess,
                       ErrorHandler onError) = 0;
  virtual void updateTask(const QString &taskId,
                          const UpdateTaskRequest &request,
                          SuccessHandler<Task> onSuccess,
                          ErrorHandler onError) = 0;
  virtual void deleteTask(const QString &taskId, SuccessVoid onSuccess,
                          ErrorHandler onError) = 0;

  virtual void listDependencies(const QString &taskId,
                                  SuccessHandler<Paginated<TaskDependency>>
                                      onSuccess,
                                  ErrorHandler onError) = 0;
  virtual void createDependency(const QString &blockerTaskId,
                                const QString &blockedTaskId,
                                SuccessHandler<TaskDependency> onSuccess,
                                ErrorHandler onError) = 0;
  virtual void deleteDependency(const QString &taskId,
                                const QString &dependencyId,
                                SuccessVoid onSuccess,
                                ErrorHandler onError) = 0;

  virtual void getGantt(const QString &projectId,
                        SuccessHandler<GanttData> onSuccess,
                        ErrorHandler onError) = 0;

  virtual void listAttachments(const QString &taskId,
                               SuccessHandler<Paginated<Attachment>> onSuccess,
                               ErrorHandler onError) = 0;
  virtual void uploadAttachment(const QString &taskId, const QString &fileName,
                                const QString &mimeType, qint64 sizeBytes,
                                SuccessHandler<Attachment> onSuccess,
                                ErrorHandler onError) = 0;
  virtual void deleteAttachment(const QString &taskId,
                                const QString &attachmentId,
                                SuccessVoid onSuccess,
                                ErrorHandler onError) = 0;
};
