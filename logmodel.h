#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

struct LogItem {
    int id;
    QString time;
    QString level;
    QString thread;
    QString source;
    QString category;
    QString method;
    QString content;
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
signals:

public slots:

private:
    QString m_dbFile;
    QStringList m_logFiles;
    QList<QSharedPointer<LogItem>> m_logs;
    QMutex m_mutex;
    int m_rowCount;

    QString dateTime;
    QString level ;
    QString thread ;
    QString source ;
    QString category;
    QString method ;
    QString content ;
    void createDatabase();
    void copyFromFileToDatabase(const QString& fileName);
    void doReload();

    bool event(QEvent *e) Q_DECL_OVERRIDE;
};

#endif // LOGMODEL_H
