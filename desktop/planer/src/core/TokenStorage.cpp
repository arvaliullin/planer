#include "core/TokenStorage.hpp"

#include <QSettings>

namespace {

constexpr auto kTokenKey = "auth/token";
constexpr auto kUserIdKey = "auth/userId";

} // namespace

QString TokenStorage::token() {
  QSettings settings;
  return settings.value(kTokenKey).toString();
}

QString TokenStorage::userId() {
  QSettings settings;
  return settings.value(kUserIdKey).toString();
}

void TokenStorage::setSession(const QString &token, const QString &userId) {
  QSettings settings;
  settings.setValue(kTokenKey, token);
  settings.setValue(kUserIdKey, userId);
}

void TokenStorage::clear() {
  QSettings settings;
  settings.remove(kTokenKey);
  settings.remove(kUserIdKey);
}

bool TokenStorage::hasToken() { return !token().isEmpty(); }
