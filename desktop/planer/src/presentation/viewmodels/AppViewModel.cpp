#include "presentation/viewmodels/AppViewModel.hpp"

#include <QTimer>

AppViewModel::AppViewModel(QObject *parent) : QObject(parent) {}

int AppViewModel::activeTab() const { return activeTab_; }

QVariantList AppViewModel::toasts() const { return toasts_; }

bool AppViewModel::createTaskDialogOpen() const { return createTaskDialogOpen_; }

bool AppViewModel::createProjectDialogOpen() const {
  return createProjectDialogOpen_;
}

bool AppViewModel::commandPaletteOpen() const { return commandPaletteOpen_; }

bool AppViewModel::healthDialogOpen() const { return healthDialogOpen_; }

void AppViewModel::setActiveTab(int tab) {
  if (activeTab_ == tab) {
    return;
  }
  activeTab_ = tab;
  emit activeTabChanged();
}

void AppViewModel::showToast(const QString &message) {
  toasts_.append(message);
  emit toastsChanged();

  const int index = toasts_.size() - 1;
  QTimer::singleShot(4000, this, [this, index]() { dismissToast(index); });
}

void AppViewModel::dismissToast(int index) {
  if (index < 0 || index >= toasts_.size()) {
    return;
  }
  toasts_.removeAt(index);
  emit toastsChanged();
}

void AppViewModel::openCreateTaskDialog() { setCreateTaskDialogOpen(true); }

void AppViewModel::openCreateProjectDialog() { setCreateProjectDialogOpen(true); }

void AppViewModel::openCommandPalette() { setCommandPaletteOpen(true); }

void AppViewModel::openHealthDialog() { setHealthDialogOpen(true); }

void AppViewModel::closeCreateTaskDialog() { setCreateTaskDialogOpen(false); }

void AppViewModel::closeCreateProjectDialog() { setCreateProjectDialogOpen(false); }

void AppViewModel::closeCommandPalette() { setCommandPaletteOpen(false); }

void AppViewModel::closeHealthDialog() { setHealthDialogOpen(false); }

void AppViewModel::setCreateTaskDialogOpen(bool open) {
  if (createTaskDialogOpen_ == open) {
    return;
  }
  createTaskDialogOpen_ = open;
  emit createTaskDialogOpenChanged();
}

void AppViewModel::setCreateProjectDialogOpen(bool open) {
  if (createProjectDialogOpen_ == open) {
    return;
  }
  createProjectDialogOpen_ = open;
  emit createProjectDialogOpenChanged();
}

void AppViewModel::setCommandPaletteOpen(bool open) {
  if (commandPaletteOpen_ == open) {
    return;
  }
  commandPaletteOpen_ = open;
  emit commandPaletteOpenChanged();
}

void AppViewModel::setHealthDialogOpen(bool open) {
  if (healthDialogOpen_ == open) {
    return;
  }
  healthDialogOpen_ = open;
  emit healthDialogOpenChanged();
}
