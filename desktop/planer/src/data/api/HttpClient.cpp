#include "data/api/HttpClient.hpp"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

namespace {

constexpr int kRequestTimeoutMs = 5000;

} // namespace

HttpClient::HttpClient(QObject *parent)
    : QObject(parent), networkManager_(new QNetworkAccessManager(this)) {}

void HttpClient::get(const QUrl &url, SuccessHandler onSuccess,
                     ErrorHandler onError) {
  if (shuttingDown_) {
    return;
  }

  QNetworkReply *reply = networkManager_->get(QNetworkRequest(url));
  activeReplies_.append(reply);

  auto *timer = new QTimer(reply);
  timer->setSingleShot(true);
  timer->setInterval(kRequestTimeoutMs);
  connect(timer, &QTimer::timeout, reply, [reply]() {
    if (reply->isRunning()) {
      reply->abort();
    }
  });
  timer->start();

  connect(reply, &QNetworkReply::finished, this,
          [this, reply, onSuccess = std::move(onSuccess),
           onError = std::move(onError)]() {
            finishRequest(reply, std::move(onSuccess), std::move(onError));
          });
}

void HttpClient::cancelAll() {
  if (shuttingDown_) {
    return;
  }

  shuttingDown_ = true;

  const auto replies = activeReplies_;
  for (QNetworkReply *reply : replies) {
    if (reply->isRunning()) {
      reply->abort();
    }
  }
}

void HttpClient::finishRequest(QNetworkReply *reply, SuccessHandler onSuccess,
                               ErrorHandler onError) {
  activeReplies_.removeAll(reply);
  reply->deleteLater();

  if (shuttingDown_) {
    return;
  }

  if (reply->error() != QNetworkReply::NoError) {
    onError(QStringLiteral("Не удалось связаться с сервером"));
    return;
  }

  QJsonParseError parseError;
  const QJsonDocument document =
      QJsonDocument::fromJson(reply->readAll(), &parseError);
  if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
    onError(QStringLiteral("Некорректный ответ сервера"));
    return;
  }

  onSuccess(document.object());
}
