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

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void loadFromFiles(const QStringList& fileNames);
signals:

public slots:

private:
    QList<QSharedPointer<LogItem>> m_logs;
};

#endif // LOGMODEL_H
