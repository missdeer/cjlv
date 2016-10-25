#include "stdafx.h"
#include "sourcewindow.h"
#include "ui_sourcewindow.h"

SourceWindow::SourceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceWindow)
{
    ui->setupUi(this);
    connect(this, &SourceWindow::tabCloseRequested, ui->tabWidget, &SourceViewTabWidget::onTabCloseRequested);
    connect(this, &SourceWindow::currentChanged, this, [this](int index){ this->ui->tabWidget->setCurrentIndex(index);});
}

SourceWindow::~SourceWindow()
{
    delete ui;
}

void SourceWindow::gotoLine(const QString &logFile, const QString &sourceFile, int line)
{
    ui->tabWidget->gotoLine(logFile, sourceFile, line);
}