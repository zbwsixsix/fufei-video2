#ifndef WINDOW1_H
#define WINDOW1_H

#include <QWidget>
#include "videoplayer.h"
#include "playerwidget.h"
namespace Ui {
class window1;
}

class window1 : public QWidget
{
    Q_OBJECT

public:
    explicit window1(QWidget *parent = nullptr);
    ~window1();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::window1 *ui;
};

#endif // WINDOW1_H
