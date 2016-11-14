#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "tabwidget.h"
#include "codeeditortabwidget.h"
#include "sourceviewtabwidget.h"

extern QTabWidget* g_mainTabWidget;

SourceViewTabWidget::SourceViewTabWidget(QWidget *parent /*= 0*/)
    : QTabWidget(parent)
    , m_mainTabWidget(nullptr)
{
    setTabsClosable(true);
    setDocumentMode(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QTabWidget::tabCloseRequested, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QWidget::customContextMenuRequested, this, &SourceViewTabWidget::onCustomContextMenuRequested);
    connect(this, &QTabWidget::currentChanged, this, &SourceViewTabWidget::onCurrentChanged);
}

void SourceViewTabWidget::onCloseAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        onTabCloseRequested(index);
    }
}

void SourceViewTabWidget::onCloseAllButThis()
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

void SourceViewTabWidget::onCloseCurrent()
{
    onTabCloseRequested(currentIndex());
}

void SourceViewTabWidget::onTabCloseRequested(int index)
{
    QWidget* w = widget(index);
    removeTab(index);
    delete w;
}

void SourceViewTabWidget::gotoLine(const QString &logFile, const QString &sourceFile, int line)
{
    CodeEditorTabWidget* v = getCodeEditorTabWidget(logFile);
    v->gotoLine(sourceFile, line);
}

void SourceViewTabWidget::setContent(const QString &logFile, const QString &text)
{
    CodeEditorTabWidget* v = getCodeEditorTabWidget(logFile);
    v->setContent(text);
}

void SourceViewTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    int c = count();
    if (c)
    {
        bool onTabbar = false;

        for (int i=0; i<c; i++)
        {
            if ( tabBar()->tabRect(i).contains( pos ) )
            {
                onTabbar = true;
                tabBar()->setCurrentIndex(i);
                break;
            }
        }

        if (!onTabbar)
            return;

        QMenu menu(this);
        QAction* pCloseAction = new QAction(QIcon(":/image/close.png"), "Close", this);
        menu.addAction(pCloseAction);
        connect(pCloseAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseCurrent);
        QAction* pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", this);
        menu.addAction(pCloseAllAction);
        connect(pCloseAllAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseAll);
        QAction* pCloseAllButThisAction = new QAction("Close All But This", this);
        menu.addAction(pCloseAllButThisAction);
        connect(pCloseAllButThisAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseAllButThis);

        menu.addSeparator();

        QAction* pCopyFileNameAction = new QAction("Copy File Name", this);
        connect(pCopyFileNameAction, &QAction::triggered, this, &SourceViewTabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        QAction* pCopyFileFullPathAction = new QAction("Copy File Full Path", this);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &SourceViewTabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        QAction* pOpenContainerFolderAction = new QAction("Open Container Folder", this);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &SourceViewTabWidget::onOpenContainerFolder);
        menu.addAction(pOpenContainerFolderAction);

#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        scm.ShowContextMenu(&menu, this, mapToGlobal(pos), QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
        menu.exec(mapToGlobal(pos));
#endif
    }
}

void SourceViewTabWidget::onCurrentChanged(int index)
{
}

void SourceViewTabWidget::onCopyFileName()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void SourceViewTabWidget::onCopyFileFullPath()
{
    QClipboard *clipboard = QApplication::clipboard();
#if defined(Q_OS_WIN)
    clipboard->setText(QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
    clipboard->setText(tabToolTip(currentIndex()));
#endif
}

void SourceViewTabWidget::onOpenContainerFolder()
{
    QString fileFullPath = tabToolTip(currentIndex());
#if defined(Q_OS_WIN)
    QString arg = QString("/select,\"%1\"").arg(QDir::toNativeSeparators(fileFullPath));
    ::ShellExecuteW(NULL, L"open", L"explorer.exe", arg.toStdWString().c_str(), NULL, SW_SHOWNORMAL);
#endif
#if defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
               << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(fileFullPath);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
               << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif
}

int SourceViewTabWidget::findTab(const QString &path)
{
    for (int i = 0; i < m_logFiles.count(); i++)
    {
        if (path == m_logFiles.at(i))
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

CodeEditorTabWidget *SourceViewTabWidget::getCodeEditorTabWidget(const QString &file)
{
    int index = findTab(file);
    if (index >= 0)
    {
        setCurrentIndex(index);
        QWidget* w = widget(index);
        CodeEditorTabWidget* v = qobject_cast<CodeEditorTabWidget*>(w);
        return v;
    }

    // add tab
    CodeEditorTabWidget* v = new CodeEditorTabWidget(this);

    QFileInfo fi(file);
    index = m_mainTabWidget->currentIndex();
    if (index >= count())
    {
        index = addTab(v, fi.fileName(), file);
    }
    else
    {
        insertTab(index, v, fi.fileName());
        setTabToolTip(index, file);
    }
    m_logFiles.insert(index, file);
    return v;
}
