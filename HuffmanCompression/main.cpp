#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTime>
#include "ccrashstack.h"

long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    //addr2line.exe -f -e BreakpadDemo.exe 00406621
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();

//    TCHAR my_documents[MAX_PATH];
//    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, my_documents);
//    QString file_path = QString::fromWCharArray(my_documents);
    QDir dir;
    QString file_path = dir.currentPath();

    QDir *folder_path = new QDir;
    bool exist = folder_path->exists(file_path.append("\\crash"));
    if(!exist)
    {
        folder_path->mkdir(file_path);
    }
    delete folder_path;
    folder_path = nullptr;

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz");
    QString sFileName = file_path + "\\" + time +  "crash.log";

    QFile file(sFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write(sCrashInfo.toUtf8());
        file.close();
    }

    return   EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
