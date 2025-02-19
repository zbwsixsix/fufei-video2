#include "videoplayer.h"
#include <thread>
extern "C" {
#include <libavutil/imgutils.h>
}

// 初始化视频信息
int VideoPlayer::initVideoInfo() {
    int ret = initDecoder(&_vDecodeCtx,&_vStream,AVMEDIA_TYPE_VIDEO);
    RET(initDecoder);

    // 初始化像素格式转换
    ret = initSws();
    RET(initSws);

    return 0;
}

int VideoPlayer::initSws(){
    int inW = _vDecodeCtx->width;
    int inH = _vDecodeCtx->height;

    // 输出frame的参数
    _vSwsOutSpec.width = inW >> 4 << 4;// 先除以16在乘以16，保证是16的倍数
    _vSwsOutSpec.height = inH >> 4 << 4;
    // _vSwsOutSpec.pixFmt = AV_PIX_FMT_RGB24;
     _vSwsOutSpec.pixFmt = AV_PIX_FMT_RGBA;
    _vSwsOutSpec.size = av_image_get_buffer_size(_vSwsOutSpec.pixFmt,_vSwsOutSpec.width,_vSwsOutSpec.height, 1);

    // 初始化像素格式转换的上下文
    // _vSwsCtx = sws_getContext(inW,inH,_vDecodeCtx->pix_fmt,
    //                           _vSwsOutSpec.width,_vSwsOutSpec.height,_vSwsOutSpec.pixFmt,
    //                           SWS_BILINEAR, nullptr, nullptr, nullptr);

    _vSwsCtx = sws_getContext(
        inW, inH, _vDecodeCtx->pix_fmt,
        _vSwsOutSpec.width, _vSwsOutSpec.height, AV_PIX_FMT_RGBA,
        SWS_BILINEAR| SWS_ACCURATE_RND,
        nullptr, nullptr, nullptr
        );
    if (!_vSwsCtx) {
        qDebug() << "sws_getContext error";
        return -1;
    }

    sws_setColorspaceDetails(_vSwsCtx,
                             sws_getCoefficients(SWS_CS_DEFAULT), // 输入色彩空间
                             0, // 输入范围 (0-255)
                             sws_getCoefficients(SWS_CS_ITU709), // 输出色彩空间
                             1, // 输出范围 (JPEG范围)
                             0, 1 << 16, 1 << 16
                             );


    // 初始化像素格式转换的输入frame
    _vSwsInFrame = av_frame_alloc();
    if (!_vSwsInFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // 初始化像素格式转换的输出frame
    _vSwsOutFrame = av_frame_alloc();
    if (!_vSwsOutFrame) {
        qDebug() << "av_frame_alloc error";
        return -1;
    }

    // _vSwsOutFrame的data[0]指向的内存空间
    int ret = av_image_alloc(_vSwsOutFrame->data,
                             _vSwsOutFrame->linesize,
                             _vSwsOutSpec.width,
                             _vSwsOutSpec.height,
                             _vSwsOutSpec.pixFmt,
                             1);
    RET(av_image_alloc);

    return 0;
}

void VideoPlayer::addVideoPkt(AVPacket &pkt){
    _vMutex.lock();
    _vPktList.push_back(pkt);
    _vMutex.signal();
    _vMutex.unlock();
}

void VideoPlayer::clearVideoPktList(){
    _vMutex.lock();
    for(AVPacket &pkt : _vPktList){
        av_packet_unref(&pkt);
    }
    _vPktList.clear();
    _vMutex.unlock();
}

void VideoPlayer::freeVideo(){
    clearVideoPktList();
    // 在 clearVideoPktList() 后刷新解码器
    if (_hasVideo) {
        avcodec_flush_buffers(_vDecodeCtx);
    }

    avcodec_free_context(&_vDecodeCtx);
    av_frame_free(&_vSwsInFrame);
    if (_vSwsOutFrame) {
        av_freep(&_vSwsOutFrame->data[0]);
        av_frame_free(&_vSwsOutFrame);
    }
    sws_freeContext(_vSwsCtx);
    _vSwsCtx = nullptr;
    _vStream = nullptr;
    _vTime = 0;
    _vCanFree = false;
    _vSeekTime = -1;
}

void VideoPlayer::decodeVideo( double startTime){
    // 处理参考帧丢失的容错机制
    if (_vDecodeCtx->codec_id == AV_CODEC_ID_HEVC) {
        _vDecodeCtx->skip_frame = AVDISCARD_NONREF; // 跳过非参考帧
        _vDecodeCtx->err_recognition |= AV_EF_EXPLODE; // 严格错误检测
    }



    while (true) {
        // 如果是暂停，并且没有Seek操作
        if (_state == Paused && _vSeekTime == -1) {
            continue;
        }

        if (_state == Stopped) {
            _vCanFree = true;
            break;
        }

        _vMutex.lock();
        if(_vPktList.empty()){
            _vMutex.unlock();
            continue;
        }

        // 取出头部的视频包
        AVPacket pkt = _vPktList.front();
        _vPktList.pop_front();
        _vMutex.unlock();

        // 视频时钟
        if (pkt.dts != AV_NOPTS_VALUE) {

            _vTime = av_q2d(_vStream->time_base) * pkt.dts;

            // qDebug() << "_vTime" << _vTime << "startTime" << startTime ;

                _vTime=_vTime-startTime;
        }

        // 发送压缩数据到解码器
        int ret = avcodec_send_packet(_vDecodeCtx, &pkt);
        // 释放pkt
        av_packet_unref(&pkt);
        CONTINUE(avcodec_send_packet);

        while (true) {
            ret = avcodec_receive_frame(_vDecodeCtx, _vSwsInFrame);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                break;
            } else BREAK(avcodec_receive_frame);

            // 一定要在解码成功后，再进行下面的判断
            // 发现视频的时间是早于seekTime的，直接丢弃
            if(_vSeekTime >= 0){
                // if (_hasVideo) avcodec_flush_buffers(_vDecodeCtx);
                // if (_hasAudio) avcodec_flush_buffers(_aDecodeCtx);
                _vSeekTime=_vSeekTime-startTime;
                qDebug() << "_vTime是" << _vTime<<"_vSeekTime"<<_vSeekTime;
                if (_vTime < _vSeekTime) {
                    continue;// 丢掉
                } else {
                    _vSeekTime = -1;
                }
            }

            // 像素格式的转换
            sws_scale(_vSwsCtx,
                      _vSwsInFrame->data, _vSwsInFrame->linesize,
                      0, _vDecodeCtx->height,
                      _vSwsOutFrame->data, _vSwsOutFrame->linesize);

            if(_hasAudio){// 有音频
                // 如果视频包过早被解码出来，那就需要等待对应的音频时钟到达
                while (_vTime > _aTime && _state == Playing) {
                    SDL_Delay(1);
                }
            }

            uint8_t *data = (uint8_t *)av_malloc(_vSwsOutSpec.size);
            memcpy(data, _vSwsOutFrame->data[0], _vSwsOutSpec.size);
            // 发出信号
            emit frameDecoded(this,data,_vSwsOutSpec);
            // qDebug()<< "渲染了一帧"<< _vTime << _aTime;
        }
    }
}
