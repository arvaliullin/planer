#pragma once

#include "domain/entities/Enums.hpp"
#include "domain/entities/User.hpp"

#include <optional>

#include <QDate>
#include <QDateTime>
#include <QString>

struct Task {
  QString id;
  QString key;
  QString projectId;
  int number = 0;
  QString title;
  QString description;
  TaskType type = TaskType::Task;
  TaskStatus status = TaskStatus::Backlog;
  TaskPriority priority = TaskPriority::Normal;
  std::optional<UserRef> assignee;
  UserRef reporter;
  std::optional<QDate> startDate;
  std::optional<QDate> dueDate;
  QDateTime createdAt;
  QDateTime updatedAt;
};

struct CreateTaskRequest {
  QString title;
  QString description;
  TaskType type = TaskType::Task;
  TaskPriority priority = TaskPriority::Normal;
  std::optional<QString> assigneeId;
  std::optional<QDate> startDate;
  std::optional<QDate> dueDate;
};

struct UpdateTaskRequest {
  std::optional<QString> title;
  std::optional<QString> description;
  std::optional<TaskType> type;
  std::optional<TaskStatus> status;
  std::optional<TaskPriority> priority;
  std::optional<QString> assigneeId;
  std::optional<bool> clearAssignee = false;
  std::optional<QDate> startDate;
  std::optional<QDate> dueDate;
  std::optional<bool> clearStartDate = false;
  std::optional<bool> clearDueDate = false;
};

struct TaskDependency {
  QString id;
  QString blockerTaskId;
  QString blockerKey;
  QString blockedTaskId;
  QString blockedKey;
};

struct Attachment {
  QString id;
  QString taskId;
  QString fileName;
  QString mimeType;
  qint64 sizeBytes = 0;
  QDateTime uploadedAt;
};

struct GanttTask {
  QString id;
  QString key;
  QString title;
  TaskStatus status = TaskStatus::Backlog;
  std::optional<UserRef> assignee;
  std::optional<QDate> startDate;
  std::optional<QDate> dueDate;
  int progress = 0;
};

struct GanttDependency {
  QString id;
  QString blockerId;
  QString blockedId;
};

struct GanttData {
  QString projectId;
  QList<GanttTask> tasks;
  QList<GanttDependency> dependencies;
};
