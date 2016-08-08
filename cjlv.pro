#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets sql xml

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app
CONFIG += c++11 precompile_header
PRECOMPILED_HEADER = stdafx.h

include($$PWD/3rdparty/quazip-0.7.2/quazip.pri)
include($$PWD/3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/3rdparty/lua-5.3.3/src/lua.pri)

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
    3rdparty/sqlite3/sqlite3.c \
    preferencedialog.cpp \
    extensiondialog.cpp \
    extensionmodel.cpp \
    extension.cpp

HEADERS  += mainwindow.h \
    logview.h \
    logmodel.h \
    tabwidget.h \
    settings.h \
    scintillaconfig.h \
    codeeditortabwidget.h \
    codeeditor.h \
    preferencedialog.h \
    extensiondialog.h \
    extensionmodel.h \
    extension.h \
    stdafx.h

FORMS    += mainwindow.ui \
    preferencedialog.ui \
    extensiondialog.ui

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
    INCLUDEPATH += /usr/local/include
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -F $$PWD/3rdparty/scintilla/bin -framework ScintillaEdit  -lz -framework CoreServices -lobjc

    CONFIG(release, debug|release) : {
        QMAKE_INFO_PLIST = osxInfo.plist
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
        mkdir_extensions.commands = mkdir -p \"$${TARGET}.app/Contents/PlugIns/extensions\"
        copy_extensions.commands = 'cp -R \"$$PWD/extensions\" \"$${TARGET}.app/Contents/PlugIns\"'
        copy_extensions.depends = mkdir_extensions
        copy_themes.commands = cp -R \"$$PWD/resource/MacOSX/themes\" \"$${TARGET}.app/Contents/Resources\"
        copy_language.commands = cp -R \"$$PWD/resource/language\" \"$${TARGET}.app/Contents/Resources\"
        copy_langmap.commands = cp \"$$PWD/resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
        copy_fonts.commands = cp -R \"$$PWD/Fonts\" \"$${TARGET}.app/Contents/Resources/\"
        mkdir_framework.commands = mkdir -p \"$${TARGET}.app/Contents/Frameworks\"
        clean_scintilla.depends = mkdir_framework
        clean_scintilla.commands = rm -rf \"$${TARGET}.app/Contents/Frameworks/ScintillaEdit.framework\"
        copy_scintilla.depends = clean_scintilla
        copy_scintilla.commands = cp -R \"$$PWD/3rdparty/scintilla/bin/ScintillaEdit.framework\" \"$${TARGET}.app/Contents/Frameworks\"

        dmg_installer.depends =  copy_scintilla copy_themes copy_language copy_langmap copy_extensions copy_fonts
        dmg_installer.commands = $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg
        QMAKE_EXTRA_TARGETS +=  mkdir_framework clean_scintilla copy_scintilla copy_themes copy_language copy_langmap mkdir_extensions copy_extensions copy_fonts dmg_installer
        POST_TARGETDEPS += mkdir_framework clean_scintilla copy_scintilla copy_themes copy_language copy_langmap mkdir_extensions copy_extensions copy_fonts
        QMAKE_POST_LINK += $$quote($$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg -appstore-compliant)
    }
}

win32: {
    TARGET = CJLV
    QT += winextras
    QMAKE_LFLAGS += "/LTCG"
    INCLUDEPATH += $$PWD/3rdparty/Everything-SDK/include $$PWD/3rdparty/Everything-SDK/ipc

    SOURCES += everythingwrapper.cpp ShellContextMenu.cpp
    HEADERS += everythingwrapper.h ShellContextMenu.h

    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -L$$PWD/3rdparty/Everything-SDK/lib -lScintillaEdit -lzlib -lUser32 -lShell32 -lpsapi -lOle32

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    CONFIG(release, debug|release): {
        LIBS += -L$$PWD/3rdparty/scintilla/bin/release

        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
        copy_extensions.commands = '$(COPY_DIR) $$shell_path($$PWD/extensions) $$shell_path($$OUT_PWD/Release/extensions/)'
        copy_themes.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/Windows/themes) $$shell_path($$OUT_PWD/Release/themes/)'
        copy_language.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/language) $$shell_path($$OUT_PWD/Release/language/)'
        copy_langmap.commands = '$(COPY_FILE) $$shell_path($$PWD/resource/langmap.xml) $$shell_path($$OUT_PWD/Release/langmap.xml)'
        copy_fonts.commands = '$(COPY_DIR) $$shell_path($$PWD/Fonts) $$shell_path($$OUT_PWD/Release/Fonts/)'

        contains(QMAKE_HOST.arch, x86_64): {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything64.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-msvc-x64.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x64.iss)'
        }
        else: {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything32.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything32.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-msvc-x86.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x86.iss)'
        }

        QMAKE_EXTRA_TARGETS +=  copy_extensions copy_iss copy_themes copy_language copy_langmap copy_everything_dll copy_everything_exe copy_fonts
        POST_TARGETDEPS += copy_extensions copy_iss copy_themes copy_language copy_langmap copy_everything_dll copy_everything_exe copy_fonts
        QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --release --force \"$${OUT_PWD}/Release/$${TARGET}.exe\")
    }
    else: LIBS += -L$$PWD/3rdparty/scintilla/bin/debug
}

