#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets xml charts network qml quick quickwidgets

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app
CONFIG += c++11 precompile_header
PRECOMPILED_HEADER = stdafx.h

include($$PWD/3rdparty/qtkeychain/qt5keychain.pri)
include($$PWD/3rdparty/quazip-0.7.2/quazip.pri)
include($$PWD/3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/3rdparty/lua-5.3.3/src/lua.pri)
include($$PWD/3rdparty/scintilla/qt/ScintillaEdit/ScintillaEdit.pri)
include($$PWD/3rdparty/sqlite3/sqlite3.pri)
include($$PWD/Boost.pri)

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 LOKI_FUNCTOR_IS_NOT_A_SMALLOBJECT

QML_FILES = qml/main.qml \
    qml/control/qmldir \
    qml/control/CheckBoxIndicator.qml \
    qml/control/RangeSliderBackground.qml \
    qml/control/RangeSliderHandle.qml \

SOURCES += main.cpp\
        mainwindow.cpp \
    logview.cpp \
    logmodel.cpp \
    tabwidget.cpp \
    settings.cpp \
    scintillaconfig.cpp \
    codeeditortabwidget.cpp \
    codeeditor.cpp \
    preferencedialog.cpp \
    extensiondialog.cpp \
    extensionmodel.cpp \
    extension.cpp \
    sourcewindow.cpp \
    sourceviewtabwidget.cpp \
    presencemodel.cpp \
    presencewidget.cpp \
    prtlistitemdelegate.cpp \
    shortcutedit.cpp \
    quickwidgetapi.cpp \
    sqlite3helper.cpp \
    popupmenutoolbutton.cpp

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
    stdafx.h \
    sourcewindow.h \
    sourceviewtabwidget.h \
    presencemodel.h \
    presencewidget.h \
    prtlistitemdelegate.h \
    shortcutedit.h \
    quickwidgetapi.h \
    sqlite3helper.h \
    popupmenutoolbutton.h

FORMS    += mainwindow.ui \
    preferencedialog.ui \
    extensiondialog.ui \
    sourcewindow.ui

DISTFILES += \
    cjlv.png \
    qml/control/qmldir

RESOURCES += \
    cjlv.qrc

# Windows icons
RC_FILE = cjlv.rc

# Mac OS X icon
macx: {
    OBJECTIVE_SOURCES += \
        mdfindwrapper.mm

    #QMAKE_MAC_SDK = macosx10.11
    ICON = cjlv.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon
    INCLUDEPATH += /usr/local/include
    LIBS+=-L$$PWD/3rdparty/zlib-1.2.8 -lz -framework CoreServices -lobjc

    CONFIG(release, debug|release) : {
        QMAKE_INFO_PLIST = osxInfo.plist
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt
        #mkdir_extensions.commands = mkdir -p \"$${TARGET}.app/Contents/PlugIns/extensions\"
        #copy_extensions.commands = 'cp -R \"$$PWD/extensions\" \"$${TARGET}.app/Contents/PlugIns\"'
        #copy_extensions.depends = mkdir_extensions
        #copy_themes.commands = cp -R \"$$PWD/resource/themes\" \"$${TARGET}.app/Contents/Resources\"
        #copy_language.commands = cp -R \"$$PWD/resource/language\" \"$${TARGET}.app/Contents/Resources\"
        #copy_langmap.commands = cp \"$$PWD/resource/langmap.xml\" \"$${TARGET}.app/Contents/Resources/\"
        #copy_fonts.commands = cp -R \"$$PWD/Fonts\" \"$${TARGET}.app/Contents/Resources/\"

        #deploy.depends += copy_fonts
        deploy.commands += $$MACDEPLOYQT \"$${OUT_PWD}/$${TARGET}.app\" -appstore-compliant \"-qmldir=$${PWD}/qml\"

        APPCERT = Developer ID Application: Fan Yang (Y73SBCN2CG)
        INSTALLERCERT = 3rd Party Mac Developer Installer: Fan Yang (Y73SBCN2CG)
        BUNDLEID = com.cisco.jabber.viewer

        codesign_bundle.depends += deploy
        codesign_bundle.commands = codesign -s \"$${APPCERT}\" -v -f --timestamp=none --deep \"$${OUT_PWD}/$${TARGET}.app\"
        makedmg.depends += codesign_bundle
        makedmg.commands = hdiutil create -srcfolder \"$${TARGET}.app\" -volname \"$${TARGET}\" -format UDBZ \"$${TARGET}.dmg\" -ov -scrub -stretch 2g
        QMAKE_EXTRA_TARGETS +=  deploy codesign_bundle makedmg
    }
}

win32: {
    RESOURCES += \
        win.qrc
    win32-msvc* {
        QMAKE_LFLAGS += "/LTCG"
        QMAKE_CXXFLAGS_RELEASE += /Zi
        QMAKE_LFLAGS_RELEASE += /DEBUG
        LIBS += -L$$PWD/3rdparty/zlib-1.2.8 -lzlib
    } else {
        LIBS += -lz
    }

    DISTFILES += \
        cjlv.rc
    TARGET = CJLV
    QT += winextras
    INCLUDEPATH += $$PWD/3rdparty/Everything-SDK/include $$PWD/3rdparty/Everything-SDK/ipc

    SOURCES += everythingwrapper.cpp ShellContextMenu.cpp
    HEADERS += everythingwrapper.h ShellContextMenu.h

    LIBS+=-L$$PWD/3rdparty/Everything-SDK/lib -lUser32 -lShell32 -lpsapi -lOle32

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    CONFIG(release, debug|release): {
        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
        #copy_extensions.commands = '$(COPY_DIR) $$shell_path($$PWD/extensions) $$shell_path($$OUT_PWD/Release/)'
        #copy_themes.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/themes) $$shell_path($$OUT_PWD/Release/)'
        #copy_language.commands = '$(COPY_DIR) $$shell_path($$PWD/resource/language) $$shell_path($$OUT_PWD/Release/)'
        #copy_langmap.commands = '$(COPY_FILE) $$shell_path($$PWD/resource/langmap.xml) $$shell_path($$OUT_PWD/Release/langmap.xml)'
        #copy_fonts.commands = '$(COPY_DIR) $$shell_path($$PWD/Fonts) $$shell_path($$OUT_PWD/Release/)'

        contains(QMAKE_HOST.arch, x86_64): {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything64.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            win32-msvc* {
                copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-msvc-x64.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x64.iss)'
            } else {
                copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-static-x64.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x64.iss)'
            }
        }
        else: {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything32.dll) $$shell_path($$OUT_PWD/Release/Everything.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything32.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
            win32-msvc* {
                copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-msvc-x86.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x86.iss)'
            } else {
                copy_iss.commands = '$(COPY_FILE) $$shell_path($$PWD/cjlv-static-x86.iss) $$shell_path($$OUT_PWD/Release/cjlv-msvc-x64.iss)'
            }
        }

        QMAKE_EXTRA_TARGETS += copy_iss copy_everything_dll copy_everything_exe
        POST_TARGETDEPS += copy_iss copy_everything_dll copy_everything_exe
        #QMAKE_POST_LINK += $$quote($$WINDEPLOYQT --release --force \"$${OUT_PWD}/Release/$${TARGET}.exe\")
    }
    else: LIBS += -L$$PWD/3rdparty/scintilla/bin/debug
}

