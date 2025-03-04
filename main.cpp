#include "playerwidget.h"

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#include<window1.h>
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif
#undef main /* We don't want SDL to override our main() */
#endif

int main(int argc, char *argv[])
{
    // #ifdef _WIN32
    //     QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //     QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    //     //获取主屏分辨率
    //     qreal cx = GetSystemMetrics(SM_CXSCREEN);
    //     qreal scale = cx / 1920.0;

    //     if(scale > 1.0) {
    //         qputenv("QT_SCALE_FACTOR", QString::number(scale).toLatin1());
    //     }
    // #endif
    // 初始化 SDL 音频子系统（只需一次）
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "SDL_Init error" << SDL_GetError();
        return -1;
    }
    QApplication a(argc, argv);
    window1 w;
    w.show();

    // PlayerWidget w2;
    // w2.show();



    // PlayerWidget w;
    // w.show();
    // QString filename1 = "d:/Users/zhangbowen/Videos/02test.mp4"; // 替换为实际路径
    // w.getPlayer()->setFilename(filename1);
    // w.getPlayer()->play();

    // 第二个播放器窗口
    // PlayerWidget w2;
    // w2.show();
    // QString filename2 = "d:/Users/zhangbowen/Videos/45min.mp4"; // 替换为实际路径
    // w2.getPlayer()->setFilename(filename2);
    // w2.getPlayer()->play();

    // PlayerWidget w3;
    // w3.show();

    return a.exec();
}
