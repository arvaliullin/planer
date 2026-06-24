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
  QNetworkReply *reply = networkManager_->get(QNetworkRequest(url));

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

void HttpClient::finishRequest(QNetworkReply *reply, SuccessHandler onSuccess,
                               ErrorHandler onError) {
  reply->deleteLater();

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
