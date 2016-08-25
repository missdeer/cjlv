#ifndef LOGMODEL_H
#define LOGMODEL_H

#include "extension.h"

struct lua_State;

struct LogItem {
    int id;
    QDateTime time;
    QString level;
    QString thread;
    QString source;
    QString category;
    QString method;
    QString content;
    QString logFile;
    int line;
};

class LogModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit LogModel(QObject *parent = 0);
    ~LogModel();

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void loadFromFiles(const QStringList& fileNames);
    void reload();
    void query(int offset);
    void copyCell(const QModelIndex& cell);
    void copyRow(int row);
    void copyCells(const QModelIndexList& cells);
    void copyRows(const QList<int>& rows);
    const QString& getLogContent(const QModelIndex &index);
    QString getLogSourceFile(const QModelIndex &index);
    QString getLogFileName(const QModelIndex &index);
    int getLogFileLine(const QModelIndex &index, QString& fileName);
    int getId(const QModelIndex &index);
    void runExtension(ExtensionPtr e);
signals:
    void logItemReady(int, QSharedPointer<LogItem>);
    void logItemsReady(QMap<int, QSharedPointer<LogItem>>);
    void dataLoaded();
    void rowCountChanged();
public slots:
    void onLogItemReady(int i, QSharedPointer<LogItem> log);
    void onLogItemsReady(QMap<int, QSharedPointer<LogItem>> logs);
    void onFilter(const QString& keyword);
private:
    lua_State* m_L;
    QString m_searchField;
    QString m_sqlCount;
    QString m_sqlFetch ;
    QString m_keyword;
    QString m_dbFile;
    QStringList m_logFiles;
    QMap<int, QSharedPointer<LogItem>> m_logs;
    QList<int> m_inQuery;
    QMutex m_eventMutex;
    QMutex m_queryMutex;
    QFuture<void> m_queryFuture;
    int m_rowCount;
    int m_totalRowCount;
    bool m_stopQuerying;
    bool m_regexpMode;
    bool m_luaMode;

    QString dateTime;
    QString level ;
    QString thread ;
    QString source ;
    QString category;
    QString method ;
    QString content ;
    void createDatabase();
    int copyFromFileToDatabase(const QString& fileName);
    void doReload();
    void doQuery(int offset);
    bool parseLine(const QString &line, QStringList& results);
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void createDatabaseIndex();
    void generateSQLStatements(int offset, QString& sqlFetch, QString& sqlCount);
    void doFilter(const QString& content, const QString& field, bool regexpMode, bool luaMode);
};

#endif // LOGMODEL_H
