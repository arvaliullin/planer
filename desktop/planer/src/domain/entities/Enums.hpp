#pragma once

#include <QString>

enum class TaskStatus {
  Backlog,
  Todo,
  InProgress,
  Review,
  Done,
  Cancelled,
};

enum class TaskPriority { Low, Normal, High, Critical };

enum class TaskType { Task, Bug };

enum class ProjectRole { Admin, Member, Viewer };

QString taskStatusToString(TaskStatus status);
TaskStatus taskStatusFromString(const QString &value);

QString taskPriorityToString(TaskPriority priority);
TaskPriority taskPriorityFromString(const QString &value);

QString taskTypeToString(TaskType type);
TaskType taskTypeFromString(const QString &value);

QString projectRoleToString(ProjectRole role);
ProjectRole projectRoleFromString(const QString &value);

QString taskStatusLabel(TaskStatus status);
QString taskPriorityLabel(TaskPriority priority);

int ganttProgressForStatus(TaskStatus status);
