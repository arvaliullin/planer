#include "domain/usecases/CheckHealthUseCase.hpp"

CheckHealthUseCase::CheckHealthUseCase(IHealthRepository &repository)
    : repository_(repository) {}

void CheckHealthUseCase::execute(std::function<void(HealthStatus)> onSuccess,
                                 std::function<void(QString)> onError) {
  repository_.checkHealth(std::move(onSuccess), std::move(onError));
}
