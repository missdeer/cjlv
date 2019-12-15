#-------------------------------------------------
#
# Project created by QtCreator 2016-06-13T14:46:16
#
#-------------------------------------------------

QT       += core gui concurrent widgets xml charts network qml quick quickwidgets gui-private

TARGET = "Cisco Jabber Log Viewer"
TEMPLATE = app
CONFIG += c++17 precompile_header
PRECOMPILED_HEADER = stdafx.h

lessThan(QT_MAJOR_VERSION, 5): error("Qt 5 or higher is required")
greaterThan(QT_MINOR_VERSION, 10): win32-*g++*: CONFIG-= qtquickcompiler

include($$PWD/3rdparty/qtkeychain/qt5keychain.pri)
include($$PWD/3rdparty/qtsingleapplication/qtsingleapplication.pri)
include($$PWD/3rdparty/lua/src/lua.pri)
include($$PWD/3rdparty/scintilla-latest.pri)
include($$PWD/3rdparty/sqlite3/sqlite3.pri)
include($$PWD/3rdparty/rapidxml-1.13/rapidxml.pri)
include($$PWD/Boost.pri)

QML_FILES = qml/main.qml \
    qml/control/qmldir \
    qml/control/CheckBoxIndicator.qml \
    qml/control/RangeSliderBackground.qml \
    qml/control/RangeSliderHandle.qml \

SOURCES += main.cpp\
        mainwindow.cpp \
    logview.cpp \
    logmodel.cpp \
    readlinefromfile.cpp \
    scopedguard.cpp \
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
    popupmenutoolbutton.cpp \
    utils.cpp \
    logtableview.cpp

HEADERS  += mainwindow.h \
    finishedqueryevent.h \
    logview.h \
    logmodel.h \
    readlinefromfile.h \
    rowcountevent.h \
    scopedguard.h \
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
    popupmenutoolbutton.h \
    utils.h \
    logtableview.h

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
    include($$PWD/3rdparty/kdmactouchbar.pri)
    HEADERS += \
        macheader.h 
    OBJECTIVE_SOURCES += \
        mdfindwrapper.mm \
        darkmode.mm

    #QMAKE_MAC_SDK = macosx10.11
    ICON = cjlv.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon
    INCLUDEPATH += /usr/local/include
    LIBS+= -lobjc -framework CoreServices

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

        APPCERT = 
        INSTALLERCERT = 
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
    win32-*msvc* {
        QMAKE_CXXFLAGS_RELEASE += /Zi
        #QMAKE_LFLAGS_RELEASE += /DEBUG
    }

    DISTFILES += \
        cjlv.rc
    TARGET = CJLV
    QT += winextras
    INCLUDEPATH += $$PWD/3rdparty/Everything-SDK/include $$PWD/3rdparty/Everything-SDK/ipc

    SOURCES += everythingwrapper.cpp ShellContextMenu.cpp
    HEADERS += everythingwrapper.h ShellContextMenu.h

    LIBS+=-L$$PWD/3rdparty/Everything-SDK/lib -lUser32 -lShell32 -lpsapi -lOle32 -luuid

    contains(QMAKE_HOST.arch, x86_64): LIBS += -lEverything64
    else: LIBS += -lEverything32

    CONFIG(release, debug|release): {
        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe

        contains(QMAKE_HOST.arch, x86_64): {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything64.dll) $$shell_path($$OUT_PWD/Release/Everything64.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything64.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
        }
        else: {
            copy_everything_dll.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/dll/Everything32.dll) $$shell_path($$OUT_PWD/Release/Everything32.dll)'
            copy_everything_exe.commands = '$(COPY_FILE) $$shell_path($$PWD/3rdparty/Everything-SDK/exe/Everything32.exe) $$shell_path($$OUT_PWD/Release/Everything.exe)'
        }

        QMAKE_EXTRA_TARGETS += copy_everything_dll copy_everything_exe
        POST_TARGETDEPS += copy_everything_dll copy_everything_exe
    }
}
