#ifndef POLYGONSELECTIONWIDGET_H
#define POLYGONSELECTIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class PolygonWidget;
}
QT_END_NAMESPACE

class PolygonSelectionWidget : public QWidget
{
    Q_OBJECT  // 确保有这个宏

public:
    PolygonSelectionWidget(QWidget *parent = nullptr);
    void clearPoints();
    void savePoints();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::PolygonWidget *ui;

    QVector<QPoint> points;
    // 新增提示标签
    QLabel *hintLabel;
};

#endif // POLYGONSELECTIONWIDGET_H
