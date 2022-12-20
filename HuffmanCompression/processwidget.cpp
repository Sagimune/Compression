#include "processwidget.h"
#include "ui_processwidget.h"

processwidget::processwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::processwidget)
{
    ui->setupUi(this);
    ui->frame->hide();

    ui->fileprocess->setMaximum(100);
    ui->fileprocess->setMinimum(0);
    ui->fileprocess->setValue(0);

    ui->allprocess->setMaximum(100);
    ui->allprocess->setMinimum(0);
    ui->allprocess->setValue(0);

}

processwidget::~processwidget()
{
    delete ui;
}



