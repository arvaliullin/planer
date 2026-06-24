#include "presentation/viewmodels/SessionViewModel.hpp"

#include "core/TokenStorage.hpp"

SessionViewModel::SessionViewModel(IPlanerApiClient &client, AppViewModel &app,
                                   QObject *parent)
    : QObject(parent), client_(client), app_(app),
      authenticated_(TokenStorage::hasToken()) {}

bool SessionViewModel::authenticated() const { return authenticated_; }

bool SessionViewModel::loading() const { return loading_; }

QString SessionViewModel::errorMessage() const { return errorMessage_; }

QVariantMap SessionViewModel::currentUser() const { return currentUser_; }

void SessionViewModel::restoreSession() {
  if (!TokenStorage::hasToken()) {
    return;
  }
  setLoading(true);
  client_.getMe(
      [this](const User &user) {
        setLoading(false);
        setCurrentUser(EntityMapper::userToVariant(user));
        setAuthenticated(true);
      },
      [this](const ApiError &error) {
        setLoading(false);
        if (error.kind == ApiErrorKind::Unauthorized) {
          TokenStorage::clear();
          setAuthenticated(false);
        }
      });
}

void SessionViewModel::login(const QString &email, const QString &password) {
  if (loading_) {
    return;
  }
  clearError();
  setLoading(true);
  client_.login(
      {email, password},
      [this](const AuthResult &result) {
        setLoading(false);
        setCurrentUser(EntityMapper::userToVariant(result.user));
        setAuthenticated(true);
      },
      [this](const ApiError &error) {
        setLoading(false);
        errorMessage_ = error.userMessage();
        emit errorMessageChanged();
      });
}

void SessionViewModel::registerUser(const QString &email,
                                    const QString &password,
                                    const QString &displayName) {
  if (loading_) {
    return;
  }
  clearError();
  setLoading(true);
  client_.registerUser(
      {email, password, displayName},
      [this](const AuthResult &result) {
        setLoading(false);
        setCurrentUser(EntityMapper::userToVariant(result.user));
        setAuthenticated(true);
      },
      [this](const ApiError &error) {
        setLoading(false);
        errorMessage_ = error.userMessage();
        emit errorMessageChanged();
      });
}

void SessionViewModel::logout() {
  TokenStorage::clear();
  currentUser_ = {};
  setAuthenticated(false);
  emit currentUserChanged();
}

void SessionViewModel::handleApiError(const ApiError &error) {
  app_.showToast(error.userMessage());
  if (error.kind == ApiErrorKind::Unauthorized) {
    logout();
    emit sessionExpired();
  }
}

void SessionViewModel::setLoading(bool loading) {
  if (loading_ == loading) {
    return;
  }
  loading_ = loading;
  emit loadingChanged();
}

void SessionViewModel::setAuthenticated(bool value) {
  if (authenticated_ == value) {
    return;
  }
  authenticated_ = value;
  emit authenticatedChanged();
}

void SessionViewModel::setCurrentUser(const QVariantMap &user) {
  currentUser_ = user;
  emit currentUserChanged();
}

void SessionViewModel::clearError() {
  if (errorMessage_.isEmpty()) {
    return;
  }
  errorMessage_.clear();
  emit errorMessageChanged();
}
