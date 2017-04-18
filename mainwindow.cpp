#include "stdafx.h"
#include "settings.h"
#include "preferencedialog.h"
#include "extensiondialog.h"
#include "extensionmodel.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#if defined(Q_OS_WIN)
QWinTaskbarButton *g_winTaskbarButton = nullptr;
QWinTaskbarProgress *g_winTaskbarProgress = nullptr;
#endif
QTabWidget* g_mainTabWidget = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    thumbbar(nullptr),
    m_nam(nullptr),
    m_prt(nullptr)
{
    ui->setupUi(this);
    g_mainTabWidget = ui->tabWidget;
#if defined(Q_OS_WIN)
    ui->tabbarTopPlaceholder->setVisible(false);
#endif

    QActionGroup *searchModeGroup = new QActionGroup(this);
    searchModeGroup->addAction(ui->actionFilter);
    searchModeGroup->addAction(ui->actionSearch);

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
    em->scanExtensions();;
    connect(em, &ExtensionModel::extensionCreated, this, &MainWindow::onExtensionCreated);
    connect(em, &ExtensionModel::extensionModified, this, &MainWindow::onExtensionModified);
    connect(em, &ExtensionModel::extensionRemoved, this, &MainWindow::onExtensionRemoved);
    connect(em, &ExtensionModel::extensionScanned, this, &MainWindow::onExtensionScanned);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);
    ui->actionSearch->setChecked(g_settings->searchOrFitler());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openLogs(const QStringList &logs)
{
    QList<QString> names {
        "jabber.log",
        "jabber.log.1",
        "jabber.log.2",
        "jabber.log.3",
        "jabber.log.4",
        "jabber.log.5"
    };
    for(const QString& fileName : logs)
    {
        QFileInfo fi(fileName);
        if (fi.isFile())
        {
            if (fi.suffix().toLower() == "zip")
            {
                ui->tabWidget->openZipBundle(fileName);
            }

            if (names.contains( fi.fileName()))
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

TabWidget *MainWindow::getMainTabWidget()
{
    return ui->tabWidget;
}

void MainWindow::onExtensionScanned()
{
    ui->menuExtension->addSeparator();
    QList<ExtensionPtr>& extensions = ExtensionModel::instance(this)->extensions();
    std::for_each(extensions.begin(), extensions.end(),
                  std::bind(&MainWindow::onExtensionCreated, this, std::placeholders::_1));
}

void MainWindow::onStatusBarMessageChanges(const QString &msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::onClipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text();

    if (originalText.startsWith("http://prt.jabberqa.cisco.com/#/conversations/"))
    {
        if (QMessageBox::question(this,
                                  tr("Open PRT from URL"),
                                  QString(tr("URL %1 is detected from clipboard, open it now?")).arg(originalText),
                                  QMessageBox::Yes | QMessageBox::No,
                                  QMessageBox::Yes) == QMessageBox::Yes)
        {
            openPRTFromURL(originalText);
        }
    }
}

void MainWindow::onPRTRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    m_prt->close();
    ui->tabWidget->openZipBundle(m_prt->fileName());
}

void MainWindow::onPRTRequestReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prt->write(reply->readAll());
    }
}

void MainWindow::onPRTTrackingSystemRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(m_prtInfo);
    if (!doc.isObject())
    {
        qDebug() << "content received is not a json object" << QString(m_prtInfo);
        return;
    }

    QJsonObject docObj = doc.object();
    QJsonValue emailsObj = docObj["emails"];
    if (!emailsObj.isArray())
    {
        qDebug() << "emails node is expected to be an array";
        return;
    }

    QJsonArray emails = emailsObj.toArray();
    for (auto email : emails)
    {
        if (!email.isObject())
        {
            qDebug() << "email element is expected to be an object";
            continue;
        }
        QJsonObject ele = email.toObject();
        QJsonValue attachmentsObj = ele["attachments"];
        if (!attachmentsObj.isArray())
        {
            qDebug() << "attachments node is expected to be an array";
            continue;
        }
        QJsonArray attachments = attachmentsObj.toArray();
        for (auto attachment : attachments)
        {
            if (!attachment.isObject())
            {
                qDebug() << "attachment is expected to be an object";
                continue;
            }
            QJsonObject attachmentObj = attachment.toObject();
            QJsonValue path = attachmentObj["file_directory"];
            QJsonValue name = attachmentObj["file_name"];
            if (!path.isString() || !name.isString())
            {
                qDebug() << "path or name is missing.";
                continue;
            }
            QString u = QString("http://prt.jabberqa.cisco.com/download?path=%1&name=%2").arg(path.toString()).arg(name.toString());
            downloadPRT(u);
            return;
        }
    }
}

void MainWindow::onPRTTrackingSystemRequestReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prtInfo.append(reply->readAll());
    }
}

void MainWindow::onPRTRequestError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "network error:" << e << reply->errorString();
}

void MainWindow::onPRTRequestSslErrors(const QList<QSslError> & es)
{
    for(const QSslError &e : es)
    {
        qDebug() << "ssl error:" << e.errorString();
    }
}

void MainWindow::onIPCMessageReceived(const QString &message, QObject* /*socket*/)
{
    QStringList logs = message.split('\n');
    openLogs(logs);
    raise();
    activateWindow();
}

void MainWindow::onExtensionRemoved(ExtensionPtr e)
{
    QList<QAction*> actions = ui->menuExtension->actions();
    auto it = std::find_if(actions.begin(), actions.end(),
                 [e](QAction* action) { return (action->data().toString() == e->uuid());});

    if (actions.end() != it)
    {
        ui->menuExtension->removeAction(*it);
        (*it)->deleteLater();
    }
}

void MainWindow::onExtensionCreated(ExtensionPtr e)
{
    QAction* action = new QAction(QString("%1 by %2").arg(e->title()).arg(e->author()), this);
    connect(action, &QAction::triggered, ui->tabWidget, &TabWidget::onExtensionActionTriggered);
    action->setData(e->uuid());
    ui->menuExtension->addAction(action);
}

void MainWindow::onExtensionModified(ExtensionPtr e)
{
    QList<QAction*> actions = ui->menuExtension->actions();
    auto it = std::find_if(actions.begin(), actions.end(),
                 [e](QAction* action) { return (action->data().toString() == e->uuid());});

    if (actions.end() != it)
    {
        (*it)->setText(QString("%1 by %2").arg(e->title()).arg(e->author()));
    }
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

    std::for_each(fileNames.begin(), fileNames.end(),
                  std::bind(&TabWidget::openZipBundle, ui->tabWidget, std::placeholders::_1));
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
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
    QString dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/Library/Logs/Jabber";
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

void MainWindow::openPRTFromURL(const QString &u)
{
    QString rawURL(u);
    QUrl url(rawURL.replace("#", "api/v1"));
    QNetworkRequest req(url);
    req.setRawHeader("token", g_settings->prtTrackingSystemToken().toUtf8());
    if (!m_nam)
        m_nam = new QNetworkAccessManager(this);
    m_prtInfo.clear();
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTTrackingSystemRequestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(onPRTRequestSslErrors(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTTrackingSystemRequestFinished()));
}

void MainWindow::on_actionOpenFromPRTTrackingSystemURL_triggered()
{
    bool ok;
    QString u = QInputDialog::getText(this,
                                      tr("Input URL"),
                                      tr("Input a valid PRT Tracking System URL"),
                                      QLineEdit::Normal,
                                      QString(),
                                      &ok);

    if (ok && u.startsWith("http://prt.jabberqa.cisco.com/#/conversations/"))
        openPRTFromURL(u);
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
                       tr("Easy to use tool for reading logs generated by Cisco Jabber.\r\nContact me at fyang3@cisco.com if you have any problem about this tool.\r\nBuilt at " __DATE__ " " __TIME__));
}

void MainWindow::on_actionInputKeyword_triggered()
{
    ui->tabWidget->inputKeyword();
}

void MainWindow::on_actionSearchFieldContent_triggered()
{
    ui->tabWidget->searchFieldContent();
}

void MainWindow::on_actionSearchFieldID_triggered()
{
    ui->tabWidget->searchFieldID();
}

void MainWindow::on_actionSearchFieldDateTime_triggered()
{
    ui->tabWidget->searchFieldDateTime();
}

void MainWindow::on_actionSearchFieldThread_triggered()
{
    ui->tabWidget->searchFieldThread();
}

void MainWindow::on_actionSearchFieldCategory_triggered()
{
    ui->tabWidget->searchFieldCategory();
}

void MainWindow::on_actionSearchFieldSourceFile_triggered()
{
    ui->tabWidget->searchFieldSourceFile();
}

void MainWindow::on_actionSearchFieldMethod_triggered()
{
    ui->tabWidget->searchFieldMethod();
}

void MainWindow::on_actionSearchFieldLogFile_triggered()
{
    ui->tabWidget->searchFieldLogFile();
}

void MainWindow::on_actionSearchFieldLine_triggered()
{
    ui->tabWidget->searchFieldLine();
}

void MainWindow::on_actionSearchFieldLevel_triggered()
{
    ui->tabWidget->searchFieldLevel();
}

void MainWindow::on_actionClearKeyword_triggered()
{
    ui->tabWidget->clearKeyword();
}

void MainWindow::on_actionEditExtensions_triggered()
{
    if (!g_extensionDialog)
    {
        g_extensionDialog = new ExtensionDialog(this);
        connect(g_extensionDialog, &ExtensionDialog::runExtension, ui->tabWidget, &TabWidget::onRunExtension);
    }

#if defined(Q_OS_MAC)
    if (g_extensionDialog->isVisible())
    {
        g_extensionDialog->activateWindow();
        g_extensionDialog->raise();
    }
    else
#endif
        g_extensionDialog->show();
}

void MainWindow::on_actionRegexpMode_triggered()
{
    ui->tabWidget->enableRegexpMode(ui->actionRegexpMode->isChecked());
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

    if (!e->mimeData()->hasUrls())
        return;
    QList<QUrl>&& urls = e->mimeData()->urls();

    std::for_each(urls.begin(), urls.end(),
                  [&logs](const QUrl& url) {logs << url.toLocalFile();});
    openLogs(logs);
}

void MainWindow::showEvent(QShowEvent *e)
{
#if defined(Q_OS_WIN)
    if (!thumbbar)
    {
        thumbbar = new QWinThumbnailToolBar(this);
        thumbbar->setWindow(windowHandle());

        QWinThumbnailToolButton *openZipButton = new QWinThumbnailToolButton(thumbbar);
        openZipButton->setToolTip("Open Zip Log Bundle");
        openZipButton->setIcon(QIcon(":/image/open-zip-file.png"));
        openZipButton->setDismissOnClick(true);
        connect(openZipButton, SIGNAL(clicked()), ui->actionOpenZipLogBundle, SIGNAL(triggered()));

        QWinThumbnailToolButton *openRawLogFileButton = new QWinThumbnailToolButton(thumbbar);
        openRawLogFileButton->setToolTip("Open Raw Log File");
        openRawLogFileButton->setIcon(QIcon(":/image/open-file.png"));
        openRawLogFileButton->setDismissOnClick(true);
        connect(openRawLogFileButton, SIGNAL(clicked()), ui->actionOpenRawLogFile, SIGNAL(triggered()));

        QWinThumbnailToolButton *openLogFolderButton = new QWinThumbnailToolButton(thumbbar);
        openLogFolderButton->setToolTip("Open Log Folder");
        openLogFolderButton->setIcon(QIcon(":/image/open-folder.png"));
        openLogFolderButton->setDismissOnClick(true);
        connect(openLogFolderButton, SIGNAL(clicked()), ui->actionOpenLogFolder, SIGNAL(triggered()));

        QWinThumbnailToolButton *openCurrentInstalledJabberLogFolderButton = new QWinThumbnailToolButton(thumbbar);
        openCurrentInstalledJabberLogFolderButton->setToolTip("Open Current Installed Jabber Log Folder");
        openCurrentInstalledJabberLogFolderButton->setIcon(QIcon(":/image/open-installed-folder.png"));
        openCurrentInstalledJabberLogFolderButton->setDismissOnClick(true);
        connect(openCurrentInstalledJabberLogFolderButton, SIGNAL(clicked()), ui->actionOpenCurrentInstalledJabberLogFolder, SIGNAL(triggered()));

        thumbbar->addButton(openZipButton);
        thumbbar->addButton(openRawLogFileButton);
        thumbbar->addButton(openLogFolderButton);
        thumbbar->addButton(openCurrentInstalledJabberLogFolderButton);
    }

    if (!g_winTaskbarButton)
    {
        g_winTaskbarButton = new QWinTaskbarButton(this);
        g_winTaskbarButton->setWindow(windowHandle());
    }

    if (!g_winTaskbarProgress)
    {
        g_winTaskbarProgress = g_winTaskbarButton->progress();
    }
#endif

    e->accept();
}

void MainWindow::downloadPRT(const QString &u)
{
    QUrl url(u);
    QNetworkRequest req(url);
    req.setRawHeader("token", g_settings->prtTrackingSystemToken().toUtf8());
    if (!m_nam)
        m_nam = new QNetworkAccessManager(this);

    QString tempDir = g_settings->temporaryDirectory();
    if (tempDir.isEmpty())
    {
        tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
    }
    QDir dir(tempDir);
    if (!dir.exists())
        dir.mkpath(tempDir);

    int index = u.lastIndexOf(QChar(':'));
    index = u.indexOf(QChar('_'), index);
    QString f(u.mid(index+1));

    m_prt = new QFile(tempDir % "/" % f);
    m_prt->open(QIODevice::WriteOnly | QIODevice::Truncate);

    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTRequestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(onPRTRequestSslErrors(QList<QSslError>)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTRequestFinished()));
}

void MainWindow::on_actionHelpContent_triggered()
{
    QDesktopServices::openUrl(QUrl::fromUserInput("https://wiki.cisco.com/display/CUCBU/Cisco+Jabber+Log+Viewer"));
}

void MainWindow::on_actionLogLevelFatal_triggered()
{
    g_settings->setFatalEnabled(ui->actionLogLevelFatal->isChecked());
}

void MainWindow::on_actionLogLevelError_triggered()
{
    g_settings->setErrorEnabled(ui->actionLogLevelError->isChecked());
}

void MainWindow::on_actionLogLevelWarn_triggered()
{
    g_settings->setWarnEnabled(ui->actionLogLevelWarn->isChecked());
}

void MainWindow::on_actionLogLevelInfo_triggered()
{
    g_settings->setInfoEnabled(ui->actionLogLevelInfo->isChecked());
}

void MainWindow::on_actionLogLevelDebug_triggered()
{
    g_settings->setDebugEnabled(ui->actionLogLevelDebug->isChecked());
}

void MainWindow::on_actionLogLevelTrace_triggered()
{
    g_settings->setTraceEnabled(ui->actionLogLevelTrace->isChecked());
}
