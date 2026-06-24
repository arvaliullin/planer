#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "presentation/viewmodels/AppViewModel.hpp"
#include "presentation/viewmodels/SessionViewModel.hpp"
#include "presentation/viewmodels/TasksViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>

class TaskDetailViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(QVariantMap task READ task NOTIFY taskChanged)
  Q_PROPERTY(QVariantList dependencies READ dependencies NOTIFY dependenciesChanged)
  Q_PROPERTY(QVariantList attachments READ attachments NOTIFY attachmentsChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
  TaskDetailViewModel(IPlanerApiClient &client, TasksViewModel &tasks,
                      SessionViewModel &session, AppViewModel &app,
                      QObject *parent = nullptr);

  QVariantMap task() const;
  QVariantList dependencies() const;
  QVariantList attachments() const;
  bool loading() const;

  Q_INVOKABLE void loadDetails();
  Q_INVOKABLE void saveField(const QString &field, const QVariant &value);
  Q_INVOKABLE void addDependency(const QString &blockedTaskId);
  Q_INVOKABLE void removeDependency(const QString &dependencyId);
  Q_INVOKABLE void uploadAttachment(const QString &fileName,
                                    const QString &mimeType, qint64 sizeBytes);
  Q_INVOKABLE void deleteAttachment(const QString &attachmentId);

signals:
  void taskChanged();
  void dependenciesChanged();
  void attachmentsChanged();
  void loadingChanged();

private:
  void setLoading(bool loading);
  void refreshFromTasks();

  IPlanerApiClient &client_;
  TasksViewModel &tasks_;
  SessionViewModel &session_;
  AppViewModel &app_;
  QVariantMap task_;
  QVariantList dependencies_;
  QVariantList attachments_;
  bool loading_ = false;
};
