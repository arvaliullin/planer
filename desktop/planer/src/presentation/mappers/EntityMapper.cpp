#include "presentation/mappers/EntityMapper.hpp"

#include "domain/entities/Enums.hpp"

#include <QDate>

namespace EntityMapper {

QVariantMap userToVariant(const User &user) {
  return {{"id", user.id},
          {"email", user.email},
          {"displayName", user.displayName},
          {"createdAt", user.createdAt.toString(Qt::ISODate)}};
}

QVariantMap projectToVariant(const Project &project) {
  return {{"id", project.id},
          {"key", project.key},
          {"name", project.name},
          {"description", project.description},
          {"ownerId", project.ownerId},
          {"role", projectRoleToString(project.role)},
          {"createdAt", project.createdAt.toString(Qt::ISODate)}};
}

QVariantMap taskToVariant(const Task &task) {
  QVariantMap map;
  map.insert(QStringLiteral("id"), task.id);
  map.insert(QStringLiteral("key"), task.key);
  map.insert(QStringLiteral("projectId"), task.projectId);
  map.insert(QStringLiteral("number"), task.number);
  map.insert(QStringLiteral("title"), task.title);
  map.insert(QStringLiteral("description"), task.description);
  map.insert(QStringLiteral("type"), taskTypeToString(task.type));
  map.insert(QStringLiteral("status"), taskStatusToString(task.status));
  map.insert(QStringLiteral("statusLabel"), taskStatusLabel(task.status));
  map.insert(QStringLiteral("priority"), taskPriorityToString(task.priority));
  map.insert(QStringLiteral("priorityLabel"), taskPriorityLabel(task.priority));
  map.insert(QStringLiteral("reporterId"), task.reporter.id);
  map.insert(QStringLiteral("reporterName"), task.reporter.displayName);
  if (task.assignee) {
    map.insert(QStringLiteral("assigneeId"), task.assignee->id);
    map.insert(QStringLiteral("assigneeName"), task.assignee->displayName);
  } else {
    map.insert(QStringLiteral("assigneeId"), QString());
    map.insert(QStringLiteral("assigneeName"), QString());
  }
  map.insert(QStringLiteral("startDate"),
             task.startDate ? task.startDate->toString(Qt::ISODate) : QString());
  map.insert(QStringLiteral("dueDate"),
             task.dueDate ? task.dueDate->toString(Qt::ISODate) : QString());
  map.insert(QStringLiteral("createdAt"), task.createdAt.toString(Qt::ISODate));
  map.insert(QStringLiteral("updatedAt"), task.updatedAt.toString(Qt::ISODate));
  return map;
}

QVariantMap dependencyToVariant(const TaskDependency &dependency) {
  return {{"id", dependency.id},
          {"blockerTaskId", dependency.blockerTaskId},
          {"blockerKey", dependency.blockerKey},
          {"blockedTaskId", dependency.blockedTaskId},
          {"blockedKey", dependency.blockedKey}};
}

QVariantMap attachmentToVariant(const Attachment &attachment) {
  return {{"id", attachment.id},
          {"taskId", attachment.taskId},
          {"fileName", attachment.fileName},
          {"mimeType", attachment.mimeType},
          {"sizeBytes", attachment.sizeBytes},
          {"uploadedAt", attachment.uploadedAt.toString(Qt::ISODate)}};
}

QVariantMap ganttTaskToVariant(const GanttTask &task) {
  QVariantMap map;
  map.insert(QStringLiteral("id"), task.id);
  map.insert(QStringLiteral("key"), task.key);
  map.insert(QStringLiteral("title"), task.title);
  map.insert(QStringLiteral("status"), taskStatusToString(task.status));
  map.insert(QStringLiteral("progress"), task.progress);
  if (task.assignee) {
    map.insert(QStringLiteral("assigneeName"), task.assignee->displayName);
  }
  map.insert(QStringLiteral("startDate"),
             task.startDate ? task.startDate->toString(Qt::ISODate) : QString());
  map.insert(QStringLiteral("dueDate"),
             task.dueDate ? task.dueDate->toString(Qt::ISODate) : QString());
  map.insert(QStringLiteral("hasDates"),
             task.startDate.has_value() || task.dueDate.has_value());
  return map;
}

QVariantMap ganttDependencyToVariant(const GanttDependency &dependency) {
  return {{"id", dependency.id},
          {"blockerId", dependency.blockerId},
          {"blockedId", dependency.blockedId}};
}

UpdateTaskRequest updateRequestFromVariant(const QVariantMap &map) {
  UpdateTaskRequest request;
  if (map.contains(QStringLiteral("title"))) {
    request.title = map.value(QStringLiteral("title")).toString();
  }
  if (map.contains(QStringLiteral("description"))) {
    request.description = map.value(QStringLiteral("description")).toString();
  }
  if (map.contains(QStringLiteral("type"))) {
    request.type = taskTypeFromString(map.value(QStringLiteral("type")).toString());
  }
  if (map.contains(QStringLiteral("status"))) {
    request.status =
        taskStatusFromString(map.value(QStringLiteral("status")).toString());
  }
  if (map.contains(QStringLiteral("priority"))) {
    request.priority = taskPriorityFromString(
        map.value(QStringLiteral("priority")).toString());
  }
  if (map.contains(QStringLiteral("assigneeId"))) {
    const QString assigneeId = map.value(QStringLiteral("assigneeId")).toString();
    if (assigneeId.isEmpty()) {
      request.clearAssignee = true;
    } else {
      request.assigneeId = assigneeId;
    }
  }
  if (map.contains(QStringLiteral("startDate"))) {
    const QString value = map.value(QStringLiteral("startDate")).toString();
    if (value.isEmpty()) {
      request.clearStartDate = true;
    } else {
      request.startDate = QDate::fromString(value, Qt::ISODate);
    }
  }
  if (map.contains(QStringLiteral("dueDate"))) {
    const QString value = map.value(QStringLiteral("dueDate")).toString();
    if (value.isEmpty()) {
      request.clearDueDate = true;
    } else {
      request.dueDate = QDate::fromString(value, Qt::ISODate);
    }
  }
  return request;
}

Task taskFromVariant(const QVariantMap &map) {
  Task task;
  task.id = map.value(QStringLiteral("id")).toString();
  task.key = map.value(QStringLiteral("key")).toString();
  task.projectId = map.value(QStringLiteral("projectId")).toString();
  task.number = map.value(QStringLiteral("number")).toInt();
  task.title = map.value(QStringLiteral("title")).toString();
  task.description = map.value(QStringLiteral("description")).toString();
  task.type = taskTypeFromString(map.value(QStringLiteral("type")).toString());
  task.status =
      taskStatusFromString(map.value(QStringLiteral("status")).toString());
  task.priority =
      taskPriorityFromString(map.value(QStringLiteral("priority")).toString());
  const QString assigneeId = map.value(QStringLiteral("assigneeId")).toString();
  if (!assigneeId.isEmpty()) {
    task.assignee = UserRef{assigneeId,
                            map.value(QStringLiteral("assigneeName")).toString()};
  }
  const QString startDate = map.value(QStringLiteral("startDate")).toString();
  if (!startDate.isEmpty()) {
    task.startDate = QDate::fromString(startDate, Qt::ISODate);
  }
  const QString dueDate = map.value(QStringLiteral("dueDate")).toString();
  if (!dueDate.isEmpty()) {
    task.dueDate = QDate::fromString(dueDate, Qt::ISODate);
  }
  return task;
}

} // namespace EntityMapper
