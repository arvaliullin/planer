#include "presentation/viewmodels/ProjectsViewModel.hpp"

#include "presentation/mappers/EntityMapper.hpp"

ProjectsViewModel::ProjectsViewModel(IPlanerApiClient &client,
                                     SessionViewModel &session, AppViewModel &app,
                                     QObject *parent)
    : QObject(parent), client_(client), session_(session), app_(app) {}

QVariantList ProjectsViewModel::projects() const { return projects_; }

QVariantMap ProjectsViewModel::activeProject() const { return activeProject_; }

QString ProjectsViewModel::filter() const { return filter_; }

QVariantList ProjectsViewModel::filteredProjects() const {
  return filteredProjects_;
}

bool ProjectsViewModel::loading() const { return loading_; }

void ProjectsViewModel::loadProjects() {
  if (loading_) {
    return;
  }
  setLoading(true);
  client_.listProjects(
      [this](const Paginated<Project> &page) {
        setLoading(false);
        projects_.clear();
        for (const Project &project : page.items) {
          projects_.append(EntityMapper::projectToVariant(project));
        }
        emit projectsChanged();
        rebuildFiltered();

        if (activeProject_.isEmpty() && !projects_.isEmpty()) {
          selectProject(projects_.first().toMap().value(QStringLiteral("id")).toString());
        }
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void ProjectsViewModel::selectProject(const QString &projectId) {
  for (const QVariant &item : projects_) {
    const QVariantMap map = item.toMap();
    if (map.value(QStringLiteral("id")).toString() == projectId) {
      activeProject_ = map;
      emit activeProjectChanged();
      emit projectSelected(projectId);
      return;
    }
  }
}

void ProjectsViewModel::createProject(const QString &key, const QString &name,
                                      const QString &description) {
  if (loading_) {
    return;
  }
  setLoading(true);
  client_.createProject(
      {key, name, description},
      [this](const Project &project) {
        setLoading(false);
        projects_.append(EntityMapper::projectToVariant(project));
        emit projectsChanged();
        rebuildFiltered();
        selectProject(project.id);
        app_.setCreateProjectDialogOpen(false);
        app_.showToast(QStringLiteral("Проект создан"));
      },
      [this](const ApiError &error) {
        setLoading(false);
        session_.handleApiError(error);
      });
}

void ProjectsViewModel::setFilter(const QString &filter) {
  if (filter_ == filter) {
    return;
  }
  filter_ = filter;
  emit filterChanged();
  rebuildFiltered();
}

void ProjectsViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void ProjectsViewModel::rebuildFiltered() {
  filteredProjects_.clear();
  const QString needle = filter_.trimmed().toLower();
  for (const QVariant &item : projects_) {
    const QVariantMap map = item.toMap();
    if (needle.isEmpty()) {
      filteredProjects_.append(item);
      continue;
    }
    const QString name = map.value(QStringLiteral("name")).toString().toLower();
    const QString key = map.value(QStringLiteral("key")).toString().toLower();
    if (name.contains(needle) || key.contains(needle)) {
      filteredProjects_.append(item);
    }
  }
  emit filterChanged();
}
