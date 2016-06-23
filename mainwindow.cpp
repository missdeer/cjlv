#include <QtCore>
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDir>
#include <QLineEdit>
#include "settings.h"
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

    connect(ui->tabWidget, &TabWidget::statusBarMessage, this, &MainWindow::onStatusBarMessageChanges);
    connect(ui->actionClose, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseCurrent);
    connect(ui->actionCloseAll, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseAll);
    connect(ui->actionCloseAllButThis, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseAllButThis);
    connect(ui->actionCopyCurrentCell, &QAction::triggered, ui->tabWidget, &TabWidget::onCopyCurrentCell);
    connect(ui->actionCopyCurrentRow, &QAction::triggered, ui->tabWidget, &TabWidget::onCopyCurrentRow);
    connect(ui->actionCopySelectedCells, &QAction::triggered, ui->tabWidget, &TabWidget::onCopySelectedCells);
    connect(ui->actionCopySelectedRows, &QAction::triggered, ui->tabWidget, &TabWidget::onCopySelectedRows);
    connect(ui->actionScrollToTop, &QAction::triggered, ui->tabWidget, &TabWidget::onScrollToTop);
    connect(ui->actionScrollToBottom, &QAction::triggered, ui->tabWidget, &TabWidget::onScrollToBottom);
    connect(ui->actionReload, &QAction::triggered, ui->tabWidget, &TabWidget::onReload);
    connect(ui->actionGotoById, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoById);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    g_settings.setSearchField("content");
    g_settings.setSearchOrFitler(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStatusBarMessageChanges(const QString &msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_actionOpenZipLogBundle_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open Cisco Jabber Log Zip Bundles"),
                                                    g_settings.lastOpenedDirectory(),
                                                    tr("Cisco Jabber Log Zip Bundle (*.zip);;All files (*.*)"));
    if (fileNames.isEmpty())
        return;

    QFileInfo fi(fileNames.at(0));
    g_settings.setLastOpenedDirectory(fi.absolutePath());

    Q_FOREACH(const QString& fileName, fileNames)
    {
        ui->tabWidget->openZipBundle(fileName);
    }
}

void MainWindow::on_actionOpenRawLogFile_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Open Cisco Jabber Log Files"),
                                                          g_settings.lastOpenedDirectory(),
                                                          tr("Cisco Jabber Log Files (jabber.log*);;All files (*.*)"));

    if (fileNames.isEmpty())
        return;

    QFileInfo fi(fileNames.at(0));
    g_settings.setLastOpenedDirectory(fi.absolutePath());

    ui->tabWidget->openRawLogFile(fileNames);
}

void MainWindow::on_actionOpenLogFolder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                    g_settings.lastOpenedDirectory(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    g_settings.setLastOpenedDirectory(dir);
    ui->tabWidget->openFolder(dir, false);
}

void MainWindow::on_actionOpenCurrentInstalledJabberLogFolder_triggered()
{
#if defined(Q_OS_WIN)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Jabber";
#endif

    QDir d(dir);
    if (!d.exists())
    {
        QMessageBox::critical(this,
                             tr("Warning"),
                             QString(tr("Folder '%1' doesn't exist.")).arg(dir),
                             QMessageBox::Ok);
        return;
    }

    ui->tabWidget->openFolder(dir, true);
}

void MainWindow::on_actionSearch_triggered()
{
    ui->radioSearch->setChecked(ui->actionSearch->isChecked());
    g_settings.setSearchOrFitler(ui->radioSearch->isChecked());
}

void MainWindow::on_actionFilter_triggered()
{
    ui->radioFilter->setChecked(ui->actionFilter->isChecked());
    g_settings.setSearchOrFitler(ui->radioSearch->isChecked());
}

void MainWindow::on_actionPreference_triggered()
{
    QMessageBox::warning(this, tr("Warning"), tr("Not implemented yet."), QMessageBox::Ok);
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, this->windowTitle(), tr("Easy to use tool for Cisco Jabber log reading.\r\nContact me at fyang3@cisco.com if you have any problem about this application."));
}

void MainWindow::on_radioSearch_clicked()
{
    ui->actionSearch->setChecked(ui->radioSearch->isChecked());
    g_settings.setSearchOrFitler(ui->radioSearch->isChecked());
}

void MainWindow::on_radioFilter_clicked()
{
    ui->actionFilter->setChecked(ui->radioFilter->isChecked());
    g_settings.setSearchOrFitler(ui->radioSearch->isChecked());
}

void MainWindow::on_cbKeyword_editTextChanged(const QString &text)
{
    ui->tabWidget->filter(text);
}

void MainWindow::on_cbKeyword_currentIndexChanged(const QString &text)
{
    ui->tabWidget->filter(text.trimmed());
}

void MainWindow::on_actionInputKeyword_triggered()
{
    ui->cbKeyword->setFocus();
    ui->cbKeyword->lineEdit()->selectAll();
}

void MainWindow::on_actionSearchFieldContent_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Content"));
    g_settings.setSearchField("content");
}

void MainWindow::on_actionSearchFieldID_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field ID"));
    g_settings.setSearchField("id");
}

void MainWindow::on_actionSearchFieldDateTime_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Date Time"));
    g_settings.setSearchField("time");
}

void MainWindow::on_actionSearchFieldThread_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Thread"));
    g_settings.setSearchField("thread");
}

void MainWindow::on_actionSearchFieldCategory_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Category"));
    g_settings.setSearchField("category");
}

void MainWindow::on_actionSearchFieldSourceFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Source File"));
    g_settings.setSearchField("source");
}

void MainWindow::on_actionSearchFieldMethod_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Method"));
    g_settings.setSearchField("method");
}

void MainWindow::on_actionSearchFieldLogFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Log File"));
    g_settings.setSearchField("log");
}

void MainWindow::on_actionSearchFieldLine_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Line"));
    g_settings.setSearchField("line");
}

void MainWindow::on_actionSearchFieldLevel_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Level"));
    g_settings.setSearchField("level");
}

void MainWindow::on_actionClearKeyword_triggered()
{
    ui->cbKeyword->setFocus();
    ui->cbKeyword->clearEditText();
}

void MainWindow::on_actionEditExtensions_triggered()
{
    QMessageBox::warning(this, tr("Warning"), tr("Not implemented yet."), QMessageBox::Ok);
}
