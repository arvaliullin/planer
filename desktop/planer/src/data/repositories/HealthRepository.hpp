#pragma once

#include "data/api/HttpClient.hpp"
#include "domain/ports/IHealthRepository.hpp"

#include <QString>

// HealthRepository запрашивает GET /health.
class HealthRepository : public IHealthRepository {
public:
  HealthRepository(HttpClient &httpClient, QString apiBaseUrl);

  void checkHealth(std::function<void(HealthStatus)> onSuccess,
                   std::function<void(QString)> onError) override;

private:
  HttpClient &httpClient_;
  QString apiBaseUrl_;
};
