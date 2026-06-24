#include "presentation/AppContext.hpp"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#ifdef PLANER_QT_STATIC
#include <QtGlobal>
#endif

int main(int argc, char *argv[]) {
#ifdef PLANER_QT_STATIC
  if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
    qputenv("QT_QPA_PLATFORM", "xcb");
  }
#endif

  QGuiApplication app(argc, argv);

  QQuickStyle::setStyle(QStringLiteral("Material"));

  AppContext appContext(&app);

  int exitCode = -1;
  {
    QQmlApplicationEngine engine;
    appContext.registerContext(engine);

    QObject::connect(&app, &QCoreApplication::aboutToQuit, &appContext,
                     &AppContext::shutdown);

    engine.loadFromModule(QStringLiteral("planer"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty()) {
      return -1;
    }

    exitCode = app.exec();
    appContext.shutdown();
  }

  return exitCode;
}
