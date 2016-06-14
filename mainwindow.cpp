
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QMdiSubWindow>
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

    connect(ui->actionClose, &QAction::triggered, ui->tabWidget, &TabWidget::closeCurrent);
    connect(ui->actionCloseAll, &QAction::triggered, ui->tabWidget, &TabWidget::closeAll);
    connect(ui->actionCloseAllButThis, &QAction::triggered, ui->tabWidget, &TabWidget::closeAllButThis);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenZipLogBundle_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Cisco Jabber Log Zip Bundle"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Cisco Jabber Log Zip Bundle (*.zip);;All files (*.*)"));
    if (fileName.isEmpty())
        return;

    ui->tabWidget->openZipBundle(fileName);
}

void MainWindow::on_actionOpenRawLogFile_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Open Cisco Jabber Log Files"),
                                                          QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                          tr("Cisco Jabber Log Files (jabber.log*);;All files (*.*)"));

    if (fileNames.isEmpty())
        return;

    ui->tabWidget->openRawLogFile(fileNames);
}

void MainWindow::on_actionOpenLogFolder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    ui->tabWidget->openFolder(dir);
}

void MainWindow::on_actionOpenCurrentInstalledJabberLogFolder_triggered()
{
#if defined(Q_OS_WIN)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Log/Jabber";
#endif

    if (dir.isEmpty())
        return;
    ui->tabWidget->openFolder(dir);
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
    QMessageBox::about(this, this->windowTitle(), tr("Easy to use tool for Cisco Jabber log reading.\r\nContact me at fyang3@cisco.com if you have any problem about this application."));
}

void MainWindow::on_radioSearch_clicked()
{
    ui->actionSearch->setChecked(ui->radioSearch->isChecked());
}

void MainWindow::on_radioFilter_clicked()
{
    ui->actionFilter->setChecked(ui->radioFilter->isChecked());
}

void MainWindow::on_cbKeyword_editTextChanged(const QString &text)
{

}

void MainWindow::on_cbKeyword_currentIndexChanged(const QString &text)
{

}
