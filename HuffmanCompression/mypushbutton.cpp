#include "mypushbutton.h"

#include<QIcon>
#include<QPropertyAnimation>//动画特效头文件
#include<QRect>
class MainScene;
void mypushbutton::icomshow(QString path)
{
    bool ret=pix.load(path);
    if(!ret){
        qDebug()<<"图片加载失败";
        return;
    }

    this->setFixedSize(pix.width(),pix.height());/* 设置图片固定大小 */
    this->setStyleSheet(QString("QPushButton{border-image: url(%1);}").arg(path));/* 设置不规则图片样式 */
//    this->setIcon(pix);/* 设置图标 */
//    this->setIconSize(QSize(pix.width(),pix.height()));/* 设置图标大小 */
}
mypushbutton::mypushbutton(QString phpath,QString pressImg):phpath(phpath)
{
     this->pressImgPath=pressImg;
        icomshow(phpath);
}
void mypushbutton::zoom1()
{
   zoom(10,0);
}

void mypushbutton::zoom2()
{
    zoom(-10,0);
}

void mypushbutton::zoom(int upy,int dpy)
{
    QPropertyAnimation *animation=new QPropertyAnimation(this,"geometry");/* 创建动态对象 */
    animation->setDuration(200);// 设置动画时间间隔
    //QRect(this->x(),this-y(),this->width(),this->height());
    animation->setStartValue(QRect(QPoint(this->x(),this->y()+upy),QPoint(this->width(),this->height())));/* 起始位置 */
    animation->setEndValue(QRect(QPoint(this->x(),this->y()+dpy),QPoint(this->width(),this->height())));/* 起始位置 */
    animation->setEasingCurve(QEasingCurve::OutBounce);/* 设置弹跳效果 */
    animation->start();/* 设置开始弹跳 */
}

void mypushbutton::mousePressEvent(QMouseEvent *e)
{
    if(this->pressImgPath!=""){//传入的按下图片不为空，说明需要有按下状态切换图片
        icomshow(this->pressImgPath);
    }
    // 让父类执行其他内容
   // this->zoom1();
    return QPushButton::mousePressEvent(e);

}

void mypushbutton::mouseReleaseEvent(QMouseEvent *e)
{
    if(this->pressImgPath!=""){// 传入的按下图片不为空，说明需要切换成初始的图片
        icomshow(this->phpath);
    }
    //this->zoom2();
    return QPushButton::mouseReleaseEvent(e);
}
