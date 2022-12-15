/********************************************************************************
** Form generated from reading UI file 'viewfiles.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWFILES_H
#define UI_VIEWFILES_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_viewfiles
{
public:
    QTreeView *treeView;

    void setupUi(QWidget *viewfiles)
    {
        if (viewfiles->objectName().isEmpty())
            viewfiles->setObjectName(QStringLiteral("viewfiles"));
        viewfiles->resize(1243, 547);
        treeView = new QTreeView(viewfiles);
        treeView->setObjectName(QStringLiteral("treeView"));
        treeView->setGeometry(QRect(10, 10, 1231, 421));

        retranslateUi(viewfiles);

        QMetaObject::connectSlotsByName(viewfiles);
    } // setupUi

    void retranslateUi(QWidget *viewfiles)
    {
        viewfiles->setWindowTitle(QApplication::translate("viewfiles", "Form", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class viewfiles: public Ui_viewfiles {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWFILES_H
