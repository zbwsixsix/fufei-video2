#include "mainwindow.h"

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "user32.lib")
#undef main /* We don't want SDL to override our main() */
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    //获取主屏分辨率
    qreal cx = GetSystemMetrics(SM_CXSCREEN);
    qreal scale = cx / 1920.0;

    if(scale > 1.0) {
        qputenv("QT_SCALE_FACTOR", QString::number(scale).toLatin1());
    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
