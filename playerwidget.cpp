#include "playerwidget.h"
#include "ui_playerwidget.h"
#include "videoplayer.h"
#include <QFileDialog>
#include <QMessageBox>

#define FILEPATH "../test/"

PlayerWidget::PlayerWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlayerWidget) {
    ui->setupUi(this);

    qRegisterMetaType<VideoPlayer::VideoSwsSpec>("VideoSwsSpec&");

    _player = new VideoPlayer(this);

    connect(_player, &VideoPlayer::stateChanged, this, &PlayerWidget::onPlayerStateChanged);
    connect(_player, &VideoPlayer::timeChanged, this, &PlayerWidget::onPlayerTimeChanged);
    connect(_player, &VideoPlayer::initFinished, this, &PlayerWidget::onPlayerInitFinished);
    connect(_player, &VideoPlayer::playFailed, this, &PlayerWidget::onPlayerPlayFailed);
    connect(_player, &VideoPlayer::frameDecoded, ui->videoWidget, &VideoWidget::onPlayerFrameDecoded);
    connect(_player, &VideoPlayer::stateChanged, ui->videoWidget, &VideoWidget::onPlayerStateChanged);
    connect(_player, &VideoPlayer::frameDecoded, this, &PlayerWidget::onVideoFrameDecoded);

    connect(ui->currentSlider, &VideoSlider::clicked, this, &PlayerWidget::onSliderClicked);

    ui->volumnSlider->setRange(VideoPlayer::Volumn::Min, VideoPlayer::Volumn::Max);
    ui->volumnSlider->setValue(ui->volumnSlider->maximum() >> 2);

    _polygonWidget = new PolygonSelectionWidget(ui->videoWidget);
    _polygonWidget->setGeometry(0, 0, ui->videoWidget->width(), ui->videoWidget->height());
    _polygonWidget->setStyleSheet("background-color: rgba(255, 255, 255, 255);");

    _polygonWidget->setVideoRect(ui->videoWidget->rect());

    _polygonWidget->setEnabled(true);
    _polygonWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    _polygonWidget->setFocusPolicy(Qt::StrongFocus);

    connect(ui->selectokbtn, &QPushButton::clicked, _polygonWidget, &PolygonSelectionWidget::savePoints);
    connect(ui->clearareabtn, &QPushButton::clicked, _polygonWidget, &PolygonSelectionWidget::clearPoints);

    ui->videoWidget->installEventFilter(this);

    // 初始化分辨率为无效值
    _videoResolution = QSize(0, 0);
}

bool PlayerWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->videoWidget && event->type() == QEvent::Resize) {
        _polygonWidget->setGeometry(0, 0, ui->videoWidget->width(), ui->videoWidget->height());
    }
    return QMainWindow::eventFilter(obj, event);
}

PlayerWidget::~PlayerWidget() {
    delete ui;
    delete _player;
}

void PlayerWidget::closeEvent(QCloseEvent *event) {
    disconnect(_player, nullptr, this, nullptr);
    _player->stop();
    emit windowClosed(this);
}

void PlayerWidget::onSliderClicked(VideoSlider *slider) {
    _player->setTime(slider->value());
}

void PlayerWidget::onPlayerPlayFailed() {
    QMessageBox::critical(nullptr, "提示", "播放失败");
}

void PlayerWidget::onPlayerTimeChanged(VideoPlayer *player) {
    ui->currentSlider->setValue(player->getTime());
}

void PlayerWidget::onPlayerInitFinished(VideoPlayer *player) {
    int duration = player->getDuration();
    qDebug() << "duration是多少" << duration;

    // 获取并存储视频分辨率
    _videoResolution = player->getResolution();
    qDebug() << "视频分辨率:" << _videoResolution.width() << "x" << _videoResolution.height();

    // 将分辨率传递给 PolygonSelectionWidget
    _polygonWidget->setVideoResolution(_videoResolution);

    ui->currentSlider->setRange(0, duration);
    ui->durationLabel->setText(getTimeText(duration));
}

void PlayerWidget::onPlayerStateChanged(VideoPlayer *player) {
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
        ui->selectokbtn->setEnabled(false);

        ui->durationLabel->setText(getTimeText(0));
        ui->currentSlider->setValue(0);
        ui->playWidget->setCurrentWidget(ui->openFilePage);
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->muteBtn->setEnabled(true);
        ui->volumnSlider->setEnabled(true);
        ui->selectokbtn->setEnabled(true);
        ui->playWidget->setCurrentWidget(ui->videoPage);
    }
}

void PlayerWidget::on_stopBtn_clicked() {
    _player->stop();
}

void PlayerWidget::on_openFileBtn_clicked() {
    QString filename = QFileDialog::getOpenFileName(nullptr,
                                                    "选择多媒体文件",
                                                    FILEPATH,
                                                    "多媒体文件 (*.mp4 *.avi *.mkv *.mp3 *.aac )");
    qDebug() << "打开文件" << filename;
    if (filename.isEmpty()) return;

    _player->setFilename(filename);
    _player->play();
}

void PlayerWidget::on_currentSlider_valueChanged(int value) {
    ui->currentLabel->setText(getTimeText(value));
}

void PlayerWidget::on_volumnSlider_valueChanged(int value) {
    ui->volumnLabel->setText(QString("%1").arg(value));
    _player->setVolumn(value);
}

void PlayerWidget::on_playBtn_clicked() {
    VideoPlayer::State state = _player->getState();
    if (state == VideoPlayer::Playing) {
        _player->pause();
    } else {
        _player->play();
    }
}

QString PlayerWidget::getTimeText(int value) {
    QString h = QString("0%1").arg(value / 3600).right(2);
    QString m = QString("0%1").arg((value / 60) % 60).right(2);
    QString s = QString("0%1").arg(value % 60).right(2);
    return QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

void PlayerWidget::on_muteBtn_clicked() {
    if (_player->isMute()) {
        _player->setMute(false);
        ui->muteBtn->setText("静音");
    } else {
        _player->setMute(true);
        ui->muteBtn->setText("开音");
    }
}

void PlayerWidget::on_fastBackwardBtn_clicked() {
    int currentTime = _player->getTime();
    if (currentTime - 15 < 0) {
        _player->setTime(0);
    } else {
        _player->setTime(currentTime - 15);
    }
}

void PlayerWidget::on_fastForwardBtn_clicked() {
    int currentTime = _player->getTime();
    _player->setTime(currentTime + 20);
}

void PlayerWidget::onVideoFrameDecoded(VideoPlayer *player, uint8_t *data, VideoPlayer::VideoSwsSpec &spec) {
    Q_UNUSED(player);
    Q_UNUSED(data);
    Q_UNUSED(spec);
    _polygonWidget->setVideoRect(ui->videoWidget->getVideoRect());
    // 可选：每次帧解码时更新分辨率（通常不需要，因为分辨率在初始化时已设置）
    // _polygonWidget->setVideoResolution(_videoResolution);
}
