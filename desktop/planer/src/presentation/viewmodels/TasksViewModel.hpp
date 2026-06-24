#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "presentation/viewmodels/AppViewModel.hpp"
#include "presentation/viewmodels/ProjectsViewModel.hpp"
#include "presentation/viewmodels/SessionViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class TasksViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList tasks READ tasks NOTIFY tasksChanged)
  Q_PROPERTY(QString selectedTaskId READ selectedTaskId NOTIFY selectedTaskChanged)
  Q_PROPERTY(QVariantMap selectedTask READ selectedTask NOTIFY selectedTaskChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(bool hasTasks READ hasTasks NOTIFY tasksChanged)

public:
  TasksViewModel(IPlanerApiClient &client, ProjectsViewModel &projects,
                 SessionViewModel &session, AppViewModel &app,
                 QObject *parent = nullptr);

  QVariantList tasks() const;
  QString selectedTaskId() const;
  QVariantMap selectedTask() const;
  bool loading() const;
  bool hasTasks() const;

  Q_INVOKABLE void loadTasks();
  Q_INVOKABLE void selectTask(const QString &taskId);
  Q_INVOKABLE void clearSelection();
  Q_INVOKABLE void createTask(const QString &title,
                              const QString &description = QString());
  Q_INVOKABLE void updateTaskStatus(const QString &taskId,
                                    const QString &status);
  Q_INVOKABLE void updateTaskDates(const QString &taskId, const QString &startDate,
                                   const QString &dueDate);

  void replaceTaskInList(const Task &task);

signals:
  void tasksChanged();
  void selectedTaskChanged();
  void loadingChanged();
  void taskUpdated(const QVariantMap &task);

private:
  void setLoading(bool loading);
  void applyTaskSelection();

  IPlanerApiClient &client_;
  ProjectsViewModel &projects_;
  SessionViewModel &session_;
  AppViewModel &app_;
  QVariantList tasks_;
  QString selectedTaskId_;
  QVariantMap selectedTask_;
  bool loading_ = false;
};
