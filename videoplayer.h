﻿#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QDebug>
#include <list>
#include "condmutex.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#define ERROR_BUF \
    char errbuf[1024]; \
    av_strerror(ret, errbuf, sizeof (errbuf));

#define CODE(func,code) \
    if (ret < 0) { \
        ERROR_BUF; \
        qDebug() << #func << "error" << errbuf; \
        code; \
    }

#define END(func) CODE(func,fataError(); return;)
#define RET(func) CODE(func, return ret;)
#define CONTINUE(func) CODE(func, continue;)
#define BREAK(func) CODE(func, break;)

/**
 * 预处理视频数据（不负责显示、渲染视频）
 */
class VideoPlayer : public QObject {
    Q_OBJECT
public:
    // 状态
    typedef enum {
        Stopped = 0,
        Playing,
        Paused
    } State;

    // 音量
    typedef enum {
        Min = 0,
        Max = 100
    } Volumn;

    // 视频frame参数
    typedef struct {
        int width;
        int height;
        AVPixelFormat pixFmt;
        int size;
    } VideoSwsSpec;

    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    /** 播放 */
    void play();
    /** 暂停 */
    void pause();
    /** 停止 */
    void stop();
    /** 是否正在播放中 */
    bool isPlaying();
    /** 获取当前的状态 */
    State getState();
    /** 设置文件名 */
    void setFilename(QString &filename);
    /** 获取总时长(单位是妙，1秒=1000毫秒=1000000微妙)*/
    int getDuration();
    /** 当前的播放时刻（单位是秒） */
    int getTime();
    /** 设置当前的播放时刻（单位是秒） */
    void setTime(int seekTime);

    double startTime;


    /** 设置音量 */
    void setVolumn(int volumn);
    int getVolumn();
    /** 设置静音 */
    void setMute(bool mute);
    bool isMute();

    void closeAudio();


signals:
    void stateChanged(VideoPlayer *player);
    void timeChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);
    void frameDecoded(VideoPlayer *player,uint8_t *data,VideoSwsSpec &spec);

private:

    // 正弦波参数
    double _sinePhase = 0.0; // 相位
     double _sineFreq = 440.0; // 频率（Hz，例如 A 音）
    const int _sineSampleRate = 44100; // 采样率

    SDL_AudioDeviceID _audioDeviceId = 0;
    /******** 音频相关 ********/
    typedef struct {
        int sampleRate;
        AVSampleFormat sampleFmt;
        AVChannelLayout chLayout;
        int chs;
        int bytesPerSampleFrame;
    } AudioSwrSpec;

    /** 解码上下文 */
    AVCodecContext *_aDecodeCtx = nullptr;
    /** 流 */
    AVStream *_aStream = nullptr;
    /** 存放音频包的列表 */
    std::list<AVPacket> _aPktList;
    /** 音频包列表的锁 */
    CondMutex _aMutex;
    /** 音频重采样上下文 */
    SwrContext *_aSwrCtx = nullptr;
    /** 音频重采样输入\输出参数 */
    AudioSwrSpec _aSwrInSpec;
    AudioSwrSpec _aSwrOutSpec;
    /** 音频重采样输入\输出frame */
    AVFrame *_aSwrInFrame = nullptr;
    AVFrame *_aSwrOutFrame = nullptr;
    /** 音频重采样输出PCM的索引（从哪个位置开始取出PCM数据填充到SDL的音频缓冲区） */
    int _aSwrOutIdx = 0;
    /** 音频重采样输出PCM的大小 */
    int _aSwrOutSize = 0;
    /** 音量 */
    int _volumn = Max;
    /** 静音 */
    bool _mute = false;
    /** 音频时钟，当前音频包对应的时间值 */
    double _aTime = 0;
    /** 是否有音频流 */
    bool _hasAudio = false;
    /** 音频资源是否可以释放 */
    bool _aCanFree = false;
    /** 外面设置的当前播放时刻（用于完成seek功能） */
    int _aSeekTime = -1;

    /** 初始化音频信息 */
    int initAudioInfo();
    /** 初始化SDL */
    int initSDL();
    /** 添加数据包到音频包列表中 */
    void addAudioPkt(AVPacket &pkt);
    /** 清空音频包列表 */
    void clearAudioPktList();
    /** SDL填充缓冲区的回调函数 */
    static void sdlAudioCallbackFunc(void *userdata, Uint8 *stream, int len);
    /** SDL填充缓冲区的回调函数 */
    void sdlAudioCallback(Uint8 *stream, int len,double startTime);
    /** 音频解码 */
    int decodeAudio(double startTime);
    /** 初始化音频重采样 */
    int initSwr();

    /******** 视频相关 ********/
    /** 解码上下文 */
    AVCodecContext *_vDecodeCtx = nullptr;
    /** 流 */
    AVStream *_vStream = nullptr;
    /** 像素格式转换的输入\输出frame */
    AVFrame *_vSwsInFrame = nullptr, *_vSwsOutFrame = nullptr;
    /** 像素格式转换的上下文 */
    SwsContext *_vSwsCtx = nullptr;
    /** 像素格式转换的输出frame的参数 */
    VideoSwsSpec _vSwsOutSpec;
    /** 存放视频包的列表 */
    std::list<AVPacket> _vPktList;
    /** 视频包列表的锁 */
    CondMutex _vMutex;
    /** 视频时钟，当前视频包对应的时间值 */
    double _vTime = 0;
    /** 是否有视频流 */
    bool _hasVideo = false;
    /** 视频资源是否可以释放 */
    bool _vCanFree = false;
    /** 外面设置的当前播放时刻（用于完成seek功能） */
    int _vSeekTime = -1;

    /** 初始化视频信息 */
    int initVideoInfo();
    /** 初始化视频像素格式转换 */
    int initSws();
    /** 添加数据包到视频包列表中 */
    void addVideoPkt(AVPacket &pkt);
    /** 清空视频包列表 */
    void clearVideoPktList();
    /** 解码视频 */
    void decodeVideo(double startTime);


    /******** 其他 ********/
    /** 当前的状态 */
    State _state = Stopped;
    /** fmtCtx是否可以释放 */
    bool _fmtCtxCanFree = false;
    /** 文件名 */
    QString _filename;
    // 解封装上下文
    AVFormatContext *_fmtCtx = nullptr;
    /** 外面设置的当前播放时刻（用于完成seek功能） */
    int _seekTime = -1;

    /** 初始化解码器和解码上下文 */
    int initDecoder(AVCodecContext **decodeCtx,AVStream **stream,AVMediaType type);

    /** 改变状态 */
    void setState(State state);
    /** 读取文件数据 */
    void readFile();
    /** 释放资源 */
    void free();
    void freeAudio();
    void freeVideo();
    /** 严重错误 */
    void fataError();
};

#endif // VIDEOPLAYER_H
