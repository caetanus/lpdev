#-------------------------------------------------
#
# Project created by QtCreator 2012-08-25T09:28:21
#
#-------------------------------------------------

QT       -= gui

TARGET = lpdev
TEMPLATE = lib
CONFIG += staticlib

SOURCES += lpdev.cpp

HEADERS += lpdev.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
