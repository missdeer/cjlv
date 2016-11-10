#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "extension.h"

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
class QWinThumbnailToolBar;
#if defined(Q_OS_WIN)
class QWinTaskbarButton;
class QWinTaskbarProgress;
#endif
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openLogs(const QStringList& logs);
public slots:
    void onStatusBarMessageChanges(const QString& msg);
private slots:
    void onIPCMessageReceived(const QString &message, QObject *socket);
    void onExtensionScanned();
    void onExtensionRemoved(ExtensionPtr e);
    void onExtensionCreated(ExtensionPtr e);
    void onExtensionModified(ExtensionPtr e);

    void on_actionOpenZipLogBundle_triggered();

    void on_actionOpenRawLogFile_triggered();

    void on_actionOpenLogFolder_triggered();

    void on_actionOpenCurrentInstalledJabberLogFolder_triggered();

    void on_actionSearch_triggered();

    void on_actionFilter_triggered();

    void on_actionPreference_triggered();

    void on_actionAbout_triggered();

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

private:
    Ui::MainWindow *ui;
    QWinThumbnailToolBar *thumbbar;
    void dragEnterEvent(QDragEnterEvent *e);

    void dropEvent(QDropEvent *e);

    void showEvent(QShowEvent *e);
};

#if defined(Q_OS_WIN)
extern QWinTaskbarButton *g_winTaskbarButton;
extern QWinTaskbarProgress *g_winTaskbarProgress;
#endif
#endif // MAINWINDOW_H
