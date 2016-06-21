#include <QFile>
#include <QFileInfo>
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

    // change editor style by file type, xml style by default
    QString lang = "xml";

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray c = f.readAll();
        setText(c.data());

        emptyUndoBuffer();
        m_sc.initEditorStyle(this, lang);
        colourise(0, -1);

        gotoPos(positionFromLine(line-1));
        setCurrentPos(positionFromLine(line-1));
        grabFocus();
        f.close();
    }
}

void CodeEditor::gotoLine(int line)
{
    gotoPos(positionFromLine(line-1));
    setCurrentPos(positionFromLine(line-1));
    grabFocus();
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
