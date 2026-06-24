#pragma once

#include <QDateTime>
#include <QString>

struct User {
  QString id;
  QString email;
  QString displayName;
  QDateTime createdAt;
};

struct UserRef {
  QString id;
  QString displayName;
};
