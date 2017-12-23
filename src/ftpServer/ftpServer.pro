#-------------------------------------------------
#
# Project created by QtCreator 2017-12-15T19:48:38
#
#-------------------------------------------------

QT       += core gui

LIBS += -lpthread libwsock32 libws2_32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ftpServer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG(debug, debug|release) {
DESTDIR = $$PWD/../../bin/debug
OUT_PWD = $$PWD/../../bin/debug
} else {
DESTDIR = $$PWD/../../bin/release
OUT_PWD = $$PWD/../../bin/release
}

SOURCES += \
        main.cpp \
        ftpserver.cpp \
    server.cpp \
    serverthread.cpp \
    serverconfig.cpp \
    listenthread.cpp

HEADERS += \
        ftpserver.h \
    server.h \
    serverthread.h \
    serverconfig.h \
    listenthread.h

FORMS += \
        ftpserver.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/ -lcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/ -lcommon
else:unix: LIBS += -L$$OUT_PWD/../common/ -lcommon

INCLUDEPATH += $$PWD/../common
DEPENDPATH += $$PWD/../common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libcommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/libcommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/common.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/common.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../common/libcommon.a

