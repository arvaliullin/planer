#pragma once

#include <optional>

#include <QJsonObject>
#include <QString>

struct HealthComponentDto {
  bool ok = false;
  QString error;
};

struct HealthDto {
  std::optional<HealthComponentDto> postgres;
  std::optional<HealthComponentDto> minio;

  static HealthDto fromJson(const QJsonObject &object);
};
