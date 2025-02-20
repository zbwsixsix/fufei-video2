#include "playerwindow.h"
#include "ui_playerwindow.h"
#include "videoplayer.h"
#include <QFileDialog>
#include <QMessageBox>

#define FILEPATH "../test/"

PlayerWindow::PlayerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlayerWindow) {
    ui->setupUi(this);

    // 注册信号的参数类型，保证能够发出信号
    qRegisterMetaType<VideoPlayer::VideoSwsSpec>("VideoSwsSpec&");

    // 创建播放器
    _player = new VideoPlayer();
    connect(_player, &VideoPlayer::stateChanged,this, &PlayerWindow::onPlayerStateChanged);
    connect(_player, &VideoPlayer::timeChanged,this, &PlayerWindow::onPlayerTimeChanged);
    connect(_player, &VideoPlayer::initFinished,this, &PlayerWindow::onPlayerInitFinished);
    connect(_player, &VideoPlayer::playFailed,this, &PlayerWindow::onPlayerPlayFailed);
    connect(_player, &VideoPlayer::frameDecoded,ui->videoWidget, &VideoWidget::onPlayerFrameDecoded);
    connect(_player, &VideoPlayer::stateChanged,ui->videoWidget, &VideoWidget::onPlayerStateChanged);
    // 监听时间滑块的点击
    connect(ui->currentSlider, &VideoSlider::clicked,
                this, &PlayerWindow::onSliderClicked);

    // 设置音量滑块的范围
    ui->volumnSlider->setRange(VideoPlayer::Volumn::Min,VideoPlayer::Volumn::Max);
    ui->volumnSlider->setValue(ui->volumnSlider->maximum() >> 2);
}

PlayerWindow::~PlayerWindow() {
    delete ui;
    delete _player;
}

void PlayerWindow::onSliderClicked(VideoSlider *slider) {
    _player->setTime(slider->value());
}

void PlayerWindow::onPlayerPlayFailed() {
    QMessageBox::critical(nullptr,"提示","播放失败");
}

void PlayerWindow::onPlayerTimeChanged(VideoPlayer *player) {
    ui->currentSlider->setValue(player->getTime());
}

void PlayerWindow::onPlayerInitFinished(VideoPlayer *player) {
    int duration = player->getDuration();
    qDebug()<< "duration是多少"<<duration;
    // 设置一些slider的范围
    ui->currentSlider->setRange(0,duration);
    // 设置label的文字
    ui->durationLabel->setText(getTimeText(duration));
}

/**
 * onPlayerStateChanged方法的发射虽然在子线程中执行(VideoPlayer::readFile())，
 * 但是此方法是在主线程执行，因为它的connect是在主线程执行的
 */
void PlayerWindow::onPlayerStateChanged(VideoPlayer *player) {
    VideoPlayer::State state = player->getState();
    if (state == VideoPlayer::Playing) {
        ui->playBtn->setText("暂停");
    } else {
        ui->playBtn->setText("播放");
    }

    if (state == VideoPlayer::Stopped) {
        ui->playBtn->setEnabled(false);
        ui->stopBtn->setEnabled(false);
        ui->currentSlider->setEnabled(false);
        ui->volumnSlider->setEnabled(false);
        ui->muteBtn->setEnabled(false);

        ui->durationLabel->setText(getTimeText(0));
        ui->currentSlider->setValue(0);

        // 显示打开文件的页面
        ui->playWidget->setCurrentWidget(ui->openFilePage);
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->volumnSlider->setEnabled(true);
        ui->muteBtn->setEnabled(true);

        // 显示播放视频的页面
        ui->playWidget->setCurrentWidget(ui->videoPage);
    }
}

void PlayerWindow::on_stopBtn_clicked() {
    _player->stop();
}

void PlayerWindow::on_openFileBtn_clicked() {
    QString filename = QFileDialog::getOpenFileName(nullptr,
                       "选择多媒体文件",
                       FILEPATH,
                       "多媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.aac )");
    qDebug() << "打开文件" << filename;
    if (filename.isEmpty()) return;

    // 开始播放打开的文件
    _player->setFilename(filename);
    _player->play();
}

void PlayerWindow::on_currentSlider_valueChanged(int value) {
    ui->currentLabel->setText(getTimeText(value));
}

void PlayerWindow::on_volumnSlider_valueChanged(int value) {
    ui->volumnLabel->setText(QString("%1").arg(value));
    _player->setVolumn(value);
}

void PlayerWindow::on_playBtn_clicked() {
    VideoPlayer::State state = _player->getState();
    if (state == VideoPlayer::Playing) {
        _player->pause();
    } else {
        _player->play();
    }
}

QString PlayerWindow::getTimeText(int value){
    QString h = QString("0%1").arg(value / 3600).right(2);
    QString m = QString("0%1").arg((value / 60) % 60).right(2);
    QString s = QString("0%1").arg(value % 60).right(2);

    return  QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

void PlayerWindow::on_muteBtn_clicked()
{
    if (_player->isMute()) {
        _player->setMute(false);
        ui->muteBtn->setText("静音");
    } else {
        _player->setMute(true);
        ui->muteBtn->setText("开音");
    }
}




void PlayerWindow::on_fastBackwardBtn_clicked()
{
    int currentTime = _player->getTime();
    if (currentTime - 15 < 0) {
        _player->setTime(0);
    } else {
        _player->setTime(currentTime - 15); // 快退10秒
    }
}


void PlayerWindow::on_fastForwardBtn_clicked()
{
    int currentTime = _player->getTime();
    _player->setTime(currentTime + 20); // 快进10秒
}

