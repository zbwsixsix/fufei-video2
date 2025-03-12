#include "PolygonSelectionWidget.h"
#include "ui_PolygonSelectionWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QDir>

PolygonSelectionWidget::PolygonSelectionWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PolygonWidget)
{
    ui->setupUi(this);

    // 检查按钮是否为空
    if (!ui->clearbtn) qDebug() << "Clear button is null!";
    if (!ui->savebtn) qDebug() << "Save button is null!";

    // 连接信号与槽
    bool clearConnected = connect(ui->clearbtn, &QPushButton::clicked, this, &PolygonSelectionWidget::clearPoints);
    bool saveConnected = connect(ui->savebtn, &QPushButton::clicked, this, &PolygonSelectionWidget::savePoints);
    qDebug() << "Clear button connected:" << clearConnected;
    qDebug() << "Save button connected:" << saveConnected;

    // 初始化提示标签（可选）
    // hintLabel = new QLabel(this);
    // hintLabel->setText("当前点数: 0");

    // 初始化 UI 中的 label
    ui->label->setText("当前点数: 0"); // 默认显示点数为 0
}

void PolygonSelectionWidget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "PolygonSelectionWidget mousePressEvent: " << event->pos();
    if (event->button() == Qt::LeftButton) {
        points.append(event->pos());
        update();
        ui->label->setText(QString("当前点数: %1").arg(points.size())); // 更新点数
    } else if (event->button() == Qt::RightButton && !points.isEmpty()) {
        points.removeLast();
        update();
        ui->label->setText(QString("当前点数: %1").arg(points.size())); // 更新点数
    }
    QWidget::mousePressEvent(event); // 调用基类的鼠标事件处理函数
}

void PolygonSelectionWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制点
    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    foreach (const QPoint &point, points) {
        painter.drawEllipse(point, 3, 3);
    }

    // 绘制多边形
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);
    if (points.size() >= 2) {
        for (int i = 0; i < points.size() - 1; ++i) {
            painter.drawLine(points[i], points[i + 1]);
        }
        if (points.size() >= 3) {
            painter.drawLine(points.last(), points.first());
        }
    }
}

void PolygonSelectionWidget::clearPoints()
{
    qDebug() << "Clear button clicked! Call stack:" << Q_FUNC_INFO;
    points.clear();
    update();
    ui->label->setText("当前点数: 0"); // 重置点数显示
}

void PolygonSelectionWidget::savePoints()
{
    qDebug() << "Save button clicked!";
    qDebug() << "Current working directory:" << QDir::currentPath();

    // 获取窗口尺寸
    double windowWidth = width();
    double windowHeight = height();

    QFile file("points_percent.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QPoint &point : points) {
            // 计算百分比
            double xPercent = (point.x() / windowWidth) * 100.0;
            double yPercent = (point.y() / windowHeight) * 100.0;
            // 写入百分比坐标，保留两位小数
            out << QString::number(xPercent, 'f', 2) << "%," << QString::number(yPercent, 'f', 2) << "%\n";
        }
        file.close();
        qDebug() << "Points saved to points_percent.txt";
        ui->label->setText("保存完成"); // 显示保存成功的提示
    } else {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        ui->label->setText("保存失败"); // 显示保存失败的提示
    }
}
