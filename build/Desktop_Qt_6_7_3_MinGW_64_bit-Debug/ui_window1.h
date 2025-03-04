/********************************************************************************
** Form generated from reading UI file 'window1.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOW1_H
#define UI_WINDOW1_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_window1
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *window1)
    {
        if (window1->objectName().isEmpty())
            window1->setObjectName("window1");
        window1->resize(633, 463);
        pushButton = new QPushButton(window1);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(120, 200, 93, 28));
        pushButton_2 = new QPushButton(window1);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(320, 200, 93, 28));

        retranslateUi(window1);

        QMetaObject::connectSlotsByName(window1);
    } // setupUi

    void retranslateUi(QWidget *window1)
    {
        window1->setWindowTitle(QCoreApplication::translate("window1", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("window1", "\346\211\223\345\274\200\350\247\206\351\242\2211", nullptr));
        pushButton_2->setText(QCoreApplication::translate("window1", "\346\211\223\345\274\200\350\247\206\351\242\2212", nullptr));
    } // retranslateUi

};

namespace Ui {
    class window1: public Ui_window1 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOW1_H
