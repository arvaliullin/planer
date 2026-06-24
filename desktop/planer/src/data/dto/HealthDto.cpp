#include "data/dto/HealthDto.hpp"

namespace {

std::optional<HealthComponentDto> parseComponent(const QJsonObject &object,
                                                 const char *key) {
  if (!object.contains(QLatin1String(key)) ||
      !object.value(QLatin1String(key)).isObject()) {
    return std::nullopt;
  }

  const QJsonObject componentObject =
      object.value(QLatin1String(key)).toObject();
  HealthComponentDto component;
  component.ok = componentObject.value(QStringLiteral("ok")).toBool(false);
  component.error = componentObject.value(QStringLiteral("error")).toString();
  return component;
}

} // namespace

HealthDto HealthDto::fromJson(const QJsonObject &object) {
  HealthDto dto;
  dto.postgres = parseComponent(object, "postgres");
  dto.minio = parseComponent(object, "minio");
  return dto;
}
