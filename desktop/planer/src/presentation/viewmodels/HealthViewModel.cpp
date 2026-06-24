#include "presentation/viewmodels/HealthViewModel.hpp"

#include "domain/entities/HealthStatus.hpp"

HealthViewModel::HealthViewModel(CheckHealthUseCase &useCase, QString apiBaseUrl,
                                 QObject *parent)
    : QObject(parent), useCase_(useCase), apiBaseUrl_(std::move(apiBaseUrl)) {}

bool HealthViewModel::loading() const { return loading_; }

bool HealthViewModel::hasResult() const { return hasResult_; }

QString HealthViewModel::errorMessage() const { return errorMessage_; }

QString HealthViewModel::apiBaseUrl() const { return apiBaseUrl_; }

bool HealthViewModel::postgresPresent() const { return postgresPresent_; }

bool HealthViewModel::postgresOk() const { return postgresOk_; }

QString HealthViewModel::postgresError() const { return postgresError_; }

bool HealthViewModel::minioPresent() const { return minioPresent_; }

bool HealthViewModel::minioOk() const { return minioOk_; }

QString HealthViewModel::minioError() const { return minioError_; }

void HealthViewModel::checkHealth() {
  if (loading_) {
    return;
  }

  setLoading(true);
  errorMessage_.clear();
  emit resultChanged();

  useCase_.execute(
      [this](const HealthStatus &status) {
        setLoading(false);
        applyStatus(status);
      },
      [this](const ApiError &error) {
        setLoading(false);
        applyError(error.userMessage());
      });
}

void HealthViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void HealthViewModel::applyStatus(const HealthStatus &status) {
  hasResult_ = true;
  errorMessage_.clear();

  postgresPresent_ = status.postgres.has_value();
  postgresOk_ = postgresPresent_ && status.postgres->ok;
  postgresError_ = postgresPresent_ ? status.postgres->error : QString();

  minioPresent_ = status.minio.has_value();
  minioOk_ = minioPresent_ && status.minio->ok;
  minioError_ = minioPresent_ ? status.minio->error : QString();

  emit resultChanged();
}

void HealthViewModel::applyError(const QString &message) {
  hasResult_ = false;
  errorMessage_ = message;
  postgresPresent_ = false;
  minioPresent_ = false;
  emit resultChanged();
}
