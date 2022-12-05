#include "mainwindow.h"

#include <QApplication>
#include "compression.h"

/*
 * 使用示例
 * Compression T;
    T.Zip("D:/tmp/qwq.pdf");
    T.UnZip("D:/tmp/qwq.pdf.huffmanzip");
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
