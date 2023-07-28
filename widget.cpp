#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    ,flag(true)
{
    ui->setupUi(this);
    vapip = nullptr;
    mynetmanager = new MyNetManager();
    mynetmanager->mask_flag=  1;
    connect(mynetmanager,&MyNetManager::faceInfoOK,this,[&](const QString& f, QRect r){
            if(f=="") ui->textBrowser->setText("没有识别到人脸");
            else ui->textBrowser->setText(f);
            this->rect = r;
    });
}

Widget::~Widget()
{
//    vapi->v4l2_stop();
//    vapi->exit();
//    vapi->wait();
//    delete vapi;
//    vapi.v4l2_stop();

    delete ui;
}

void Widget::recvImage(QImage image)
{
    Saveimage = image;
    QPixmap mmp = QPixmap::fromImage(image);
    ui->label->setPixmap(mmp);
    ui->label->setScaledContents(true);
}

void Widget::on_pushButton_clicked()
{
    system("echo 1 > /sys/class/leds/user1/brightness");
    system("echo 1 > /sys/class/leds/user2/brightness");
    QString tem = temCollect();
    QString hum = humCollect();
    QString ssstr = "\n温度 : "+tem + "\n湿度 : "+hum;
    ui->textBrowser->setText("\n温度 : "+tem);
}

void Widget::on_pushButton_2_clicked()
{
    mynetmanager->beginFaceDetect(this->Saveimage);
    QPixmap mmp = QPixmap::fromImage(Saveimage);
    ui->label_2->setPixmap(mmp);
    ui->label_2->setScaledContents(true);
    system("echo 0 > /sys/class/leds/user1/brightness");
    system("echo 0 > /sys/class/leds/user2/brightness");
//    if(mynetmanager->mask_flag==0) {
//        system("./beep1 /dev/input/event0");
//        mynetmanager->mask_flag=  1;
//    }
}

void Widget::on_pushButton_3_clicked()
{
    connect(&vapi, &V4l2Api::sendImage, this, &Widget::recvImage);
    vapi.start();
//    if(vapip != nullptr) return;
//    vapip = new V4l2Api;
//    connect(vapip, &V4l2Api::sendImage, this, &Widget::recvImage);
//    vapip->start();
}

void Widget::on_ptn_close_clicked()
{
//    system("./beep0 /dev/input/event0");
    disconnect(&vapi, &V4l2Api::sendImage, this, &Widget::recvImage);
    ui->label->clear();
    ui->label_2->clear();
    ui->textBrowser->clear();
//    if(vapip == nullptr) return;
//    disconnect(vapip, &V4l2Api::sendImage, this, &Widget::recvImage);
//    vapip->exit();
//    vapip->wait();
//    delete vapip;
//    vapip = nullptr;
//    qDebug() << "delete over";
//    vapi.v4l2_stop();
}


/*温度湿度*/
QString Widget::temCollect()
{
    int temp_raw = 0;
    int temp_offset = 0;
    float temp_scale = 0;
    float tem_float =0;
    QString tem;
    const char *device1 ="iio:device0";//温湿度
    /*read temp data*/
    read_sysfs_int(device1, "in_temp_raw", &temp_raw);
    read_sysfs_int(device1, "in_temp_offset", &temp_offset);
    read_sysfs_float(device1, "in_temp_scale", &temp_scale);
    tem_float =(temp_raw + temp_offset) * temp_scale / 1000;
    tem =QString::number(tem_float,'f', 2);
    return tem;
}
QString Widget::humCollect()
{
    int hum_raw = 0;
    int hum_offset = 0;
    float hum_scale = 0;
    float hum_float =0;
    QString hum;
    const char *device1 ="iio:device0";//温湿度
    read_sysfs_int(device1, "in_humidityrelative_raw", &hum_raw);
    read_sysfs_int(device1, "in_humidityrelative_offset", &hum_offset);
    read_sysfs_float(device1, "in_humidityrelative_scale", &hum_scale);
    hum_float = (hum_raw + hum_offset) * hum_scale / 1000;
    hum =QString::number(hum_float,'f', 2);
    return hum;
}

int Widget::read_sysfs_float(const char *device, const char *filename, float *val)
{
    int ret = 0;
    FILE *sysfsfp;
    char temp[128];
    memset(temp, '0', 128);
    ret = sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
    if (ret < 0)
        goto error;
    sysfsfp = fopen(temp, "r");
    if (!sysfsfp)
    {
        ret = -errno;
        goto error;
    }
    errno = 0;
    if (fscanf(sysfsfp, "%f\n", val) != 1)
    {
        ret = errno ? -errno : -ENODATA;
        if (fclose(sysfsfp))
            perror("read_sysfs_float(): Failed to close dir");
        goto error;
    }
    if (fclose(sysfsfp))
        ret = -errno;
error:
    return ret;
}
int Widget::read_sysfs_int(const char *device, const char *filename, int *val)
{
    int ret = 0;
    FILE *sysfsfp;
    char temp[128];
    memset(temp, '0', 128);
    ret = sprintf(temp, "/sys/bus/iio/devices/%s/%s", device, filename);
    if (ret < 0)
        goto error;
    sysfsfp = fopen(temp, "r");
    if (!sysfsfp)
    {
        ret = -errno;
        goto error;
    }
    errno = 0;
    if (fscanf(sysfsfp, "%d\n", val) != 1)
    {
        ret = errno ? -errno : -ENODATA;
        if (fclose(sysfsfp))
            perror("read_sysfs_float(): Failed to close dir");
        goto error;
    }
    if (fclose(sysfsfp))
        ret = -errno;
error:
    return ret;
}
