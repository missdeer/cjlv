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
    static QObject *APIProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    LogView(QWidget *parent = NULL, Sqlite3HelperPtr sqlite3Helper = Sqlite3HelperPtr(new Sqlite3Helper), QuickWidgetAPIPtr api = QuickWidgetAPIPtr(new QuickWidgetAPI));
    ~LogView();

    Sqlite3HelperPtr getSqlite3Helper();
    QuickWidgetAPIPtr getQuickWidgetAPI();
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

    void scrollToTop();
    void scrollToBottom();
    void gotoById(int i);

    void reload();
    int rowCount();
    void filter(const QString& keyword);

    void enableRegexpMode(bool enabled);

    void inputKeyword();

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
protected:

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onDataLoaded();
    void onRowCountChanged();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onHHeaderCustomContextMenuRequested(const QPoint &pos);
    void onBrowseSourceFileWithOpenGrok();
    void onSourceFilePreview();
    void onOpenSourceFileInVS();
    void onContentPreview();
    void onLogFilePreview();
    void onCbKeywordEditTextChanged(const QString &text);
    void onCbKeywordCurrentIndexChanged(const QString &text);
    void onReloadSearchResult();
    void onOpenSelectedRowsInNewTab();
    void onShowLogItemsBetweenSelectedRows();
    void onSetBeginAnchor();
    void onSetEndAnchor();
    void onOpenRowsBetweenAnchorsInNewTab();
    void onShowLogItemsBetweenAnchors();
    void onLogTableChartTabWidgetCurrentChanged(int index);
    void onHHeaderContextMenuActionTriggered();
public slots:
    void onClearKeyword();
private:
    QSplitter* m_verticalSplitter;
    QTabWidget* m_logTableChartTabWidget;
    QTableView *m_logsTableView;
    QtCharts::QChartView* m_levelStatisticChart;
    QtCharts::QChartView* m_threadStatisticChart;
    QtCharts::QChartView* m_sourceFileStatisticChart;
    QtCharts::QChartView* m_sourceLineStatisticChart;
    QtCharts::QChartView* m_categoryStatisticChart;
    QtCharts::QChartView* m_methodStatisticChart;
    CodeEditorTabWidget* m_codeEditorTabWidget;
    LogModel* m_logModel;
    PresenceWidget* m_presenceWidget;
    QComboBox* m_cbSearchKeyword;
    QQuickWidget* m_extraToolPanel;
    QToolButton* m_extraToolPanelVisibleButton;
    QuickWidgetAPIPtr m_api;
    QTimer* m_keywordChangedTimer;
    QString m_path;
    QString m_extractDir;
    QMutex m_mutex;
    int m_lastId;
    int m_lastColumn;
    QModelIndex m_beginAnchor;
    QModelIndex m_endAnchor;

    QString m_crashInfo;
    QList<bool> m_hheaderColumnHidden;

    bool event(QEvent *e) Q_DECL_OVERRIDE;

    void extract(LogView* v, const QString& fileName, const QString& dirName);
    void extractContent(const QModelIndex& index);
    void openSourceFile(const QModelIndex& index, std::function<void(const QString&, int)> callback);
    void openLog(const QModelIndex& index);
    void gotoLogLine(const QModelIndex& index);
    void showCodeEditorPane();
    void setChart(QtCharts::QChartView* chartView, const QList<QSharedPointer<StatisticItem> >& sis, const QString& label);
    MainWindow *getMainWindow();
    void openCrashReport();
    void openSourceFileWithBuiltinEditor(const QString& filePath, int line);
    void openSourceFileInVS(const QString& filePath, int line);
    void openSourceFileWithOpenGrok(const QString& filePath, int line);
    void initialize();
};

class SourceWindow;
extern SourceWindow* g_sourceWindow;
#endif // LOGVIEW_H
