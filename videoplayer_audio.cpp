#include "videoplayer.h"
// 初始化音频信息
int VideoPlayer::initAudioInfo() {
    int ret = initDecoder(&_aDecodeCtx,&_aStream,AVMEDIA_TYPE_AUDIO);
    RET(initDecoder);

    // 打印音频流参数以确认
    qDebug() << "Audio sample rate:" << _aDecodeCtx->sample_rate;
    qDebug() << "Audio sample format:" << _aDecodeCtx->sample_fmt;
    qDebug() << "Audio channel layout:" << _aDecodeCtx->ch_layout.u.mask;
    qDebug() << "Audio channels:" << _aDecodeCtx->ch_layout.nb_channels;


    // 初始化音频重采样
    ret = initSwr();
    RET(initSwr);

    // 初始化SDL
    ret = initSDL();
    RET(initSDL);


    // 获取视频的起始时间并存储到 startTime
    if (_fmtCtx) {
        startTime = static_cast<double>(_fmtCtx->start_time) / AV_TIME_BASE;  // 转换为秒
    }
    return 0;
}

int VideoPlayer::initSwr() {

    qDebug() << "Audio sample rate:" << _aDecodeCtx->sample_rate;
    qDebug() << "Audio sample format:" << _aDecodeCtx->sample_fmt;
    qDebug() << "Audio channel layout:" << _aDecodeCtx->ch_layout.u.mask;
    qDebug() << "Audio channels:" << _aDecodeCtx->ch_layout.nb_channels;

    // 重采样输入参数
    _aSwrInSpec.sampleFmt = _aDecodeCtx->sample_fmt;
    _aSwrInSpec.sampleRate = _aDecodeCtx->sample_rate;
        // _aSwrInSpec.chLayout = _aDecodeCtx->ch_layout;
    av_channel_layout_copy(&_aSwrInSpec.chLayout, &_aDecodeCtx->ch_layout);  //
    _aSwrInSpec.chs = _aDecodeCtx->ch_layout.nb_channels;

    // 重采样输出参数
    _aSwrOutSpec.sampleFmt = AV_SAMPLE_FMT_S16;
    _aSwrOutSpec.sampleRate = _aDecodeCtx->sample_rate;;
    av_channel_layout_copy(&_aSwrOutSpec.chLayout, &_aDecodeCtx->ch_layout);

    _aSwrOutSpec.chs = _aDecodeCtx->ch_layout.nb_channels;  // 显式设置立体声
    _aSwrOutSpec.bytesPerSampleFrame = _aSwrOutSpec.chs * av_get_bytes_per_sample(_aSwrOutSpec.sampleFmt);

    // 验证声道布局
    char buf[64];
    av_channel_layout_describe(&_aSwrInSpec.chLayout, buf, sizeof(buf));
    qDebug() << "Input layout:" << buf;
    av_channel_layout_describe(&_aSwrOutSpec.chLayout, buf, sizeof(buf));
    qDebug() << "Output layout:" << buf;

    // _aSwrOutSpec.chs = _aSwrOutSpec.chLayout.nb_channels;
    // _aSwrOutSpec.bytesPerSampleFrame = _aSwrOutSpec.chs * av_get_bytes_per_sample(_aSwrOutSpec.sampleFmt);

    // // 创建重采样上下文
    // _aSwrCtx = swr_alloc();
    // if (!_aSwrCtx) {
    //     qDebug() << "swr_alloc error";
    //     return -1;
    // }


    int ret = swr_alloc_set_opts2(
        &_aSwrCtx,
        &_aSwrOutSpec.chLayout,
        _aSwrOutSpec.sampleFmt,
        _aSwrOutSpec.sampleRate,
        &_aSwrInSpec.chLayout,
        _aSwrInSpec.sampleFmt,
        _aSwrInSpec.sampleRate,
        0, nullptr
        );
    if (ret < 0 || !_aSwrCtx) {
        qDebug() << "swr_alloc_set_opts2 failed";
        return -1;
    }

    // 初始化重采样上下文
     ret = swr_init(_aSwrCtx);
    RET(swr_init);

    // 初始化重采样的输入frame
    _aSwrInFrame = av_frame_alloc();
    if (!_aSwrInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化重采样的输出frame
    _aSwrOutFrame = av_frame_alloc();
    if (!_aSwrOutFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化重采样的输出frame的data[0]空间
    ret = av_samples_alloc(_aSwrOutFrame->data,
                           &_aSwrOutFrame->linesize[0],
                           _aSwrOutSpec.chs,
                           4096, _aSwrOutSpec.sampleFmt, 1);
    RET(av_samples_alloc);

    return 0;
}

void VideoPlayer::freeAudio(){
    _aSwrOutIdx = 0;
    _aSwrOutSize =0;
    _aTime = 0;
    _aCanFree = true;
    _aSeekTime = -1;

    clearAudioPktList();
    avcodec_free_context(&_aDecodeCtx);
    swr_free(&_aSwrCtx);
    av_frame_free(&_aSwrInFrame);
    if(_aSwrOutFrame){
        av_freep(&_aSwrOutFrame->data[0]);// 因手动创建了data[0]的空间
        av_frame_free(&_aSwrOutFrame);
    }

    // 停止播放
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

void VideoPlayer::sdlAudioCallbackFunc(void *userdata, uint8_t *stream, int len){
    VideoPlayer *player = (VideoPlayer *)userdata;
    // qDebug() << "DDDDDDDLLLLLstartTime" << player->startTime ;
    player->sdlAudioCallback(stream,len,player->startTime);
}

int VideoPlayer::initSDL(){
    // 音频参数
    SDL_AudioSpec spec;
    // 采样率
    spec.freq = _aSwrOutSpec.sampleRate;
    // 采样格式（s16le）
    spec.format = AUDIO_S16LSB;
    // 声道数
    spec.channels = _aSwrOutSpec.chs;
    // 音频缓冲区的样本数量（这个值必须是2的幂）
    spec.samples = 512;
    // 回调
    spec.callback = sdlAudioCallbackFunc;
    // 传递给回调的参数
    spec.userdata = this;

    // 打开音频设备
    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        return -1;
    }

    return 0;
}

void VideoPlayer::addAudioPkt(AVPacket &pkt){
    _aMutex.lock();
    _aPktList.push_back(pkt);
    _aMutex.signal();
    _aMutex.unlock();
}

void VideoPlayer::clearAudioPktList(){
    _aMutex.lock();
    for(AVPacket &pkt : _aPktList){
        av_packet_unref(&pkt);
    }
    _aPktList.clear();
    _aMutex.unlock();
}

void VideoPlayer::sdlAudioCallback(Uint8 *stream, int len,double startTime){
    // 清零（静音）
    SDL_memset(stream, 0, len);

    // len：SDL音频缓冲区剩余的大小（还未填充的大小）
    while (len > 0) {
        if (_state == Paused) break;
        if (_state == Stopped) {
            _aCanFree = true;
            break;
        }

        // 说明当前PCM的数据已经全部拷贝到SDL的音频缓冲区了
        // 需要解码下一个pkt，获取新的PCM数据
        if (_aSwrOutIdx >= _aSwrOutSize) {
            // 全新PCM的大小
            _aSwrOutSize = decodeAudio(startTime);
            // 索引清0
            _aSwrOutIdx = 0;
            // 没有解码出PCM数据，那就静音处理
            if (_aSwrOutSize <= 0) {
                // 假定PCM的大小
                _aSwrOutSize = 1024;
                // 给PCM填充0（静音）
                memset(_aSwrOutFrame->data[0], 0, _aSwrOutSize);
            }
        }

        // 本次需要填充到stream中的PCM数据大小
        int fillLen = _aSwrOutSize - _aSwrOutIdx;
        fillLen = std::min(fillLen, len);

        // 获取当前音量
        int volumn = _mute ? 0 : ((_volumn * 1.0 / Max) * SDL_MIX_MAXVOLUME);

        // 填充SDL缓冲区
        SDL_MixAudio(stream,
                     _aSwrOutFrame->data[0] + _aSwrOutIdx,
                     fillLen, volumn);

        // 移动偏移量
        len -= fillLen;
        stream += fillLen;
        _aSwrOutIdx += fillLen;
    }
}

/**
 * @brief VideoPlayer::decodeAudio
 * @return 解码出来的pcm大小
 */
int VideoPlayer::decodeAudio(double startTime){
    // 加锁
    _aMutex.lock();

    if (_aPktList.empty() || _state == Stopped) {
        _aMutex.unlock();
        return 0;
    }

    // 取出头部的数据包
    AVPacket pkt = _aPktList.front();
    // 从头部中删除
    _aPktList.pop_front();

    // 解锁
    _aMutex.unlock();

    // 保存音频时钟
    if (pkt.pts != AV_NOPTS_VALUE) {
        _aTime = av_q2d(_aStream->time_base) *pkt.pts;
        // qDebug() << "_aTime" << _vTime <<"AAAstartTime" << startTime ;
        _aTime=_aTime-startTime;

        // 通知外界：播放时间点发生了改变
        emit timeChanged(this);
    }

    // 如果是视频，不能在这个位置判断（不能提前释放pkt，不然会导致B帧、P帧解码失败，画面撕裂）
    // 发现音频的时间是早于seekTime的，直接丢弃
    if (_aSeekTime >= 0) {
        _aSeekTime=_aSeekTime-startTime;
        qDebug() << "_aTime是" << _aTime<<"_aSeekTime"<<_aSeekTime;
        if (_aTime < _aSeekTime) {
            // 释放pkt
            av_packet_unref(&pkt);
            return 0;
        } else {
            _aSeekTime = -1;
        }
    }

    // 发送压缩数据到解码器
    int ret = avcodec_send_packet(_aDecodeCtx, &pkt);
    // 释放pkt
    av_packet_unref(&pkt);
    RET(avcodec_send_packet);

    // 获取解码后的数据
    ret = avcodec_receive_frame(_aDecodeCtx, _aSwrInFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return 0;
    } else RET(avcodec_receive_frame);

    // 重采样输出的样本数
    int outSamples = av_rescale_rnd(_aSwrOutSpec.sampleRate,
                                    _aSwrInFrame->nb_samples,
                                    _aSwrInSpec.sampleRate, AV_ROUND_UP);

    // 由于解码出来的PCM。跟SDL要求的PCM格式可能不一致，需要进行重采样
    ret = swr_convert(_aSwrCtx,
                      _aSwrOutFrame->data,
                      outSamples,
                      (const uint8_t **) _aSwrInFrame->data,
                      _aSwrInFrame->nb_samples);
    RET(swr_convert);

    return ret * _aSwrOutSpec.bytesPerSampleFrame;
}
