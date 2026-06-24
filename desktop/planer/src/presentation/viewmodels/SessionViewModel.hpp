#pragma once

#include "domain/ports/IPlanerApiClient.hpp"

#include "core/ApiError.hpp"
#include "presentation/mappers/EntityMapper.hpp"
#include "presentation/viewmodels/AppViewModel.hpp"

#include <QObject>
#include <QString>
#include <QVariantMap>

class SessionViewModel : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool authenticated READ authenticated NOTIFY authenticatedChanged)
  Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
  Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)
  Q_PROPERTY(QVariantMap currentUser READ currentUser NOTIFY currentUserChanged)

public:
  SessionViewModel(IPlanerApiClient &client, AppViewModel &app,
                   QObject *parent = nullptr);

  bool authenticated() const;
  bool loading() const;
  QString errorMessage() const;
  QVariantMap currentUser() const;

  Q_INVOKABLE void restoreSession();
  Q_INVOKABLE void login(const QString &email, const QString &password);
  Q_INVOKABLE void registerUser(const QString &email, const QString &password,
                                const QString &displayName);
  Q_INVOKABLE void logout();

  void handleApiError(const ApiError &error);

signals:
  void authenticatedChanged();
  void loadingChanged();
  void errorMessageChanged();
  void currentUserChanged();
  void sessionExpired();

private:
  void setLoading(bool loading);
  void setAuthenticated(bool value);
  void setCurrentUser(const QVariantMap &user);
  void clearError();

  IPlanerApiClient &client_;
  AppViewModel &app_;
  bool authenticated_ = false;
  bool loading_ = false;
  QString errorMessage_;
  QVariantMap currentUser_;
};
