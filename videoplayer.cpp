#include "videoplayer.h"
#include <thread>

#define AUDIO_MAX_PKT_SIZE 1000
#define VIDEO_MAX_PKT_SIZE 500

VideoPlayer::VideoPlayer(QObject *parent) : QObject(parent) {

        // 关闭之前的音频设备
    // closeAudio();

    // 初始化Audio子系统
    if (SDL_Init(SDL_INIT_AUDIO)) {
        // 返回值不是0，就代表失败
        qDebug() << "SDL_Init error" << SDL_GetError();
        emit playFailed(this);
        return;
    }
}



void VideoPlayer::closeAudio() {

    // 停止播放
    if (_audioDeviceId != 0) {
        SDL_PauseAudioDevice(_audioDeviceId, 1);
        SDL_Delay(10);
        SDL_ClearQueuedAudio(_audioDeviceId);    // 清空音频队列
        SDL_CloseAudioDevice(_audioDeviceId);
        _audioDeviceId = 0;
        qDebug() << "Audio device closed for player instance";
    }
}


VideoPlayer::~VideoPlayer() {
    // 不再对外发送消息
    disconnect();

    stop();
    closeAudio();
    // SDL_QuitSubSystem(SDL_INIT_AUDIO);  // 只清理当前实例的音频子系统
}

void VideoPlayer::play() {
    if (_state == Playing) return;
    // 状态可能是：暂停、停止、正常完毕

    if(_state == Stopped){
        // 开始线程：读取文件
        std::thread([this](){
            readFile();
        }).detach();// detach 等到readFile方法执行完，这个线程就会销毁
    }else{
        setState(Playing);
    }
}

void VideoPlayer::pause() {
    if (_state != Playing) return;
    // 状态可能是：正在播放

    setState(Paused);
}

void VideoPlayer::stop() {
    if (_state == Stopped) return;
    // 状态可能是：正在播放、暂停、正常完毕

    // 改变状态
    _state = Stopped;
    // 释放资源
    free();

    // 通知外界
    emit stateChanged(this);
}

bool VideoPlayer::isPlaying() {
    return _state == Playing;
}

VideoPlayer::State VideoPlayer::getState() {
    return _state;
}

void VideoPlayer::setFilename(QString &filename) {
    _filename = filename;
}

int VideoPlayer::getDuration(){
    return _fmtCtx ? round(_fmtCtx->duration * av_q2d(AV_TIME_BASE_Q)) : 0;
}

int VideoPlayer::getTime(){
    if (_hasAudio) {
        return round(_aTime); // 有音频时以音频时间为准
    } else {
        return round(_vTime); // 无音频时使用视频时间
    }
}

void VideoPlayer::setVolumn(int volumn){
    _volumn = volumn;
}

void VideoPlayer::setTime(int seekTime){
    // if (_state == Playing) {
    //     pause();
    // }

    // 清空视频包列表
    // clearVideoPktList();

    // 清空音频包列表
    // clearAudioPktList();

    _seekTime = seekTime;
    qDebug()<< "_seekTime是vvvvvvvvv"<< _seekTime;
    _seekTime = seekTime+startTime;
    qDebug()<< "_seekTime+startTime是vvvvvvvvv"<< _seekTime;

    // 重新初始化视频信息
    // 进行seek操作
    // if (_fmtCtx) {
    //     int ret = av_seek_frame(_fmtCtx, -1, seekTime * AV_TIME_BASE, AVSEEK_FLAG_BACKWARD);
    //     if (ret < 0) {
    //         ERROR_BUF;
    //         qDebug() << "av_seek_frame error" << errbuf;
    //     }
    // }

    // // 重新初始化视频信息
    // initVideoInfo();

    // 如果之前是播放状态，继续播放
    // if (_state == Paused) {
    //     play();
    // }
    // _seekTime =73714;
}

int VideoPlayer::getVolumn(){
    return _volumn;
}

void VideoPlayer::setMute(bool mute) {
    _mute = mute;
}

bool VideoPlayer::isMute() {
    return _mute;
}

// 新增：实现 getResolution 方法
QSize VideoPlayer::getResolution() const {
    if (_vDecodeCtx) {
        return QSize(_vDecodeCtx->width, _vDecodeCtx->height);
    }
    return QSize(0, 0); // 如果视频未初始化，返回无效分辨率
}

void VideoPlayer::readFile() {
    freeAudio();
    int ret = 0;

    // 创建解封装上下文、打开文件
    ret = avformat_open_input(&_fmtCtx, _filename.toUtf8().data(), nullptr, nullptr);
    END(avformat_open_input);

    // 检索流信息
    ret = avformat_find_stream_info(_fmtCtx, nullptr);
    END(avformat_find_stream_info);

    av_dump_format(_fmtCtx, 0, _filename.toUtf8().data(), 0);
    fflush(stderr);

    double startTime = static_cast<double>(_fmtCtx->start_time) / AV_TIME_BASE;
    qDebug() << "VVVVVVVstartTime" << startTime;

    // 初始化音频信息（允许失败）
    _hasAudio = (initAudioInfo() >= 0);
    if (!_hasAudio) {
        qDebug() << "No audio stream found, proceeding with video only";
    }

    // 初始化视频信息
    _hasVideo = (initVideoInfo() >= 0);
    if (!_hasVideo) {
        qDebug() << "No video stream found";
        if (!_hasAudio) {
            emit playFailed(this);
            free();
            return;
        }
    }

    // 初始化完毕
    emit initFinished(this);
    setState(Playing);

    // 如果有音频，启动音频播放
    if (_hasAudio && _audioDeviceId != 0) {
        SDL_PauseAudioDevice(_audioDeviceId, 0);
        qDebug() << "Audio device status after start:" << SDL_GetAudioDeviceStatus(_audioDeviceId);
    } else {
        qDebug() << "Skipping audio playback due to no audio stream or device";
    }

    // 启动视频解码线程
    std::thread([this, startTime]() {
        this->decodeVideo(startTime);
    }).detach();

    // 读取数据包
    AVPacket pkt;
    while (_state != Stopped) {
        // 处理seek操作（保持不变）
        if (_seekTime >= 0) {
            int streamIdx = _hasAudio ? _aStream->index : _vStream->index;
            AVRational timeBase = _fmtCtx->streams[streamIdx]->time_base;
            int64_t ts = _seekTime / av_q2d(timeBase);
            ret = avformat_seek_file(_fmtCtx, streamIdx, INT64_MIN, ts, INT64_MAX, AVSEEK_FLAG_BACKWARD);
            if (ret < 0) {
                qDebug() << "seek失败" << _seekTime << ts << streamIdx;
                _seekTime = -1;
            } else {
                qDebug() << "seek成功" << _seekTime << ts << streamIdx;
                clearAudioPktList();
                clearVideoPktList();
                _vSeekTime = _seekTime;
                _aSeekTime = _seekTime;
                _aTime = 0;
                _vTime = 0;
                _seekTime = -1;
            }
        }

        int vSize = _vPktList.size();
        int aSize = _aPktList.size();
        if (vSize >= VIDEO_MAX_PKT_SIZE || aSize >= AUDIO_MAX_PKT_SIZE) {
            SDL_Delay(1);
            continue;
        }

        ret = av_read_frame(_fmtCtx, &pkt);
        if (ret == 0) {
            if (_hasAudio && pkt.stream_index == _aStream->index) {
                addAudioPkt(pkt);
            } else if (_hasVideo && pkt.stream_index == _vStream->index) {
                addVideoPkt(pkt);
            } else {
                av_packet_unref(&pkt);
            }
        } else if (ret == AVERROR_EOF) {
            if (vSize == 0 && (!_hasAudio || aSize == 0)) {
                _fmtCtxCanFree = true;
                break;
            }
        } else {
            ERROR_BUF;
            qDebug() << "av_read_frame error" << errbuf;
            continue;
        }
    }
    if (_fmtCtxCanFree) {
        stop();
    } else {
        _fmtCtxCanFree = true;
    }
}

int VideoPlayer::initDecoder(AVCodecContext **decodeCtx,AVStream **stream,AVMediaType type) {
    // 根据type寻找最合适的流信息
    // 返回值是流索引
    int ret = av_find_best_stream(_fmtCtx, type, -1, -1, nullptr, 0);
    RET(av_find_best_stream);

    // 检验流
    int streamIdx = ret;
    *stream = _fmtCtx->streams[streamIdx];
    if (!*stream) {
        qDebug() << "stream is empty";
        return -1;
    }

    // 为当前流找到合适的解码器
    const AVCodec *decoder = avcodec_find_decoder((*stream)->codecpar->codec_id);
    if (!decoder) {
        qDebug() << "decoder not found" << (*stream)->codecpar->codec_id;
        return -1;
    }

    // 初始化解码上下文
    *decodeCtx = avcodec_alloc_context3(decoder);
    if (!decodeCtx) {
        qDebug() << "avcodec_alloc_context3 error";
        return -1;
    }

    // 从流中拷贝参数到解码上下文中
    ret = avcodec_parameters_to_context(*decodeCtx, (*stream)->codecpar);
    RET(avcodec_parameters_to_context);

    // 打开解码器
    ret = avcodec_open2(*decodeCtx, decoder, nullptr);
    RET(avcodec_open2);
    return 0;
}

void VideoPlayer::setState(State state) {
    if (state == _state) return;

    _state = state;

    emit stateChanged(this);
}



void VideoPlayer::free() {
    //停止功能能否正常使用
    // while (_hasAudio && !_aCanFree) SDL_Delay(1); // 等待音频线程结束
    // while (_hasVideo && !_vCanFree) SDL_Delay(1); // 等待视频线程结束
    // freeAudio();
    // freeVideo();
    // if (_fmtCtx) {
    //     avformat_close_input(&_fmtCtx);
    //     _fmtCtx = nullptr;
    // }
    // _fmtCtxCanFree = false;
    // qDebug() << "free 成功" ;
}

void VideoPlayer::fataError(){
    setState(Stopped);
    free();
    emit playFailed(this);
}
