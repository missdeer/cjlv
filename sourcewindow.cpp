#include "stdafx.h"
#if defined(Q_OS_WIN)
#include "ShellContextMenu.h"
#endif
#include "codeeditortabwidget.h"
#include "sourcewindow.h"
#include "ui_sourcewindow.h"

SourceWindow::SourceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SourceWindow)
{
    ui->setupUi(this);
}

SourceWindow::~SourceWindow()
{
    delete ui;
}

void SourceWindow::gotoLine(const QString &fileName, int line)
{

}
