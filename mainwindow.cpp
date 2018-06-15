#include "stdafx.h"
#include "popupmenutoolbutton.h"
#include "prtlistitemdelegate.h"
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
static QProgressDialog* g_progressDialog = nullptr;
static QAtomicInt g_loadingReferenceCount;

void MainWindow::applyProxySettings()
{
    if (g_settings->proxyType() != QNetworkProxy::NoProxy
            && !g_settings->proxyHostName().isEmpty()
            && g_settings->proxyPort() > 0
            && g_settings->proxyPort() < 65535)
    {
        QNetworkProxy proxy(g_settings->proxyType(),
                            g_settings->proxyHostName(),
                            g_settings->proxyPort());
        m_nam->setProxy(proxy);
    }
}

MainWindow::MainWindow(QSplashScreen &splash, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    thumbbar(nullptr),
    m_nam(new QNetworkAccessManager(this)),
    m_prt(nullptr),
    m_crashInfo(nullptr)
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
    connect(ui->actionCloseAllButThis, &QAction::triggered, ui->tabWidget, &TabWidget::onCloseAllButThis);
    connect(ui->actionCopyCurrentCellWithXMLFormatted, &QAction::triggered, ui->tabWidget, &TabWidget::onCopyCurrentCellWithXMLFormatted);
    connect(ui->actionCopyCurrentRowWithXMLFormatted, &QAction::triggered, ui->tabWidget, &TabWidget::onCopyCurrentRowWithXMLFormatted);
    connect(ui->actionCopySelectedCellsWithXMLFormatted, &QAction::triggered, ui->tabWidget, &TabWidget::onCopySelectedCellsWithXMLFormatted);
    connect(ui->actionCopySelectedRowsWithXMLFormatted, &QAction::triggered, ui->tabWidget, &TabWidget::onCopySelectedRowsWithXMLFormatted);
    connect(ui->actionAddCurrentRowToBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onAddCurrentRowToBookmark);
    connect(ui->actionRemoveCurrentRowFromBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onRemoveCurrentRowFromBookmark);
    connect(ui->actionAddSelectedRowsToBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onAddSelectedRowsToBookmark);
    connect(ui->actionRemoveSelectedRowsFromBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onRemoveSelectedRowsFromBookmark);
    connect(ui->actionRemoveAllBookmarks, &QAction::triggered, ui->tabWidget, &TabWidget::onRemoveAllBookmarks);
    connect(ui->actionGotoFirstBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoFirstBookmark);
    connect(ui->actionGotoPreviousBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoPreviousBookmark);
    connect(ui->actionGotoNextBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoNextBookmark);
    connect(ui->actionGotoLastBookmark, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoLastBookmark);
    connect(ui->actionScrollToTop, &QAction::triggered, ui->tabWidget, &TabWidget::onScrollToTop);
    connect(ui->actionScrollToBottom, &QAction::triggered, ui->tabWidget, &TabWidget::onScrollToBottom);
    connect(ui->actionGotoById, &QAction::triggered, ui->tabWidget, &TabWidget::onGotoById);
    connect(ui->actionExit, &QAction::triggered, qApp, &QApplication::quit);

    splash.showMessage("Loading extensions...");
    ExtensionModel* em = ExtensionModel::instance(this);
    em->scanExtensions();;
    connect(em, &ExtensionModel::extensionCreated, this, &MainWindow::onExtensionCreated);
    connect(em, &ExtensionModel::extensionModified, this, &MainWindow::onExtensionModified);
    connect(em, &ExtensionModel::extensionRemoved, this, &MainWindow::onExtensionRemoved);
    connect(em, &ExtensionModel::extensionScanned, this, &MainWindow::onExtensionScanned);

    // add popup menu tool button
    QMenu* popupMenu = new QMenu(this);
    popupMenu->addAction(ui->actionOpenCurrentInstalledJabberLogFolder);
    popupMenu->addSeparator();

    QAction* actionOpenJabberLog  = new QAction(tr("Open jabber.log"), this);
    actionOpenJabberLog->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog);
    QAction* actionOpenJabberLog1 = new QAction(tr("Open jabber.log.1"), this);
    actionOpenJabberLog1->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog1, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog1);
    QAction* actionOpenJabberLog2 = new QAction(tr("Open jabber.log.2"), this);
    actionOpenJabberLog2->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog2, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog2);
    QAction* actionOpenJabberLog3 = new QAction(tr("Open jabber.log.3"), this);
    actionOpenJabberLog3->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog3, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog3);
    QAction* actionOpenJabberLog4 = new QAction(tr("Open jabber.log.4"), this);
    actionOpenJabberLog4->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog4, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog4);
    QAction* actionOpenJabberLog5 = new QAction(tr("Open jabber.log.5"), this);
    actionOpenJabberLog5->setIcon(QIcon(":/image/open-installed-folder.png"));
    connect(actionOpenJabberLog5, &QAction::triggered, this, &MainWindow::onActionOpenJabberLogTriggered);
    popupMenu->addAction(actionOpenJabberLog5);

    PopupMenuToolButton* toolButton = new PopupMenuToolButton(this);
    toolButton->setMenu(popupMenu);
    toolButton->setDefaultAction(ui->actionOpenCurrentInstalledJabberLogFolder);
    ui->mainToolBar->insertWidget(ui->actionOpenFromPRTTrackingSystemURL, toolButton);

    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &MainWindow::onClipboardChanged);
    ui->actionSearch->setChecked(g_settings->searchOrFitler());

    splash.showMessage("Getting PRT Tracking System token...");
    getPRTTrackingSystemToken();

    splash.showMessage("Creating dock windows...");
    createDockWindows();

    applyProxySettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    QDir dir(g_settings->temporaryDirectory());
    dir.removeRecursively();
    dir.mkpath(g_settings->temporaryDirectory());
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

void MainWindow::onPRTListItemDoubleClicked(QListWidgetItem *item)
{
    QVariant d = item->data(Qt::UserRole);
    openPRTFromURL(QString("http://prt.jabberqa.cisco.com/#/conversations/%1").arg(d.toString()));
}

void MainWindow::onPRTListItemActivated(QListWidgetItem *item)
{
    QVariant d = item->data(Qt::UserRole);
    openPRTFromURL(QString("http://prt.jabberqa.cisco.com/#/conversations/%1").arg(d.toString()));
}

void MainWindow::onOpenPRTListByDefaultWebBrowser()
{
    QDesktopServices::openUrl(QUrl("http://prt.jabberqa.cisco.com/#/conversations/page/1"));
}

void MainWindow::onOpenConversationByDefaultWebBrowser()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QListWidget* list = qobject_cast<QListWidget*>(action->parentWidget()->parentWidget());
    QListWidgetItem* item = list->currentItem();
    if (item)
    {
        QVariant d = item->data(Qt::UserRole);
        QDesktopServices::openUrl(QUrl(QString("http://prt.jabberqa.cisco.com/#/conversations/%1").arg(d.toString())));
    }
}

void MainWindow::onOpenPRTViaListWidgetContextMenuItem()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QListWidget* list = qobject_cast<QListWidget*>(action->parentWidget()->parentWidget());
    QListWidgetItem* item = list->currentItem();
    if (item)
    {
        QVariant d = item->data(Qt::UserRole);
        openPRTFromURL(QString("http://prt.jabberqa.cisco.com/#/conversations/%1").arg(d.toString()));
    }
}

void MainWindow::onListWidgetCustomContextMenuRequested(const QPoint &pos)
{
    QListWidget* list = qobject_cast<QListWidget*>(sender());
    QMenu menu(list);

    QAction* action = menu.addAction(tr("Open PRT"));
    connect(action, &QAction::triggered, this, &MainWindow::onOpenPRTViaListWidgetContextMenuItem);

    action = menu.addAction(tr("Browse PRT information..."));
    connect(action, &QAction::triggered, this, &MainWindow::onOpenConversationByDefaultWebBrowser);

    action = menu.addAction(tr("Browse PRT list..."));
    connect(action, &QAction::triggered, this, &MainWindow::onOpenPRTListByDefaultWebBrowser);

    menu.exec(list->viewport()->mapToGlobal(pos));
}

void MainWindow::onClipboardChanged()
{
    QClipboard *clipboard = QApplication::clipboard();
    QString originalText = clipboard->text().trimmed();

    QString pattern = "http:\\/\\/prt\\.jabberqa\\.cisco.com\\/#\\/?conversations\\/[0-9a-zA-Z]{24,24}";
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    if (regex.match(originalText).hasMatch())
    {
        if (isMinimized())
            showMaximized();
        raise();
        activateWindow();

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

void MainWindow::onCrashInfoRequestFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    // save to file and open it
    QString tempDir = g_settings->temporaryDirectory();
    if (tempDir.isEmpty())
    {
        tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
    }
    QDir dir(tempDir);
    if (!dir.exists())
        dir.mkpath(tempDir);

    QFile f(tempDir % "/" % m_crashUrl.mid(m_crashUrl.lastIndexOf(QChar('/')),-1));
    f.open(QIODevice::WriteOnly | QIODevice::Truncate);
    f.write(m_crashInfo);
    f.close();
}

void MainWindow::onCrashInfoRequestReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_crashInfo.append(reply->readAll());
    }
}

void MainWindow::onCrashInfoRequestRedirected(const QUrl &url)
{
    m_crashUrl = url.toString();
}

void MainWindow::onPRTListFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    // parse json response
    QJsonDocument doc = QJsonDocument::fromJson(m_prtList);
    if (!doc.isObject())
    {
        qDebug() << "content received is not a json object" << QString(m_prtList);
        return;
    }

    QJsonObject docObj = doc.object();
    QJsonValue conversationsVal = docObj["conversations"];
    if (!conversationsVal.isArray())
    {
        qDebug() << "conversation node is expected to be an array";
        return;
    }

    QUrlQuery query(reply->request().url());
    QString platform = query.queryItemValue("platform");
    QMap<QString, QListWidget*> listWidgetMap = {
        { "Windows", m_windowsPRTList },
        { "Mac", m_macPRTList },
        { "iOS", m_iOSPRTList },
        { "Android", m_androidPRTList },
    };
    QListWidget* list = listWidgetMap[platform];
    list->clear();
    QJsonArray conversations = conversationsVal.toArray();
    for (auto conversation : conversations)
    {
        QJsonObject con = conversation.toObject();
        QListWidgetItem* item = new QListWidgetItem(con["topic"].toString(),list);
        item->setToolTip(con["content"].toString().replace(" [", " \n["));
        item->setData(Qt::UserRole, con["id"].toString());
        item->setData(Qt::UserRole + 1, con["topic"].toString());
        item->setData(Qt::UserRole + 2, con["owner"].toString());
        item->setData(Qt::UserRole + 3, con["content"].toString());
        list->addItem(item);
    }

    // next platform
    QMap<QString, QString> nextMap = {
        { "Windows", "Mac" },
        { "Mac", "iOS" },
        { "iOS", "Android" },
        { "Android", "Windows" },
    };
    const QString& nextPlatform = nextMap[platform];
    if (nextPlatform != "Windows")
    {
        getPRTList(nextPlatform);
    }
    else
    {
        QTimer::singleShot(60 * 60 * 1000, [this, nextPlatform]() { getPRTList(nextPlatform);});
    }
}

void MainWindow::onPRTListReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prtList.append(reply->readAll());
    }
}

void MainWindow::onPRTTrackingSystemLoginFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QJsonDocument doc = QJsonDocument::fromJson(m_prtTrackingSystemLoginInfo);
    if (!doc.isObject())
    {
        qDebug() << "content received is not a json object" << QString(m_prtTrackingSystemLoginInfo);
        return;
    }

    QJsonObject docObj = doc.object();
    QJsonValue tokenVal = docObj["token"];
    if (!tokenVal.isString())
    {
        qDebug() << "token node is expected to be a string";
        return;
    }

    QString token = tokenVal.toString();
    g_settings->setPrtTrackingSystemToken(token);

    QTimer::singleShot(15*60*1000, [this](){getPRTTrackingSystemToken(); });

    // get PRT list
    getPRTList("Windows");
}

void MainWindow::onPRTTrackingSystemLoginReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prtTrackingSystemLoginInfo.append(reply->readAll());
    }
}

void MainWindow::onPRTDownloadFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    closeProgressDialog();
    m_prt->close();

    if (!m_crashUrl.isEmpty())
    {
        QString tempDir = g_settings->temporaryDirectory();
        if (tempDir.isEmpty())
        {
            tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) % "/CiscoJabberLogs";
        }
        QDir dir(tempDir);
        if (!dir.exists())
            dir.mkpath(tempDir);

        QString crashInfo = tempDir % "/" % m_crashUrl.mid(m_crashUrl.lastIndexOf(QChar('/')),-1);
        ui->tabWidget->openZipBundle(m_prt->fileName(), crashInfo);
    }
    else
        ui->tabWidget->openZipBundle(m_prt->fileName());
    m_prt->deleteLater();
}

void MainWindow::onPRTDownloadReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prt->write(reply->readAll());
    }
}

void MainWindow::onPRTInfoRequestFinished()
{
    closeProgressDialog();
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    m_crashUrl.clear();

    QJsonDocument doc = QJsonDocument::fromJson(m_prtInfo);
    if (!doc.isObject())
    {
        qDebug() << "content received is not a json object" << QString(m_prtInfo);
        return;
    }

    QJsonObject docObj = doc.object();

    QJsonValue tagName = docObj["tagName"];
    bool isCrash = false;
    if (tagName.isString() && tagName.toString() == "Crash")
        isCrash = true;

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

#if defined(Q_OS_WIN)
        if (isCrash)
        {
            QJsonValue bodyConvertVal = ele["body_converted"];
            if (bodyConvertVal.isString())
            {
                QString bodyConverted = bodyConvertVal.toString();
                QString pattern = "http:\\/\\/jabber\\-prt\\.cisco\\.com\\/ProblemReportTriageTool\\/\\?prt_id=(\\d+)";
                QRegularExpression re(pattern);
                QRegularExpressionMatch match = re.match(bodyConverted);
                QString captured = match.captured(1);

                if (!captured.isEmpty())
                {
                    if (QMessageBox::question(this, tr("Found PRT with dump"),
                                          tr("It seems to be a crash PRT, do you want to download the PRT with dump file?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes) == QMessageBox::Yes)
                    {
                        getJabberWinPRTInfo(captured);
                        return;
                    }
                }
            }
        }
#endif

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
            if (!name.toString().toLower().endsWith(".zip"))
            {
                qDebug() << "zip bundle is expected." << name.toString();
                continue;
            }

            if (isCrash)
            {
                if (docObj["platform"].isString() && docObj["platform"].toString() != "Windows")
                {
                    if (QMessageBox::question(this, tr("Found PRT with crash information"),
                                          tr("It seems to be a crash PRT, do you want to download crash information?"),
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::Yes) == QMessageBox::Yes)
                    {
                        getCrashInfo(docObj["id"].toString(), path.toString(), name.toString(), docObj["platform"].toString());
                    }
                }
            }

            QString u = QString("http://prt.jabberqa.cisco.com/download?path=%1&name=%2").arg(path.toString()).arg(name.toString());
            downloadPRT(u);
            return;
        }
    }
}

void MainWindow::onPRTInfoRequestReadyRead()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode >= 200 && statusCode < 300)
    {
        m_prtInfo.append(reply->readAll());
    }
}

void MainWindow::onJabberWinPRTInfoRequestFinished()
{
    closeProgressDialog();
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();

    QString pattern = "http:\\/\\/jabber\\-prt\\.cisco\\.com\\/jabberPrtReport\\/uploads\\/[0-9a-zA-Z_\\-]+\\.zip";
    QRegularExpression re(pattern);
    QRegularExpressionMatch match = re.match(QString(m_prtInfo));
    QString u = match.captured();
    if (!u.isEmpty())
        downloadPRT(u);
    else
        qDebug() << "can't find PRT link";
}

void MainWindow::onPRTRequestError(QNetworkReply::NetworkError e)
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    qDebug() << "network error:" << e << reply->errorString();
}

void MainWindow::onIPCMessageReceived(const QString &message, QObject* /*socket*/)
{
    QStringList logs = message.split('\n');
    openLogs(logs);
    if (isMinimized())
        showMaximized();
    raise();
    activateWindow();
}

void MainWindow::onExtensionRemoved(ExtensionPtr e)
{
    QList<QAction*> actions;
    QMenu* pMenu = nullptr;
    auto itMenu = m_extensionMenu.find(e->category());
    if (m_extensionMenu.end() != itMenu)
    {
        pMenu = *itMenu;
        actions.append(pMenu->actions());
    }
    else
    {
        pMenu = ui->menuExtension;
        actions.append(ui->menuExtension->actions());
    }
    auto it = std::find_if(actions.begin(), actions.end(),
                           [e](QAction* action) { return (action->data().toString() == e->uuid());});

    if (actions.end() != it)
    {
        pMenu->removeAction(*it);
        (*it)->deleteLater();
    }
}

void MainWindow::onExtensionCreated(ExtensionPtr e)
{
    static int extensionCount = 1;
    QAction* action = new QAction(QString("%1 by %2").arg(e->title()).arg(e->author()), this);
    connect(action, &QAction::triggered, ui->tabWidget, &TabWidget::onExtensionActionTriggered);
    action->setData(e->uuid());
    action->setToolTip(e->comment());
    if (!e->shortcut().isEmpty())
        action->setShortcut(QKeySequence(e->shortcut()));
    else
    {
        if (extensionCount <= 10)
            action->setShortcut(QKeySequence(QString("Ctrl+%1").arg(extensionCount++)));
        else if (extensionCount <= 20)
            action->setShortcut(QKeySequence(QString("Ctrl+Shift+%1").arg(extensionCount++%10)));
        else if (extensionCount <= 30)
            action->setShortcut(QKeySequence(QString("Ctrl+Alt+%1").arg(extensionCount++%10)));
        else if (extensionCount <= 40)
            action->setShortcut(QKeySequence(QString("Shift+Alt+%1").arg(extensionCount++%10)));
    }
    if (e->category().isEmpty())
    {
        ui->menuExtension->addAction(action);
    }
    else
    {
        auto it = m_extensionMenu.find(e->category());
        if (m_extensionMenu.end() != it)
        {
            QMenu* pMenu = *it;
            pMenu->addAction(action);
        }
        else
        {
            QMenu* pMenu = ui->menuExtension->addMenu(e->category());
            m_extensionMenu.insert(e->category(), pMenu);
            pMenu->addAction(action);
        }
    }
}

void MainWindow::onExtensionModified(ExtensionPtr e)
{
    QList<QAction*> actions;
    QMenu* pMenu = nullptr;
    auto itMenu = m_extensionMenu.find(e->category());
    if (m_extensionMenu.end() != itMenu)
    {
        pMenu = *itMenu;
        actions.append(pMenu->actions());
    }
    else
    {
        pMenu = ui->menuExtension;
        actions.append(ui->menuExtension->actions());
    }

    auto it = std::find_if(actions.begin(), actions.end(),
                           [e](QAction* action) { return (action->data().toString() == e->uuid());});

    if (actions.end() != it)
    {
        (*it)->setText(QString("%1 by %2").arg(e->title()).arg(e->author()));
    }
}

void MainWindow::onActionOpenJabberLogTriggered()
{
#if defined(Q_OS_WIN)
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs/jabber.log";
#endif
#if defined(Q_OS_MAC)
    QString path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/Library/Logs/Jabber/jabber.log";
#endif

    QAction *action = qobject_cast<QAction*>(sender());
    QString caption = action->text();
    QStringList prefixes = { ".1", ".2", ".3", ".4", ".5"};
    for (const auto & prefix : prefixes)
    {
        if (caption.endsWith(prefix))
        {
            path.append(prefix);
            break;
        }
    }

    if (!QFile::exists(path))
    {
        QMessageBox::critical(this,
                             tr("Warning"),
                             QString(tr("File '%1' doesn't exist.")).arg(path),
                             QMessageBox::Ok);
        return;
    }
    QStringList files;
    files.append(path);
    ui->tabWidget->openRawLogFile(files);
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

    std::for_each(fileNames.begin(), fileNames.end(),[&](const QString& p){ ui->tabWidget->openZipBundle(p);});
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
    QUrl url(rawURL.replace("#/", "api/v1/").replace("#", "api/v1/"));
    QNetworkRequest req(url);
    req.setRawHeader("token", g_settings->prtTrackingSystemToken().toUtf8());
    m_prtInfo.clear();
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTInfoRequestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTInfoRequestFinished()));
    showProgressDialog(QString(tr("Getting PRT information of %1...")).arg(u));
}

void MainWindow::getPRTTrackingSystemToken()
{
    QUrl url("http://prt.jabberqa.cisco.com/api/v1/login");
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    req.setRawHeader("Accept", "application/json, text/plain, */*");
    QString postBody = QString("{\"username\":\"%1\",\"password\":\"%2\"}").arg(g_settings->cecId()).arg(g_settings->cecPassword());
    m_prtTrackingSystemLoginInfo.clear();
    QNetworkReply* reply = m_nam->post(req, postBody.toUtf8());
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTTrackingSystemLoginReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTTrackingSystemLoginFinished()));
}

void MainWindow::createDockWindows()
{
    ui->menuWindow->addSeparator();

    struct {
        QString title;
        QListWidget*& listWidget;
        QKeySequence shortcut;
    } mm [] = {
        {tr("Windows PRT"), m_windowsPRTList, QKeySequence("Shift+W")},
        {tr("macOS PRT"),   m_macPRTList,     QKeySequence("Shift+M")},
        {tr("iOS PRT"),     m_iOSPRTList,     QKeySequence("Shift+I")},
        {tr("Android PRT"), m_androidPRTList, QKeySequence("Shift+A")},
    };

    for ( auto m : mm)
    {
        QDockWidget *dock = new QDockWidget(m.title, this);
        dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        m.listWidget = new QListWidget(dock);
        m.listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        m.listWidget->setResizeMode(QListView::Adjust);
        m.listWidget->setWordWrap(true);
        m.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        m.listWidget->setItemDelegate(new PRTListItemDelegate(m.listWidget));
        connect(m.listWidget, &QWidget::customContextMenuRequested, this, &MainWindow::onListWidgetCustomContextMenuRequested);
        connect(m.listWidget, &QListWidget::itemActivated, this, &MainWindow::onPRTListItemActivated);
        connect(m.listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onPRTListItemDoubleClicked);
        dock->setWidget(m.listWidget);
        addDockWidget(Qt::LeftDockWidgetArea, dock);
        ui->menuWindow->addAction(dock->toggleViewAction());
        QAction* action = dock->toggleViewAction();
        action->setShortcut(m.shortcut);
        dock->close();
    }
}

void MainWindow::getPRTList(const QString& platform)
{
    QUrl url("http://prt.jabberqa.cisco.com/api/v1/conversations/page/1");
    QUrlQuery query;
    query.addQueryItem("platform",  platform);
    url.setQuery(query);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    req.setRawHeader("Accept", "application/json, text/plain, */*");
    req.setRawHeader("token", g_settings->prtTrackingSystemToken().toUtf8());
    m_prtList.clear();
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTListReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTListFinished()));
}

void MainWindow::getJabberWinPRTInfo(const QString &id)
{
    QUrl url("http://jabber-prt.cisco.com/ProblemReportTriageTool/scripts/getReportById.php");
    QUrlQuery query;
    query.addQueryItem("reportID", id);
    url.setQuery(query);
    QNetworkRequest req(url);
    m_prtInfo.clear();
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTInfoRequestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onJabberWinPRTInfoRequestFinished()));
    showProgressDialog(QString(tr("Getting Jabber Win PRT information of ID %1...")).arg(id));
}

void MainWindow::getCrashInfo(const QString &id, const QString &fileDirectory, const QString &fileName, const QString &platform)
{
    QUrl url("http://prt.jabberqa.cisco.com/analysis");
    QUrlQuery query;
    query.addQueryItem("conversation_id", id);
    query.addQueryItem("file_directory", fileDirectory);
    query.addQueryItem("file_name", fileName);
    query.addQueryItem("platform", platform);
    query.addQueryItem("user", g_settings->cecId());
    query.addQueryItem("cpve", "false");
    query.addQueryItem("crypto", "false");
    url.setQuery(query);
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, QVariant(true));
    m_crashInfo.clear();

    QNetworkReply* reply = m_nam->get(req);
    connect(reply, SIGNAL(readyRead()), this, SLOT(onCrashInfoRequestReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onCrashInfoRequestFinished()));
    connect(reply, SIGNAL(redirected(const QUrl&)), this, SLOT(onCrashInfoRequestRedirected(const QUrl&)));
}

void MainWindow::showProgressDialog(const QString &title)
{
    g_loadingReferenceCount.ref();
#if defined(Q_OS_WIN)
    g_winTaskbarButton->setOverlayIcon(QIcon(":/image/loading.png"));

    g_winTaskbarProgress->setRange(0, 0);
    g_winTaskbarProgress->setVisible(true);
    qApp->processEvents();
#endif
    if (!g_progressDialog)
    {
        g_progressDialog = new QProgressDialog(this);
        g_progressDialog->setWindowModality(Qt::WindowModal);
        g_progressDialog->setAutoClose(true);
        g_progressDialog->setAutoReset(true);
        g_progressDialog->setCancelButton(nullptr);
        g_progressDialog->setRange(0,0);
        g_progressDialog->setMinimumDuration(0);
    }
    g_progressDialog->setLabelText(title);
    g_progressDialog->show();
    qApp->processEvents();
}

void MainWindow::closeProgressDialog()
{
    if (!g_loadingReferenceCount.deref() && g_progressDialog)
    {
        g_progressDialog->setValue(200);
        g_progressDialog->deleteLater();
        g_progressDialog=nullptr;

#if defined(Q_OS_WIN)
        if (g_winTaskbarButton)
            g_winTaskbarButton->clearOverlayIcon();

        if (g_winTaskbarProgress)
            g_winTaskbarProgress->setVisible(false);
#endif
    }
}

void MainWindow::on_actionOpenFromPRTTrackingSystemURL_triggered()
{
    bool ok;
    QString u = QInputDialog::getText(this,
                                      tr("Input URL"),
                                      tr("Input a valid PRT Tracking System URL"),
                                      QLineEdit::Normal,
                                      QString(),
                                      &ok).trimmed();

    QString pattern = "http:\\/\\/prt\\.jabberqa\\.cisco.com\\/#\\/?conversations\\/[0-9a-zA-Z]{24,24}";
    QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
    if (ok && regex.match(u).hasMatch())
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
    applyProxySettings();
    getPRTTrackingSystemToken();
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

        QWinThumbnailToolButton *openPRTUrlButton = new QWinThumbnailToolButton(thumbbar);
        openPRTUrlButton->setToolTip("Open From PRT Tracking System URL");
        openPRTUrlButton->setIcon(QIcon(":/image/open-prt-url.png"));
        openPRTUrlButton->setDismissOnClick(true);
        connect(openPRTUrlButton, SIGNAL(clicked()), ui->actionOpenFromPRTTrackingSystemURL, SIGNAL(triggered()));

        thumbbar->addButton(openZipButton);
        thumbbar->addButton(openRawLogFileButton);
        thumbbar->addButton(openLogFolderButton);
        thumbbar->addButton(openCurrentInstalledJabberLogFolderButton);
        thumbbar->addButton(openPRTUrlButton);
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
    connect(reply, SIGNAL(readyRead()), this, SLOT(onPRTDownloadReadyRead()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onPRTRequestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(onPRTDownloadFinished()));
    showProgressDialog(QString(tr("Downloading PRT from %1...")).arg(u));
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

void MainWindow::on_actionOpenFromJabberWinPRTSite_triggered()
{
    bool ok = false;
    int id = QInputDialog::getInt(this, "Input PRT Id", "Please input PRT Id", 0, 0, 99999, 1, &ok);
    if (ok)
        getJabberWinPRTInfo(QString("%1").arg(id));
}

void MainWindow::on_actionGotoPRTTrackingSystem_triggered()
{
    QDesktopServices::openUrl(QUrl("http://prt.jabberqa.cisco.com/#/conversations"));
}

void MainWindow::on_actionGotoJabberWinPRTWebsite_triggered()
{
    QDesktopServices::openUrl(QUrl("http://jabber-prt.cisco.com/ProblemReportTriageTool/"));
}
