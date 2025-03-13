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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PolygonWidget
{
public:

    void setupUi(QWidget *PolygonWidget)
    {
        if (PolygonWidget->objectName().isEmpty())
            PolygonWidget->setObjectName("PolygonWidget");
        PolygonWidget->resize(800, 600);

        retranslateUi(PolygonWidget);

        QMetaObject::connectSlotsByName(PolygonWidget);
    } // setupUi

    void retranslateUi(QWidget *PolygonWidget)
    {
        PolygonWidget->setWindowTitle(QCoreApplication::translate("PolygonWidget", "Polygon Selection", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PolygonWidget: public Ui_PolygonWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_POLYGONSELECTIONWIDGET_H
