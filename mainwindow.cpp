
#include <QApplication>
#include <QActionGroup>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QActionGroup *group = new QActionGroup(this);
    group->addAction(ui->actionFilter);
    group->addAction(ui->actionSearch);
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
    ui->radioSearch->setChecked(ui->actionSearch->isChecked());
}

void MainWindow::on_actionFilter_triggered()
{
    ui->radioFilter->setChecked(ui->actionFilter->isChecked());
}

void MainWindow::on_actionPreference_triggered()
{

}

void MainWindow::on_actionAbout_triggered()
{

}

void MainWindow::on_radioSearch_clicked()
{
    ui->actionSearch->setChecked(ui->radioSearch->isChecked());
}

void MainWindow::on_radioFilter_clicked()
{
    ui->actionFilter->setChecked(ui->radioFilter->isChecked());
}
