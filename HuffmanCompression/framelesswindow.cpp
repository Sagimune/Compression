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
#include<iostream>
#include<QLineEdit>
#include"compression.h"
using namespace std;
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

    QLineEdit *pathedit = new QLineEdit(contentWidget);
    pathedit->setGeometry(5,345,300,40);
    pathedit->setText("1.zip");
    pathedit->setStyleSheet("QLineEdit{border-radius:9px;}");
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


    mypushbutton *newfile = new mypushbutton(":/new/prefix1/Resoures/newfile.png");
    newfile->setGeometry(250,400,25,30);
    newfile->setParent(contentWidget);
    connect(newfile,&QPushButton::clicked,[=]()
    {
        zipcompression *test = new zipcompression;
        QString fileName = text->toPlainText();
        fileName.replace("file:///","*");
        char ch[255][1024];
        memset(ch,0,sizeof(ch));
        //fileName.remove("file:///");
        qDebug()<<fileName;
        if (!fileName.isEmpty())fileName = fileName.replace(QRegExp("/"), "\\\\");
        std::string stdfilename = fileName.toStdString();
        int order=stdfilename.find('*');
        fileName = QString::fromStdString(stdfilename);
        fileName.remove(0,1);
        stdfilename = fileName.toStdString();
        int i;
        order=stdfilename.find("*");
        for(i = 0;order<stdfilename.length();i++)
        {

            sprintf(test->filepath[i], "%s",(QString::fromStdString(stdfilename.substr(0,order)).remove('\n')).toStdString().data());
            std::string str = stdfilename.substr(0,order);
            int order2 =str.find_last_of("\\");
            sprintf(test->filename[i], "%s",str.substr(order2+1,str.size()-order2-1).data());
            fileName = QString::fromStdString(stdfilename);
            fileName.remove(0,order+1);
            stdfilename = fileName.toStdString();
            order=stdfilename.find("*");
            qDebug()<<"order: "<<order<<"order2: "<<order2;
            qDebug()<<"path"<<i<<" :"<<test->filepath[i];
            qDebug()<<"name"<<i<<" :"<<test->filename[i];
        }
        qDebug()<<QString::fromStdString(stdfilename)<<i;
        int order2 =stdfilename.find_last_of("\\");
        sprintf(test->filepath[i], "%s",(QString::fromStdString(stdfilename).remove('\n')).toStdString().data());
        sprintf(test->filename[i], "%s",stdfilename.substr(order2+1,stdfilename.size()-order2-1).data());
        qDebug()<<"path"<<i<<" :"<<test->filepath[i];
        qDebug()<<"name"<<i<<" :"<<test->filename[i];
        QByteArray ba = fileName.toLatin1(); // must
        //sprintf(ch, "%s", ba.data());

        //qDebug()<<ch;
        QString savepath=QFileDialog::getExistingDirectory(this," 选择一个目录 ","./",QFileDialog::ShowDirsOnly);
        savepath.append("\\\\");
        QString name = pathedit->text();
        if(name.isEmpty()) name="1.zip";
        savepath.append(name);
        if(!name.endsWith(".zip")) savepath.append(".zip");
        if (!savepath.isEmpty())savepath = savepath.replace(QRegExp("/"), "\\\\");
        else return;
        QByteArray da = savepath.toLatin1(); // must
        char sh[1024];
        sprintf(sh, "%s", da.data());
        qDebug()<<sh;
        test->compressionFile(sh,i+1);

    });
    newfile->show();

    mypushbutton *openfile = new mypushbutton(":/new/prefix1/Resoures/openfile.jpg");
    openfile->setGeometry(700,400,25,30);
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
    jieya->setGeometry(475,400,25,30);
    jieya->setParent(contentWidget);
    connect(jieya,&QPushButton::clicked,[=]()
    {
        zipcompression *test = new zipcompression;
        QString fileName = text->toPlainText();
        fileName.remove(0,8);
        char* ch;
        if (!fileName.isEmpty())fileName = fileName.replace(QRegExp("/"), "\\\\");
        QByteArray ba = fileName.toLatin1(); // must
        ch=ba.data();  //  zip文件路径
        qDebug()<<ch;
        QString savepath=QFileDialog::getExistingDirectory(this," 选择一个目录 ","./",QFileDialog::ShowDirsOnly);
        if (!savepath.isEmpty())savepath = savepath.replace(QRegExp("/"), "\\\\");

        qDebug()<<savepath;
        QByteArray da = savepath.toLatin1(); // must
        char * sh;
        sh=da.data();
        qDebug()<<sh;
        test->decompress(ch,sh);
        // must
    });
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
            newfile->setGeometry(250,400,25,30);
            openfile->setGeometry(700,400,25,30);
            zuixiaohua->setGeometry(1110,0,25,25);
            jieya->setGeometry(475,400,25,30);
            text->setGeometry(5,30,1180,300);
            i++;
        }
    });
    zuidahua->show();


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
