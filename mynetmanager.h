#ifndef MYNETMANAGER_H
#define MYNETMANAGER_H

#include <QWidget>
#include <QThread>
#include <QNetworkAccessManager>

class MyNetManager : public QWidget
{
    Q_OBJECT
public:
    int mask_flag;
    explicit MyNetManager(QWidget *parent = nullptr);
    ~MyNetManager();
    void beginFaceDetect(QImage img);

    QString faceInfo;
private:
    void imgReply(QNetworkReply *reply); // 通过百度数据信息返回一个完整的字符串
    void tokenReply(QNetworkReply *reply); // 密钥获取
    QString accessToken;
    QNetworkAccessManager *m_pManager;
    QNetworkAccessManager *imgManager;
    QSslConfiguration sslConfig;
    QThread *childThread;
signals:
    void faceInfoOK(const QString& f, QRect rect);
    void beginFaceDetectwork(QImage img,QString accessToken,QSslConfiguration sslConfig,QThread* overThread);

};

#endif // MYNETMANAGER_H
