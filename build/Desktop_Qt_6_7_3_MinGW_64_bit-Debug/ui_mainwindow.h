/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "videoslider.h"
#include "videowidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QStackedWidget *playWidget;
    QWidget *openFilePage;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *openFileBtn;
    QWidget *videoPage;
    QGridLayout *gridLayout;
    VideoWidget *videoWidget;
    QHBoxLayout *horizontalLayout;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *playBtn;
    QPushButton *stopBtn;
    QPushButton *pushButton_2;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_3;
    VideoSlider *currentSlider;
    QLabel *currentLabel;
    QLabel *label_2;
    QLabel *durationLabel;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *muteBtn;
    VideoSlider *volumnSlider;
    QLabel *volumnLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(822, 658);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        playWidget = new QStackedWidget(centralwidget);
        playWidget->setObjectName("playWidget");
        playWidget->setMinimumSize(QSize(0, 300));
        openFilePage = new QWidget();
        openFilePage->setObjectName("openFilePage");
        horizontalLayout_5 = new QHBoxLayout(openFilePage);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        openFileBtn = new QPushButton(openFilePage);
        openFileBtn->setObjectName("openFileBtn");
        openFileBtn->setMaximumSize(QSize(70, 16777215));

        horizontalLayout_5->addWidget(openFileBtn);

        playWidget->addWidget(openFilePage);
        videoPage = new QWidget();
        videoPage->setObjectName("videoPage");
        gridLayout = new QGridLayout(videoPage);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        videoWidget = new VideoWidget(videoPage);
        videoWidget->setObjectName("videoWidget");

        gridLayout->addWidget(videoWidget, 0, 0, 1, 1);

        playWidget->addWidget(videoPage);

        verticalLayout->addWidget(playWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        playBtn = new QPushButton(centralwidget);
        playBtn->setObjectName("playBtn");
        playBtn->setEnabled(false);
        playBtn->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_2->addWidget(playBtn);

        stopBtn = new QPushButton(centralwidget);
        stopBtn->setObjectName("stopBtn");
        stopBtn->setEnabled(false);
        stopBtn->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_2->addWidget(stopBtn);

        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName("pushButton_2");

        horizontalLayout_2->addWidget(pushButton_2);

        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");

        horizontalLayout_2->addWidget(pushButton);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        currentSlider = new VideoSlider(centralwidget);
        currentSlider->setObjectName("currentSlider");
        currentSlider->setEnabled(false);
        currentSlider->setMinimumSize(QSize(250, 0));
        currentSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_3->addWidget(currentSlider);

        currentLabel = new QLabel(centralwidget);
        currentLabel->setObjectName("currentLabel");
        currentLabel->setMinimumSize(QSize(0, 0));
        currentLabel->setMaximumSize(QSize(70, 16777215));
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        currentLabel->setFont(font);
        currentLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(currentLabel);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName("label_2");
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(label_2);

        durationLabel = new QLabel(centralwidget);
        durationLabel->setObjectName("durationLabel");
        durationLabel->setMaximumSize(QSize(70, 16777215));
        durationLabel->setFont(font);
        durationLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_3->addWidget(durationLabel);


        horizontalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        muteBtn = new QPushButton(centralwidget);
        muteBtn->setObjectName("muteBtn");
        muteBtn->setEnabled(false);
        muteBtn->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_4->addWidget(muteBtn);

        volumnSlider = new VideoSlider(centralwidget);
        volumnSlider->setObjectName("volumnSlider");
        volumnSlider->setEnabled(false);
        volumnSlider->setMinimumSize(QSize(100, 0));
        volumnSlider->setMaximumSize(QSize(100, 16777215));
        volumnSlider->setMaximum(100);
        volumnSlider->setValue(100);
        volumnSlider->setOrientation(Qt::Orientation::Horizontal);

        horizontalLayout_4->addWidget(volumnSlider);

        volumnLabel = new QLabel(centralwidget);
        volumnLabel->setObjectName("volumnLabel");
        volumnLabel->setMinimumSize(QSize(30, 0));
        volumnLabel->setMaximumSize(QSize(30, 16777215));
        volumnLabel->setFont(font);
        volumnLabel->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_4->addWidget(volumnLabel);


        horizontalLayout_2->addLayout(horizontalLayout_4);


        horizontalLayout->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 822, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        playWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        openFileBtn->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        playBtn->setText(QCoreApplication::translate("MainWindow", "\346\222\255\346\224\276", nullptr));
        stopBtn->setText(QCoreApplication::translate("MainWindow", "\345\201\234\346\255\242", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\345\277\253\351\200\200\345\210\26030s", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\350\267\263\345\210\26060s", nullptr));
        currentLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "/", nullptr));
        durationLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00", nullptr));
        muteBtn->setText(QCoreApplication::translate("MainWindow", "\351\235\231\351\237\263", nullptr));
        volumnLabel->setText(QCoreApplication::translate("MainWindow", "100", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
