#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QImage>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QSslConfiguration>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    void doWork(QImage img,QString accessToken,QSslConfiguration sslConfig,QThread* overThread);

signals:
    void resultReady(QNetworkRequest req,QByteArray postData,QThread* overThread);

};

#endif // WORKER_H
