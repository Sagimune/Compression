#ifndef VIEWFILES_H
#define VIEWFILES_H

#include <QWidget>
#include"zipcompression.h"
namespace Ui {
class viewfiles;
}

class viewfiles : public QWidget
{
    Q_OBJECT

public:

    explicit viewfiles(char *zipfilename,QWidget *parent = nullptr);
    zipcompression zipfile;

    ~viewfiles();
    QString path[10]={""};
    QIcon fileExtensionIcon(const QString & extension) const ;


private:
    Ui::viewfiles *ui;
};

#endif // VIEWFILES_H
