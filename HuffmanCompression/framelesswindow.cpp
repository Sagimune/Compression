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
/*圆角窗口的设置将原本的窗口隐藏，导入另外一个窗口，同时将本来的窗口
隐藏，没有使用ui进行布局，直接安排位置，最大化，最小化还有关闭按钮将
本来窗口上的隐藏，同时自己进行重写，同时将各种功能按钮进行连接*/
FramelessWindow::FramelessWindow(QWidget *contentWidget, QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint);      // 去掉边框
    setAttribute(Qt::WA_TranslucentBackground);   // 背景透明
    addButton = new QPushButton(" 添加 ");        //添加按钮
    yasuoButton = new QPushButton(" 压缩 ");      //压缩按钮
    jieyaButton = new QPushButton(" 解压 ");      //解压按钮
    layout = new QVBoxLayout();
    text = new QTextEdit(contentWidget);        //添加文本框
    text->setGeometry(5,30,1180,300);           //确定文本框大小
    text->show();
    text->setStyleSheet("QTextEdit{border-radius:9px;}");//文本框圆角设置

    QLineEdit *pathedit = new QLineEdit(contentWidget);
    pathedit->setGeometry(5,345,300,40);
    pathedit->setText("1.huffmanzip");
    pathedit->setStyleSheet("QLineEdit{border-radius:9px;}");
    /*定义一个文本框作为新建压缩包的命名，并将新建的压缩包命名为1.huffmanzip，可以在程序中自定义压缩包的名字*/
    mypushbutton *exit = new mypushbutton(":/new/prefix1/Resoures/close.png");
    exit->setGeometry(1160,0,25,25);
    exit->setParent(contentWidget);
    connect(exit,&QPushButton::clicked,[=]()
    {
        this->close();
        contentWidget->close();
    });
    exit->show();
    /*在去除自带的关闭按钮后，自己设计关闭按钮*/
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
        fileName.remove("\n");
        if(fileName.isEmpty()) return;
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

        QString savepath=QFileDialog::getExistingDirectory(this," 选择一个目录 ","./",QFileDialog::ShowDirsOnly);
        savepath.append("\\\\");
        QString name = pathedit->text();
        if(name.isEmpty()) name="1.huffmanzip";
        savepath.append(name);
        if(!name.endsWith(".huffmanzip")) savepath.append(".huffmanzip");
        if (!savepath.isEmpty())savepath = savepath.replace(QRegExp("/"), "\\\\");
        else return;
        QByteArray da = savepath.toLatin1(); // must
        char sh[1024];
        sprintf(sh, "%s", da.data());
        qDebug()<<sh;
        test->compressionFile(sh,i+1);
        text->clear();

    });
    newfile->show();
    /*新建文件夹按钮，读取文本框中的单个或多个文件路径，和自己选择压缩文件的输出路径，包含对于读取路径的处理使之符合函数的要求
    自己选择文件输出的路径是通过新建一个窗口来实现的，方便使用者选择输出路径*/

    mypushbutton *openfile = new mypushbutton(":/new/prefix1/Resoures/openfile.jpg");
    openfile->setGeometry(700,400,25,30);
    openfile->setParent(contentWidget);
    openfile->show();
    connect(openfile,&QPushButton::clicked,[=]()
    {
        QString fileName = QFileDialog::getOpenFileName(this,tr(" 选择文件 "), "", tr("Text files (*.huffmanzip)"));
        char* ch;
        QByteArray ba = fileName.toLatin1(); // must
        ch=ba.data();
        qDebug()<<ch;
        viewfiles *view = new viewfiles(ch);
        view->show();
    });
    /*对于文本框中的压缩包进行观看，显示文本框中压缩包的内容*/
    mypushbutton *zuixiaohua = new mypushbutton(":/new/prefix1/Resoures/hide.png");
    zuixiaohua->setGeometry(1110,0,25,25);
    zuixiaohua->setParent(contentWidget);
    zuixiaohua->show();
    connect(zuixiaohua,&QPushButton::clicked,[=]()
    {
        this->lower();
        contentWidget->lower();
    });
    /*因为删掉了隐藏窗口按钮，所以在这里重新设计并实现窗口的隐藏*/
    mypushbutton *jieya = new mypushbutton(":/new/prefix1/Resoures/jieya.png");
    jieya->setGeometry(475,400,25,30);
    jieya->setParent(contentWidget);
    connect(jieya,&QPushButton::clicked,[=]()
    {
        zipcompression *test = new zipcompression;
        QString fileName = text->toPlainText();
        if(fileName.isEmpty()) return;
        fileName.remove(0,8);
        char* ch;
        if (!fileName.isEmpty())fileName = fileName.replace(QRegExp("/"), "\\\\");
        QByteArray ba = fileName.toLatin1(); // must
        ch=ba.data();  //  zip文件路径
        qDebug()<<ch;
        QString savepath=QFileDialog::getExistingDirectory(this," 选择一个目录 ","./",QFileDialog::ShowDirsOnly);
        if (!savepath.isEmpty())savepath = savepath.replace(QRegExp("/"), "\\\\");
        else return;
        savepath =savepath+"\\\\";
        qDebug()<<savepath;
        QByteArray da = savepath.toLatin1(); // must
        char * sh;
        sh=da.data();
        qDebug()<<sh;
        //QWidget* decompression = new QWidget;
        //decompression->resize(640,360);

        //decompression->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
        //decompression->show();
        test->decompress(ch,sh);

        text->clear();

        // must
    });
    /*解压按钮，读取文本框中的压缩包名称，并选择解压文件的输出路径，在文件解压时会有进度条来提示解压什么时候完成*/
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
            pathedit->setGeometry(5,670,500,60);
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
            pathedit->setGeometry(5,345,300,40);
            i++;
        }
    });
    zuidahua->show();
    /*删除了自带的最大化按钮，为了符合整体程序的设计风格，对于最大化按钮进行重新设计，
     在初次点击最大化按钮会将程序最大化，在此点击会恢复默认大小，通过一个计数器来实现对于最大化和恢复默认信号的识别*/

    contentWidget->setLayout(layout);
    contentWidget->setObjectName("contentWidget");
    contentWidget->setStyleSheet("#contentWidget{background: lightblue; border-radius: 4px;}" // 定制圆角
                                 ".QLabel{background: blue;}.QTextEdit{background: white;}");
    /*对于传入的窗口进行阴影的添加，设计窗口*/


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
