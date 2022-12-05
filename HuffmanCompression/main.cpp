#include "framelesswindow.h"
#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include<QFileDialog>
#include<QFile>
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPushButton *addButton = new QPushButton(" 添加 ");
    QPushButton *yasuoButton = new QPushButton(" 压缩 ");
    QPushButton *jieyaButton = new QPushButton(" 解压 ");
    QVBoxLayout *layout = new QVBoxLayout();
    QTextEdit *text = new QTextEdit();
    layout->addWidget(text);
    layout->addWidget(addButton);
    layout->addWidget(yasuoButton);
    layout->addWidget(jieyaButton);
    QWidget *contentWidget = new QWidget();
    contentWidget->setLayout(layout);
    contentWidget->setObjectName("contentWidget");
    contentWidget->setStyleSheet("#contentWidget{background: lightgray; border-radius: 4px;}" // 定制圆角
                                 ".QLabel{background: gray;}.QTextEdit{background: white;}");
    // 创建无边框、有阴影、可拖动的窗口
    FramelessWindow *window = new FramelessWindow(contentWidget);
    window->resize(1200,600);

    QObject::connect(addButton, &QPushButton::clicked, [&] {
        QString fileName = QFileDialog::getOpenFileName(window,QFileDialog::tr(" 选择文件 "), "");
        text->setText(fileName);
    });
    window->show();
    // 创建包含主要控件的 Widget

    return app.exec();
}
