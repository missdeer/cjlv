#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QWidget>
#include <QStringList>
#include <QMutex>
#include "codeeditortabwidget.h"
#include "extension.h"

class LogModel;
struct StatisticItem;

QT_BEGIN_NAMESPACE
class QTableView;
class QSplitter;
class QTimer;
QT_END_NAMESPACE


class LogView : public QWidget
{
    Q_OBJECT
public:
    LogView(QWidget *parent = NULL);
    ~LogView();

    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path);

    bool matched(const QString& path);
    bool matched(const QStringList& paths);

    void copyCurrentCell();
    void copyCurrentRow();
    void copySelectedCells();
    void copySelectedRows();

    void scrollToTop();
    void scrollToBottom();
    void gotoById(int i);

    void reload();
    int rowCount();
    void filter(const QString& keyword);
signals:
    void rowCountChanged();
    void runExtension(ExtensionPtr e);
protected:

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onDataLoaded();
    void onRowCountChanged();
    void onCustomContextMenuRequested(const QPoint &pos);
    void onBrowseSourceFileWithOpenGrok();
    void onSourceFilePreview();
    void onContentPreview();
    void onLogFilePreview();
private:
    QSplitter* m_verticalSplitter;
    QTabWidget* m_logTableChartTabWidget;
    QTableView *m_tableView;
    QtCharts::QChartView* m_levelStatisticChart;
    QtCharts::QChartView* m_threadStatisticChart;
    QtCharts::QChartView* m_sourceFileStatisticChart;
    QtCharts::QChartView* m_sourceLineStatisticChart;
    QtCharts::QChartView* m_categoryStatisticChart;
    QtCharts::QChartView* m_methodStatisticChart;
    CodeEditorTabWidget* m_codeEditorTabWidget;
    LogModel* m_model;
    QString m_path;
    QString m_extractDir;
    QMutex m_mutex;
    int m_lastId;
    int m_lastColumn;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

    void extract(LogView* v, const QString& fileName, const QString& dirName);
    void showProgressDialog();
    void closeProgressDialog();
    void extractContent(const QModelIndex& index);
    void openSourceFile(const QModelIndex& index, bool openWithBuiltinEditor);
    void openLog(const QModelIndex& index);
    void gotoLogLine(const QModelIndex& index);
    void showCodeEditorPane();
    void setChart(QtCharts::QChartView* chartView, const QList<QSharedPointer<StatisticItem> >& sis, const QString& label);
};

class SourceWindow;
extern SourceWindow* g_sourceWindow;
#endif // LOGVIEW_H
