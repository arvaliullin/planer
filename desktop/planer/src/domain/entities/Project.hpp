#pragma once

#include "domain/entities/Enums.hpp"

#include <QDateTime>
#include <QString>

struct Project {
  QString id;
  QString key;
  QString name;
  QString description;
  QString ownerId;
  ProjectRole role = ProjectRole::Member;
  QDateTime createdAt;
};

struct ProjectMember {
  QString userId;
  QString email;
  QString displayName;
  ProjectRole role = ProjectRole::Member;
};

struct CreateProjectRequest {
  QString key;
  QString name;
  QString description;
};

struct UpdateProjectRequest {
  QString name;
  QString description;
};

struct AddMemberRequest {
  QString email;
  ProjectRole role = ProjectRole::Member;
};
