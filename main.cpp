#include "playerwidget.h"
#include <QApplication>
#ifdef _WIN32
#include <windows.h>
#include <window1.h>
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif
#undef main
#endif

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_AUDIO)) {
        qDebug() << "SDL_Init error" << SDL_GetError();
        return -1;
    }

    QApplication a(argc, argv);
    window1 w;
    w.show();
    int ret = a.exec();
    SDL_Quit(); // 添加 SDL 清理
    return ret;
}
