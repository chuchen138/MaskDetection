#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{

}


void Worker::doWork(QImage img,QString accessToken,QSslConfiguration sslConfig,QThread* overThread){
    qDebug() << "turn to base64 encode";
    // 转成base64编码
    QByteArray ba;
    QBuffer buff(&ba);
    img.save(&buff, "jpg");
    QString b64str = ba.toBase64();
    // qDebug() << b64str;
    qDebug() << "base64 success over";

    // 请求体body设置
    QJsonObject postJson;
    QJsonDocument doc;

    postJson.insert("image", b64str);
    postJson.insert("image_type", "BASE64");
    postJson.insert("face_field", "age,expression,face_shape,gender,glasses"
                    "emotion,face_type,mask,beauty");
    doc.setObject(postJson);
    QByteArray postData=doc.toJson(QJsonDocument::Compact);
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query;
    query.addQueryItem("access_token", accessToken);
    url.setQuery(query);
    qDebug() << "Completion of assembly";
    // 组装请求
    QNetworkRequest req;

    req.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    req.setUrl(url);
    req.setSslConfiguration(sslConfig);
    emit resultReady(req, postData,overThread);
    qDebug() << "worker over";
}



