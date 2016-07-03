#ifndef EXTENSIONMODEL_H
#define EXTENSIONMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "extension.h"

class ExtensionModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    static ExtensionModel* instance(QObject *parent = 0);

    virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void scanExtensions();

    void runByUuid(const QString& uuid);

    QList<ExtensionPtr>& extensions()
    {
        return m_extensions;
    }

private:
    static ExtensionModel* m_instance;

    QList<ExtensionPtr> m_extensions;

    explicit ExtensionModel(QObject *parent = 0);

    void scanExtensionsFromDirectory(const QString& path, const QString& from);
};

#endif // EXTENSIONMODEL_H
