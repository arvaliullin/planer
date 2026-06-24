#include "domain/entities/Enums.hpp"

QString taskStatusToString(TaskStatus status) {
  switch (status) {
  case TaskStatus::Backlog:
    return QStringLiteral("backlog");
  case TaskStatus::Todo:
    return QStringLiteral("todo");
  case TaskStatus::InProgress:
    return QStringLiteral("in_progress");
  case TaskStatus::Review:
    return QStringLiteral("review");
  case TaskStatus::Done:
    return QStringLiteral("done");
  case TaskStatus::Cancelled:
    return QStringLiteral("cancelled");
  }
  return QStringLiteral("backlog");
}

TaskStatus taskStatusFromString(const QString &value) {
  if (value == QLatin1String("todo")) {
    return TaskStatus::Todo;
  }
  if (value == QLatin1String("in_progress")) {
    return TaskStatus::InProgress;
  }
  if (value == QLatin1String("review")) {
    return TaskStatus::Review;
  }
  if (value == QLatin1String("done")) {
    return TaskStatus::Done;
  }
  if (value == QLatin1String("cancelled")) {
    return TaskStatus::Cancelled;
  }
  return TaskStatus::Backlog;
}

QString taskPriorityToString(TaskPriority priority) {
  switch (priority) {
  case TaskPriority::Low:
    return QStringLiteral("low");
  case TaskPriority::Normal:
    return QStringLiteral("normal");
  case TaskPriority::High:
    return QStringLiteral("high");
  case TaskPriority::Critical:
    return QStringLiteral("critical");
  }
  return QStringLiteral("normal");
}

TaskPriority taskPriorityFromString(const QString &value) {
  if (value == QLatin1String("low")) {
    return TaskPriority::Low;
  }
  if (value == QLatin1String("high")) {
    return TaskPriority::High;
  }
  if (value == QLatin1String("critical")) {
    return TaskPriority::Critical;
  }
  return TaskPriority::Normal;
}

QString taskTypeToString(TaskType type) {
  return type == TaskType::Bug ? QStringLiteral("bug") : QStringLiteral("task");
}

TaskType taskTypeFromString(const QString &value) {
  return value == QLatin1String("bug") ? TaskType::Bug : TaskType::Task;
}

QString projectRoleToString(ProjectRole role) {
  switch (role) {
  case ProjectRole::Admin:
    return QStringLiteral("admin");
  case ProjectRole::Member:
    return QStringLiteral("member");
  case ProjectRole::Viewer:
    return QStringLiteral("viewer");
  }
  return QStringLiteral("member");
}

ProjectRole projectRoleFromString(const QString &value) {
  if (value == QLatin1String("admin")) {
    return ProjectRole::Admin;
  }
  if (value == QLatin1String("viewer")) {
    return ProjectRole::Viewer;
  }
  return ProjectRole::Member;
}

QString taskStatusLabel(TaskStatus status) {
  switch (status) {
  case TaskStatus::Backlog:
    return QStringLiteral("Backlog");
  case TaskStatus::Todo:
    return QStringLiteral("Todo");
  case TaskStatus::InProgress:
    return QStringLiteral("In Progress");
  case TaskStatus::Review:
    return QStringLiteral("Review");
  case TaskStatus::Done:
    return QStringLiteral("Done");
  case TaskStatus::Cancelled:
    return QStringLiteral("Cancelled");
  }
  return QStringLiteral("Backlog");
}

QString taskPriorityLabel(TaskPriority priority) {
  switch (priority) {
  case TaskPriority::Low:
    return QStringLiteral("Low");
  case TaskPriority::Normal:
    return QStringLiteral("Normal");
  case TaskPriority::High:
    return QStringLiteral("High");
  case TaskPriority::Critical:
    return QStringLiteral("Critical");
  }
  return QStringLiteral("Normal");
}

int ganttProgressForStatus(TaskStatus status) {
  switch (status) {
  case TaskStatus::InProgress:
    return 50;
  case TaskStatus::Review:
    return 75;
  case TaskStatus::Done:
    return 100;
  default:
    return 0;
  }
}
