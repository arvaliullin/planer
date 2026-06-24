#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class AppViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(int activeTab READ activeTab WRITE setActiveTab NOTIFY activeTabChanged)
  Q_PROPERTY(QVariantList toasts READ toasts NOTIFY toastsChanged)
  Q_PROPERTY(bool createTaskDialogOpen READ createTaskDialogOpen WRITE
                 setCreateTaskDialogOpen NOTIFY createTaskDialogOpenChanged)
  Q_PROPERTY(bool createProjectDialogOpen READ createProjectDialogOpen WRITE
                 setCreateProjectDialogOpen NOTIFY createProjectDialogOpenChanged)
  Q_PROPERTY(bool commandPaletteOpen READ commandPaletteOpen WRITE
                 setCommandPaletteOpen NOTIFY commandPaletteOpenChanged)
  Q_PROPERTY(bool healthDialogOpen READ healthDialogOpen WRITE setHealthDialogOpen
                 NOTIFY healthDialogOpenChanged)

public:
  explicit AppViewModel(QObject *parent = nullptr);

  int activeTab() const;
  QVariantList toasts() const;
  bool createTaskDialogOpen() const;
  bool createProjectDialogOpen() const;
  bool commandPaletteOpen() const;
  bool healthDialogOpen() const;

  void setActiveTab(int tab);

  Q_INVOKABLE void showToast(const QString &message);
  Q_INVOKABLE void dismissToast(int index);
  Q_INVOKABLE void openCreateTaskDialog();
  Q_INVOKABLE void openCreateProjectDialog();
  Q_INVOKABLE void openCommandPalette();
  Q_INVOKABLE void openHealthDialog();
  Q_INVOKABLE void closeCreateTaskDialog();
  Q_INVOKABLE void closeCreateProjectDialog();
  Q_INVOKABLE void closeCommandPalette();
  Q_INVOKABLE void closeHealthDialog();

  void setCreateTaskDialogOpen(bool open);
  void setCreateProjectDialogOpen(bool open);
  void setCommandPaletteOpen(bool open);
  void setHealthDialogOpen(bool open);

signals:
  void activeTabChanged();
  void toastsChanged();
  void createTaskDialogOpenChanged();
  void createProjectDialogOpenChanged();
  void commandPaletteOpenChanged();
  void healthDialogOpenChanged();

private:
  int activeTab_ = 0;
  QVariantList toasts_;
  bool createTaskDialogOpen_ = false;
  bool createProjectDialogOpen_ = false;
  bool commandPaletteOpen_ = false;
  bool healthDialogOpen_ = false;
};
