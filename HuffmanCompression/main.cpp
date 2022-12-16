#include "framelesswindow.h"
#include "compression.h"
#include "test.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget *contentWidget = new QWidget;
    FramelessWindow *window = new FramelessWindow(contentWidget);
    window->resize(1200,600);
    window->show();
    // 创建包含主要控件的 Widget

    test();

    return app.exec();
}
