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

    qDebug() << "Swr Out: fmt=" << av_get_sample_fmt_name(_aSwrOutSpec.sampleFmt)
             << "rate=" << _aSwrOutSpec.sampleRate << "chs=" << _aSwrOutSpec.chs;


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
    ret = av_samples_alloc(_aSwrOutFrame->data, &_aSwrOutFrame->linesize[0],
                           _aSwrOutSpec.chs, 4096, _aSwrOutSpec.sampleFmt, 1);
    qDebug() << "Allocated buffer size:" << _aSwrOutFrame->linesize[0];

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
    // SDL_CloseAudio();
    closeAudio();
}

void VideoPlayer::sdlAudioCallbackFunc(void *userdata, uint8_t *stream, int len){
    VideoPlayer *player = (VideoPlayer *)userdata;
    // qDebug() << "DDDDDDDLLLLLstartTime" << player->startTime ;
    player->sdlAudioCallback(stream,len,player->startTime);
}

int VideoPlayer::initSDL(){
    int numDevices = SDL_GetNumAudioDevices(0);
    qDebug() << "Available audio devices:" << numDevices;
    for (int i = 0; i < numDevices; ++i) {
        qDebug() << "Device" << i << ":" << SDL_GetAudioDeviceName(i, 0);
    }
    // 音频参数
    SDL_AudioSpec spec;
    spec.freq = _aSwrOutSpec.sampleRate;     // 采样率（如44.1kHz）
    spec.format = AUDIO_S16LSB;              // 采样格式（16位有符号小端）
    spec.channels = _aSwrOutSpec.chs;        // 声道数（如2声道立体声）
    spec.samples = 512;                      // 音频缓冲区大小（必须是2的幂）
    spec.callback = sdlAudioCallbackFunc;    // 音频回调函数
    spec.userdata = this;                    // 回调函数的用户数据（传递this指针）
    // 打开独立的音频设备
    _audioDeviceId = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
    // _audioDeviceId = SDL_OpenAudioDevice(SDL_GetAudioDeviceName(1, 0), 0, &spec, nullptr, 0);
    if (_audioDeviceId == 0) {
        qDebug() << "AAAAA SDL_OpenAudioDevice error" << SDL_GetError();
        return -1;
    }
    qDebug() << "SDL: freq=" << spec.freq << "channels=" << (int)spec.channels
             << "format=AUDIO_S16LSB" << "device ID=" << _audioDeviceId;


    // 打开音频设备
    // 验证缓冲区大小


    if (SDL_OpenAudio(&spec, nullptr)) {
        qDebug() << "SDL_OpenAudio error" << SDL_GetError();
        return -1;
    }
    SDL_AudioStatus status = SDL_GetAudioDeviceStatus(_audioDeviceId);
    qDebug() << "Initial device status:" << (status == SDL_AUDIO_PLAYING ? "PLAYING" : "STOPPED");
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
    // qDebug() << "sdlAudioCallback called for device" << _audioDeviceId << "len:" << len;
    SDL_memset(stream, 0, len);
    // 清零（静音）


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
                qDebug() << "No audio decoded, filling silence"<< _aSwrOutSize;
            } else {
                // qDebug() << "Decoded audio size:" << _aSwrOutSize;
            }
        }

        // 本次需要填充到stream中的PCM数据大小
        int fillLen = std::min(_aSwrOutSize - _aSwrOutIdx, len);
        int volumn = _mute ? 0 : ((_volumn * 1.0 / Max) * SDL_MIX_MAXVOLUME);
        // qDebug() << "Fill length:" << fillLen << "Volume:" << volumn << "Mute:" << _mute;

        SDL_MixAudio(stream, _aSwrOutFrame->data[0] + _aSwrOutIdx, fillLen, volumn);

        // 移动偏移量
        len -= fillLen;
        stream += fillLen;
        _aSwrOutIdx += fillLen;
    }
}


// 修改回调函数以生成正弦波
// void VideoPlayer::sdlAudioCallback(Uint8 *stream, int len, double startTime) {
//     (void)startTime; // 未使用 startTime
//     int16_t *buffer = (int16_t *)stream;
//     int samples = len / sizeof(int16_t);

//     for (int i = 0; i < samples; i++) {
//         double time = _sinePhase + (i * _sineFreq / _sineSampleRate);
//         buffer[i] = static_cast<int16_t>(32767 * sin(2.0 * M_PI * time));
//     }

//     _sinePhase += samples * _sineFreq / _sineSampleRate;
//     _sinePhase = fmod(_sinePhase, 1.0);

//     qDebug() << "Callback for device" << _audioDeviceId << "freq:" << _sineFreq << "len:" << len;
// }


/**
 * @brief VideoPlayer::decodeAudio
 * @return 解码出来的pcm大小
 */
int VideoPlayer::decodeAudio(double startTime) {


    _aMutex.lock();
    if (_aPktList.empty() || _state == Stopped) {
        _aMutex.unlock();
        qDebug() << "No audio packets available or stopped";
        return 0;
    }

    AVPacket pkt = _aPktList.front();
    _aPktList.pop_front();
    _aMutex.unlock();

    if (pkt.pts != AV_NOPTS_VALUE) {
        _aTime = av_q2d(_aStream->time_base) * pkt.pts;
        _aTime = _aTime - startTime;
        // qDebug() << "Audio time:" << _aTime;
        emit timeChanged(this);
    }

    if (_aSeekTime >= 0) {
        _aSeekTime = _aSeekTime - startTime;
        if (_aTime < _aSeekTime) {
            av_packet_unref(&pkt);
            qDebug() << "Audio packet skipped due to seek";
            return 0;
        } else {
            _aSeekTime = -1;
        }
    }

    int ret = avcodec_send_packet(_aDecodeCtx, &pkt);
    av_packet_unref(&pkt);
    if (ret < 0) {
        ERROR_BUF;
        qDebug() << "avcodec_send_packet error:" << errbuf;
        return 0;
    }

    ret = avcodec_receive_frame(_aDecodeCtx, _aSwrInFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        qDebug() << "No frame received (EAGAIN or EOF)";
        return 0;
    } else if (ret < 0) {
        ERROR_BUF;
        qDebug() << "avcodec_receive_frame error:" << errbuf;
        return 0;
    }

    int outSamples = av_rescale_rnd(_aSwrOutSpec.sampleRate,
                                    _aSwrInFrame->nb_samples,
                                    _aSwrInSpec.sampleRate, AV_ROUND_UP);
    ret = swr_convert(_aSwrCtx,
                      _aSwrOutFrame->data,
                      outSamples,
                      (const uint8_t **)_aSwrInFrame->data,
                      _aSwrInFrame->nb_samples);
    if (ret < 0) {
        ERROR_BUF;
        qDebug() << "swr_convert error:" << errbuf;
        return 0;
    }

    int pcmSize = ret * _aSwrOutSpec.bytesPerSampleFrame;
    // qDebug() << "Decoded PCM size:" << pcmSize;



    int16_t *pcmData = (int16_t *)_aSwrOutFrame->data[0];
    bool allZero = true;
    for (int i = 0; i < pcmSize / 2; i++) {
        if (pcmData[i] != 0) {
            allZero = false;
            break;
        }
    }
    // qDebug() << "PCM data all zero:" << allZero;
    return pcmSize;
}
