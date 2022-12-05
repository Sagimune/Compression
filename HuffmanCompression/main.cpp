#include "mainwindow.h"

#include <QApplication>
#include "compression.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Compression TT;
    TT.Zip("D:/tmp/qwq.txt");
    Compression T;
    T.UnZip("D:/tmp/qwq.txt.huffmanzip");
    w.show();
    return a.exec();
}
