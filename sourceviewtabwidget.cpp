#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
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

void SourceViewTabWidget::onCustomContextMenuRequested(const QPoint &pos)
{

}

void SourceViewTabWidget::onCurrentChanged(int index)
{

}
