#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "extensionmodel.h"
#include "logview.h"
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    connect(this, &QTabWidget::tabCloseRequested, this, &TabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &TabWidget::onTabCloseRequested);
    connect(this, &QWidget::customContextMenuRequested, this, &TabWidget::onCustomContextMenuRequested);
    connect(this, &QTabWidget::currentChanged, this, &TabWidget::onCurrentChanged);
}

void TabWidget::openZipBundle(const QString &path)
{
    int index = findTab(path);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    LogView* v = new LogView(this);
    v->openZipBundle(path);
    index = addTab(v, v->windowTitle(), path);
    setTabIcon(index, QIcon(":/image/open-zip-file.png"));
}

void TabWidget::openRawLogFile(const QStringList &paths)
{
    int index = findTab(paths);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    LogView* v = new LogView(this);
    v->openRawLogFile(paths);
    index = addTab(v, v->windowTitle(), paths.join("\n"));
    setTabIcon(index, QIcon(":/image/open-file.png"));
}

void TabWidget::openFolder(const QString &path, bool installed)
{
    QString p = path;

    while(p.at(p.length()-1) == QChar('/') || p.at(p.length()-1) == QChar('\\'))
        p.remove(p.length() -1, 1);
    int index = findTab(p);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    LogView* v = new LogView(this);
    v->openFolder(p);
    index = addTab(v, v->windowTitle(), p);
    setTabIcon(index, QIcon(installed ? ":/image/open-installed-folder.png" : ":/image/open-folder.png"));
}

void TabWidget::filter(const QString& keyword)
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->filter(keyword);
    }
}

void TabWidget::onCopyCurrentCell()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->copyCurrentCell();
    }
}

void TabWidget::onCopyCurrentRow()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->copyCurrentRow();
    }
}

void TabWidget::onCopySelectedCells()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->copySelectedCells();
    }
}

void TabWidget::onCopySelectedRows()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->copySelectedRows();
    }
}

void TabWidget::onCopyFileName()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void TabWidget::onCopyFileFullPath()
{
    QClipboard *clipboard = QApplication::clipboard();
#if defined(Q_OS_WIN)
    clipboard->setText(QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
    clipboard->setText(tabToolTip(currentIndex()));
#endif
}

void TabWidget::onOpenContainerFolder()
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

void TabWidget::onScrollToTop()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->scrollToTop();
    }
}

void TabWidget::onScrollToBottom()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->scrollToBottom();
    }
}

void TabWidget::onReload()
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        v->reload();
    }
}

void TabWidget::onGotoById()
{
    QWidget* w = currentWidget();
    if (w)
    {
        int i = QInputDialog::getInt(this, tr("Goto By Id"), tr("Input Id:"), 1, 1);
        LogView* v = qobject_cast<LogView*>(w);
        v->gotoById(i);
    }
}

void TabWidget::onRunExtension(ExtensionPtr e)
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        emit v->runExtension(e);
    }
}

void TabWidget::onExtensionActionTriggered()
{
    QAction* extensionAction = qobject_cast<QAction*>(sender());
    QString uuid = extensionAction->data().toString();
    ExtensionPtr e = ExtensionModel::instance(this)->extensionByUuid(uuid);
    if (e)
        onRunExtension(e);
}

void TabWidget::onCustomContextMenuRequested(const QPoint &pos)
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
        connect(pCloseAction, &QAction::triggered, this, &TabWidget::onCloseCurrent);
        QAction* pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", this);
        menu.addAction(pCloseAllAction);
        connect(pCloseAllAction, &QAction::triggered, this, &TabWidget::onCloseAll);
        QAction* pCloseAllButThisAction = new QAction("Close All But This", this);
        menu.addAction(pCloseAllButThisAction);
        connect(pCloseAllButThisAction, &QAction::triggered, this, &TabWidget::onCloseAllButThis);

        menu.addSeparator();

        QAction* pCopyFileNameAction = new QAction("Copy File Name", this);
        connect(pCopyFileNameAction, &QAction::triggered, this, &TabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        QAction* pCopyFileFullPathAction = new QAction("Copy File Full Path", this);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &TabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        QAction* pOpenContainerFolderAction = new QAction("Open Container Folder", this);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &TabWidget::onOpenContainerFolder);
        menu.addAction(pOpenContainerFolderAction);

#if defined(Q_OS_WIN)
        CShellContextMenu scm;
        scm.ShowContextMenu(&menu, this, mapToGlobal(pos), QDir::toNativeSeparators(tabToolTip(currentIndex())));
#else
        menu.exec(mapToGlobal(pos));
#endif
    }
}

void TabWidget::onCurrentChanged(int /*index*/)
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        QString msg = QString("Row Count: %1").arg(v->rowCount());
        emit statusBarMessage(msg);
    }
    else
    {
        emit statusBarMessage("");
    }
    // notify source view tab widget if exists
}

void TabWidget::onLogViewRowCountChanged()
{
    LogView *v = qobject_cast<LogView*>(sender());
    if (v == currentWidget())
    {
        QString msg = QString("Row Count: %1").arg(v->rowCount());
        emit statusBarMessage(msg);
    }
}

void TabWidget::onCloseAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        onTabCloseRequested(index);
    }
}

void TabWidget::onCloseAllButThis()
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

void TabWidget::onCloseCurrent()
{
    onTabCloseRequested(currentIndex());
}

void TabWidget::onTabCloseRequested(int index)
{
    QWidget* w = widget(index);
    LogView* v = qobject_cast<LogView*>(w);
    disconnect(v, &LogView::rowCountChanged, this, &TabWidget::onLogViewRowCountChanged);
    removeTab(index);
    delete w;
    // notify source view tab widget if exists
}

int TabWidget::findTab(const QString &path)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        LogView* v = qobject_cast<LogView*>(w);
        if (v->matched(path))
            return index;
    }

    return -1;
}

int TabWidget::findTab(const QStringList &paths)
{
    for (int index = 0; index < count(); index++)
    {
        QWidget* w = widget(index);
        LogView* v = qobject_cast<LogView*>(w);
        if (v->matched(paths))
            return index;
    }

    return -1;
}

int TabWidget::addTab(LogView *w, const QString &text, const QString &tooltip)
{
    connect(w, &LogView::rowCountChanged, this, &TabWidget::onLogViewRowCountChanged);
    int index = QTabWidget::addTab(w, text);
    setTabToolTip(index, tooltip);
    setCurrentIndex(index);
    return index;
}
