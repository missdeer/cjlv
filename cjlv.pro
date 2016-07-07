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
    $$PWD/3rdparty/sqlite3 \
    $$PWD/3rdparty/lua-5.3.3/src

LIBS += -L$$PWD/3rdparty/lua-5.3.3/src

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
    extension.h

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
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -F $$PWD/3rdparty/scintilla/bin -framework ScintillaEdit  -lz -framework CoreServices -lobjc -llua

    CONFIG(release, debug|release) : {
        QMAKE_INFO_PLIST = osxInfo.plist
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
        copy_extensions.commands = 'cp -R \"$$PWD/extensions\" \"$${TARGET}.app/Contents/PlugIns\"'
        copy_themes.commands = cp -R \"$$PWD/resource/MacOSX/themes\" \"$${TARGET}.app/Contents/Resources\"
        copy_language.commands = cp -R \"$$PWD/resource/language\" \"$${TARGET}.app/Contents/Resources\"
        copy_langmap.commands = cp \"$$PWD/resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
        mkdir_framework.commands = mkdir -p \"$${TARGET}.app/Contents/Frameworks\"
        clean_scintilla.depends = mkdir_framework
        clean_scintilla.commands = rm -rf \"$${TARGET}.app/Contents/Frameworks/ScintillaEdit.framework\"
        copy_scintilla.depends = clean_scintilla
        copy_scintilla.commands = cp -R \"$$PWD/3rdparty/scintilla/bin/ScintillaEdit.framework\" \"$${TARGET}.app/Contents/Frameworks\"

        dmg_installer.depends = copy_extensions copy_scintilla copy_themes copy_language copy_langmap
        dmg_installer.commands = $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg
        QMAKE_EXTRA_TARGETS +=  mkdir_framework copy_extensions clean_scintilla copy_scintilla copy_themes copy_language copy_langmap dmg_installer
        POST_TARGETDEPS += mkdir_framework copy_extensions clean_scintilla copy_scintilla copy_themes copy_language copy_langmap
        QMAKE_POST_LINK += $$quote($$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -dmg -appstore-compliant)
    }
}

win32: {
    INCLUDEPATH += $$PWD/3rdparty/Everything-SDK/include $$PWD/3rdparty/Everything-SDK/ipc
    SOURCES += everythingwrapper.cpp
    HEADERS += everythingwrapper.h

    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lScintillaEdit3 -lzlib -L$$PWD/3rdparty/Everything-SDK/lib -lUser32 -lShell32 -lpsapi -lOle32 -llua5.3

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    CONFIG(release, debug|release): {
        LIBS += -L$$PWD/3rdparty/scintilla/bin/release

        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
        copy_extensions.commands = '$(COPY_DIR) $$shell_path($$PWD/extensions) $$shell_path($$OUT_PWD/Release/extensions/)'
        copy_themes.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/Windows/themes) $$shell_path($$OUT_PWD/Release/themes/)'
        copy_language.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/language) $$shell_path($$OUT_PWD/Release/language/)'
        copy_langmap.commands = '$(COPY_FILE) $$shell_path($$PWD/resource/langmap.xml) $$shell_path($$OUT_PWD/Release/langmap.xml)'
        copy_lua.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/lua-5.3.3/src/lua5.3.dll) $$shell_path($$OUT_PWD/Release/lua5.3.dll)'
        copy_scintilla.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/scintilla/bin/release/ScintillaEdit3.dll) $$shell_path($$OUT_PWD/Release/ScintillaEdit3.dll)'
        copy_everything.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'

        contains(QMAKE_HOST.arch, x86_64): {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything64.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-win64.iss) $$shell_path($$OUT_PWD/Release/cjlv-win64.iss)'
        }
        else: {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything32.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything32.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-win32.iss) $$shell_path($$OUT_PWD/Release/cjlv-win32.iss)'
        }

        QMAKE_EXTRA_TARGETS +=  copy_extensions copy_scintilla copy_lua copy_iss copy_themes copy_language copy_langmap copy_everything_dll copy_everything_exe
        POST_TARGETDEPS += copy_extensions copy_scintilla copy_lua copy_iss copy_themes copy_language copy_langmap copy_everything_dll copy_everything_exe
        QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --release --force \"$${OUT_PWD}/Release/$${TARGET}.exe\")
    }
    else: LIBS += -L$$PWD/3rdparty/scintilla/bin/debug
}

