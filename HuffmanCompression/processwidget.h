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

private:
    Ui::processwidget *ui;

};

#endif // PROCESSWIDGET_H
