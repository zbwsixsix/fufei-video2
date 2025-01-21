#include "videoslider.h"
#include <QMouseEvent>
#include <QStyle>

VideoSlider::VideoSlider(QWidget *parent) : QSlider(parent) {

}

void VideoSlider::mousePressEvent(QMouseEvent *ev) {
    // 根据点击位置的x值，计算出对应的value
    int value = QStyle::sliderValueFromPosition(minimum(),maximum(),ev->pos().x(),width());
    setValue(value);

    QSlider::mousePressEvent(ev);

    // 发出信号
    emit clicked(this);
}
