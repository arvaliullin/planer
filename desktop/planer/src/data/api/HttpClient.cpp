#include "data/api/HttpClient.hpp"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

namespace {

constexpr int kRequestTimeoutMs = 5000;

} // namespace

HttpClient::HttpClient(QObject *parent)
    : QObject(parent), networkManager_(new QNetworkAccessManager(this)) {}

void HttpClient::setAuthToken(const QString &token) { authToken_ = token; }

QNetworkRequest HttpClient::authorizedRequest(const QUrl &url) const {
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader,
                    QStringLiteral("application/json"));
  if (!authToken_.isEmpty()) {
    request.setRawHeader("Authorization",
                         QStringLiteral("Bearer %1").arg(authToken_).toUtf8());
  }
  return request;
}

void HttpClient::get(const QUrl &url, SuccessHandler onSuccess,
                     ErrorHandler onError) {
  if (shuttingDown_) {
    return;
  }

  QNetworkReply *reply = networkManager_->get(authorizedRequest(url));
  activeReplies_.append(reply);
  sendRequest(reply, std::move(onSuccess), std::move(onError));
}

void HttpClient::post(const QUrl &url, const QJsonObject &body,
                      SuccessHandler onSuccess, ErrorHandler onError) {
  if (shuttingDown_) {
    return;
  }

  const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
  QNetworkReply *reply =
      networkManager_->post(authorizedRequest(url), payload);
  activeReplies_.append(reply);
  sendRequest(reply, std::move(onSuccess), std::move(onError));
}

void HttpClient::patch(const QUrl &url, const QJsonObject &body,
                       SuccessHandler onSuccess, ErrorHandler onError) {
  if (shuttingDown_) {
    return;
  }

  const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
  QNetworkRequest request = authorizedRequest(url);
  QNetworkReply *reply =
      networkManager_->sendCustomRequest(request, "PATCH", payload);
  activeReplies_.append(reply);
  sendRequest(reply, std::move(onSuccess), std::move(onError));
}

void HttpClient::deleteResource(const QUrl &url, SuccessHandler onSuccess,
                                ErrorHandler onError) {
  if (shuttingDown_) {
    return;
  }

  QNetworkReply *reply = networkManager_->deleteResource(authorizedRequest(url));
  activeReplies_.append(reply);
  sendRequest(reply, std::move(onSuccess), std::move(onError));
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

void HttpClient::sendRequest(QNetworkReply *reply, SuccessHandler onSuccess,
                             ErrorHandler onError) {
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
            activeReplies_.removeAll(reply);
            reply->deleteLater();

            if (shuttingDown_) {
              return;
            }

            if (reply->error() != QNetworkReply::NoError) {
              onError(QStringLiteral("Не удалось связаться с сервером"));
              return;
            }

            if (reply->operation() == QNetworkAccessManager::DeleteOperation) {
              onSuccess({});
              return;
            }

            QJsonParseError parseError;
            const QJsonDocument document =
                QJsonDocument::fromJson(reply->readAll(), &parseError);
            if (parseError.error != QJsonParseError::NoError ||
                !document.isObject()) {
              onError(QStringLiteral("Некорректный ответ сервера"));
              return;
            }

            onSuccess(document.object());
          });
}
