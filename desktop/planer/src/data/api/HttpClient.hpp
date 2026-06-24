#pragma once

#include <functional>

#include <QJsonObject>
#include <QObject>
#include <QUrl>

class QNetworkReply;

// HttpClient выполняет HTTP GET запросы.
class HttpClient : public QObject {
  Q_OBJECT

public:
  using SuccessHandler = std::function<void(const QJsonObject &)>;
  using ErrorHandler = std::function<void(const QString &)>;

  explicit HttpClient(QObject *parent = nullptr);

  void get(const QUrl &url, SuccessHandler onSuccess, ErrorHandler onError);

private:
  void finishRequest(QNetworkReply *reply, SuccessHandler onSuccess,
                     ErrorHandler onError);

  class QNetworkAccessManager *networkManager_;
};
