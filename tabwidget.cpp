

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
    LogView* v = new LogView();
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
    LogView* v = new LogView();
    v->openRawLogFile(paths);
    index = addTab(v, v->windowTitle(), paths.join("\n"));
    setTabIcon(index, QIcon(":/image/open-file.png"));
}

void TabWidget::openFolder(const QString &path)
{
    int index = findTab(path);
    if (index >= 0)
    {
        setCurrentIndex(index);
        return;
    }
    LogView* v = new LogView();
    v->openFolder(path);
    index = addTab(v, v->windowTitle(), path);
    setTabIcon(index, QIcon(":/image/open-folder.png"));
}

void TabWidget::closeAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        onTabCloseRequested(index);
    }
}

void TabWidget::closeAllButThis()
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

void TabWidget::closeCurrent()
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
