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


void processwidget::init(int method)
{
    ui->fileprocess->setValue(0);
    ui->allprocess->setValue(0);
    ui->frame->hide();

    if(method == 0)
    {
        ui->label_3->setText(QString("压缩时间"));
    }
    else if(method == 1)
    {
        ui->label_3->setText(QString("解压时间"));
    }
}

void processwidget::finish(double alltime, double compressrate)
{
    ui->fileprocess->setValue(100.0);
    ui->allprocess->setValue(100.0);
    char buffer[255];
    sprintf(buffer, "%.2lf", alltime);
    ui->compresstime->setText(buffer);
    sprintf(buffer, "%.2lf\%", compressrate);
    ui->compressrate->setText(buffer);
    ui->frame->show();
}

void processwidget::on_finish_clicked()
{
    this->init(0);
    this->hide();
}

void processwidget::movefileprocess(double value)
{
    qDebug() << "recv fileprocess value: " << value;
    ui->fileprocess->setValue(value);
}
void processwidget::moveallprocess(double value)
{
    qDebug() << "recv allprocess value: " << value;
    ui->allprocess->setValue(value);
}
