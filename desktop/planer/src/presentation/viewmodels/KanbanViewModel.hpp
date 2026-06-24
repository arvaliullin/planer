#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "presentation/viewmodels/TasksViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantList>

class KanbanViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList columns READ columns NOTIFY columnsChanged)

public:
  explicit KanbanViewModel(TasksViewModel &tasks, QObject *parent = nullptr);

  QVariantList columns() const;

  Q_INVOKABLE void refresh();
  Q_INVOKABLE void moveTask(const QString &taskId, const QString &newStatus);

signals:
  void columnsChanged();

private:
  void rebuildColumns();

  TasksViewModel &tasks_;
  QVariantList columns_;
};
