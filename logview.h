#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QWidget>
#include <QStringList>
#include <QMutex>
#include "codeeditortabwidget.h"
#include "extension.h"
#include "sqlite3helper.h"
#include "quickwidgetapi.h"

class MainWindow;
class PresenceWidget;
class LogModel;
class LogTableView;
struct StatisticItem;

QT_BEGIN_NAMESPACE
class QTableView;
class QSplitter;
class QTimer;
class QQmlEngine;
class QJSEngine;
QT_END_NAMESPACE


class LogView : public QWidget
{
    Q_OBJECT
public:
    explicit LogView(QWidget *parent = NULL);
    ~LogView();

    Sqlite3HelperPtr getSqlite3Helper();
    void openZipBundle(const QString& zipBundle, const QString& crashInfo);
    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path);

    bool matched(const QString& path);
    bool matched(const QStringList& paths);

    void copyCurrentCell();
    void copyCurrentRow();
    void copySelectedCells();
    void copySelectedRows();
    void copyCurrentCellWithXMLFormatted();
    void copyCurrentRowWithXMLFormatted();
    void copySelectedCellsWithXMLFormatted();
    void copySelectedRowsWithXMLFormatted();

    void addCurrentRowToBookmark();
    void removeCurrentRowFromBookmark();
    void addSelectedRowsToBookmark();
    void removeSelectedRowsFromBookmark();
    void removeAllBookmarks();
    void gotoFirstBookmark();
    void gotoPreviousBookmark();
    void gotoNextBookmark();
    void gotoLastBookmark();

    void scrollToTop();
    void scrollToBottom();
    void gotoById(int i);

    int rowCount();

    void enableRegexpMode(bool enabled);
    void inputKeyword();
    void onClearKeyword();

    void searchFieldContent();
    void searchFieldID();
    void searchFieldDateTime();
    void searchFieldThread();
    void searchFieldCategory();
    void searchFieldSourceFile();
    void searchFieldMethod();
    void searchFieldLogFile();
    void searchFieldLine();
    void searchFieldLevel();

    void onRunExtension(ExtensionPtr e);

    const QString& getPath() const;
    void setPath(const QString &path);

signals:
    void rowCountChanged();
    void runExtension(ExtensionPtr e);

private slots:
    void onDataLoaded();
    void onShowLogItemsBetweenSelectedRows();
    void onLogTableChartTabWidgetCurrentChanged(int index);
    void openSourceFileWithBuiltinEditor(const QString& filePath, int line);
    void openSourceFileInVS(const QString& filePath, int line);
    void openSourceFileWithOpenGrok(const QString& filePath, int line);

private:
    Sqlite3HelperPtr m_sqlite3Helper;
    QSplitter* m_verticalSplitter;
    QTabWidget* m_logTableChartTabWidget;
    QtCharts::QChartView* m_levelStatisticChart;
    QtCharts::QChartView* m_threadStatisticChart;
    QtCharts::QChartView* m_sourceFileStatisticChart;
    QtCharts::QChartView* m_sourceLineStatisticChart;
    QtCharts::QChartView* m_categoryStatisticChart;
    QtCharts::QChartView* m_methodStatisticChart;
    CodeEditorTabWidget* m_codeEditorTabWidget;
    PresenceWidget* m_presenceWidget;
    QString m_path;
    QString m_extractDir;
    QMutex m_mutex;

    QString m_crashInfo;

    QList<LogTableView*> m_logTableViews;

    bool event(QEvent *e) Q_DECL_OVERRIDE;

    void extract(LogView* v, const QString& fileName, const QString& dirName);
    void extractContent(const QString& text);
    void openLog(const QString &logFile);
    void gotoLogLine(int line, const QString& logFile);
    void showCodeEditorPane();
    void setChart(QtCharts::QChartView* chartView, const QList<QSharedPointer<StatisticItem> >& sis, const QString& label);
    MainWindow *getMainWindow();
    void openCrashReport();
    void initialize();
    LogTableView *createLogTableView();
};

class SourceWindow;
extern SourceWindow* g_sourceWindow;
#endif // LOGVIEW_H
