#include "stdafx.h"
#include "sourcewindow.h"
#include "ui_sourcewindow.h"

SourceWindow::SourceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceWindow)
{
    ui->setupUi(this);
    connect(this, &SourceWindow::currentChanged, [this](int i){
        ui->tabWidget->setCurrentIndex(i);
    });
    connect(this, &SourceWindow::tabCloseRequested, ui->tabWidget, &SourceViewTabWidget::onTabCloseRequested);
}

SourceWindow::~SourceWindow()
{
    delete ui;
}

SourceViewTabWidget *SourceWindow::getSourceViewTabWidget()
{
    return ui->tabWidget;
}

void SourceWindow::setMainTabWidget(TabWidget *w)
{
    ui->tabWidget->setMainTabWidget(w);
}
