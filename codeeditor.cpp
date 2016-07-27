#include "stdafx.h"
#include "codeeditor.h"

CodeEditor::CodeEditor(QWidget *parent)
    : ScintillaEdit (parent)
    , m_isContent(false)
{
}

void CodeEditor::initialize()
{
    m_sc.initScintilla(this);
    m_sc.initEditorStyle(this);

    connect(this, &ScintillaEdit::linesAdded, this, &CodeEditor::linesAdded);
    connect(this, &ScintillaEdit::marginClicked, this, &CodeEditor::marginClicked);
}

void CodeEditor::setContent(const QString &content)
{
    m_isContent = true;
    setText(content.toLatin1().data());

    emptyUndoBuffer();
    m_sc.initEditorStyle(this);
    colourise(0, -1);
}

void CodeEditor::gotoLine(const QString &fileName, int line)
{
    m_fileName = fileName;

    const QString& lang = m_sc.matchPatternLanguage(fileName);

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly))
    {
        clear();
        while (!f.atEnd())
        {
            QByteArray c = f.readAll();
            appendText(c.length(), c.data());
        }
        f.close();

        emptyUndoBuffer();
        m_sc.initEditorStyle(this, lang);
        colourise(0, -1);

        grabFocus();
        gotoPos(positionFromLine(line-1));
        setCurrentPos(positionFromLine(line-1));
    }
}

void CodeEditor::gotoLine(int line)
{
    grabFocus();
    gotoPos(positionFromLine(line-1));
    setCurrentPos(positionFromLine(line-1));
}

bool CodeEditor::matched(const QString &fileName)
{
    return QFileInfo(fileName) == QFileInfo(m_fileName);
}

void CodeEditor::linesAdded(int /*linesAdded*/)
{
    ScintillaEdit* sci = qobject_cast<ScintillaEdit*>(sender());
    sptr_t line_count = sci->lineCount();
    sptr_t left = sci->marginLeft() + 2;
    sptr_t right = sci->marginRight() + 2;
    sptr_t width = left + right + sci->textWidth(STYLE_LINENUMBER, QString("%1").arg(line_count).toStdString().c_str());
    if (width > sci->marginWidthN(0))
        sci->setMarginWidthN(0, width);
}

void CodeEditor::marginClicked(int position, int /*modifiers*/, int margin)
{
    ScintillaEdit* sci = qobject_cast<ScintillaEdit*>(sender());
    if (sci->marginTypeN(margin) == SC_MARGIN_SYMBOL)
    {
        sptr_t line = sci->lineFromPosition(position);
        sptr_t foldLevel = sci->foldLevel(line);
        if (foldLevel & SC_FOLDLEVELHEADERFLAG)
        {
            sci->toggleFold(line);
        }
    }
}

bool CodeEditor::isContent() const
{
    return m_isContent;
}

void CodeEditor::setLanguage(const QString &lang)
{
    m_sc.initEditorStyle(this, lang);
}
