#ifndef POLYGONSELECTIONWIDGET_H
#define POLYGONSELECTIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QLabel>
#include <QSize>

QT_BEGIN_NAMESPACE
namespace Ui {
class PolygonWidget;
}
QT_END_NAMESPACE

class PolygonSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    PolygonSelectionWidget(QWidget *parent = nullptr);
    void clearPoints();
    void savePoints();
    void setVideoRect(const QRect& rect);
    void setVideoResolution(const QSize& resolution);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::PolygonWidget *ui;
    QVector<QPoint> points;
    QRect videoRect;
    QSize videoResolution;
    QVector<QPoint> computeConvexHull(const QVector<QPoint>& points) const; // 计算凸包
};

#endif // POLYGONSELECTIONWIDGET_H
