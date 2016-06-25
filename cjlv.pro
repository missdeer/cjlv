#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets sql xml

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app
CONFIG += c++11
include($$PWD/3rdparty/quazip-0.7.2/quazip.pri)

INCLUDEPATH += $$PWD/3rdparty/scintilla/qt/ScintillaEditBase \
    $$PWD/3rdparty/scintilla/qt/ScintillaEdit \
    $$PWD/3rdparty/scintilla/include \
    $$PWD/3rdparty/scintilla/src \
    $$PWD/3rdparty/scintilla/lexlib \
    $$PWD/3rdparty/sqlite3

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT

SOURCES += main.cpp\
        mainwindow.cpp \
    logview.cpp \
    logmodel.cpp \
    tabwidget.cpp \
    settings.cpp \
    scintillaconfig.cpp \
    codeeditortabwidget.cpp \
    codeeditor.cpp \
    3rdparty/sqlite3/sqlite3.c

HEADERS  += mainwindow.h \
    logview.h \
    logmodel.h \
    tabwidget.h \
    settings.h \
    scintillaconfig.h \
    codeeditortabwidget.h \
    codeeditor.h

FORMS    += mainwindow.ui

DISTFILES += \
    cjlv.png

RESOURCES += \
    cjlv.qrc

# Windows icons
RC_FILE = cjlv.rc

# Mac OS X icon
macx: {
    OBJECTIVE_SOURCES += \
        mdfindwrapper.mm

    QMAKE_MAC_SDK = macosx10.11
    ICON = cjlv.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -F $$PWD/3rdparty/scintilla/bin -framework ScintillaEdit  -lz -framework CoreServices -lobjc

    CONFIG(release, debug|release) : {
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
        copy_themes.commands = cp -R \"$$PWD/resource/MacOSX/themes\" \"$${TARGET}.app/Contents/Resources\"
        copy_language.commands = cp -R \"$$PWD/resource/language\" \"$${TARGET}.app/Contents/Resources\"
        copy_langmap.commands = cp \"$$PWD/resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
        mkdir_framework.commands = mkdir -p \"$${TARGET}.app/Contents/Frameworks\"
        clean_scintilla.depends = mkdir_framework
        clean_scintilla.commands = rm -rf \"$${TARGET}.app/Contents/Frameworks/ScintillaEdit.framework\"
        copy_scintilla.depends = clean_scintilla
        copy_scintilla.commands = cp -R \"$$PWD/3rdparty/scintilla/bin/ScintillaEdit.framework\" \"$${TARGET}.app/Contents/Frameworks\"

        dmg_installer.depends = copy_scintilla copy_themes copy_language copy_langmap
        dmg_installer.commands = $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg
        QMAKE_EXTRA_TARGETS +=  mkdir_framework clean_scintilla copy_scintilla copy_themes copy_language copy_langmap dmg_installer
        POST_TARGETDEPS += mkdir_framework clean_scintilla copy_scintilla copy_themes copy_language copy_langmap
        QMAKE_POST_LINK += $$quote($$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg -appstore-compliant)
    }
}

win32: {
    INCLUDEPATH += $$PWD/3rdparty/Everything-SDK/include $$PWD/3rdparty/Everything-SDK/ipc
    SOURCES += everythingwrapper.cpp

    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lScintillaEdit3 -lzlib -L$$PWD/3rdparty/Everything-SDK/lib -lUser32 -lShell32

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    CONFIG(release, debug|release): {
        LIBS += -L$$PWD/3rdparty/scintilla/bin/release

        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
        copy_themes.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/Windows/themes) $$shell_path($$OUT_PWD/Release/themes/)'
        copy_language.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/language) $$shell_path($$OUT_PWD/Release/language/)'
        copy_langmap.commands = '$(COPY_FILE) $$shell_path($$PWD/resource/langmap.xml) $$shell_path($$OUT_PWD/Release/langmap.xml)'
        copy_scintilla.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/scintilla/bin/release/ScintillaEdit3.dll) $$shell_path($$OUT_PWD/Release/ScintillaEdit3.dll)'
        copy_everything.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'

        contains(QMAKE_HOST.arch, x86_64): copy_everything.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
        else: LIBS += copy_everything.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything32.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'

        QMAKE_EXTRA_TARGETS +=  copy_scintilla copy_themes copy_language copy_langmap copy_everything
        POST_TARGETDEPS += copy_scintilla copy_themes copy_language copy_langmap copy_everything
        QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --release --force \"$${OUT_PWD}/Release/$${TARGET}.exe\")
    }
    else: LIBS += -L$$PWD/3rdparty/scintilla/bin/debug
}

