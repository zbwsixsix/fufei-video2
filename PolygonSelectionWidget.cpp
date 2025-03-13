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
    // 初始化分辨率为默认值
    videoResolution = QSize(0, 0);
}

void PolygonSelectionWidget::setVideoRect(const QRect& rect) {
    videoRect = rect;
}

// 新增：实现设置视频分辨率的方法
void PolygonSelectionWidget::setVideoResolution(const QSize& resolution) {
    videoResolution = resolution;
    qDebug() << "Video resolution set to:" << resolution.width() << "x" << resolution.height();
}

void PolygonSelectionWidget::mousePressEvent(QMouseEvent *event)
{
    if (!videoRect.contains(event->pos())) {
        qDebug() << "Click outside video area: " << event->pos() << ", videoRect: " << videoRect;
        return;
    }

    qDebug() << "PolygonSelectionWidget mousePressEvent: " << event->pos();
    if (event->button() == Qt::LeftButton) {
        points.append(event->pos());
        update();
    } else if (event->button() == Qt::RightButton && !points.isEmpty()) {
        points.removeLast();
        update();
    }
    QWidget::mousePressEvent(event);
}

void PolygonSelectionWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (points.size() >= 3) {
        painter.setPen(Qt::blue);
        painter.setBrush(QColor(255, 255, 255, 70));
        QPolygon polygon(points);
        painter.drawPolygon(polygon);
    }

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

    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    foreach (const QPoint &point, points) {
        painter.drawEllipse(point, 3, 3);
    }
}

void PolygonSelectionWidget::clearPoints()
{
    qDebug() << "Clear button clicked! Call stack:" << Q_FUNC_INFO;
    points.clear();
    update();
}

void PolygonSelectionWidget::savePoints()
{
    qDebug() << "Save button clicked!";
    qDebug() << "Current working directory:" << QDir::currentPath();

    // 获取视频区域的尺寸
    double videoWidth = videoRect.width();
    double videoHeight = videoRect.height();

    if (videoWidth <= 0 || videoHeight <= 0) {
        qDebug() << "videoRect is invalid, falling back to widget size: " << videoRect;
        videoWidth = width();
        videoHeight = height();
    }

    // 检查分辨率是否有效
    if (videoResolution.width() <= 0 || videoResolution.height() <= 0) {
        qDebug() << "Invalid video resolution, cannot save points as pixel coordinates";
        return;
    }

    QFile file("points_percent.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QPoint &point : points) {
            // 计算相对于 videoRect 左上角的坐标
            double relativeX = point.x() - videoRect.x();
            double relativeY = point.y() - videoRect.y();

            // 计算百分比
            double xPercent = (relativeX / videoWidth) * 100.0;
            double yPercent = (relativeY / videoHeight) * 100.0;

            // 将百分比转换为像素坐标（乘以视频分辨率）
            double pixelX = (xPercent / 100.0) * videoResolution.width();
            double pixelY = (yPercent / 100.0) * videoResolution.height();

            qDebug() <<"视频像素分辨率"<<videoResolution.width()<< videoResolution.height();

            // 写入像素坐标，保留两位小数
            out << QString::number(pixelX, 'f', 2) << "," << QString::number(pixelY, 'f', 2) << "\n";
        }
        file.close();
        qDebug() << "Points saved to points_percent.txt as pixel coordinates";
    } else {
        qDebug() << "Failed to open file for writing:" << file.errorString();
    }
}
