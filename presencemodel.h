#ifndef PRESENCEMODEL_H
#define PRESENCEMODEL_H


class PresenceModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PresenceModel(QObject *parent = 0);
    ~PresenceModel();

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const ;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const ;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const ;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const ;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const ;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const ;

    void requestReceivedPresenceBuddyList();
signals:
    void receivedPresenceBuddyList(QStringList);
public slots:
    void onDatabaseCreated(QString dbFile);
private:
    QMutex m_mutex;
    QString m_dbFile;
    void doRequestReceivedPresenceBuddyList();
};

#endif // PRESENCEMODEL_H
