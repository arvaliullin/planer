#pragma once

#include "domain/entities/User.hpp"

#include <QString>

struct AuthResult {
  QString token;
  User user;
};

struct RegisterRequest {
  QString email;
  QString password;
  QString displayName;
};

struct LoginRequest {
  QString email;
  QString password;
};
