#pragma once

#include <QString>

enum class ApiErrorKind {
  Network,
  Parse,
  BadRequest,
  Unauthorized,
  Forbidden,
  NotFound,
  Conflict,
  Validation,
  PayloadTooLarge,
  UnsupportedMedia,
  ServiceUnavailable,
  Internal,
};

struct ApiError {
  ApiErrorKind kind = ApiErrorKind::Internal;
  QString message;

  static ApiError network();
  static ApiError unauthorized();
  static ApiError forbidden();
  static ApiError notFound(const QString &what);
  static ApiError conflict(const QString &message);
  static ApiError validation(const QString &message);
  static ApiError badRequest(const QString &message);
  static ApiError internal(const QString &message);

  QString userMessage() const;
};
