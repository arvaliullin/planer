#pragma once

#include "domain/entities/Project.hpp"
#include "domain/entities/Task.hpp"
#include "domain/entities/User.hpp"

#include <QVariantMap>

namespace EntityMapper {

QVariantMap userToVariant(const User &user);
QVariantMap projectToVariant(const Project &project);
QVariantMap taskToVariant(const Task &task);
QVariantMap dependencyToVariant(const TaskDependency &dependency);
QVariantMap attachmentToVariant(const Attachment &attachment);
QVariantMap ganttTaskToVariant(const GanttTask &task);
QVariantMap ganttDependencyToVariant(const GanttDependency &dependency);

Task taskFromVariant(const QVariantMap &map);
UpdateTaskRequest updateRequestFromVariant(const QVariantMap &map);

} // namespace EntityMapper
