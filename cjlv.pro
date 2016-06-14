#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app

include(3rdparty/quazip-0.7.2/quazip.pri)

SOURCES += main.cpp\
        mainwindow.cpp \
    logview.cpp \
    logmodel.cpp \
    tabwidget.cpp

HEADERS  += mainwindow.h \
    logview.h \
    logmodel.h \
    tabwidget.h

FORMS    += mainwindow.ui

DISTFILES += \
    cjlv.png

RESOURCES += \
    cjlv.qrc

# Windows icons
RC_FILE = cjlv.rc

# Mac OS X icon
macx: {
    QMAKE_MAC_SDK = macosx10.11
    ICON = cjlv.icns
    icon.path = $$PWD
    icon.files += cjlv.png
    INSTALLS += icon
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lz
}

win32: {
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lzlib
}
