#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QGraphicsDropShadowEffect>
#include "compression.h"

struct FramelessWindowPrivate;

class FramelessWindow : public QWidget {
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *contentWidget, QWidget *parent = 0);
    ~FramelessWindow();

    QPushButton *addButton;
    QPushButton *yasuoButton;
    QPushButton *jieyaButton;
    QVBoxLayout *layout;
    QTextEdit *text;
    QGraphicsDropShadowEffect *shadowEffect;
    QGridLayout *lo;

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

private:
    FramelessWindowPrivate *d;
};

#endif // FRAMELESSWINDOW_H
