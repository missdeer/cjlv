#include "stdafx.h"
#if defined(Q_OS_WIN)
#    include "ShellContextMenu.h"
#endif
#include "codeeditortabwidget.h"
#include "sourceviewtabwidget.h"
#include "tabwidget.h"

extern QTabWidget *g_mainTabWidget;

SourceViewTabWidget::SourceViewTabWidget(QWidget *parent /*= 0*/) : QTabWidget(parent), m_mainTabWidget(nullptr)
{
    setTabsClosable(true);
    setDocumentMode(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setTabBarAutoHide(true);
    connect(this, &QTabWidget::tabCloseRequested, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &QWidget::customContextMenuRequested, this, &SourceViewTabWidget::onCustomContextMenuRequested);
    connect(this, &QTabWidget::currentChanged, this, &SourceViewTabWidget::onCurrentChanged);
}

void SourceViewTabWidget::onCloseAll()
{
    for (int index = count() - 1; index >= 0; index--)
    {
        onTabCloseRequested(index);
    }
}

void SourceViewTabWidget::onCloseAllButThis()
{
    for (int index = count() - 1; index > currentIndex(); index--)
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
    auto *w = widget(index);
    removeTab(index);
    delete w;
    // re-arrange m_logFiles container
    m_logFiles.removeAt(index);
}

void SourceViewTabWidget::gotoLine(const QString &logFile, const QString &sourceFile, int line)
{
    auto *v = getCodeEditorTabWidget(logFile);
    v->gotoLine(sourceFile, line);
}

void SourceViewTabWidget::setContent(const QString &logFile, const QString &text)
{
    auto *v = getCodeEditorTabWidget(logFile);
    v->setContent(text);
}

void SourceViewTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    int c = count();
    if (c)
    {
        bool onTabbar = false;

        for (int i = 0; i < c; i++)
        {
            if (tabBar()->tabRect(i).contains(pos))
            {
                onTabbar = true;
                tabBar()->setCurrentIndex(i);
                break;
            }
        }

        if (!onTabbar)
            return;

        QMenu menu(this);
        auto *pCloseAction = new QAction(QIcon(":/image/close.png"), "Close", &menu);
        menu.addAction(pCloseAction);
        connect(pCloseAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseCurrent);
        auto *pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", &menu);
        menu.addAction(pCloseAllAction);
        connect(pCloseAllAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseAll);
        auto *pCloseAllButThisAction = new QAction("Close All But This", &menu);
        menu.addAction(pCloseAllButThisAction);
        connect(pCloseAllButThisAction, &QAction::triggered, this, &SourceViewTabWidget::onCloseAllButThis);

        menu.addSeparator();

        auto *pCopyFileNameAction = new QAction("Copy File Name", &menu);
        connect(pCopyFileNameAction, &QAction::triggered, this, &SourceViewTabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        auto *pCopyFileFullPathAction = new QAction("Copy File Full Path", &menu);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &SourceViewTabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        auto *pOpenContainerFolderAction = new QAction("Open Container Folder", &menu);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &SourceViewTabWidget::onOpenContainerFolder);
        menu.addAction(pOpenContainerFolderAction);

#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        QPoint            p = mapToGlobal(pos);
        scm.ShowContextMenu(&menu, this, p, QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
        menu.exec(mapToGlobal(pos));
#endif
    }
}

void SourceViewTabWidget::onCurrentChanged(int /*index*/) {}

void SourceViewTabWidget::onCopyFileName()
{
    auto *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void SourceViewTabWidget::onCopyFileFullPath()
{
    auto *clipboard = QApplication::clipboard();
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
    ::ShellExecuteW(nullptr, L"open", L"explorer.exe", arg.toStdWString().c_str(), nullptr, SW_SHOWNORMAL);
#endif
#if defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e") << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(fileFullPath);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e") << QLatin1String("tell application \"Finder\" to activate");
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
        auto *w = widget(index);
        auto *v = qobject_cast<CodeEditorTabWidget *>(w);
        return v;
    }

    // add tab
    auto *v = new CodeEditorTabWidget(this);

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
