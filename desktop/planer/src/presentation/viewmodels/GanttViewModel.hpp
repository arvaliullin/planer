#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "presentation/viewmodels/AppViewModel.hpp"
#include "presentation/viewmodels/ProjectsViewModel.hpp"
#include "presentation/viewmodels/SessionViewModel.hpp"
#include "presentation/viewmodels/TasksViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantList>

class GanttViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList scheduledTasks READ scheduledTasks NOTIFY dataChanged)
  Q_PROPERTY(QVariantList unscheduledTasks READ unscheduledTasks NOTIFY dataChanged)
  Q_PROPERTY(QVariantList dependencies READ dependencies NOTIFY dataChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(int scale READ scale WRITE setScale NOTIFY scaleChanged)
  Q_PROPERTY(QString highlightedTaskId READ highlightedTaskId NOTIFY
                 highlightedTaskIdChanged)

public:
  GanttViewModel(IPlanerApiClient &client, ProjectsViewModel &projects,
                 TasksViewModel &tasks, SessionViewModel &session,
                 AppViewModel &app, QObject *parent = nullptr);

  QVariantList scheduledTasks() const;
  QVariantList unscheduledTasks() const;
  QVariantList dependencies() const;
  bool loading() const;
  int scale() const;
  QString highlightedTaskId() const;

  Q_INVOKABLE void loadGantt();
  Q_INVOKABLE void setScale(int scale);
  Q_INVOKABLE void highlightDependency(const QString &blockerId,
                                       const QString &blockedId);
  Q_INVOKABLE void clearHighlight();
  Q_INVOKABLE void moveTaskDates(const QString &taskId, const QString &startDate,
                                 const QString &dueDate);

signals:
  void dataChanged();
  void loadingChanged();
  void scaleChanged();
  void highlightedTaskIdChanged();

private:
  void setLoading(bool loading);
  void splitTasks(const QList<GanttTask> &tasks);

  IPlanerApiClient &client_;
  ProjectsViewModel &projects_;
  TasksViewModel &tasks_;
  SessionViewModel &session_;
  AppViewModel &app_;
  QVariantList scheduledTasks_;
  QVariantList unscheduledTasks_;
  QVariantList dependencies_;
  bool loading_ = false;
  int scale_ = 0;
  QString highlightedTaskId_;
};
