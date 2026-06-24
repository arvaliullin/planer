#include "core/ApiError.hpp"

ApiError ApiError::network() {
  return {ApiErrorKind::Network,
          QStringLiteral("Не удалось связаться с сервером")};
}

ApiError ApiError::unauthorized() {
  return {ApiErrorKind::Unauthorized,
          QStringLiteral("Требуется авторизация")};
}

ApiError ApiError::forbidden() {
  return {ApiErrorKind::Forbidden, QStringLiteral("Недостаточно прав")};
}

ApiError ApiError::notFound(const QString &what) {
  return {ApiErrorKind::NotFound,
          QStringLiteral("%1 не найден").arg(what)};
}

ApiError ApiError::conflict(const QString &message) { return {ApiErrorKind::Conflict, message}; }

ApiError ApiError::validation(const QString &message) {
  return {ApiErrorKind::Validation, message};
}

ApiError ApiError::badRequest(const QString &message) {
  return {ApiErrorKind::BadRequest, message};
}

ApiError ApiError::internal(const QString &message) {
  return {ApiErrorKind::Internal, message};
}

QString ApiError::userMessage() const { return message; }
