#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QMap>
#include <QSharedPointer>

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
    void filter(const QString& keyword);
    void query(int offset);
signals:
    void logItemReady(int, QSharedPointer<LogItem>);
public slots:
    void onLogItemReady(int i, QSharedPointer<LogItem> log);
private:
    QString m_sqlCount;
    QString m_sqlFetch ;
    QString m_keyword;
    QString m_dbFile;
    QStringList m_logFiles;
    QMap<int, QSharedPointer<LogItem>> m_logs;
    QList<int> m_inQuery;
    QMutex m_eventMutex;
    QMutex m_queryMutex;
    int m_rowCount;
    int m_totalRowCount;

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
    bool event(QEvent *e) Q_DECL_OVERRIDE;
};

#endif // LOGMODEL_H
