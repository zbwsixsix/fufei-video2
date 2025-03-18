#include "window1.h"
#include "ui_window1.h"

window1::window1(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::window1)
{
    ui->setupUi(this);
}

window1::~window1()
{
    delete ui;
}

void window1::on_pushButton_clicked()
{
    PlayerWidget *w = new PlayerWidget(); // 动态分配
    w->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    QString filename1 = "d:/Users/zhangbowen/Videos/02test.mp4";
    w->getPlayer()->setFilename(filename1);
    w->getPlayer()->play();
     w->show();
}


void window1::on_pushButton_2_clicked()
{
    PlayerWidget *w = new PlayerWidget(); // 动态分配
    w->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    w->show();
    QString filename1 = "d:/Users/zhangbowen/Videos/60min2.mp4";
    w->getPlayer()->setFilename(filename1);
    w->getPlayer()->play();
}


void window1::on_pushButton_3_clicked()
{
    PlayerWidget *w = new PlayerWidget(); // 动态分配
    w->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
    w->show();
    QString filename1 = "d:/Users/zhangbowen/Videos/4min.mp4";
    w->getPlayer()->setFilename(filename1);
    w->getPlayer()->play();
}

