#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets sql xml

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app

include($$PWD/3rdparty/quazip-0.7.2/quazip.pri)

INCLUDEPATH += $$PWD/3rdparty/rapidxml-1.13 \
    $$PWD/3rdparty/scintilla/qt/ScintillaEditBase \
    $$PWD/3rdparty/scintilla/qt/ScintillaEdit \
    $$PWD/3rdparty/scintilla/include \
    $$PWD/3rdparty/scintilla/src \
    $$PWD/3rdparty/scintilla/lexlib

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT

SOURCES += main.cpp\
        mainwindow.cpp \
    logview.cpp \
    logmodel.cpp \
    tabwidget.cpp \
    settings.cpp \
    scintillaconfig.cpp

HEADERS  += mainwindow.h \
    logview.h \
    logmodel.h \
    tabwidget.h \
    settings.h \
    scintillaconfig.h

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
    #icon.files += cjlv.png
    INSTALLS += icon
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -F $$PWD/3rdparty/scintilla/bin -framework ScintillaEdit  -lz

    copy_themes.commands = cp -R \"$$PWD/resource/MacOSX/themes\" \"$${INSTALLER_NAME}.app/Contents/Resources\"
    copy_language.commands = cp -R \"$$PWD/resource/language\" \"$${INSTALLER_NAME}.app/Contents/Resources\"
    copy_scintilla.commands = cp -R \"$$PWD/3rdparty/scintilla/bin/ScintillaEdit.framework\" \"$${INSTALLER_NAME}.app/Contents/Resources\"

    codesign_installer.commands = codesign -s \"$(SIGNING_IDENTITY)\" $(SIGNING_FLAGS) \"$${INSTALLER_NAME}.app\"
    dmg_installer.commands = hdiutil create -srcfolder "$${INSTALLER_NAME}.app" -volname \"Cisco Jabber Log Viewer\" -format UDBZ "CiscoJabberLogViewer-installer.dmg" -ov -scrub -stretch 2g
    QMAKE_EXTRA_TARGETS +=  copy_scintilla copy_themes copy_language codesign_installer dmg_installer
}

win32: {
    CONFIG(release, debug|release): LIBS += -L$$PWD/3rdparty/scintilla/bin/release
    else: LIBS += -L$$PWD/3rdparty/scintilla/bin/debug
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lScintillaEdit3 -lzlib
}
