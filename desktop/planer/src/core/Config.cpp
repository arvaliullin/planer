#include "core/Config.hpp"

#include <QProcessEnvironment>
#include <QSettings>
#include <QUrl>

namespace {

constexpr auto kDefaultApiBaseUrl = "http://localhost:8080";
constexpr auto kEnvApiUrlKey = "PLANER_API_URL";
constexpr auto kSettingsApiUrlKey = "api/baseUrl";

QString normalizeBaseUrl(QString url) {
  url = url.trimmed();
  if (url.isEmpty()) {
    return QString::fromLatin1(kDefaultApiBaseUrl);
  }
  if (!url.endsWith(QLatin1Char('/'))) {
    return url;
  }
  return url.chopped(1);
}

} // namespace

QString Config::apiBaseUrl() {
  const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  if (env.contains(kEnvApiUrlKey)) {
    return normalizeBaseUrl(env.value(kEnvApiUrlKey));
  }

  QSettings settings;
  if (settings.contains(kSettingsApiUrlKey)) {
    return normalizeBaseUrl(settings.value(kSettingsApiUrlKey).toString());
  }

  return QString::fromLatin1(kDefaultApiBaseUrl);
}
