#include "framelesswindow.h"
#include "compression.h"
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
    QWidget *contentWidget = new QWidget;
    FramelessWindow *window = new FramelessWindow(contentWidget);
    window->resize(1200,600);
    window->show();
    // 创建包含主要控件的 Widget

    return app.exec();
}
