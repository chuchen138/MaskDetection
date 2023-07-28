#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QImage>
#include <QRect>
#include "v412.h"
#include "mynetmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void recvImage(QImage image);

    void on_pushButton_3_clicked();

    void on_ptn_close_clicked();

private:
    int read_sysfs_float(const char *device, const char *filename, float *val);
    int read_sysfs_int(const char *device, const char *filename, int *val);
    QString temCollect();
    QString humCollect();

    Ui::Widget *ui;
    V4l2Api *vapip;
    V4l2Api vapi;
    bool flag;
    QImage Saveimage;
    MyNetManager *mynetmanager;
    QRect rect;
};
#endif // WIDGET_H
