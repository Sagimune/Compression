#include "viewfiles.h"
#include "ui_viewfiles.h"
#include"zipcompression.h"
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
viewfiles::viewfiles(char* zipfilename,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::viewfiles)
{
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(this);
    zipfile.viewzip(zipfilename);
    QStandardItem *item[255][255];
    int foldernum=0;
    int j=0;
    int folderlength;
    QString foldername="_";
    model->setHorizontalHeaderLabels(QStringList()<<"name"<<""<<" 压缩后大小 "<<" 类型 "<<" 压缩方式 ");
    qDebug()<<"cp2";
    for(int i = 0;i<zipfile.filecount;i++)
    {
        qDebug()<<i<<"=i";
        qDebug()<<QString(QLatin1String(zipfile.filename[i]));
        QString str=QString(QLatin1String(zipfile.filename[i]));
        if(str.endsWith('\\'))
        {
            qDebug()<<"file"<<i<<"is a folder";
            j = 0;
            QStandardItem *item = new QStandardItem(str.remove(str.length()-1,1));
            QFileIconProvider icon_provider;
            QIcon icon = icon_provider.icon(QFileIconProvider::Folder);
            item->setIcon(icon);
            model->setItem(foldernum++,0,item);
            folderlength = str.length()+1;
            foldername=str; qDebug()<<foldername;
        }
        else if(str.startsWith(foldername))
        {
            qDebug()<<"file"<<i<<"is under a folder";
            item[foldernum-1][i] = new QStandardItem(str.remove(0,folderlength));
            model->item(foldernum-1,0)->setChild(j++,0,item[foldernum-1][i]);
            std::string stdpath = str.toStdString();
            int dotnum = stdpath.find(".");
            QIcon icon = fileExtensionIcon(str.remove(0,dotnum));
            item[foldernum-1][i]->setIcon(icon);
            qDebug()<<"NO."<<i<<"output completely";
        }
        else
        {
            qDebug()<<"file"<<i<<" is a single file ";
            QStandardItem *item = new QStandardItem(str);
            model->setItem(foldernum++,0,item);
            std::string stdpath = str.toStdString();
            int dotnum = stdpath.find(".");
            QIcon icon = fileExtensionIcon(str.remove(0,dotnum));
            item->setIcon(icon);
        }
    }
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
