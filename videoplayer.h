#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QDebug>
#include <list>
#include "condmutex.h"
#include <QSize> // 新增：用于返回分辨率

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
    typedef enum {
        Stopped = 0,
        Playing,
        Paused
    } State;

    typedef enum {
        Min = 0,
        Max = 100
    } Volumn;

    typedef struct {
        int width;
        int height;
        AVPixelFormat pixFmt;
        int size;
    } VideoSwsSpec;

    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    void play();
    void pause();
    void stop();
    bool isPlaying();
    State getState();
    void setFilename(QString &filename);
    int getDuration();
    int getTime();
    void setTime(int seekTime);

    double startTime;

    void setVolumn(int volumn);
    int getVolumn();
    void setMute(bool mute);
    bool isMute();

    void closeAudio();

    // 新增：获取视频分辨率
    QSize getResolution() const;

signals:
    void stateChanged(VideoPlayer *player);
    void timeChanged(VideoPlayer *player);
    void initFinished(VideoPlayer *player);
    void playFailed(VideoPlayer *player);
    void frameDecoded(VideoPlayer *player, uint8_t *data, VideoSwsSpec &spec);

private:
    SDL_AudioDeviceID _audioDeviceId = 0;

    typedef struct {
        int sampleRate;
        AVSampleFormat sampleFmt;
        AVChannelLayout chLayout;
        int chs;
        int bytesPerSampleFrame;
    } AudioSwrSpec;

    AVCodecContext *_aDecodeCtx = nullptr;
    AVStream *_aStream = nullptr;
    std::list<AVPacket> _aPktList;
    CondMutex _aMutex;
    SwrContext *_aSwrCtx = nullptr;
    AudioSwrSpec _aSwrInSpec;
    AudioSwrSpec _aSwrOutSpec;
    AVFrame *_aSwrInFrame = nullptr;
    AVFrame *_aSwrOutFrame = nullptr;
    int _aSwrOutIdx = 0;
    int _aSwrOutSize = 0;
    int _volumn = Max;
    bool _mute = false;
    double _aTime = 0;
    bool _hasAudio = false;
    bool _aCanFree = false;
    int _aSeekTime = -1;

    int initAudioInfo();
    int initSDL();
    void addAudioPkt(AVPacket &pkt);
    void clearAudioPktList();
    static void sdlAudioCallbackFunc(void *userdata, Uint8 *stream, int len);
    void sdlAudioCallback(Uint8 *stream, int len, double startTime);
    int decodeAudio(double startTime);
    int initSwr();

    AVCodecContext *_vDecodeCtx = nullptr;
    AVStream *_vStream = nullptr;
    AVFrame *_vSwsInFrame = nullptr, *_vSwsOutFrame = nullptr;
    SwsContext *_vSwsCtx = nullptr;
    VideoSwsSpec _vSwsOutSpec;
    std::list<AVPacket> _vPktList;
    CondMutex _vMutex;
    double _vTime = 0;
    bool _hasVideo = false;
    bool _vCanFree = false;
    int _vSeekTime = -1;

    int initVideoInfo();
    int initSws();
    void addVideoPkt(AVPacket &pkt);
    void clearVideoPktList();
    void decodeVideo(double startTime);

    State _state = Stopped;
    bool _fmtCtxCanFree = false;
    QString _filename;
    AVFormatContext *_fmtCtx = nullptr;
    int _seekTime = -1;

    int initDecoder(AVCodecContext **decodeCtx, AVStream **stream, AVMediaType type);
    void setState(State state);
    void readFile();
    void free();
    void freeAudio();
    void freeVideo();
    void fataError();
};

#endif // VIDEOPLAYER_H
