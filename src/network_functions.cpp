#include "network_functions.h"

#include <QObject>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

QString GET(QNetworkAccessManager* mgr, const QNetworkRequest& request){
  QNetworkReply* reply = mgr->get(request);
  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  reply->deleteLater();
  QString response = QString::fromUtf8(reply->readAll());
  return response;
}

QString POST(QNetworkAccessManager* mgr, const QNetworkRequest& request, QByteArray&& postData)
{
  QNetworkReply* reply = mgr->post(request, postData);
  QEventLoop loop;
  QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
  loop.exec();
  QString response = QString::fromUtf8(reply->readAll());
  return response;
}

QJsonArray SendImage(QNetworkAccessManager* mgr, QString url, QString token, QByteArray&& image){
  QUrl url_query(url);
  url_query.setQuery("fd_threshold=0.1&demographics=true");
  QNetworkRequest request(url_query.toString());
  request.setRawHeader("Authorization", "Bearer " + token.toUtf8());
  request.setHeader(QNetworkRequest::ContentTypeHeader, "image/jpeg");
  request.setRawHeader("accept", "application/json");
  auto response = POST(mgr, request, std::move(image));
  QJsonDocument response_json = QJsonDocument::fromJson(response.toUtf8());
  auto data = response_json["data"].toArray();
  return data;
}
