#pragma once

#include "domain/entities/HealthStatus.hpp"
#include "domain/ports/IHealthRepository.hpp"

#include <functional>

#include <QString>

// CheckHealthUseCase запрашивает статус /health.
class CheckHealthUseCase {
public:
  explicit CheckHealthUseCase(IHealthRepository &repository);

  void execute(std::function<void(HealthStatus)> onSuccess,
               std::function<void(QString)> onError);

private:
  IHealthRepository &repository_;
};
