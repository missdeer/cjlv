#include <QtCore>
#include <QInputDialog>
#include "logview.h"
#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    connect(this, &QTabWidget::tabCloseRequested, this, &TabWidget::onTabCloseRequested);
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
    int index = findTab(path);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    LogView* v = new LogView(this);
    v->openFolder(path);
    index = addTab(v, v->windowTitle(), path);
    setTabIcon(index, QIcon(installed ? ":/image/open-installed-folder.png" : ":/image/open-folder.png"));
}

void TabWidget::filter(const QString& keyword)
{
    QWidget* w = currentWidget();
    if (w)
    {
        LogView* v = qobject_cast<LogView*>(w);
        emit v->filter(keyword);
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
    removeTab(index);
    delete w;
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

int TabWidget::addTab(QWidget *w, const QString &text, const QString &tooltip)
{
    int index = QTabWidget::addTab(w, text);
    setTabToolTip(index, tooltip);
    setCurrentIndex(index);
    return index;
}
