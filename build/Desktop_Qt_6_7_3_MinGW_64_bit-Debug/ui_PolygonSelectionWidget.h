/********************************************************************************
** Form generated from reading UI file 'PolygonSelectionWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_POLYGONSELECTIONWIDGET_H
#define UI_POLYGONSELECTIONWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PolygonWidget
{
public:
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *savebtn;
    QPushButton *clearbtn;
    QLabel *label;

    void setupUi(QWidget *PolygonWidget)
    {
        if (PolygonWidget->objectName().isEmpty())
            PolygonWidget->setObjectName("PolygonWidget");
        PolygonWidget->resize(800, 600);
        horizontalLayoutWidget = new QWidget(PolygonWidget);
        horizontalLayoutWidget->setObjectName("horizontalLayoutWidget");
        horizontalLayoutWidget->setGeometry(QRect(500, 540, 291, 41));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        savebtn = new QPushButton(horizontalLayoutWidget);
        savebtn->setObjectName("savebtn");

        horizontalLayout->addWidget(savebtn);

        clearbtn = new QPushButton(horizontalLayoutWidget);
        clearbtn->setObjectName("clearbtn");

        horizontalLayout->addWidget(clearbtn);

        label = new QLabel(horizontalLayoutWidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);


        retranslateUi(PolygonWidget);

        QMetaObject::connectSlotsByName(PolygonWidget);
    } // setupUi

    void retranslateUi(QWidget *PolygonWidget)
    {
        PolygonWidget->setWindowTitle(QCoreApplication::translate("PolygonWidget", "Polygon Selection", nullptr));
        savebtn->setText(QCoreApplication::translate("PolygonWidget", "\351\200\211\346\213\251\345\256\214\346\210\220", nullptr));
        clearbtn->setText(QCoreApplication::translate("PolygonWidget", "\346\270\205\347\251\272", nullptr));
        label->setText(QCoreApplication::translate("PolygonWidget", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PolygonWidget: public Ui_PolygonWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLYGONSELECTIONWIDGET_H
