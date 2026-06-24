#pragma once

#include <functional>

#include <QJsonObject>
#include <QNetworkRequest>
#include <QObject>
#include <QUrl>

class QNetworkReply;

class HttpClient : public QObject {
  Q_OBJECT

public:
  using SuccessHandler = std::function<void(const QJsonObject &)>;
  using ErrorHandler = std::function<void(const QString &)>;

  explicit HttpClient(QObject *parent = nullptr);

  void get(const QUrl &url, SuccessHandler onSuccess, ErrorHandler onError);
  void post(const QUrl &url, const QJsonObject &body, SuccessHandler onSuccess,
            ErrorHandler onError);
  void patch(const QUrl &url, const QJsonObject &body, SuccessHandler onSuccess,
             ErrorHandler onError);
  void deleteResource(const QUrl &url, SuccessHandler onSuccess,
                      ErrorHandler onError);

  void setAuthToken(const QString &token);
  void cancelAll();

private:
  QNetworkRequest authorizedRequest(const QUrl &url) const;
  void sendRequest(QNetworkReply *reply, SuccessHandler onSuccess,
                   ErrorHandler onError);

  class QNetworkAccessManager *networkManager_;
  QList<QNetworkReply *> activeReplies_;
  QString authToken_;
  bool shuttingDown_ = false;
};
