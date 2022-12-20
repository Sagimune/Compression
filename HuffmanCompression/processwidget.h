#ifndef PROCESSWIDGET_H
#define PROCESSWIDGET_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class processwidget;
}

class processwidget : public QWidget
{
    Q_OBJECT

public:
    explicit processwidget(QWidget *parent = nullptr);
    ~processwidget();

    void init();
    void finish(double alltime, double compressrate);

public slots:
    void on_finish_clicked();
    void movefileprocess(double value);
    void moveallprocess(double value);

private:
    Ui::processwidget *ui;

};

#endif // PROCESSWIDGET_H
