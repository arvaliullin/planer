#include "presentation/AppContext.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);

  QQuickStyle::setStyle(QStringLiteral("Material"));

  QQmlApplicationEngine engine;
  AppContext appContext;
  appContext.registerContext(engine);

  engine.loadFromModule(QStringLiteral("planer"), QStringLiteral("Main"));
  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  return app.exec();
}
