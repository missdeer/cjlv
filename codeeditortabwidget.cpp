#include <QtCore>
#if defined(Q_OS_WIN)
#include <Windows.h>
#include <Shellapi.h>
#endif
#include <QApplication>
#include <QFileInfo>
#include <QMenu>
#include <QClipboard>
#include "codeeditor.h"
#include "codeeditortabwidget.h"

CodeEditorTabWidget::CodeEditorTabWidget(QWidget *parent)
    : QTabWidget (parent)
{
    setTabPosition(QTabWidget::South);
    setTabsClosable(true);
    setDocumentMode(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTabWidget::tabCloseRequested, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &CodeEditorTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::currentChanged, this, &CodeEditorTabWidget::onCurrentChanged);
    connect(this, &QWidget::customContextMenuRequested, this, &CodeEditorTabWidget::onCustomContextMenuRequested);
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

void CodeEditorTabWidget::onCopyFileName()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void CodeEditorTabWidget::onCopyFileFullPath()
{
    QClipboard *clipboard = QApplication::clipboard();
#if defined(Q_OS_WIN)
    clipboard->setText(tabToolTip(currentIndex()).replace(QChar('/'), QChar('\\')));
#else
    clipboard->setText(tabToolTip(currentIndex()));
#endif
}

void CodeEditorTabWidget::onOpenContainerFolder()
{
    QString fileFullPath = tabToolTip(currentIndex());
#if defined(Q_OS_WIN)
    QString arg = QString("/select,\"%1\"").arg(fileFullPath.replace(QChar('/'), QChar('\\')));
    qDebug() << arg;
    ::ShellExecuteW(NULL, L"open", L"explorer.exe", arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#endif
#if defined(Q_OS_MAC)
//    QFileInfo fi(fileFullPath);
//    QDesktopServices::openUrl(QString("open \"%1\"").arg(fi.filePath()));
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileFullPath));
#endif
}

void CodeEditorTabWidget::onCloseAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        onTabCloseRequested(index);
    }
}

void CodeEditorTabWidget::onCloseAllButThis()
{
    for (int index = count() -1; index > currentIndex(); index--)
    {
        onTabCloseRequested(index);
    }
    while (count() > 1)
    {
        onTabCloseRequested(0);
    }
}

void CodeEditorTabWidget::onCloseCurrent()
{
    onTabCloseRequested(currentIndex());
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

void CodeEditorTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    QMenu menu(this);
    QAction* pCloseAction = new QAction(QIcon(":/image/close.png"), "Close", this);
    menu.addAction(pCloseAction);
    connect(pCloseAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseCurrent);
    QAction* pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", this);
    menu.addAction(pCloseAllAction);
    connect(pCloseAllAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseAll);
    QAction* pCloseAllButThisAction = new QAction("Close All But This", this);
    menu.addAction(pCloseAllButThisAction);
    connect(pCloseAllButThisAction, &QAction::triggered, this, &CodeEditorTabWidget::onCloseAllButThis);

    if (tabText(currentIndex()) != "Content")
    {
        menu.addSeparator();

        QAction* pCopyFileNameAction = new QAction("Copy File Name", this);
        connect(pCopyFileNameAction, &QAction::triggered, this, &CodeEditorTabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        QAction* pCopyFileFullPathAction = new QAction("Copy File Full Path", this);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &CodeEditorTabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        QAction* pOpenContainerFolderAction = new QAction("Open Container Folder", this);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &CodeEditorTabWidget::onOpenContainerFolder);
        menu.addAction(pOpenContainerFolderAction);
    }
    menu.exec(mapToGlobal(pos));
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
