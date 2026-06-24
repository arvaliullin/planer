#include "presentation/viewmodels/TaskDetailViewModel.hpp"

#include "presentation/mappers/EntityMapper.hpp"

TaskDetailViewModel::TaskDetailViewModel(IPlanerApiClient &client,
                                         TasksViewModel &tasks,
                                         SessionViewModel &session,
                                         AppViewModel &app, QObject *parent)
    : QObject(parent), client_(client), tasks_(tasks), session_(session),
      app_(app) {
  connect(&tasks_, &TasksViewModel::selectedTaskChanged, this,
          [this]() {
            refreshFromTasks();
            loadDetails();
          });
  connect(&tasks_, &TasksViewModel::taskUpdated, this,
          [this](const QVariantMap &task) {
            if (task.value(QStringLiteral("id")).toString() ==
                task_.value(QStringLiteral("id")).toString()) {
              task_ = task;
              emit taskChanged();
            }
          });
}

QVariantMap TaskDetailViewModel::task() const { return task_; }

QVariantList TaskDetailViewModel::dependencies() const { return dependencies_; }

QVariantList TaskDetailViewModel::attachments() const { return attachments_; }

bool TaskDetailViewModel::loading() const { return loading_; }

void TaskDetailViewModel::loadDetails() {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    dependencies_.clear();
    attachments_.clear();
    emit dependenciesChanged();
    emit attachmentsChanged();
    return;
  }

  setLoading(true);
  client_.listDependencies(
      taskId,
      [this](const Paginated<TaskDependency> &page) {
        dependencies_.clear();
        for (const TaskDependency &dep : page.items) {
          dependencies_.append(EntityMapper::dependencyToVariant(dep));
        }
        emit dependenciesChanged();

        client_.listAttachments(
            tasks_.selectedTaskId(),
            [this](const Paginated<Attachment> &attachmentsPage) {
              setLoading(false);
              attachments_.clear();
              for (const Attachment &attachment : attachmentsPage.items) {
                attachments_.append(EntityMapper::attachmentToVariant(attachment));
              }
              emit attachmentsChanged();
            },
            [this](const ApiError &error) {
              setLoading(false);
              session_.handleApiError(error);
            });
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::saveField(const QString &field, const QVariant &value) {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    return;
  }

  QVariantMap patch;
  patch.insert(field, value);

  UpdateTaskRequest request = EntityMapper::updateRequestFromVariant(patch);
  setLoading(true);
  client_.updateTask(
      taskId, request,
      [this](const Task &task) {
        setLoading(false);
        tasks_.replaceTaskInList(task);
        task_ = EntityMapper::taskToVariant(task);
        emit taskChanged();
        app_.showToast(QStringLiteral("Изменения сохранены"));
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::addDependency(const QString &blockedTaskId) {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    return;
  }
  setLoading(true);
  client_.createDependency(
      taskId, blockedTaskId,
      [this](const TaskDependency &dependency) {
        setLoading(false);
        dependencies_.append(EntityMapper::dependencyToVariant(dependency));
        emit dependenciesChanged();
        app_.showToast(QStringLiteral("Зависимость добавлена"));
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::removeDependency(const QString &dependencyId) {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    return;
  }
  setLoading(true);
  client_.deleteDependency(
      taskId, dependencyId,
      [this, dependencyId]() {
        setLoading(false);
        for (int i = 0; i < dependencies_.size(); ++i) {
          if (dependencies_.at(i).toMap().value(QStringLiteral("id")).toString() ==
              dependencyId) {
            dependencies_.removeAt(i);
            break;
          }
        }
        emit dependenciesChanged();
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::uploadAttachment(const QString &fileName,
                                           const QString &mimeType,
                                           qint64 sizeBytes) {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    return;
  }
  setLoading(true);
  client_.uploadAttachment(
      taskId, fileName, mimeType, sizeBytes,
      [this](const Attachment &attachment) {
        setLoading(false);
        attachments_.append(EntityMapper::attachmentToVariant(attachment));
        emit attachmentsChanged();
        app_.showToast(QStringLiteral("Файл загружен"));
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::deleteAttachment(const QString &attachmentId) {
  const QString taskId = tasks_.selectedTaskId();
  if (taskId.isEmpty()) {
    return;
  }
  setLoading(true);
  client_.deleteAttachment(
      taskId, attachmentId,
      [this, attachmentId]() {
        setLoading(false);
        for (int i = 0; i < attachments_.size(); ++i) {
          if (attachments_.at(i).toMap().value(QStringLiteral("id")).toString() ==
              attachmentId) {
            attachments_.removeAt(i);
            break;
          }
        }
        emit attachmentsChanged();
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TaskDetailViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void TaskDetailViewModel::refreshFromTasks() {
  task_ = tasks_.selectedTask();
  emit taskChanged();
}
