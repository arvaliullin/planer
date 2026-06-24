#pragma once

#include "domain/entities/HealthStatus.hpp"
#include "domain/ports/IPlanerApiClient.hpp"

#include "core/ApiError.hpp"

#include <functional>

class CheckHealthUseCase {
public:
  explicit CheckHealthUseCase(IPlanerApiClient &client);

  void execute(std::function<void(HealthStatus)> onSuccess,
               std::function<void(ApiError)> onError);

private:
  IPlanerApiClient &client_;
};
