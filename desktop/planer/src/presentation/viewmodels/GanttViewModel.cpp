#include "presentation/viewmodels/GanttViewModel.hpp"

#include "presentation/mappers/EntityMapper.hpp"

GanttViewModel::GanttViewModel(IPlanerApiClient &client,
                               ProjectsViewModel &projects, TasksViewModel &tasks,
                               SessionViewModel &session, AppViewModel &app,
                               QObject *parent)
    : QObject(parent), client_(client), projects_(projects), tasks_(tasks),
      session_(session), app_(app) {
  connect(&projects_, &ProjectsViewModel::projectSelected, this,
          &GanttViewModel::loadGantt);
}

QVariantList GanttViewModel::scheduledTasks() const { return scheduledTasks_; }

QVariantList GanttViewModel::unscheduledTasks() const {
  return unscheduledTasks_;
}

QVariantList GanttViewModel::dependencies() const { return dependencies_; }

bool GanttViewModel::loading() const { return loading_; }

int GanttViewModel::scale() const { return scale_; }

QString GanttViewModel::highlightedTaskId() const { return highlightedTaskId_; }

void GanttViewModel::loadGantt() {
  const QString projectId =
      projects_.activeProject().value(QStringLiteral("id")).toString();
  if (projectId.isEmpty()) {
    scheduledTasks_.clear();
    unscheduledTasks_.clear();
    dependencies_.clear();
    emit dataChanged();
    return;
  }
  if (loading_) {
    return;
  }
  setLoading(true);
  client_.getGantt(
      projectId,
      [this](const GanttData &data) {
        setLoading(false);
        splitTasks(data.tasks);
        dependencies_.clear();
        for (const GanttDependency &dep : data.dependencies) {
          dependencies_.append(EntityMapper::ganttDependencyToVariant(dep));
        }
        emit dataChanged();
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void GanttViewModel::setScale(int scale) {
  if (scale_ == scale) {
    return;
  }
  scale_ = scale;
  emit scaleChanged();
}

void GanttViewModel::highlightDependency(const QString &blockerId,
                                         const QString &blockedId) {
  Q_UNUSED(blockedId);
  highlightedTaskId_ = blockerId;
  emit highlightedTaskIdChanged();
  tasks_.selectTask(blockerId);
}

void GanttViewModel::clearHighlight() {
  if (highlightedTaskId_.isEmpty()) {
    return;
  }
  highlightedTaskId_.clear();
  emit highlightedTaskIdChanged();
}

void GanttViewModel::moveTaskDates(const QString &taskId,
                                   const QString &startDate,
                                   const QString &dueDate) {
  tasks_.updateTaskDates(taskId, startDate, dueDate);
  loadGantt();
}

void GanttViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void GanttViewModel::splitTasks(const QList<GanttTask> &tasks) {
  scheduledTasks_.clear();
  unscheduledTasks_.clear();
  for (const GanttTask &task : tasks) {
    const QVariantMap variant = EntityMapper::ganttTaskToVariant(task);
    if (variant.value(QStringLiteral("hasDates")).toBool()) {
      scheduledTasks_.append(variant);
    } else {
      unscheduledTasks_.append(variant);
    }
  }
}
