#pragma once

#include <optional>

#include <QString>

struct ComponentHealth {
  bool ok = false;
  QString error;
};

struct HealthStatus {
  std::optional<ComponentHealth> postgres;
  std::optional<ComponentHealth> minio;

  bool allOk() const;
};
