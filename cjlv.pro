#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cjlv
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

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
}
