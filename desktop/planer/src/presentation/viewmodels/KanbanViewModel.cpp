#include "presentation/viewmodels/KanbanViewModel.hpp"

#include "domain/entities/Enums.hpp"

namespace {

QVariantList statusOrder() {
  return {QStringLiteral("backlog"), QStringLiteral("todo"),
          QStringLiteral("in_progress"), QStringLiteral("review"),
          QStringLiteral("done")};
}

} // namespace

KanbanViewModel::KanbanViewModel(TasksViewModel &tasks, QObject *parent)
    : QObject(parent), tasks_(tasks) {
  connect(&tasks_, &TasksViewModel::tasksChanged, this,
          &KanbanViewModel::refresh);
  refresh();
}

QVariantList KanbanViewModel::columns() const { return columns_; }

void KanbanViewModel::refresh() { rebuildColumns(); }

void KanbanViewModel::moveTask(const QString &taskId, const QString &newStatus) {
  tasks_.updateTaskStatus(taskId, newStatus);
}

void KanbanViewModel::rebuildColumns() {
  columns_.clear();
  const QVariantList order = statusOrder();
  const QVariantList allTasks = tasks_.tasks();

  for (const QVariant &statusVariant : order) {
    const QString status = statusVariant.toString();
    QVariantList cards;
    for (const QVariant &taskVariant : allTasks) {
      const QVariantMap task = taskVariant.toMap();
      if (task.value(QStringLiteral("status")).toString() == status) {
        cards.append(task);
      }
    }
    columns_.append(QVariantMap{
        {QStringLiteral("status"), status},
        {QStringLiteral("title"),
         taskStatusLabel(taskStatusFromString(status))},
        {QStringLiteral("cards"), cards},
    });
  }
  emit columnsChanged();
}
