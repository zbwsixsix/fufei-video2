#include "playerwidget.h"

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
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

    QApplication a(argc, argv);
    PlayerWidget w;
    w.show();

    PlayerWidget w2;
    w2.show();


    // 示例：自动加载第一个视频文件
    // QString filename1 = "d:/Users/zhangbowen/Videos/02test.mp4";  // 替换为实际路径
    // w.findChild<VideoPlayer*>("_player")->setFilename(filename1);
    // w.findChild<VideoPlayer*>("_player")->play();

    // 创建第二个窗口并加载第二个视频
    // PlayerWidget w2;
    // w2.show();
    // // 示例：自动加载第二个视频文件
    // QString filename2 = "d:/Users/zhangbowen/Videos/13min.mp4";  // 替换为实际路径
    // w2.findChild<VideoPlayer*>("_player")->setFilename(filename2);
    // w2.findChild<VideoPlayer*>("_player")->play();


    // PlayerWidget w3;
    // w3.show();
    return a.exec();
}
