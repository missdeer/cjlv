#include "stdafx.h"
#include "ScintillaEdit.h"
#include "scintillaconfig.h"

void ScintillaConfig::initScintilla(ScintillaEdit* sci)
{
    sci->styleResetDefault();
    sci->styleClearAll();
    sci->clearDocumentStyle();
    sci->setHScrollBar(true);
    sci->setVScrollBar(true);
    sci->setXOffset(0);
    sci->setScrollWidth(1);
    sci->setScrollWidthTracking(true);
    sci->setEndAtLastLine(true);
    sci->setWhitespaceFore(true, 0x808080);
    sci->setWhitespaceBack(true, 0xFFFFFF);
    sci->setMouseDownCaptures(true);
    //sci->setReadOnly(true);
#if defined(Q_OS_WIN)
    sci->setEOLMode(SC_EOL_CRLF);
#elif defined(Q_OS_MAC)
    sci->setEOLMode(SC_EOL_CR);
#else
    sci->setEOLMode(SC_EOL_LF);
#endif
    sci->setViewEOL(false);
    sci->setViewWS(SCWS_INVISIBLE);
    sci->setStyleBits(5);
    sci->setCaretFore(0x0000FF);
    sci->setCaretLineVisible(true);
    sci->setCaretLineBack(0xFFFFD0);
    sci->setCaretLineBackAlpha(256);
    sci->setCaretPeriod(500);
    sci->setCaretWidth(2);
    sci->setCaretSticky(0);
    sci->setSelFore(true, 0xFFFFFF);
    sci->setSelBack(true, 0xC56A31);
    sci->setSelAlpha(256);
    sci->setSelEOLFilled(true);
    sci->setAdditionalSelectionTyping(true);
    sci->setVirtualSpaceOptions(SCVS_RECTANGULARSELECTION);
    sci->setHotspotActiveFore(true, 0x0000FF);
    sci->setHotspotActiveBack(true, 0xFFFFFF);
    sci->setHotspotActiveUnderline(true);
    sci->setHotspotSingleLine(false);
    sci->setControlCharSymbol(0);
    sci->setMarginLeft(4);
    //sci->setMarginRight(4);
    sci->setMarginTypeN(0, SC_MARGIN_NUMBER);
    sci->setMarginWidthN(0, 32);
    sci->setMarginMaskN(0, 0);
    sci->setMarginSensitiveN(0, false);
    sci->setMarginTypeN(1, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(1, 16);
    sci->setMarginMaskN(1, 33554431); //~SC_MASK_FOLDERS or 0x1FFFFFF or 33554431
    sci->setMarginSensitiveN(1, true);
    sci->setMarginTypeN(2, SC_MARGIN_SYMBOL);
    sci->setMarginWidthN(2, 16);
    sci->setMarginMaskN(2, SC_MASK_FOLDERS);// 0xFE000000 or -33554432
    sci->setMarginSensitiveN(2, true);

    sci->setFoldMarginColour(true, 0xE9E9E9);
    sci->setFoldMarginHiColour(true, 0xFFFFFF);

    sci->setTabWidth(4);
    sci->setUseTabs(false);
    sci->setIndent(4);
    sci->setTabIndents(false);
    sci->setBackSpaceUnIndents(false);
    sci->setIndentationGuides( SC_IV_REAL);
    sci->setHighlightGuide(1);
    sci->setPrintMagnification(1);
    sci->setPrintColourMode(0);
    sci->setPrintWrapMode(1);

    initFolderStyle( sci );

    sci->setWrapMode(SC_WRAP_NONE);
    sci->setWrapVisualFlags(SC_WRAPVISUALFLAG_END);
    sci->setWrapVisualFlagsLocation(SC_WRAPVISUALFLAGLOC_DEFAULT);
    sci->setWrapStartIndent(0);

    sci->setLayoutCache(SC_CACHE_PAGE);
    sci->linesSplit(0);
    sci->setEdgeMode(0);
    sci->setEdgeColumn(200);
    sci->setEdgeColour(0xC0DCC0);

    sci->usePopUp(true);

#if defined(Q_OS_WIN)
    sci->setTechnology(SC_TECHNOLOGY_DIRECTWRITEDC );
#endif
    sci->setBufferedDraw(false);
    sci->setPhasesDraw(SC_PHASES_TWO);
    sci->setCodePage(SC_CP_UTF8);
    sci->setWordChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_");
    sci->setZoom(1);
    sci->setWhitespaceChars(NULL);
    sci->setMouseDwellTime(2500);

    sci->setSavePoint();
    sci->setFontQuality( SC_EFF_QUALITY_ANTIALIASED);

    QString appDirPath = QApplication::applicationDirPath();
    QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
    configDir.cdUp();
    configDir.cd("Resources");
#endif
    QString themePath = configDir.absolutePath() % "/themes/Default.asTheme";
    applyThemeStyle(sci, themePath % "/global_style.xml");
}

void ScintillaConfig::initFolderStyle(ScintillaEdit *sci)
{
    sci->setFoldFlags(SC_FOLDFLAG_LINEAFTER_CONTRACTED );

    sci->markerDefine(SC_MARKNUM_FOLDEROPEN, SC_MARK_BOXMINUS);
    sci->markerDefine(SC_MARKNUM_FOLDER, SC_MARK_BOXPLUS);
    sci->markerDefine(SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
    sci->markerDefine(SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNER);
    sci->markerDefine(SC_MARKNUM_FOLDEREND, SC_MARK_BOXPLUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDEROPENMID, SC_MARK_BOXMINUSCONNECTED);
    sci->markerDefine(SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNER);

    sci->markerSetFore(SC_MARKNUM_FOLDEROPEN, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPEN, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDER, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDER, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERSUB, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERSUB, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERTAIL, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEREND, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEREND, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDEROPENMID, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDEROPENMID, 0x808080);
    sci->markerSetFore(SC_MARKNUM_FOLDERMIDTAIL, 0xFFFFFF);
    sci->markerSetBack(SC_MARKNUM_FOLDERMIDTAIL, 0x808080);
    sci->setProperty( "fold", "1");
    sci->setProperty( "fold.flags", "16");
    sci->setProperty( "fold.symbols", "1");
    sci->setProperty( "fold.compact", "0");
    sci->setProperty( "fold.at.else", "1");
    sci->setProperty( "fold.preprocessor", "1");
    sci->setProperty( "fold.view", "1");
    sci->setProperty( "fold.comment", "1");
    sci->setProperty( "fold.html", "1");
    sci->setProperty( "fold.comment.python", "1");
    sci->setProperty( "fold.quotes.python", "1");
}

void ScintillaConfig::initEditorStyle(ScintillaEdit *sci, const QString& lang)
{
    sci->setLexerLanguage(lang.toStdString().c_str());

    QString appDirPath = QApplication::applicationDirPath();
    QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
    configDir.cdUp();
    configDir.cd("Resources");
#endif
    QString themePath = configDir.absolutePath() % "/themes/Default.asTheme";
    applyThemeStyle(sci, themePath % "/global_style.xml");
    themePath.append("/");
    themePath.append(lang);
    themePath.append(".xml");
    applyThemeStyle(sci, themePath);

    QString langPath = configDir.absolutePath() % "/language/" % lang % ".xml";
    applyLanguageStyle(sci, langPath);

    initFolderStyle(sci);
}

void ScintillaConfig::applyLanguageStyle(ScintillaEdit *sci, const QString &configPath)
{
    QDomDocument doc;
    QFile file(configPath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    QString commentLine = docElem.attribute("comment_line");
    QString commentStart = docElem.attribute("comment_start");
    QString commentEnd = docElem.attribute("comment_end");

    QDomElement keywordElem = docElem.firstChildElement("keyword");
    int keywordSet = 0;
    while(!keywordElem.isNull())
    {
        QString name = keywordElem.attribute("name");
        QString keyword = keywordElem.text();
        sci->setKeyWords(keywordSet++, keyword.toStdString().c_str());
        keywordElem = keywordElem.nextSiblingElement("keyword");
    }
}

void ScintillaConfig::applyThemeStyle(ScintillaEdit *sci, const QString &themePath)
{
    QDomDocument doc;
    QFile file(themePath);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file))
    {
        file.close();
        return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    bool zeroId = false;
    for(QDomElement styleElem = docElem.firstChildElement("style");
        !styleElem.isNull();
        styleElem = styleElem.nextSiblingElement("style"))
    {
        int id = styleElem.attribute("style_id").toInt();
        if (id == 0)
        {
            if (zeroId)
                continue;
            zeroId = true;
        }
        QString foreColor = styleElem.attribute("fg_color");
        if (!foreColor.isEmpty())
        {
            int color = foreColor.toLong(NULL, 16);
            color = ((color & 0xFF0000) >> 16) | (color & 0xFF00) | ((color & 0xFF) << 16);
            sci->styleSetFore(id, color);
        }
        QString backColor = styleElem.attribute("bg_color");
        if (!backColor.isEmpty())
        {
            int color = backColor.toLong(NULL, 16);
            color = ((color & 0xFF0000) >> 16) | (color & 0xFF00) | ((color & 0xFF) << 16);
            sci->styleSetBack(id, color);
        }
        QString fontName = styleElem.attribute("font_name");
        if (!fontName.isEmpty())
            sci->styleSetFont(id, fontName.toStdString().c_str());
        else
            sci->styleSetFont(id, "Source Code Pro");
//#if defined(Q_OS_MAC)
//            sci->styleSetFont(id, "Monaco");
//#elif defined(Q_OS_WIN)
//            sci->styleSetFont(id, "Consolas");
//#else
//            sci->styleSetFont(id, "Monospace");
//#endif

        uint fontStyle = styleElem.attribute("font_style").toUInt();
        if (fontStyle & 0x01)
            sci->styleSetBold(id, true);
        if (fontStyle & 0x02)
            sci->styleSetItalic(id, true);
        if (fontStyle & 0x04)
            sci->styleSetUnderline(id, true);
        if (fontStyle & 0x08)
            sci->styleSetVisible(id, true);
        if (fontStyle & 0x10)
            sci->styleSetCase(id, true);
        if (fontStyle & 0x20)
            sci->styleSetEOLFilled(id, true);
        if (fontStyle & 0x40)
            sci->styleSetHotSpot(id, true);
        if (fontStyle & 0x80)
            sci->styleSetChangeable(id, true);
        QString fontSize = styleElem.attribute("font_size");
        if (!fontSize.isEmpty())
            sci->styleSetSize(id, std::max(12, fontSize.toInt()));
        else
            sci->styleSetSize(id, 12);
    }
}

QString ScintillaConfig::matchPatternLanguage(const QString &filename)
{
    QString appDirPath = QApplication::applicationDirPath();
    QDir configDir(appDirPath);
#if defined(Q_OS_MAC)
    configDir.cdUp();
    configDir.cd("Resources");
#endif
    QString langMapPath = configDir.absolutePath() % "/langmap.xml";
    QDomDocument doc;
    QFile file(langMapPath);
    if (!file.open(QIODevice::ReadOnly))
        return "";
    if (!doc.setContent(&file))
    {
        file.close();
        return "";
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    QDomElement langElem = docElem.firstChildElement("language");
    while(!langElem.isNull())
    {
        QString pattern = langElem.attribute("pattern");
        QString suffix = langElem.attribute("suffix");
        if (matchSuffix(filename, suffix) || matchPattern(filename, pattern))
        {
            QString name = langElem.attribute("name");
            if (name == "c")
                name = "cpp";
            return name;
        }
        langElem = langElem.nextSiblingElement("language");
    }
    return "";
}

bool ScintillaConfig::matchPattern(const QString &filename, const QString &pattern)
{
#if defined(Q_OS_WIN)
    QRegExp regex(pattern, Qt::CaseInsensitive);
#else
    QRegExp regex(pattern, Qt::CaseSensitive);
#endif
    QFileInfo fi(filename);
    if (regex.exactMatch(fi.fileName()))
        return true;
    return false;
}

bool ScintillaConfig::matchSuffix(const QString &filename, const QString &suffix)
{
    QStringList suffixes = suffix.split(' ');
    QFileInfo fi(filename);

    return (suffixes.end() != std::find_if(suffixes.begin(), suffixes.end(),
                 [&fi](const QString& ext) {
#if defined(Q_OS_WIN)
        return (QString::compare(ext, fi.suffix(), Qt::CaseInsensitive) == 0);
#else
        return (QString::compare(ext, fi.suffix(), Qt::CaseSensitive) == 0);
#endif
    }));
}
