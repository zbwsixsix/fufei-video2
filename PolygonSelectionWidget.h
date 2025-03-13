#ifndef POLYGONSELECTIONWIDGET_H
#define POLYGONSELECTIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QLabel>
#include <QSize> // 新增：用于接收分辨率

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
    // 新增：设置视频分辨率
    void setVideoResolution(const QSize& resolution);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::PolygonWidget *ui;
    QVector<QPoint> points;
    QRect videoRect;
    // 新增：存储视频分辨率
    QSize videoResolution;
};

#endif // POLYGONSELECTIONWIDGET_H
