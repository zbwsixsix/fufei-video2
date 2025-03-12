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

    connect(ui->currentSlider, &VideoSlider::clicked, this, &PlayerWidget::onSliderClicked);

    ui->volumnSlider->setRange(VideoPlayer::Volumn::Min, VideoPlayer::Volumn::Max);
    ui->volumnSlider->setValue(ui->volumnSlider->maximum() >> 2);

    // 初始化 PolygonSelectionWidget
    _polygonWidget = new PolygonSelectionWidget(ui->videoWidget);
    _polygonWidget->setGeometry(0, 0, ui->videoWidget->width(), ui->videoWidget->height()); // 初始化时使用 videoWidget 大小
    _polygonWidget->setStyleSheet("background-color: rgba(0, 255, 0, 50);"); // 半透绿色背景


    // _polygonWidget->hide(); // 默认隐藏
    _polygonWidget->setEnabled(true);
    _polygonWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false); // 确保不透传鼠标事件
    _polygonWidget->setFocusPolicy(Qt::StrongFocus); // 确保可以获得焦点

    connect(ui->selectokbtn, &QPushButton::clicked, _polygonWidget, &PolygonSelectionWidget::savePoints);
    connect(ui->clearareabtn, &QPushButton::clicked,  _polygonWidget, &PolygonSelectionWidget::clearPoints);
    // 动态调整大小（需要重写 resizeEvent 或使用事件过滤器）
    ui->videoWidget->installEventFilter(this); // 安装事件过滤器
}

// 添加事件过滤器
bool PlayerWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->videoWidget && event->type() == QEvent::Resize) {
        _polygonWidget->setGeometry(0, 0, ui->videoWidget->width(), ui->videoWidget->height());
        qDebug() << "Video widget resized, new geometry:" << _polygonWidget->geometry();
    }
    return QMainWindow::eventFilter(obj, event);
}
PlayerWidget::~PlayerWidget() {
    delete ui;
    delete _player;
}


// 重写 closeEvent
void PlayerWidget::closeEvent(QCloseEvent *event) {
    disconnect(_player, nullptr, this, nullptr);
    _player->stop(); // 停止播放，确保资源清理
    emit windowClosed(this); // 发出窗口关闭信号

}


void PlayerWidget::onSliderClicked(VideoSlider *slider) {
    _player->setTime(slider->value());
}

void PlayerWidget::onPlayerPlayFailed() {
    QMessageBox::critical(nullptr,"提示","播放失败");
}

void PlayerWidget::onPlayerTimeChanged(VideoPlayer *player) {
    ui->currentSlider->setValue(player->getTime());
}

void PlayerWidget::onPlayerInitFinished(VideoPlayer *player) {
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

        // 显示打开文件的页面
        ui->playWidget->setCurrentWidget(ui->openFilePage);
    } else {
        ui->playBtn->setEnabled(true);
        ui->stopBtn->setEnabled(true);
        ui->currentSlider->setEnabled(true);
        ui->muteBtn->setEnabled(true);
        ui->volumnSlider->setEnabled(true);

        ui->selectokbtn->setEnabled(true); // 新增按钮启用
        // 显示播放视频的页面
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

    // QString filename2="d:/Users/zhangbowen/Videos/20min.mp4";
    // 开始播放打开的文件
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

QString PlayerWidget::getTimeText(int value){
    QString h = QString("0%1").arg(value / 3600).right(2);
    QString m = QString("0%1").arg((value / 60) % 60).right(2);
    QString s = QString("0%1").arg(value % 60).right(2);

    return  QString("%1:%2:%3").arg(h).arg(m).arg(s);
}

void PlayerWidget::on_muteBtn_clicked()
{
    if (_player->isMute()) {
        _player->setMute(false);
        ui->muteBtn->setText("静音");
    } else {
        _player->setMute(true);
        ui->muteBtn->setText("开音");
    }
}




void PlayerWidget::on_fastBackwardBtn_clicked()
{
    int currentTime = _player->getTime();
    if (currentTime - 15 < 0) {
        _player->setTime(0);
    } else {
        _player->setTime(currentTime - 15); // 快退10秒
    }
}


void PlayerWidget::on_fastForwardBtn_clicked()
{
    int currentTime = _player->getTime();
    _player->setTime(currentTime + 20); // 快进10秒
}

void PlayerWidget::on_selectRegionBtn_clicked() {

    qDebug() << "_polygonWidget->isVisible()打开文件" << _polygonWidget->isVisible();
    if (_polygonWidget->isVisible()) {
        // 如果多边形选择控件已显示，则隐藏它并恢复播放
        _polygonWidget->hide();
        ui->selectokbtn->setText("开始选择区域");
        if (_player->getState() == VideoPlayer::Paused) {
            _player->play(); // 如果之前是暂停状态，则恢复播放
        }
    } else {
        // 显示多边形选择控件并暂停视频
        _polygonWidget->show();
        ui->selectokbtn->setText("退出选择区域");
        if (_player->getState() == VideoPlayer::Playing) {
            _player->pause(); // 暂停视频以便选择区域
        }
    }
}
