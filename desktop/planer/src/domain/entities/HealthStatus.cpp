#include "domain/entities/HealthStatus.hpp"

bool HealthStatus::allOk() const {
  auto componentOk = [](const std::optional<ComponentHealth> &component) {
    return !component.has_value() || component->ok;
  };
  return componentOk(postgres) && componentOk(minio);
}
