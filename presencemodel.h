#ifndef PRESENCEMODEL_H
#define PRESENCEMODEL_H


class PresenceModel : public QAbstractTableModel
{
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

};

#endif // PRESENCEMODEL_H
