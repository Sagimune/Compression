#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H
#pragma execution_character_set("utf-8")
#include<QMainWindow>
#include<QPushButton>
#include<QDebug>


#include <QObject>

class mypushbutton:public QPushButton
{
    Q_OBJECT
public:
    mypushbutton(QString phpath,QString pressImg="");
    QString phpath;
     QString pressImgPath;
     QPixmap pix;

    void icomshow(QString path);
    //弹跳特效
    void zoom1();//向下跳
    void zoom2();//向上跳
    void zoom(int upy=0,int dpy=0);//跳转函数

   // QMainWindow *p;

    //重写按钮按下和释放事件
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    int con;

signals:
    void myclicked();

};

#endif // MYPUSHBUTTON_H
