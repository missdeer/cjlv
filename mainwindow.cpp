
#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenZipLogBundle_triggered()
{

}

void MainWindow::on_actionOpenRawLogFile_triggered()
{

}

void MainWindow::on_actionOpenLogFolder_triggered()
{

}

void MainWindow::on_actionOpenCurrentInstalledJabberLogFolder_triggered()
{

}

void MainWindow::on_actionClose_triggered()
{

}

void MainWindow::on_actionCloseAll_triggered()
{

}

void MainWindow::on_actionCloseAllButThis_triggered()
{

}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionSearch_triggered()
{

}

void MainWindow::on_actionFilter_triggered()
{

}

void MainWindow::on_actionPreference_triggered()
{

}

void MainWindow::on_actionAbout_triggered()
{

}
