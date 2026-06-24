#include "presentation/AppContext.hpp"

#include "core/Config.hpp"

#include <QQmlApplicationEngine>
#include <QQmlContext>

AppContext::AppContext(QObject *parent)
    : QObject(parent), healthRepository_(httpClient_, Config::apiBaseUrl()),
      checkHealthUseCase_(healthRepository_),
      healthViewModel_(checkHealthUseCase_, Config::apiBaseUrl(), this) {
  httpClient_.setParent(this);
}

void AppContext::registerContext(QQmlApplicationEngine &engine) {
  engine.rootContext()->setContextProperty(QStringLiteral("healthViewModel"),
                                           &healthViewModel_);
}

void AppContext::shutdown() {
  if (shuttingDown_) {
    return;
  }

  shuttingDown_ = true;
  httpClient_.cancelAll();
}
