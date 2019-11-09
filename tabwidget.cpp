#include "stdafx.h"
#if defined(Q_OS_WIN)
#    include "ShellContextMenu.h"
#endif
#include "extensionmodel.h"
#include "logview.h"
#include "sourcewindow.h"
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent) : QTabWidget(parent)
{
    setTabBarAutoHide(true);
    connect(this, &QTabWidget::tabCloseRequested, this, &TabWidget::onTabCloseRequested);
    connect(this, &QTabWidget::tabBarDoubleClicked, this, &TabWidget::onTabCloseRequested);
    connect(this, &QWidget::customContextMenuRequested, this, &TabWidget::onCustomContextMenuRequested);
    connect(this, &QTabWidget::currentChanged, this, &TabWidget::onCurrentChanged);
}

void TabWidget::openZipBundle(const QString &zipBundle, const QString &crashInfo)
{
    int index = findTab(zipBundle);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    auto *v = new LogView(this);
    v->openZipBundle(zipBundle, crashInfo);
    index = addTab(v, v->windowTitle(), zipBundle);
    setTabIcon(index, QIcon(":/image/open-zip-file.png"));
}

void TabWidget::openZipBundle(const QString &path)
{
    int index = findTab(path);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    auto *v = new LogView(this);
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
    auto *v = new LogView(this);
    v->openRawLogFile(paths);
    index = addTab(v, v->windowTitle(), paths.join("\n"));
    setTabIcon(index, QIcon(":/image/open-file.png"));
}

void TabWidget::openFolder(const QString &path, bool installed)
{
    QString p = path;

    while (p.at(p.length() - 1) == QChar('/') || p.at(p.length() - 1) == QChar('\\'))
        p.remove(p.length() - 1, 1);
    int index = findTab(p);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    auto *v = new LogView(this);
    v->openFolder(p);
    index = addTab(v, v->windowTitle(), p);
    setTabIcon(index, QIcon(installed ? ":/image/open-installed-folder.png" : ":/image/open-folder.png"));
}

void TabWidget::inputKeyword()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->inputKeyword();
    }
}

void TabWidget::clearKeyword()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->onClearKeyword();
    }
}

void TabWidget::enableRegexpMode(bool enabled)
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->enableRegexpMode(enabled);
    }
}

void TabWidget::searchFieldContent()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldContent();
    }
}

void TabWidget::searchFieldID()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldID();
    }
}

void TabWidget::searchFieldDateTime()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldDateTime();
    }
}

void TabWidget::searchFieldThread()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldThread();
    }
}

void TabWidget::searchFieldCategory()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldCategory();
    }
}

void TabWidget::searchFieldSourceFile()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldSourceFile();
    }
}

void TabWidget::searchFieldMethod()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldMethod();
    }
}

void TabWidget::searchFieldLogFile()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldLogFile();
    }
}

void TabWidget::searchFieldLine()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldLine();
    }
}

void TabWidget::searchFieldLevel()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->searchFieldLevel();
    }
}

void TabWidget::onCopyCurrentCell()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copyCurrentCell();
    }
}

void TabWidget::onCopyCurrentRow()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copyCurrentRow();
    }
}

void TabWidget::onCopySelectedCells()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copySelectedCells();
    }
}

void TabWidget::onCopySelectedRows()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copySelectedRows();
    }
}

void TabWidget::onCopyCurrentCellWithXMLFormatted()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copyCurrentCellWithXMLFormatted();
    }
}

void TabWidget::onCopyCurrentRowWithXMLFormatted()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copyCurrentRowWithXMLFormatted();
    }
}

void TabWidget::onCopySelectedCellsWithXMLFormatted()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copySelectedCellsWithXMLFormatted();
    }
}

void TabWidget::onCopySelectedRowsWithXMLFormatted()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->copySelectedRowsWithXMLFormatted();
    }
}

void TabWidget::onAddCurrentRowToBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->addCurrentRowToBookmark();
    }
}

void TabWidget::onRemoveCurrentRowFromBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->removeCurrentRowFromBookmark();
    }
}

void TabWidget::onAddSelectedRowsToBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->addSelectedRowsToBookmark();
    }
}

void TabWidget::onRemoveSelectedRowsFromBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->removeSelectedRowsFromBookmark();
    }
}

void TabWidget::onRemoveAllBookmarks()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->removeAllBookmarks();
    }
}

void TabWidget::onGotoFirstBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->gotoFirstBookmark();
    }
}

void TabWidget::onGotoPreviousBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->gotoPreviousBookmark();
    }
}

void TabWidget::onGotoNextBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->gotoNextBookmark();
    }
}

void TabWidget::onGotoLastBookmark()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->gotoLastBookmark();
    }
}

void TabWidget::onCopyFileName()
{
    auto *clipboard = QApplication::clipboard();
    clipboard->setText(tabText(currentIndex()));
}

void TabWidget::onCopyFileFullPath()
{
    auto *clipboard = QApplication::clipboard();
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

void TabWidget::onScrollToTop()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->scrollToTop();
    }
}

void TabWidget::onScrollToBottom()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->scrollToBottom();
    }
}

void TabWidget::onGotoById()
{
    auto *w = currentWidget();
    if (w)
    {
        int   i = QInputDialog::getInt(this, tr("Goto By Id"), tr("Input Id:"), 1, 1);
        auto *v = qobject_cast<LogView *>(w);
        v->gotoById(i);
    }
}

void TabWidget::onRunExtension(ExtensionPtr e)
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->onRunExtension(e);
        //        emit v->runExtension(e);
    }
}

void TabWidget::onExtensionActionTriggered()
{
    auto *extensionAction = qobject_cast<QAction *>(sender());
    auto  uuid            = extensionAction->data().toString();
    auto  e               = ExtensionModel::instance(this)->extensionByUuid(uuid);
    if (e)
        onRunExtension(e);
}

void TabWidget::onNewLogTableView()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogTableView();
    }
}

void TabWidget::onNewLogLevelPieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogLevelPieChart();
    }
}

void TabWidget::onNewLogThreadPieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogThreadPieChart();
    }
}

void TabWidget::onNewLogSourceFilePieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogSourceFilePieChart();
    }
}

void TabWidget::onNewLogSourceLinePieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogSourceLinePieChart();
    }
}

void TabWidget::onNewLogCategoryPieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogCategoryPieChart();
    }
}

void TabWidget::onNewLogMethodPieChart()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogMethodPieChart();
    }
}

void TabWidget::onNewLogPresenceTableView()
{
    auto *w = currentWidget();
    if (w)
    {
        auto *v = qobject_cast<LogView *>(w);
        v->newLogPresenceTableView();
    }
}

void TabWidget::onCustomContextMenuRequested(const QPoint &pos)
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
        connect(pCloseAction, &QAction::triggered, this, &TabWidget::onCloseCurrent);
        auto *pCloseAllAction = new QAction(QIcon(":/image/closeall.png"), "Close All", &menu);
        menu.addAction(pCloseAllAction);
        connect(pCloseAllAction, &QAction::triggered, this, &TabWidget::onCloseAll);
        auto *pCloseAllButThisAction = new QAction("Close All But This", &menu);
        menu.addAction(pCloseAllButThisAction);
        connect(pCloseAllButThisAction, &QAction::triggered, this, &TabWidget::onCloseAllButThis);

        menu.addSeparator();

        auto *pCopyFileNameAction = new QAction("Copy File Name", &menu);
        connect(pCopyFileNameAction, &QAction::triggered, this, &TabWidget::onCopyFileName);
        menu.addAction(pCopyFileNameAction);
        auto *pCopyFileFullPathAction = new QAction("Copy File Full Path", &menu);
        connect(pCopyFileFullPathAction, &QAction::triggered, this, &TabWidget::onCopyFileFullPath);
        menu.addAction(pCopyFileFullPathAction);

        menu.addSeparator();

        auto *pOpenContainerFolderAction = new QAction("Open Container Folder", &menu);
        connect(pOpenContainerFolderAction, &QAction::triggered, this, &TabWidget::onOpenContainerFolder);
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

void TabWidget::onCurrentChanged(int /*index*/)
{
    auto *w = currentWidget();
    if (w)
    {
        auto *  v   = qobject_cast<LogView *>(w);
        QString msg = QString("Row Count: %1").arg(v->rowCount());
        emit    statusBarMessage(msg);
    }
    else
    {
        emit statusBarMessage("");
    }
}

void TabWidget::onLogViewRowCountChanged()
{
    auto *v = qobject_cast<LogView *>(sender());
    if (v == currentWidget())
    {
        QString msg = QString("Row Count: %1").arg(v->rowCount());
        emit    statusBarMessage(msg);
    }
}

void TabWidget::onCloseAll()
{
    for (int index = count() - 1; index >= 0; index--)
    {
        onTabCloseRequested(index);
    }
}

void TabWidget::onCloseAllButThis()
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

void TabWidget::onCloseCurrent()
{
    onTabCloseRequested(currentIndex());
}

void TabWidget::onTabCloseRequested(int index)
{
    auto *w = widget(index);
    auto *v = qobject_cast<LogView *>(w);
    disconnect(v, &LogView::rowCountChanged, this, &TabWidget::onLogViewRowCountChanged);
    removeTab(index);
    delete w;
}

int TabWidget::findTab(const QString &path)
{
    for (int index = 0; index < count(); index++)
    {
        auto *w = widget(index);
        auto *v = qobject_cast<LogView *>(w);
        if (v->matched(path))
            return index;
    }

    return -1;
}

int TabWidget::findTab(const QStringList &paths)
{
    for (int index = 0; index < count(); index++)
    {
        auto *w = widget(index);
        auto *v = qobject_cast<LogView *>(w);
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
