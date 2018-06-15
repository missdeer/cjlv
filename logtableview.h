#ifndef LOGTABLEVIEW_H
#define LOGTABLEVIEW_H

#include <QWidget>
#include <QStringList>
#include <QMutex>
#include "codeeditortabwidget.h"
#include "extension.h"
#include "sqlite3helper.h"
#include "quickwidgetapi.h"

class MainWindow;
class LogModel;
struct StatisticItem;

QT_BEGIN_NAMESPACE
class QTableView;
class QSplitter;
class QTimer;
class QQmlEngine;
class QJSEngine;
QT_END_NAMESPACE


class LogTableView : public QWidget
{
    Q_OBJECT
public:
    explicit LogTableView(QWidget *parent, Sqlite3HelperPtr sqlite3Helper, QuickWidgetAPIPtr api = QuickWidgetAPIPtr(new QuickWidgetAPI));

    LogModel *getModel();
    const QString & path() const;
    void setPath(const QString &path);
    void loadFromFiles(const QStringList& fileNames);

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
signals:
    void databaseCreated(const QString&);
    void rowCountChanged();
    void runExtension(ExtensionPtr e);
    void itemDoubleClicked(const QModelIndex &index);
    void gotoLogLine(int line, const QString& filePath);
    void extractContent(const QString &text);
    void openLog(const QString& filePath);
    void openSourceFileWithBuiltinEditor(const QString& filePath, int line);
    void openSourceFileInVS(const QString& filePath, int line);
    void openSourceFileWithOpenGrok(const QString& filePath, int line);
public slots:
    void onClearKeyword();
    void onShowLogItemsBetweenSelectedRows();

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
    void onHHeaderContextMenuActionTriggered();

private:
    QTableView *m_logsTableView;
    LogModel* m_logModel;
    QComboBox* m_cbSearchKeyword;
    QQuickWidget* m_extraToolPanel;
    QToolButton* m_extraToolPanelVisibleButton;
    QuickWidgetAPIPtr m_api;
    QTimer* m_keywordChangedTimer;
    QString m_path;
    int m_lastId;
    int m_lastColumn;
    QModelIndex m_beginAnchor;
    QModelIndex m_endAnchor;

    QList<bool> m_hheaderColumnHidden;

    void openSourceFile(const QModelIndex &index, std::function<void(const QString&, int)> callback);

    void filter(const QString &keyword);
    void initialize();
};

#endif // LOGTABLEVIEW_H
