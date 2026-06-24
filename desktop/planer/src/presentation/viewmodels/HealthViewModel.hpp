#pragma once

#include "domain/usecases/CheckHealthUseCase.hpp"

#include <QObject>
#include <QString>

class HealthViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(bool hasResult READ hasResult NOTIFY resultChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY resultChanged)
  Q_PROPERTY(QString apiBaseUrl READ apiBaseUrl CONSTANT)
  Q_PROPERTY(bool postgresPresent READ postgresPresent NOTIFY resultChanged)
  Q_PROPERTY(bool postgresOk READ postgresOk NOTIFY resultChanged)
  Q_PROPERTY(QString postgresError READ postgresError NOTIFY resultChanged)
  Q_PROPERTY(bool minioPresent READ minioPresent NOTIFY resultChanged)
  Q_PROPERTY(bool minioOk READ minioOk NOTIFY resultChanged)
  Q_PROPERTY(QString minioError READ minioError NOTIFY resultChanged)

public:
  HealthViewModel(CheckHealthUseCase &useCase, QString apiBaseUrl,
                  QObject *parent = nullptr);

  bool loading() const;
  bool hasResult() const;
  QString errorMessage() const;
  QString apiBaseUrl() const;

  bool postgresPresent() const;
  bool postgresOk() const;
  QString postgresError() const;

  bool minioPresent() const;
  bool minioOk() const;
  QString minioError() const;

  Q_INVOKABLE void checkHealth();

signals:
  void loadingChanged();
  void resultChanged();

private:
  void setLoading(bool loading);
  void applyStatus(const HealthStatus &status);
  void applyError(const QString &message);

  CheckHealthUseCase &useCase_;
  QString apiBaseUrl_;
  bool loading_ = false;
  bool hasResult_ = false;
  QString errorMessage_;
  bool postgresPresent_ = false;
  bool postgresOk_ = false;
  QString postgresError_;
  bool minioPresent_ = false;
  bool minioOk_ = false;
  QString minioError_;
};
