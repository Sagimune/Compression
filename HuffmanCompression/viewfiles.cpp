#include "viewfiles.h"
#include "ui_viewfiles.h"
#include<QTextEdit>
#include<QFileDialog>
#include<QFileSystemModel>
#include<QStandardItem>
#include<QDebug>
#include<QtWin>
#include<QIcon>
#include<QFileIconProvider>
#include<QCoreApplication>
#include<QDir>
#include<QTemporaryFile>
#include<QLabel>
#include<QString>
viewfiles::viewfiles(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::viewfiles)
{
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(this);
    path[0]="111\\";
    path[1]="111\\nul2.txt";
    path[2]="include.zip";
    path[3]="mul1.txt";
    QStandardItem *item[10][10];
    int foldernum=0;
    int j=0;
    int folderlength;
    QString foldername;
    model->setHorizontalHeaderLabels(QStringList()<<"name"<<""<<" 压缩后大小 "<<" 类型 "<<" 压缩方式 ");
    QPixmap pixmap;
    for(int i = 0;i<4;i++)
    {
        qDebug()<<i<<"=i";
        if(path[i].endsWith('\\'))
        {
            j = 0;
            QStandardItem *item = new QStandardItem(path[i].remove(path[i].length()-1,1));
            QFileIconProvider icon_provider;
            QIcon icon = icon_provider.icon(QFileIconProvider::Folder);
            item->setIcon(icon);
            model->setItem(foldernum++,0,item);
            folderlength = path[i].length()+1;
            foldername=path[i]; qDebug()<<foldername;
        }
        else if(path[i].startsWith(foldername))
        {
            item[foldernum-1][i] = new QStandardItem(path[i].remove(0,folderlength));
            model->item(foldernum-1,0)->setChild(j++,0,item[foldernum-1][i]);
            std::string stdpath = path[i].toStdString();
            int dotnum = stdpath.find(".");
            QIcon icon = fileExtensionIcon(path[i].remove(0,dotnum));
            item[foldernum-1][i]->setIcon(icon);
            qDebug()<<"NO."<<i<<"output completely";
        }
        else
        {

            QStandardItem *item = new QStandardItem(path[i]);
            model->setItem(foldernum++,0,item);
            std::string stdpath = path[i].toStdString();
            int dotnum = stdpath.find(".");
            QIcon icon = fileExtensionIcon(path[i].remove(0,dotnum));
            item->setIcon(icon);
        }
    }
    qDebug()<<"cp3";
    ui->treeView->setModel(model);

    ui->treeView->expandAll();


}
QIcon viewfiles::fileExtensionIcon(const QString & extension) const
{
QFileIconProvider icon_provider;
QIcon icon;
QTemporaryFile tmp_file(QDir::tempPath() + QDir::separator() +
QCoreApplication::applicationName() + "_XXXXXX" + extension);
tmp_file.setAutoRemove(false);

if(tmp_file.open())
{
QString file_name = tmp_file.fileName();
tmp_file.write(QByteArray());
tmp_file.close();
icon = icon_provider.icon(QFileInfo(file_name));
return icon;
tmp_file.remove();
}

else
{
qCritical()<<QString("failed to write temporary file %1") .arg(tmp_file.fileName());
}
}

viewfiles::~viewfiles()
{
    delete ui;
}
