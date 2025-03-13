#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>
#include "videoplayer.h"

/**
 * 显示（渲染）视频
 */
class VideoWidget : public QWidget {
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget();

    // 新增：获取视频内容的绘制区域
    QRect getVideoRect() const { return _rect; }

public slots:
    void onPlayerFrameDecoded(VideoPlayer *player, uint8_t *data, VideoPlayer::VideoSwsSpec &spec);
    void onPlayerStateChanged(VideoPlayer *player);

private:
    QImage *_image = nullptr;
    QRect _rect;
    void paintEvent(QPaintEvent *event) override;
    void freeImage();
};

#endif // VIDEOWIDGET_H
