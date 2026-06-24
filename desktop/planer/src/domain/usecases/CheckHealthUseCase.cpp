#include "domain/usecases/CheckHealthUseCase.hpp"

CheckHealthUseCase::CheckHealthUseCase(IPlanerApiClient &client) : client_(client) {}

void CheckHealthUseCase::execute(std::function<void(HealthStatus)> onSuccess,
                                 std::function<void(ApiError)> onError) {
  client_.checkHealth(std::move(onSuccess), std::move(onError));
}
