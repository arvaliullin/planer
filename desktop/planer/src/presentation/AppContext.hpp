#pragma once

#include "data/api/HttpClient.hpp"
#include "data/api/client/mock/MockPlanerApiClient.hpp"
#include "domain/usecases/CheckHealthUseCase.hpp"
#include "presentation/viewmodels/AppViewModel.hpp"
#include "presentation/viewmodels/GanttViewModel.hpp"
#include "presentation/viewmodels/HealthViewModel.hpp"
#include "presentation/viewmodels/KanbanViewModel.hpp"
#include "presentation/viewmodels/ProjectsViewModel.hpp"
#include "presentation/viewmodels/SessionViewModel.hpp"
#include "presentation/viewmodels/TaskDetailViewModel.hpp"
#include "presentation/viewmodels/TasksViewModel.hpp"

#include <QObject>

class QQmlApplicationEngine;

class AppContext : public QObject {
  Q_OBJECT

public:
  explicit AppContext(QObject *parent = nullptr);

  void registerContext(QQmlApplicationEngine &engine);
  void shutdown();

private:
  HttpClient httpClient_;
  MockPlanerApiClient apiClient_;
  CheckHealthUseCase checkHealthUseCase_;
  AppViewModel appViewModel_;
  SessionViewModel sessionViewModel_;
  ProjectsViewModel projectsViewModel_;
  TasksViewModel tasksViewModel_;
  KanbanViewModel kanbanViewModel_;
  GanttViewModel ganttViewModel_;
  TaskDetailViewModel taskDetailViewModel_;
  HealthViewModel healthViewModel_;
  bool shuttingDown_ = false;
};
