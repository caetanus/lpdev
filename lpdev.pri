#-------------------------------------------------
#
# Project created by QtCreator 2012-08-25T09:28:21
#
#-------------------------------------------------

QT       -= gui
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD


SOURCES += $$PWD/lpdev.cpp

HEADERS += $$PWD/lpdev.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/local/lib
    }
    INSTALLS += target
}
