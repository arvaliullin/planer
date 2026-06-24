#include "presentation/AppContext.hpp"

#include "core/Config.hpp"

#include <QQmlApplicationEngine>
#include <QQmlContext>

AppContext::AppContext(QObject *parent)
    : QObject(parent), apiClient_(this), checkHealthUseCase_(apiClient_),
      appViewModel_(this), sessionViewModel_(apiClient_, appViewModel_, this),
      projectsViewModel_(apiClient_, sessionViewModel_, appViewModel_, this),
      tasksViewModel_(apiClient_, projectsViewModel_, sessionViewModel_,
                      appViewModel_, this),
      kanbanViewModel_(tasksViewModel_, this),
      ganttViewModel_(apiClient_, projectsViewModel_, tasksViewModel_,
                      sessionViewModel_, appViewModel_, this),
      taskDetailViewModel_(apiClient_, tasksViewModel_, sessionViewModel_,
                           appViewModel_, this),
      healthViewModel_(checkHealthUseCase_, Config::apiBaseUrl(), this) {
  httpClient_.setParent(this);
  sessionViewModel_.restoreSession();
}

void AppContext::registerContext(QQmlApplicationEngine &engine) {
  auto *context = engine.rootContext();
  context->setContextProperty(QStringLiteral("app"), &appViewModel_);
  context->setContextProperty(QStringLiteral("session"), &sessionViewModel_);
  context->setContextProperty(QStringLiteral("projects"), &projectsViewModel_);
  context->setContextProperty(QStringLiteral("tasks"), &tasksViewModel_);
  context->setContextProperty(QStringLiteral("kanban"), &kanbanViewModel_);
  context->setContextProperty(QStringLiteral("gantt"), &ganttViewModel_);
  context->setContextProperty(QStringLiteral("taskDetail"), &taskDetailViewModel_);
  context->setContextProperty(QStringLiteral("health"), &healthViewModel_);
}

void AppContext::shutdown() {
  if (shuttingDown_) {
    return;
  }

  shuttingDown_ = true;
  httpClient_.cancelAll();
}
