#include "mainwindow.h"

#include <QApplication>
#include "compression.h"
#include "test.h"

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
    test t;
    Compression TT;
    TT.Zip("D:/tmp/qwq.txt");
    Compression T;
    T.UnZip("D:/tmp/qwq.txt.huffmanzip");
    w.show();
    return a.exec();
}
