#include "mynetmanager.h"
#include "mynetmanager.h"
#include <QUrl>
#include <QUrlQuery>
#include <QByteArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QBuffer>
#include <QJsonArray>
#include "worker.h"
using namespace std;

MyNetManager::MyNetManager(QWidget *parent)
    : QWidget(parent)
{

    mask_flag = 1;
    // 网络操作
    m_pManager = new QNetworkAccessManager();
    imgManager = new QNetworkAccessManager();

    qDebug() << m_pManager->supportedSchemes();
    // 拼接url
    QUrl url("https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials\
&client_id=WZY3bYq8G5ke9z72RK4PjOpD&client_secret=XEVSD3FF79nP30WIAcuN2bxZlAV8mDlZ");
//    QUrl url("https://aip.baidubce.com/oauth/2.0/token");
//    QUrlQuery query;
//    query.addQueryItem("grant_type","client_credentials");
//    query.addQueryItem("client_id","WZY3bYq8G5ke9z72RK4PjOpD");
//    query.addQueryItem("client_secret","XEVSD3FF79nP30WIAcuN2bxZlAV8mDlZ");
//    url.setQuery(query);
    qDebug() << url;
    // 是否支持ssl
    if(QSslSocket::supportsSsl()){
        qDebug() << "support ssl";
    }else{
        qDebug() << "don't support ssl";
    }

    sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::QueryPeer);
    sslConfig.setProtocol(QSsl::TlsV1_2);

    // 组装请求
    QNetworkRequest req;
    req.setUrl(url);
    req.setSslConfiguration(sslConfig);

    // 发送get请求
    connect(m_pManager, &QNetworkAccessManager::finished,this, &MyNetManager::tokenReply);
    m_pManager->get(req);
    connect(imgManager, &QNetworkAccessManager::finished,this, &MyNetManager::imgReply);

}

void MyNetManager::tokenReply(QNetworkReply *reply){
    qDebug() << "I'm ready  ";
    if(reply->error() != QNetworkReply::NoError){
        qDebug() << "error : "<< reply->errorString();
        return;
    }
    // 正常应答
    const QByteArray reply_data = reply->readAll();
    // json解析
    QJsonParseError jsonErr;
    QJsonDocument doc = QJsonDocument::fromJson(reply_data, &jsonErr);

    // 解析成功
    if(jsonErr.error == QJsonParseError::NoError){
        QJsonObject obj =  doc.object();
        if(obj.contains("access_token")){
            accessToken = obj.take("access_token").toString();
        }
        qDebug() << "access_token : " << accessToken;
    }else{
        qDebug() << "json error " << jsonErr.errorString();
    }
    reply->deleteLater();
    qDebug() << " tokenReply over";
}

void MyNetManager::beginFaceDetect(QImage img){
    childThread = new QThread(this);
    Worker *worker = new Worker;
    worker->moveToThread(childThread);
    connect(this, &MyNetManager::beginFaceDetectwork,worker, &Worker::doWork);
    connect(worker, &Worker::resultReady,this, [&](QNetworkRequest req,QByteArray postData, QThread *overThread){
        qDebug() << "resultReady";
        imgManager->post(req, postData);
        qDebug() << "resultReady over";
        overThread->exit();
        qDebug() << "resultReady over";
//        overThread->wait();
        qDebug() << "resultReady over";
    });
    connect(childThread, &QThread::finished, worker,&QObject::deleteLater);
    childThread->start();
    emit beginFaceDetectwork(img, accessToken,sslConfig,childThread);
    qDebug() << " beginFaceDetect over";
}


void MyNetManager::imgReply(QNetworkReply *reply){
    qDebug() << "imgReply begin";
    if(reply->error() != QNetworkReply::NoError){
        qDebug() << reply->errorString();
        return;
    }
    const QByteArray replyData = reply->readAll();
    qDebug() << "imgReply face information replyData : "<<replyData;
    faceInfo = "";
    QJsonParseError jsonErr;
    int arr[5];
    QJsonDocument doc = QJsonDocument::fromJson(replyData, &jsonErr);
    if (jsonErr.error==QJsonParseError::NoError ){
        //取出最外层的j son
        QJsonObject obj=doc.object();

        /*  刷新最新的网络传图  */

        if (obj.contains("result")){
            QJsonObject resultobj =obj.take("result").toObject() ;
            //取出人脸列表
            if(resultobj.contains("face_list")){
                QJsonArray faceList = resultobj.take("face_list").toArray();
                // 取出第一-张人脸信息
                QJsonObject faceObject=faceList.at(0).toObject();
                if(faceObject.contains("location")){
                    QJsonObject genderObj=faceObject.take("location").toObject();
                    if(genderObj.contains("left")){
                        arr[1] = genderObj.take("left").toDouble();
                    }
                    if(genderObj.contains("top")){
                        arr[2] = genderObj.take("top").toDouble();
                    }
                    if(genderObj.contains("width")){
                        arr[3] = genderObj.take("width").toDouble();
                    }
                    if(genderObj.contains("height")){
                        arr[4] = genderObj.take("height").toDouble();
                    }
                }
                //取出年龄
                if(faceObject.contains("age")){
                    double age=faceObject.take("age").toDouble();
                    faceInfo.append("年龄").append(QString::number(age)).append("\r\n");
                }

                if(faceObject.contains("gender")){
                    QJsonObject genderObj=faceObject.take("gender").toObject();
                    if(genderObj.contains("type")){
                        QString gender=genderObj.take("type").toString();
                        faceInfo.append("性别：").append(gender).append("\r\n");
                    }
                }
                if(faceObject.contains("expression")){
                    QJsonObject genderObj=faceObject.take("expression").toObject();
                    if(genderObj.contains("type")){
                        QString gender=genderObj.take("type").toString();
                        faceInfo.append("表情：").append(gender).append("\r\n");
                    }
                }
                if(faceObject.contains("face_shape")){
                    QJsonObject emotionObj=faceObject.take("face_shape").toObject();
                    if(emotionObj.contains("type")){
                        QString emotion=emotionObj.take("type").toString();
                        faceInfo.append("脸型：").append(emotion).append("\r\n");
                    }
                }

                if(faceObject.contains("mask")){
                    QJsonObject emotionObj=faceObject.take("mask").toObject();
                    if(emotionObj.contains("type")){
                        int emotion=emotionObj.take("type").toInt();

                        if(emotion==0)system("./beep1 /dev/input/event0");
                        faceInfo.append("口罩：").append(emotion==0?"无":"有").append("\r\n");
                    }
                }
                if(faceObject.contains("beauty")){
                    double age=faceObject.take("beauty").toDouble();
                    faceInfo.append("颜值：").append(QString::number(age));
                }
                //ui->textBrowser->setText(faceInfo);
                emit faceInfoOK(faceInfo, QRect(arr[1],arr[2],arr[3],arr[4]));
            }else{
                qDebug() << "don't have face";
            }
        }else{
            qDebug() << "don't have result";
        }
    }else{
        qDebug() << "JSON ERROR " << jsonErr.errorString();
    }
//    qDebug() << "脸部信息" << faceInfo;
    qDebug() << "faceInfo is received" ;
    reply->deleteLater();
}


MyNetManager::~MyNetManager()
{

}
