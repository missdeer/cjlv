#include "stdafx.h"
#include "presencemodel.h"

PresenceModel::PresenceModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

PresenceModel::~PresenceModel()
{

}

QModelIndex PresenceModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    return createIndex(row, column);
}

int PresenceModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int PresenceModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}

QVariant PresenceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    return QVariant();
}

Qt::ItemFlags PresenceModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant PresenceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return QVariant(tr("Id"));
        case 1:
            return QVariant(tr("Time"));
        case 2:
            return QVariant(tr("Level"));
        case 3:
            return QVariant(tr("Thread"));
        case 4:
            return QVariant(tr("Source File"));
        case 5:
            return QVariant(tr("Category"));
        case 6:
            return QVariant(tr("Method"));
        case 7:
            return QVariant(tr("Content"));
        case 8:
            return QVariant(tr("Log File"));
        case 9:
            return QVariant(tr("Line"));
        default:
            break;
        }

    }

    return QVariant();
}
