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
#include <algorithm> // 用于 std::sort
#include <QMessageBox>

PolygonSelectionWidget::PolygonSelectionWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PolygonWidget)
{
    ui->setupUi(this);
    videoResolution = QSize(0, 0);
}

void PolygonSelectionWidget::setVideoRect(const QRect& rect) {
    videoRect = rect;
}

void PolygonSelectionWidget::setVideoResolution(const QSize& resolution) {
    videoResolution = resolution;
    qDebug() << "Video resolution set to:" << resolution.width() << "x" << resolution.height();
}

#include <QPolygon>

void PolygonSelectionWidget::mousePressEvent(QMouseEvent *event)
{
    if (!videoRect.contains(event->pos())) {
        qDebug() << "Click outside video area: " << event->pos() << ", videoRect: " << videoRect;
        return;
    }

    qDebug() << "PolygonSelectionWidget mousePressEvent: " << event->pos();
    if (event->button() == Qt::LeftButton) {
        QVector<QPoint> convexHull = computeConvexHull(points);
        if (convexHull.size() >= 3) {
            QPolygon polygon(convexHull);
            if (polygon.containsPoint(event->pos(), Qt::OddEvenFill)) {
                qDebug() << "Point inside polygon, not added.";
                QMessageBox::information(this, "提示", "新点在已有区域内，无法添加！");
                return; // 点在多边形内，拒绝添加
            }
        }
        points.append(event->pos());
        update();
    } else if (event->button() == Qt::RightButton && !points.isEmpty()) {
        points.removeLast();
        update();
    }
    QWidget::mousePressEvent(event);
}

// 计算两点与参考点的叉积，用于凸包算法
static inline qreal crossProduct(const QPoint& p0, const QPoint& p1, const QPoint& p2) {
    return (p1.x() - p0.x()) * (p2.y() - p0.y()) - (p1.y() - p0.y()) * (p2.x() - p0.x());
}

// Graham 扫描算法计算凸包
QVector<QPoint> PolygonSelectionWidget::computeConvexHull(const QVector<QPoint>& points) const {
    if (points.size() < 3) return points;

    QVector<QPoint> hull;
    QVector<QPoint> sortedPoints = points;

    // 找到最左下角的点
    int startIdx = 0;
    for (int i = 1; i < sortedPoints.size(); ++i) {
        if (sortedPoints[i].y() < sortedPoints[startIdx].y() ||
            (sortedPoints[i].y() == sortedPoints[startIdx].y() && sortedPoints[i].x() < sortedPoints[startIdx].x())) {
            startIdx = i;
        }
    }
    std::swap(sortedPoints[0], sortedPoints[startIdx]);

    QPoint p0 = sortedPoints[0];
    // 按极角排序
    std::sort(sortedPoints.begin() + 1, sortedPoints.end(), [p0](const QPoint& a, const QPoint& b) {
        qreal cp = crossProduct(p0, a, b);
        if (cp == 0) {
            return (a.x() - p0.x()) * (a.x() - p0.x()) + (a.y() - p0.y()) * (a.y() - p0.y()) <
                   (b.x() - p0.x()) * (b.x() - p0.x()) + (b.y() - p0.y()) * (b.y() - p0.y());
        }
        return cp > 0;
    });

    // Graham 扫描
    hull.append(sortedPoints[0]);
    hull.append(sortedPoints[1]);
    for (int i = 2; i < sortedPoints.size(); ++i) {
        while (hull.size() > 1 && crossProduct(hull[hull.size() - 2], hull[hull.size() - 1], sortedPoints[i]) <= 0) {
            hull.removeLast();
        }
        hull.append(sortedPoints[i]);
    }

    return hull;
}

void PolygonSelectionWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 计算凸包
    QVector<QPoint> convexHull = computeConvexHull(points);

    // 绘制凸包区域
    if (convexHull.size() >= 3) {
        painter.setPen(Qt::blue);
        painter.setBrush(QColor(255, 255, 255, 70)); // 半透明填充
        QPolygon polygon(convexHull);
        painter.drawPolygon(polygon);
    }

    // 绘制凸包边线
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::NoBrush);
    if (convexHull.size() >= 2) {
        for (int i = 0; i < convexHull.size() - 1; ++i) {
            painter.drawLine(convexHull[i], convexHull[i + 1]);
        }
        if (convexHull.size() >= 3) {
            painter.drawLine(convexHull.last(), convexHull.first());
        }
    }

    // 只绘制凸包中的点
    painter.setPen(Qt::red);
    painter.setBrush(Qt::red);
    foreach (const QPoint &point, convexHull) {
        painter.drawEllipse(point, 3, 3); // 绘制凸包顶点为红色小圆点
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

    double videoWidth = videoRect.width();
    double videoHeight = videoRect.height();

    if (videoWidth <= 0 || videoHeight <= 0) {
        qDebug() << "videoRect is invalid, falling back to widget size: " << videoRect;
        videoWidth = width();
        videoHeight = height();
    }

    if (videoResolution.width() <= 0 || videoResolution.height() <= 0) {
        qDebug() << "Invalid video resolution, cannot save points as pixel coordinates";
        QMessageBox::warning(this, "提示", "视频分辨率无效，无法保存点！"); // 添加失败提示
        return;
    }

    // 使用凸包点保存
    QVector<QPoint> convexHull = computeConvexHull(points);

    QFile file("points_percent.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QPoint &point : convexHull) {
            double relativeX = point.x() - videoRect.x();
            double relativeY = point.y() - videoRect.y();

            double xPercent = (relativeX / videoWidth) * 100.0;
            double yPercent = (relativeY / videoHeight) * 100.0;

            double pixelX = (xPercent / 100.0) * videoResolution.width();
            double pixelY = (yPercent / 100.0) * videoResolution.height();

            qDebug() << "视频像素分辨率" << videoResolution.width() << videoResolution.height();
            out << QString::number(pixelX, 'f', 2) << "," << QString::number(pixelY, 'f', 2) << "\n";
        }
        file.close();
        QMessageBox::information(this, "提示", "区域点已成功保存到 points_percent.txt！"); // 添加成功提示
        qDebug() << "Convex hull points saved to points_percent.txt as pixel coordinates";
    } else {
        qDebug() << "Failed to open file for writing:" << file.errorString();
        QMessageBox::warning(this, "提示", "无法保存点，文件打开失败！"); // 添加失败提示
    }
}
