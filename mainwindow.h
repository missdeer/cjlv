#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "extension.h"

namespace Ui
{
    class MainWindow;
}

class TabWidget;
QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QWinThumbnailToolBar;
class QNetworkAccessManager;
#if defined(Q_OS_WIN)
class QWinTaskbarButton;
class QWinTaskbarProgress;
#endif
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QSplashScreen &splash, QWidget *parent = 0);
    ~MainWindow();
    void       openLogs(const QStringList &logs);
    TabWidget *getMainTabWidget();
    void       showProgressDialog(const QString &title);
    void       closeProgressDialog();
    void       applyProxySettings();

public slots:
    void onStatusBarMessageChanges(const QString &msg);
private slots:
    void onPRTListItemDoubleClicked(QListWidgetItem *item);
    void onPRTListItemActivated(QListWidgetItem *item);
    void onOpenPRTListByDefaultWebBrowser();
    void onOpenConversationByDefaultWebBrowser();
    void onOpenPRTViaListWidgetContextMenuItem();
    void onListWidgetCustomContextMenuRequested(const QPoint &pos);
    void onClipboardChanged();
    void onCrashInfoRequestFinished();
    void onCrashInfoRequestReadyRead();
    void onCrashInfoRequestRedirected(const QUrl &url);
    void onPRTListFinished();
    void onPRTListReadyRead();
    void onPRTTrackingSystemLoginFinished();
    void onPRTTrackingSystemLoginReadyRead();
    void onPRTDownloadFinished();
    void onPRTDownloadReadyRead();
    void onPRTInfoRequestFinished();
    void onPRTInfoRequestReadyRead();
    void onJabberWinPRTInfoRequestFinished();
    void onPRTRequestError(QNetworkReply::NetworkError e);
    void onIPCMessageReceived(const QString &message, QObject *socket);
    void onExtensionScanned();
    void onExtensionRemoved(ExtensionPtr e);
    void onExtensionCreated(ExtensionPtr e);
    void onExtensionModified(ExtensionPtr e);
    void onActionOpenJabberLogTriggered();

    void on_actionOpenZipLogBundle_triggered();

    void on_actionOpenRawLogFile_triggered();

    void on_actionOpenLogFolder_triggered();

    void on_actionOpenCurrentInstalledJabberLogFolder_triggered();

    void on_actionOpenFromPRTTrackingSystemURL_triggered();

    void on_actionSearch_triggered();

    void on_actionFilter_triggered();

    void on_actionPreference_triggered();

    void on_actionAbout_triggered();

    void on_actionAboutQt_triggered();

    void on_actionInputKeyword_triggered();

    void on_actionSearchFieldContent_triggered();

    void on_actionSearchFieldID_triggered();

    void on_actionSearchFieldDateTime_triggered();

    void on_actionSearchFieldThread_triggered();

    void on_actionSearchFieldCategory_triggered();

    void on_actionSearchFieldSourceFile_triggered();

    void on_actionSearchFieldMethod_triggered();

    void on_actionSearchFieldLogFile_triggered();

    void on_actionSearchFieldLine_triggered();

    void on_actionSearchFieldLevel_triggered();

    void on_actionClearKeyword_triggered();

    void on_actionEditExtensions_triggered();

    void on_actionRegexpMode_triggered();

    void on_actionHelpContent_triggered();

    void on_actionLogLevelFatal_triggered();

    void on_actionLogLevelError_triggered();

    void on_actionLogLevelWarn_triggered();

    void on_actionLogLevelInfo_triggered();

    void on_actionLogLevelDebug_triggered();

    void on_actionLogLevelTrace_triggered();

    void on_actionOpenFromJabberWinPRTSite_triggered();

    void on_actionGotoPRTTrackingSystem_triggered();

    void on_actionGotoJabberWinPRTWebsite_triggered();

    void on_actionHomepage_triggered();

    void on_actionReportIssue_triggered();

private:
    Ui::MainWindow *       ui;
    QWinThumbnailToolBar * thumbbar;
    QNetworkAccessManager *m_nam;
    QFile *                m_prt;
    QListWidget *          m_windowsPRTList;
    QListWidget *          m_macPRTList;
    QListWidget *          m_iOSPRTList;
    QListWidget *          m_androidPRTList;
    QByteArray             m_prtInfo;
    QByteArray             m_prtTrackingSystemLoginInfo;
    QByteArray             m_prtList;
    QByteArray             m_crashInfo;
    QString                m_crashUrl;
    QMap<QString, QMenu *> m_extensionMenu;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void showEvent(QShowEvent *e);

    void downloadPRT(const QString &u);
    void openPRTFromURL(const QString &u);
    void getPRTTrackingSystemToken();

    void createDockWindows();
    void getPRTList(const QString &platform);
    void getJabberWinPRTInfo(const QString &id);
    void getCrashInfo(const QString &id, const QString &fileDirectory, const QString &fileName, const QString &platform);
};

#if defined(Q_OS_WIN)
extern QWinTaskbarButton *  g_winTaskbarButton;
extern QWinTaskbarProgress *g_winTaskbarProgress;
#endif
#endif // MAINWINDOW_H
