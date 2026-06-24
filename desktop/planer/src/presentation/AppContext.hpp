#pragma once

#include "data/api/HttpClient.hpp"
#include "data/repositories/HealthRepository.hpp"
#include "domain/usecases/CheckHealthUseCase.hpp"
#include "presentation/viewmodels/HealthViewModel.hpp"

#include <QQmlApplicationEngine>

// AppContext собирает зависимости desktop-клиента.
class AppContext {
public:
  AppContext();

  void registerContext(QQmlApplicationEngine &engine);

private:
  HttpClient httpClient_;
  HealthRepository healthRepository_;
  CheckHealthUseCase checkHealthUseCase_;
  HealthViewModel healthViewModel_;
};
