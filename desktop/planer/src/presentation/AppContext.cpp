#include "presentation/AppContext.hpp"

#include "core/Config.hpp"

#include <QQmlApplicationEngine>
#include <QQmlContext>

AppContext::AppContext()
    : healthRepository_(httpClient_, Config::apiBaseUrl()),
      checkHealthUseCase_(healthRepository_),
      healthViewModel_(checkHealthUseCase_, Config::apiBaseUrl()) {}

void AppContext::registerContext(QQmlApplicationEngine &engine) {
  engine.rootContext()->setContextProperty(QStringLiteral("healthViewModel"),
                                           &healthViewModel_);
}
