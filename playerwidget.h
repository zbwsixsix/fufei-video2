#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videoplayer.h"
#include "videoslider.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class PlayerWidget;
}
QT_END_NAMESPACE

class PlayerWidget : public QMainWindow {
    Q_OBJECT

public:
    PlayerWidget(QWidget *parent = nullptr);
    ~PlayerWidget();

private slots:
    void onPlayerStateChanged(VideoPlayer *player);
    void onPlayerTimeChanged(VideoPlayer *player);
    void onPlayerInitFinished(VideoPlayer *player);
    void onPlayerPlayFailed();
    void onSliderClicked(VideoSlider *slider);

    void on_stopBtn_clicked();

    void on_openFileBtn_clicked();

    void on_currentSlider_valueChanged(int value);

    void on_volumnSlider_valueChanged(int value);

    void on_playBtn_clicked();

    void on_muteBtn_clicked();

    void on_fastBackwardBtn_clicked();

    void on_fastForwardBtn_clicked();

private:
    Ui::PlayerWidget *ui;
    VideoPlayer *_player;

    QString getTimeText(int value);
};
#endif // MAINWINDOW_H
