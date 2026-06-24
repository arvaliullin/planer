#pragma once

#include "domain/entities/HealthStatus.hpp"

#include <functional>

#include <QString>

// IHealthRepository проверяет доступность backend.
class IHealthRepository {
public:
  virtual ~IHealthRepository() = default;

  virtual void checkHealth(std::function<void(HealthStatus)> onSuccess,
                           std::function<void(QString)> onError) = 0;
};
