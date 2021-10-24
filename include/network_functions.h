#pragma once

#include <QString>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

QString GET(QNetworkAccessManager* mgr, const QNetworkRequest& request);

QString POST(QNetworkAccessManager* mgr, const QNetworkRequest& request, QByteArray&& postData);

QJsonArray SendImage(QNetworkAccessManager* mgr, QString url, QString token, QByteArray&& image);



