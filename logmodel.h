#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "extension.h"
#include "quickwidgetapi.h"
#include "sqlite3helper.h"

struct lua_State;

struct LogItem
{
    int       id;
    QDateTime time;
    QString   level;
    QString   thread;
    QString   source;
    QString   category;
    QString   method;
    QString   content;
    QString   logFile;
    int       line;
};

typedef QSharedPointer<LogItem> LogItemPtr;

struct StatisticItem
{
    QString content;
    int     count;
    double  percent;
};

typedef QSharedPointer<StatisticItem> StatisticItemPtr;

class LogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LogModel(QObject *parent, Sqlite3HelperPtr sqlite3Helper, QuickWidgetAPIPtr api);
    virtual ~LogModel();

    virtual QModelIndex   index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int           rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int           columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant      data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant      headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void           loadFromFiles(const QStringList &fileNames);
    void           reload();
    void           query(int offset);
    void           copyCell(const QModelIndex &cell);
    void           copyRow(int row);
    void           copyCells(const QModelIndexList &cells);
    void           copyRows(const QList<int> &rows);
    void           copyCellWithXMLFormatted(const QModelIndex &cell);
    void           copyRowWithXMLFormatted(int row);
    void           copyCellsWithXMLFormatted(const QModelIndexList &cells);
    void           copyRowsWithXMLFormatted(const QList<int> &rows);
    const QString &getLogContent(const QModelIndex &index);
    QString        getLogSourceFile(const QModelIndex &index);
    QString        getLogFileName(const QModelIndex &index);
    int            getLogFileLine(const QModelIndex &index, QString &fileName);
    int            getId(const QModelIndex &index);
    int            getId(int row);
    void           runLuaExtension(ExtensionPtr e);
    void           saveRowsInFolder(const QList<int> &rows, const QString &folderName);
    void           saveRowsBetweenAnchorsInFolder(const QModelIndex &beginAnchor, const QModelIndex &endAnchor, const QString &folderName);
    QString        getSqlWhereClause(const QModelIndex &beginAnchor, const QModelIndex &endAnchor);

    bool getLevelStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    bool getThreadStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    bool getSourceFileStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    bool getSourceLineStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    bool getCategoryStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    bool getMethodStatistic(QList<QSharedPointer<StatisticItem>> &sis);
    void setSearchField(const QString &searchField);
    void setRegexpMode(bool regexpMode);
    int  getMaxTotalRowCount() const;

    Sqlite3HelperPtr getSqlite3Helper()
    {
        return m_sqlite3Helper;
    }
    void postInitialize();

    void addBookmark(int row);
    void removeBookmark(int row);
    void addBookmarks(const QList<int> &rows);
    void removeBookmarks(const QList<int> &rows);
    void clearBookmarks();
    int  getFirstBookmark();
    int  getPreviousBookmark(int currId);
    int  getNextBookmark(int currId);
    int  getLastBookmark();
signals:
    void logItemReady(int, QSharedPointer<LogItem>);
    void logItemsReady(QMap<int, QSharedPointer<LogItem>>);
    void dataLoaded();
    void rowCountChanged();
    void databaseCreated(const QString &);
public slots:
    void onLogItemReady(int i, QSharedPointer<LogItem> log);
    void onLogItemsReady(QMap<int, QSharedPointer<LogItem>> logs);
    void onFilter(const QString &keyword);
    void onSearchScopeChanged();

private:
    Sqlite3HelperPtr                   m_sqlite3Helper;
    QuickWidgetAPIPtr                  m_api;
    lua_State *                        m_L;
    QString                            m_lastFilterKeyword;
    QString                            m_searchField;
    QString                            m_searchFieldOption;
    QString                            m_sqlCount;
    QString                            m_sqlFetch;
    QString                            m_keyword;
    QString                            m_dbFile;
    QStringList                        m_logFiles;
    QMap<int, QSharedPointer<LogItem>> m_logs;
    QList<int>                         m_inQuery;
    QList<int>                         m_bookmarkIds;
    int                                m_lastBookmarkId;
    QMutex                             m_eventMutex;
    QMutex                             m_queryMutex;
    QMutex                             m_dataMemberMutex;
    QWaitCondition                     m_dataMemberCondition;
    QFuture<void>                      m_queryFuture;
    int                                m_rowCount;
    int                                m_currentTotalRowCount;
    int                                m_maxTotalRowCount;
    int                                m_toQueryOffset;
    std::atomic<bool>                  m_forceQuerying;
    std::atomic<bool>                  m_stopQuerying;
    bool                               m_regexpMode;
    bool                               m_regexpModeOption;
    bool                               m_luaMode;
    bool                               m_fts;
    bool                               m_allStanza;
    bool                               m_fullRange;
    QString                            dateTime;
    QString                            level;
    QString                            thread;
    QString                            source;
    QString                            category;
    QString                            method;
    QString                            content;

    QMap<QString, int> m_levelCountMap;
    QMap<QString, int> m_threadCountMap;
    QMap<QString, int> m_sourceFileCountMap;
    QMap<QString, int> m_sourceLineCountMap;
    QMap<QString, int> m_categoryCountMap;
    QMap<QString, int> m_methodCountMap;

    void    loadFromDatabase();
    void    createDatabase();
    int     copyFromFileToDatabase(const QString &fileName);
    void    doReload();
    void    doQuery(int offset);
    bool    parseLine(const QString &line, QStringList &results);
    bool    parseLine2(const QString &line, QStringList &results);
    void    updateStatistic(QStringList &results);
    bool    event(QEvent *e) Q_DECL_OVERRIDE;
    void    createDatabaseIndex();
    void    generateSQLStatements(int offset, QString &sqlFetch, QString &sqlCount);
    QString generateSQLStatement(int from, int to);
    void    doFilter(const QString &content, bool fts, const QString &field, bool regexpMode, bool luaMode, bool saveOptions = false);
    bool    getStatistic(const QString &tableName, QList<QSharedPointer<StatisticItem>> &sis);
    void    saveStatistic();
    QString getDataSource();
    void    initialize();
};

#endif // LOGMODEL_H
