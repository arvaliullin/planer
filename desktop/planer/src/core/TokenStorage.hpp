#pragma once

#include <QString>

class TokenStorage {
public:
  static QString token();
  static QString userId();
  static void setSession(const QString &token, const QString &userId);
  static void clear();
  static bool hasToken();
};
