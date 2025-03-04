QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    condmutex.cpp \
    main.cpp \
    playerwidget.cpp \
    videoplayer.cpp \
    videoplayer_audio.cpp \
    videoplayer_video.cpp \
    videoslider.cpp \
    videowidget.cpp \
    window1.cpp

HEADERS += \
    condmutex.h \
    playerwidget.h \
    videoplayer.h \
    videoslider.h \
    videowidget.h \
    window1.h

FORMS += \
    playerwidget.ui \
    window1.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

win32{
    FFMPEG_DIR = $$PWD/sdk/ffmpeg
    SDL_DIR = $$PWD/sdk/SDL2
}

INCLUDEPATH += $${FFMPEG_DIR}/include
INCLUDEPATH += $${SDL_DIR}/include

LIBS += -L$${FFMPEG_DIR}/lib \
        -lavcodec \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale

LIBS += -L$${SDL_DIR}/lib \
        -lSDL2

Debug:DESTDIR = $$PWD/bin
Release:DESTDIR = $$PWD/bin

message("Debug Path: " $$PWD/bin)
