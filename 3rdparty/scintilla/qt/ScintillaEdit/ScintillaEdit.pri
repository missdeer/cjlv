#-------------------------------------------------
#
# Project created by QtCreator 2011-05-05T12:41:23
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += c++11

VERSION = 3.7.6

SOURCES += \
    $$PWD/ScintillaEdit.cpp \
    $$PWD/ScintillaDocument.cpp \
    $$PWD/../ScintillaEditBase/PlatQt.cpp \
    $$PWD/../ScintillaEditBase/ScintillaQt.cpp \
    $$PWD/../ScintillaEditBase/ScintillaEditBase.cpp \
    $$PWD/../../src/XPM.cxx \
    $$PWD/../../src/ViewStyle.cxx \
    $$PWD/../../src/UniConversion.cxx \
    $$PWD/../../src/Style.cxx \
    $$PWD/../../src/Selection.cxx \
    $$PWD/../../src/ScintillaBase.cxx \
    $$PWD/../../src/RunStyles.cxx \
    $$PWD/../../src/RESearch.cxx \
    $$PWD/../../src/PositionCache.cxx \
    $$PWD/../../src/PerLine.cxx \
    $$PWD/../../src/MarginView.cxx \
    $$PWD/../../src/LineMarker.cxx \
    $$PWD/../../src/KeyMap.cxx \
    $$PWD/../../src/Indicator.cxx \
    $$PWD/../../src/ExternalLexer.cxx \
    $$PWD/../../src/EditView.cxx \
    $$PWD/../../src/Editor.cxx \
    $$PWD/../../src/EditModel.cxx \
    $$PWD/../../src/Document.cxx \
    $$PWD/../../src/Decoration.cxx \
    $$PWD/../../src/ContractionState.cxx \
    $$PWD/../../src/CharClassify.cxx \
    $$PWD/../../src/CellBuffer.cxx \
    $$PWD/../../src/Catalogue.cxx \
    $$PWD/../../src/CaseFolder.cxx \
    $$PWD/../../src/CaseConvert.cxx \
    $$PWD/../../src/CallTip.cxx \
    $$PWD/../../src/AutoComplete.cxx \
    $$PWD/../../lexlib/WordList.cxx \
    $$PWD/../../lexlib/StyleContext.cxx \
    $$PWD/../../lexlib/PropSetSimple.cxx \
    $$PWD/../../lexlib/LexerSimple.cxx \
    $$PWD/../../lexlib/LexerNoExceptions.cxx \
    $$PWD/../../lexlib/LexerModule.cxx \
    $$PWD/../../lexlib/LexerBase.cxx \
    $$PWD/../../lexlib/CharacterSet.cxx \
    $$PWD/../../lexlib/CharacterCategory.cxx \
    $$PWD/../../lexlib/Accessor.cxx \
    $$PWD/../../lexers/*.cxx

HEADERS  += \
    $$PWD/ScintillaEdit.h \
    $$PWD/ScintillaDocument.h \
    $$PWD/../ScintillaEditBase/ScintillaEditBase.h \
    $$PWD/../ScintillaEditBase/ScintillaQt.h

OTHER_FILES +=

INCLUDEPATH += $$PWD/../ScintillaEdit $$PWD/../ScintillaEditBase $$PWD/../../include $$PWD/../../src $$PWD/../../lexlib $$PWD/../../include

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
	QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}
