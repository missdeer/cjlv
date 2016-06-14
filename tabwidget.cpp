#include "tabwidget.h"

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{

}

void TabWidget::closeAll()
{
    for (int index = count() -1; index >=0; index--)
    {
        QWidget* w = widget(index);
        removeTab(index);
        w->deleteLater();
    }
}

void TabWidget::closeAllButThis()
{
    for (int index = count() -1; index > currentIndex(); index--)
    {
        QWidget* w = widget(index);
        removeTab(index);
        w->deleteLater();
    }
    while (count() > 1)
    {
        QWidget* w = widget(0);
        removeTab(0);
        w->deleteLater();
    }
}

void TabWidget::closeCurrent()
{
    QWidget* w = currentWidget();
    removeTab(currentIndex());
    w->deleteLater();
}
