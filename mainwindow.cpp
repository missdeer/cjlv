#include <QtCore>
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDir>
#include <QLineEdit>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QActionGroup *searchModeGroup = new QActionGroup(this);
    searchModeGroup->addAction(ui->actionFilter);
    searchModeGroup->addAction(ui->actionSearch);

    QActionGroup *searchFieldGroup = new QActionGroup(this);
    searchFieldGroup->addAction(ui->actionSearchFieldContent);
    searchFieldGroup->addAction(ui->actionSearchFieldID);
    searchFieldGroup->addAction(ui->actionSearchFieldCategory);
    searchFieldGroup->addAction(ui->actionSearchFieldSourceFile);
    searchFieldGroup->addAction(ui->actionSearchFieldMethod);
    searchFieldGroup->addAction(ui->actionSearchFieldLogFile);
    searchFieldGroup->addAction(ui->actionSearchFieldLine);
    searchFieldGroup->addAction(ui->actionSearchFieldThread);
    searchFieldGroup->addAction(ui->actionSearchFieldDateTime);

    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));

    connect(ui->actionClose, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseCurrent);
    connect(ui->actionCloseAll, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseAll);
    connect(ui->actionCloseAllButThis, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseAllButThis);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpenZipLogBundle_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open Cisco Jabber Log Zip Bundles"),
                                                #if defined(Q_OS_WIN)
                                                    QString(),
                                                #else
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                #endif
                                                    tr("Cisco Jabber Log Zip Bundle (*.zip);;All files (*.*)"));
    if (fileNames.isEmpty())
        return;

    Q_FOREACH(const QString& fileName, fileNames)
    {
        ui->tabWidget->openZipBundle(fileName);
    }
}

void MainWindow::on_actionOpenRawLogFile_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Open Cisco Jabber Log Files"),
                                                      #if defined(Q_OS_WIN)
                                                          QString(),
                                                      #else
                                                          QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                      #endif
                                                          tr("Cisco Jabber Log Files (jabber.log*);;All files (*.*)"));

    if (fileNames.isEmpty())
        return;

    ui->tabWidget->openRawLogFile(fileNames);
}

void MainWindow::on_actionOpenLogFolder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                #if defined(Q_OS_WIN)
                                                    QString(),
                                                #else
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                #endif
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    ui->tabWidget->openFolder(dir, false);
}

void MainWindow::on_actionOpenCurrentInstalledJabberLogFolder_triggered()
{
#if defined(Q_OS_WIN)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Log/Jabber";
#endif

    QDir d(dir);
    if (!d.exists())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Folder doesn't exist."), QMessageBox::Ok);
        return;
    }

    ui->tabWidget->openFolder(dir, true);
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
    ui->tabWidget->filter(text);
}

void MainWindow::on_cbKeyword_currentIndexChanged(const QString &text)
{
    ui->tabWidget->filter(text);
}

void MainWindow::on_actionInputKeyword_triggered()
{
    ui->cbKeyword->setFocus();
}

void MainWindow::on_actionSearchFieldContent_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
}

void MainWindow::on_actionSearchFieldID_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field ID"));
}

void MainWindow::on_actionSearchFieldDateTime_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Date Time"));
}

void MainWindow::on_actionSearchFieldThread_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Thread"));
}

void MainWindow::on_actionSearchFieldCategory_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Category"));
}

void MainWindow::on_actionSearchFieldSourceFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Source File"));
}

void MainWindow::on_actionSearchFieldMethod_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Method"));
}

void MainWindow::on_actionSearchFieldLogFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Log File"));
}

void MainWindow::on_actionSearchFieldLine_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Line"));
}

void MainWindow::on_actionCopyCurrentCell_triggered()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::on_actionCopyCurrentRow_triggered()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::on_actionCopySelectedCells_triggered()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::on_actionCopySelectedRows_triggered()
{
    qDebug() << __FUNCTION__;
}

void MainWindow::on_actionLevel_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Level"));
}
