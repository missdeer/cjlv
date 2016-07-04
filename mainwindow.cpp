#include <QtCore>
#include <QApplication>
#include <QActionGroup>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDir>
#include <QLineEdit>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QHBoxLayout>
#include "settings.h"
#include "preferencedialog.h"
#include "extensiondialog.h"
#include "extensionmodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#if defined(Q_OS_WIN)
    ui->tabbarTopPlaceholder->setVisible(false);
#endif
    QWidget* searchBar = new QWidget(ui->mainToolBar);
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout * searchLayout = new QHBoxLayout(searchBar);
    searchLayout->addWidget(ui->cbKeyword);
    searchLayout->setStretch(0, 1);
    ui->mainToolBar->insertWidget(ui->actionRegexpMode, searchBar);

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
    ui->cbKeyword->lineEdit()->addAction(ui->actionInputKeyword, QLineEdit::ActionPosition::LeadingPosition);
    ui->cbKeyword->lineEdit()->addAction(ui->actionClearKeyword, QLineEdit::ActionPosition::TrailingPosition);

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

    ExtensionModel* em = ExtensionModel::instance(this);
    connect(em, &ExtensionModel::extensionCreated, this, &MainWindow::onExtensionCreated);
    connect(em, &ExtensionModel::extensionModified, this, &MainWindow::onExtensionModified);
    connect(em, &ExtensionModel::extensionRemoved, this, &MainWindow::onExtensionRemoved);

    g_settings->setSearchField("content");

    ui->actionSearch->setChecked(g_settings->searchOrFitler());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openLogs(const QStringList &logs)
{
    Q_FOREACH(const QString& fileName, logs)
    {
        QFileInfo fi(fileName);
        if (fi.isFile())
        {
            if (fi.suffix().toLower() == "zip")
            {
                ui->tabWidget->openZipBundle(fileName);
            }
            else
            {
                ui->tabWidget->openRawLogFile(QStringList() << fileName);
            }
        }

        if (fi.isDir())
        {
            ui->tabWidget->openFolder(fileName, false);
        }
    }
}

void MainWindow::loadExtensions()
{
    ExtensionModel::instance(this)->scanExtensions();
    ui->menuExtension->addSeparator();
    QList<ExtensionPtr>& extensions = ExtensionModel::instance(this)->extensions();
    Q_FOREACH(ExtensionPtr e, extensions)
    {
        QAction* action = new QAction(QString("%1 by %2").arg(e->title()).arg(e->author()), this);
        connect(action, &QAction::triggered, this, &MainWindow::onExtensionActionTriggered);
        action->setData(e->uuid());
        ui->menuExtension->addAction(action);
    }
}

void MainWindow::onStatusBarMessageChanges(const QString &msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::onExtensionRemoved(ExtensionPtr e)
{

}

void MainWindow::onExtensionCreated(ExtensionPtr e)
{

}

void MainWindow::onExtensionModified(ExtensionPtr e)
{

}

void MainWindow::onExtensionActionTriggered()
{
    QAction* extensionAction = qobject_cast<QAction*>(sender());
    QString uuid = extensionAction->data().toString();
    ExtensionModel::instance(this)->runByUuid(uuid);
}

void MainWindow::on_actionOpenZipLogBundle_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                    tr("Open Cisco Jabber Log Zip Bundles"),
                                                    g_settings->lastOpenedDirectory(),
                                                    tr("Cisco Jabber Log Zip Bundle (*.zip);;All files (*.*)"));
    if (fileNames.isEmpty())
        return;

    QFileInfo fi(fileNames.at(0));
    g_settings->setLastOpenedDirectory(fi.absolutePath());

    Q_FOREACH(const QString& fileName, fileNames)
    {
        ui->tabWidget->openZipBundle(fileName);
    }
}

void MainWindow::on_actionOpenRawLogFile_triggered()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                          tr("Open Cisco Jabber Log Files"),
                                                          g_settings->lastOpenedDirectory(),
                                                          tr("Cisco Jabber Log Files (jabber.log*);;All files (*.*)"));

    if (fileNames.isEmpty())
        return;

    QFileInfo fi(fileNames.at(0));
    g_settings->setLastOpenedDirectory(fi.absolutePath());

    ui->tabWidget->openRawLogFile(fileNames);
}

void MainWindow::on_actionOpenLogFolder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                    g_settings->lastOpenedDirectory(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;

    g_settings->setLastOpenedDirectory(dir);
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
    g_settings->setSearchOrFitler(ui->actionSearch->isChecked());
    g_settings->save();
    QMessageBox::warning(this, tr("Warning"), tr("Not implemented yet."), QMessageBox::Ok);
}

void MainWindow::on_actionFilter_triggered()
{
    g_settings->setSearchOrFitler(ui->actionSearch->isChecked());
    g_settings->save();
}

void MainWindow::on_actionPreference_triggered()
{
    PreferenceDialog dlg(this);
    dlg.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       this->windowTitle(),
                       tr("Easy to use tool for Cisco Jabber log reading.\r\nContact me at fyang3@cisco.com if you have any problem about this application.\r\nBuilt at " __DATE__ " " __TIME__));
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
    g_settings->setSearchField("content");
}

void MainWindow::on_actionSearchFieldID_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field ID"));
    g_settings->setSearchField("id");
}

void MainWindow::on_actionSearchFieldDateTime_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Date Time"));
    g_settings->setSearchField("time");
}

void MainWindow::on_actionSearchFieldThread_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Thread"));
    g_settings->setSearchField("thread");
}

void MainWindow::on_actionSearchFieldCategory_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Category"));
    g_settings->setSearchField("category");
}

void MainWindow::on_actionSearchFieldSourceFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Source File"));
    g_settings->setSearchField("source");
}

void MainWindow::on_actionSearchFieldMethod_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Method"));
    g_settings->setSearchField("method");
}

void MainWindow::on_actionSearchFieldLogFile_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Log File"));
    g_settings->setSearchField("log");
}

void MainWindow::on_actionSearchFieldLine_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Line"));
    g_settings->setSearchField("line");
}

void MainWindow::on_actionSearchFieldLevel_triggered()
{
    ui->cbKeyword->lineEdit()->setPlaceholderText(tr("Search Field Level"));
    g_settings->setSearchField("level");
}

void MainWindow::on_actionClearKeyword_triggered()
{
    ui->cbKeyword->setFocus();
    ui->cbKeyword->clearEditText();
}

void MainWindow::on_actionEditExtensions_triggered()
{
    if (!g_extensionDialog)
        g_extensionDialog = new ExtensionDialog(this);
    g_extensionDialog->show();
}

void MainWindow::on_actionRegexpMode_triggered()
{
    g_settings->setRegexMode(ui->actionRegexpMode->isChecked());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QStringList logs;
    foreach (const QUrl &url, e->mimeData()->urls())
    {
        QString fileName = url.toLocalFile();
        logs << fileName;
    }
    openLogs(logs);
}
