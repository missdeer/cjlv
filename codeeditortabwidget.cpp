#include <QFileInfo>
#include "codeeditor.h"
#include "codeeditortabwidget.h"

CodeEditorTabWidget::CodeEditorTabWidget(QWidget *parent)
    : QTabWidget (parent)
{
    setTabPosition(QTabWidget::South);
    setTabsClosable(true);
    setTabBarAutoHide(true);
    setDocumentMode(true);
    connect(this, &QTabWidget::tabCloseRequested, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::currentChanged, this, &CodeEditorTabWidget::onCurrentChanged);
}

void CodeEditorTabWidget::setContent(const QString &content)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        if (v->isContent())
        {
            setCurrentIndex(index);
            v->setContent(content);
            return;
        }
    }

    // create a new tab and set content
    CodeEditor* v = createCodeEditor("Content", "Content Field Value");
    v->setContent(content);
}

void CodeEditorTabWidget::gotoLine(const QString &fileName, int line)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        if (v->matched(fileName))
        {
            setCurrentIndex(index);
            if (line > 0)
                v->gotoLine(line);
            return;
        }
    }

    // create a new tab and goto line
    QFileInfo fi(fileName);
    CodeEditor* v = createCodeEditor(fi.fileName(), fi.absoluteFilePath());
    v->gotoLine(fileName, line);
}

void CodeEditorTabWidget::onTabCloseRequested(int index)
{
    QWidget* w = widget(index);
    removeTab(index);
    delete w;
}

void CodeEditorTabWidget::onCurrentChanged(int index)
{
    QWidget* w = widget(index);
    if (w)
    {
        CodeEditor* v = qobject_cast<CodeEditor*>(w);
        v->grabFocus();
    }
}

CodeEditor* CodeEditorTabWidget::createCodeEditor(const QString &text, const QString &tooltip)
{
    CodeEditor* v = new CodeEditor(this);
    v->initialize();
    int index = (text == "Content") ? QTabWidget::insertTab(0, v, text) : QTabWidget::addTab(v, text);
    setTabToolTip(index, tooltip);
    setCurrentIndex(index);
    return v;
}
