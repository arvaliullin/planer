#pragma once

#include "data/api/HttpClient.hpp"
#include "data/repositories/HealthRepository.hpp"
#include "domain/usecases/CheckHealthUseCase.hpp"
#include "presentation/viewmodels/HealthViewModel.hpp"

#include <QObject>

class QQmlApplicationEngine;

// AppContext собирает зависимости desktop-клиента.
class AppContext : public QObject {
  Q_OBJECT

public:
  explicit AppContext(QObject *parent = nullptr);

  void registerContext(QQmlApplicationEngine &engine);
  void shutdown();

private:
  HttpClient httpClient_;
  HealthRepository healthRepository_;
  CheckHealthUseCase checkHealthUseCase_;
  HealthViewModel healthViewModel_;
  bool shuttingDown_ = false;
};
