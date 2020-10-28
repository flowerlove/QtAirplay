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
    airserver.cpp \
    airservercallback.cpp \
    autolock.cpp \
    castscreenmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    sdlplayer.cpp

HEADERS += \
    airserver.h \
    airservercallback.h \
    autolock.h \
    castscreenmanager.h \
    mainwindow.h \
    sdlplayer.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR = ./bin

LIBS += ./external/ffmpeg/lib/x64/avcodec.lib
LIBS += ./external/ffmpeg/lib/x64/swscale.lib
LIBS += ./external/ffmpeg/lib/x64/avutil.lib
LIBS += ./external/SDL/lib/x64/SDL2.lib
LIBS += ./lib/dnssd.lib
LIBS += ./lib/airplay2.lib
LIBS += ./lib/airplay2dll.lib
LIBS += WS2_32.lib
LIBS += WinMM.lib

INCLUDEPATH += ./include
INCLUDEPATH += ./external/ffmpeg/include
INCLUDEPATH += ./external/plist/include
INCLUDEPATH += ./external/SDL/include
INCLUDEPATH += ./external/ffmpeg/include/libavcodec
INCLUDEPATH += ./external/ffmpeg/include/libavutil
INCLUDEPATH += ./external/ffmpeg/include/libswscale
