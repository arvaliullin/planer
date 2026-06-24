#include "presentation/viewmodels/TasksViewModel.hpp"

#include "domain/entities/Enums.hpp"
#include "presentation/mappers/EntityMapper.hpp"

#include <QDate>

TasksViewModel::TasksViewModel(IPlanerApiClient &client, ProjectsViewModel &projects,
                             SessionViewModel &session, AppViewModel &app,
                             QObject *parent)
    : QObject(parent), client_(client), projects_(projects), session_(session),
      app_(app) {
  connect(&projects_, &ProjectsViewModel::projectSelected, this,
          [this](const QString &) { loadTasks(); });
}

QVariantList TasksViewModel::tasks() const { return tasks_; }

QString TasksViewModel::selectedTaskId() const { return selectedTaskId_; }

QVariantMap TasksViewModel::selectedTask() const { return selectedTask_; }

bool TasksViewModel::loading() const { return loading_; }

bool TasksViewModel::hasTasks() const { return !tasks_.isEmpty(); }

void TasksViewModel::loadTasks() {
  const QString projectId =
      projects_.activeProject().value(QStringLiteral("id")).toString();
  if (projectId.isEmpty()) {
    tasks_.clear();
    emit tasksChanged();
    return;
  }
  if (loading_) {
    return;
  }
  setLoading(true);
  client_.listTasks(
      projectId,
      [this](const Paginated<Task> &page) {
        setLoading(false);
        tasks_.clear();
        for (const Task &task : page.items) {
          tasks_.append(EntityMapper::taskToVariant(task));
        }
        emit tasksChanged();
        applyTaskSelection();
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TasksViewModel::selectTask(const QString &taskId) {
  selectedTaskId_ = taskId;
  applyTaskSelection();
  emit selectedTaskChanged();
}

void TasksViewModel::clearSelection() {
  if (selectedTaskId_.isEmpty()) {
    return;
  }
  selectedTaskId_.clear();
  selectedTask_ = {};
  emit selectedTaskChanged();
}

void TasksViewModel::createTask(const QString &title,
                                const QString &description) {
  const QString projectId =
      projects_.activeProject().value(QStringLiteral("id")).toString();
  if (projectId.isEmpty() || loading_) {
    return;
  }
  setLoading(true);
  CreateTaskRequest request;
  request.title = title;
  request.description = description;
  client_.createTask(
      projectId, request,
      [this](const Task &task) {
        setLoading(false);
        tasks_.append(EntityMapper::taskToVariant(task));
        emit tasksChanged();
        selectTask(task.id);
        app_.setCreateTaskDialogOpen(false);
        app_.showToast(QStringLiteral("Задача создана"));
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void TasksViewModel::updateTaskStatus(const QString &taskId,
                                      const QString &status) {
  QVariantMap previous;
  int index = -1;
  for (int i = 0; i < tasks_.size(); ++i) {
    const QVariantMap map = tasks_.at(i).toMap();
    if (map.value(QStringLiteral("id")).toString() == taskId) {
      previous = map;
      index = i;
      break;
    }
  }
  if (index < 0) {
    return;
  }

  QVariantMap optimistic = previous;
  optimistic.insert(QStringLiteral("status"), status);
  optimistic.insert(QStringLiteral("statusLabel"),
                    taskStatusLabel(taskStatusFromString(status)));
  tasks_[index] = optimistic;
  emit tasksChanged();
  if (selectedTaskId_ == taskId) {
    selectedTask_ = optimistic;
    emit selectedTaskChanged();
  }

  UpdateTaskRequest request;
  request.status = taskStatusFromString(status);
  client_.updateTask(
      taskId, request,
      [this, taskId](const Task &task) {
        replaceTaskInList(task);
        emit taskUpdated(EntityMapper::taskToVariant(task));
      },
      [this, taskId, previous, index](const ApiError &error) {
        tasks_[index] = previous;
        emit tasksChanged();
        if (selectedTaskId_ == taskId) {
          selectedTask_ = previous;
          emit selectedTaskChanged();
        }
        session_.handleApiError(error);
      });
}

void TasksViewModel::updateTaskDates(const QString &taskId,
                                     const QString &startDate,
                                     const QString &dueDate) {
  QVariantMap previous;
  int index = -1;
  for (int i = 0; i < tasks_.size(); ++i) {
    const QVariantMap map = tasks_.at(i).toMap();
    if (map.value(QStringLiteral("id")).toString() == taskId) {
      previous = map;
      index = i;
      break;
    }
  }
  if (index < 0) {
    return;
  }

  QVariantMap optimistic = previous;
  optimistic.insert(QStringLiteral("startDate"), startDate);
  optimistic.insert(QStringLiteral("dueDate"), dueDate);
  tasks_[index] = optimistic;
  emit tasksChanged();

  UpdateTaskRequest request;
  request.startDate = QDate::fromString(startDate, Qt::ISODate);
  request.dueDate = QDate::fromString(dueDate, Qt::ISODate);
  client_.updateTask(
      taskId, request,
      [this](const Task &task) { replaceTaskInList(task); },
      [this, previous, index, taskId](const ApiError &error) {
        tasks_[index] = previous;
        emit tasksChanged();
        if (selectedTaskId_ == taskId) {
          selectedTask_ = previous;
          emit selectedTaskChanged();
        }
        session_.handleApiError(error);
      });
}

void TasksViewModel::replaceTaskInList(const Task &task) {
  const QVariantMap variant = EntityMapper::taskToVariant(task);
  for (int i = 0; i < tasks_.size(); ++i) {
    const QVariantMap map = tasks_.at(i).toMap();
    if (map.value(QStringLiteral("id")).toString() == task.id) {
      tasks_[i] = variant;
      emit tasksChanged();
      if (selectedTaskId_ == task.id) {
        selectedTask_ = variant;
        emit selectedTaskChanged();
      }
      return;
    }
  }
}

void TasksViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void TasksViewModel::applyTaskSelection() {
  if (selectedTaskId_.isEmpty()) {
    selectedTask_ = {};
    return;
  }
  for (const QVariant &item : tasks_) {
    const QVariantMap map = item.toMap();
    if (map.value(QStringLiteral("id")).toString() == selectedTaskId_) {
      selectedTask_ = map;
      return;
    }
  }
  selectedTaskId_.clear();
  selectedTask_ = {};
}
