#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "codeeditortabwidget.h"
#include "sourceviewtabwidget.h"

SourceViewTabWidget::SourceViewTabWidget(QWidget *parent /*= 0*/)
    : QTabWidget(parent)
{
    connect(this, &QTabWidget::tabCloseRequested, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QWidget::customContextMenuRequested, this, &SourceViewTabWidget::onCustomContextMenuRequested);
    connect(this, &QTabWidget::currentChanged, this, &SourceViewTabWidget::onCurrentChanged);
}

void SourceViewTabWidget::onCloseAll()
{

}

void SourceViewTabWidget::onCloseAllButThis()
{

}

void SourceViewTabWidget::onCloseCurrent()
{

}

void SourceViewTabWidget::onTabCloseRequested(int index)
{

}

void SourceViewTabWidget::gotoLine(const QString &logFile, const QString &sourceFile, int line)
{
    int index = findTab(logFile);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }

    // add tab
    CodeEditorTabWidget* v = new CodeEditorTabWidget(this);

    QFileInfo fi(logFile);
    index = addTab(v, fi.fileName(), logFile);
    logFiles.insert(index, logFile);
    v->gotoLine(sourceFile, line);
}

void SourceViewTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{

}

void SourceViewTabWidget::onCurrentChanged(int index)
{

}

int SourceViewTabWidget::findTab(const QString &path)
{
    for (int i = 0; i < logFiles.count(); i++)
    {
        if (path == logFiles.at(i))
            return i;
    }

    return -1;
}

int SourceViewTabWidget::addTab(CodeEditorTabWidget *w, const QString &text, const QString &tooltip)
{
    int index = QTabWidget::addTab(w, text);
    setTabToolTip(index, tooltip);
    setCurrentIndex(index);
    return index;
}
