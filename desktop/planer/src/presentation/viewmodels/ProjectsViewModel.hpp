#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "presentation/viewmodels/AppViewModel.hpp"
#include "presentation/viewmodels/SessionViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class ProjectsViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantList projects READ projects NOTIFY projectsChanged)
  Q_PROPERTY(QVariantMap activeProject READ activeProject NOTIFY activeProjectChanged)
  Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
  Q_PROPERTY(QVariantList filteredProjects READ filteredProjects NOTIFY filterChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
  ProjectsViewModel(IPlanerApiClient &client, SessionViewModel &session,
                    AppViewModel &app, QObject *parent = nullptr);

  QVariantList projects() const;
  QVariantMap activeProject() const;
  QString filter() const;
  QVariantList filteredProjects() const;
  bool loading() const;

  Q_INVOKABLE void loadProjects();
  Q_INVOKABLE void selectProject(const QString &projectId);
  Q_INVOKABLE void createProject(const QString &key, const QString &name,
                                 const QString &description);

  void setFilter(const QString &filter);

signals:
  void projectsChanged();
  void activeProjectChanged();
  void filterChanged();
  void loadingChanged();
  void projectSelected(const QString &projectId);

private:
  void setLoading(bool loading);
  void rebuildFiltered();

  IPlanerApiClient &client_;
  SessionViewModel &session_;
  AppViewModel &app_;
  QVariantList projects_;
  QVariantList filteredProjects_;
  QVariantMap activeProject_;
  QString filter_;
  bool loading_ = false;
};
