#include "data/repositories/HealthRepository.hpp"

#include "data/dto/HealthDto.hpp"
#include "data/mappers/HealthMapper.hpp"

#include <QUrl>

HealthRepository::HealthRepository(HttpClient &httpClient, QString apiBaseUrl)
    : httpClient_(httpClient), apiBaseUrl_(std::move(apiBaseUrl)) {}

void HealthRepository::checkHealth(std::function<void(HealthStatus)> onSuccess,
                                   std::function<void(QString)> onError) {
  const QUrl url(apiBaseUrl_ + QStringLiteral("/health"));
  httpClient_.get(
      url,
      [onSuccess = std::move(onSuccess)](const QJsonObject &object) {
        onSuccess(HealthMapper::toDomain(HealthDto::fromJson(object)));
      },
      std::move(onError));
}
