#include "data/mappers/HealthMapper.hpp"

namespace {

std::optional<ComponentHealth>
mapComponent(const std::optional<HealthComponentDto> &dto) {
  if (!dto.has_value()) {
    return std::nullopt;
  }

  ComponentHealth component;
  component.ok = dto->ok;
  component.error = dto->error;
  return component;
}

} // namespace

HealthStatus HealthMapper::toDomain(const HealthDto &dto) {
  HealthStatus status;
  status.postgres = mapComponent(dto.postgres);
  status.minio = mapComponent(dto.minio);
  return status;
}
