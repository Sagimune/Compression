#include "framelesswindow.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include<QFileDialog>
#include<QFile>
#include <QMouseEvent>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>
#include<qscreen.h>
#include"mypushbutton.h"
#include"viewfiles.h"
#include<QFileDialog>
struct FramelessWindowPrivate {
    FramelessWindowPrivate(QWidget *contentWidget) : contentWidget(contentWidget) {}

    QWidget *contentWidget;
    QPoint mousePressedPosition; // 鼠标按下时的坐标
    QPoint windowPositionAsDrag; // 鼠标按小时窗口左上角的坐标

};
int i = 1;
FramelessWindow::FramelessWindow(QWidget *contentWidget, QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);    // 去掉边框
    setAttribute(Qt::WA_TranslucentBackground); // 背景透明
    addButton = new QPushButton(" 添加 ");
    yasuoButton = new QPushButton(" 压缩 ");
    jieyaButton = new QPushButton(" 解压 ");
    layout = new QVBoxLayout();
    text = new QTextEdit(contentWidget);
    text->setGeometry(5,30,1180,300);
    text->show();
    text->setStyleSheet("QTextEdit{border-radius:9px;}");

    mypushbutton *exit = new mypushbutton(":/new/prefix1/Resoures/close.png");
    exit->setGeometry(1160,0,25,25);
    exit->setParent(contentWidget);
    connect(exit,&QPushButton::clicked,[=]()
    {
        this->close();
        contentWidget->close();
    });
    exit->show();

    mypushbutton *zuidahua = new mypushbutton(":/new/prefix1/Resoures/bigger.png");
    zuidahua->setGeometry(1135,0,25,25);
    zuidahua->setParent(contentWidget);
    zuidahua->show();

    mypushbutton *newfile = new mypushbutton(":/new/prefix1/Resoures/newfile.png");
    newfile->setGeometry(250,350,25,30);
    newfile->setParent(contentWidget);

    newfile->show();

    mypushbutton *openfile = new mypushbutton(":/new/prefix1/Resoures/openfile.jpg");
    openfile->setGeometry(700,350,25,30);
    openfile->setParent(contentWidget);
    openfile->show();
    connect(openfile,&QPushButton::clicked,[=]()
    {
        QString fileName = QFileDialog::getOpenFileName(this,tr(" 选择文件 "), "", tr("Text files (*.zip)"));
        char* ch;
        QByteArray ba = fileName.toLatin1(); // must
        ch=ba.data();
        qDebug()<<ch;
        viewfiles *view = new viewfiles(ch);
        view->show();
    });
    mypushbutton *zuixiaohua = new mypushbutton(":/new/prefix1/Resoures/hide.png");
    zuixiaohua->setGeometry(1110,0,25,25);
    zuixiaohua->setParent(contentWidget);
    zuixiaohua->show();
    connect(zuixiaohua,&QPushButton::clicked,[=]()
    {
        this->lower();
        contentWidget->lower();
    });
    mypushbutton *jieya = new mypushbutton(":/new/prefix1/Resoures/jieya.png");
    jieya->setGeometry(475,350,25,30);
    jieya->setParent(contentWidget);

    connect(zuidahua,&QPushButton::clicked,[=]()
    {
        if(i%2!=0)
        {
            this->showMaximized();
            contentWidget->showMaximized();
            exit->setGeometry(2525,0,25,25);
            zuidahua->setGeometry(2500,0,25,25);
            zuixiaohua->setGeometry(2475,0,25,25);
            text->setGeometry(5,30,2540,600);
            newfile->setGeometry(600,800,25,30);
            jieya->setGeometry(1200,800,25,30);
            openfile->setGeometry(1800,800,25,30);
            i++;
        }
        else
        {
            this->showNormal();
            contentWidget->showNormal();
            exit->setGeometry(1160,0,25,25);
            zuidahua->setGeometry(1135,0,25,25);
            newfile->setGeometry(250,350,25,30);
            openfile->setGeometry(700,350,25,30);
            zuixiaohua->setGeometry(1110,0,25,25);
            jieya->setGeometry(475,350,25,30);
            text->setGeometry(5,30,1180,300);
            i++;
        }
    });



    contentWidget->setLayout(layout);
    contentWidget->setObjectName("contentWidget");
    contentWidget->setStyleSheet("#contentWidget{background: lightblue; border-radius: 4px;}" // 定制圆角
                                 ".QLabel{background: blue;}.QTextEdit{background: white;}");



    // 创建无边框、有阴影、可拖动的窗口
    QObject::connect(addButton, &QPushButton::clicked, [&] {
        QString fileName = QFileDialog::getOpenFileName(this,QFileDialog::tr(" 选择文件 "), "");
        text->setText(fileName);
    });


    d = new FramelessWindowPrivate(contentWidget);

    // 添加阴影
    shadowEffect = new QGraphicsDropShadowEffect(contentWidget);
    shadowEffect->setColor(Qt::lightGray);
    shadowEffect->setBlurRadius(4); // 阴影的大小
    shadowEffect->setOffset(0, 0);
    contentWidget->setGraphicsEffect(shadowEffect);

    // 添加到窗口中
    lo = new QGridLayout();
    lo->addWidget(contentWidget, 0, 0);
    lo->setContentsMargins(4, 4, 4, 4); // 注意和阴影大小的协调
    setLayout(lo);


    //this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint| Qt::WindowMinimizeButtonHint);//清除所有|使用关闭|使用最小化

}

FramelessWindow::~FramelessWindow() {
    delete d;
}

void FramelessWindow::mousePressEvent(QMouseEvent *e) {
    // 记录鼠标按下时全局的位置和窗口左上角的位置
    d->mousePressedPosition = e->globalPos();
    d->windowPositionAsDrag = pos();
}

void FramelessWindow::mouseReleaseEvent(QMouseEvent *e) {
    Q_UNUSED(e)
    // 鼠标放开始设置鼠标按下的位置为 null，表示鼠标没有被按下
    d->mousePressedPosition = QPoint();
}

void FramelessWindow::mouseMoveEvent(QMouseEvent *e) {
    if (!d->mousePressedPosition.isNull()) {
        // 鼠标按下并且移动时，移动窗口, 相对于鼠标按下时的位置计算，是为了防止误差累积
        QPoint delta = e->globalPos() - d->mousePressedPosition;
        move(d->windowPositionAsDrag + delta);
    }
}
