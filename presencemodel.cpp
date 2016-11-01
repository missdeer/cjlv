#include "stdafx.h"
#include <boost/scope_exit.hpp>
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

void PresenceModel::requestReceivedPresenceBuddyList()
{
    QtConcurrent::run(this, &PresenceModel::doRequestReceivedPresenceBuddyList);
}

void PresenceModel::onDatabaseCreated(QString dbFile)
{
    m_dbFile = dbFile;
}

void PresenceModel::doRequestReceivedPresenceBuddyList()
{
    if (!m_mutex.tryLock())
    {
        qDebug() << "obtaining lock failed";
        return;
    }

    BOOST_SCOPE_EXIT(this_) {
        this_->m_mutex.unlock();
    } BOOST_SCOPE_EXIT_END

    QSqlDatabase db = QSqlDatabase::database(m_dbFile, true);
    if (!db.isValid())
    {
        db = QSqlDatabase::addDatabase("QSQLITE", m_dbFile);
        db.setDatabaseName(m_dbFile);
    }

    if (!db.isOpen())
        db.open();

    QStringList result;
    if (db.isOpen())
    {
        QSqlQuery q(db);
        QString sqlFetch("SELECT * FROM logs WHERE content LIKE '%'||?||'%' ORDER BY epoch LIMIT 1, 200000;");
        q.prepare(sqlFetch);
        q.addBindValue("Recv:<presence from=");
        if (q.exec())
        {
            int contentIndex = q.record().indexOf("content");
            while (q.next())
            {
                QString content = q.value(contentIndex).toString();

                // extract from JID
                QRegularExpression re("from=\"([^/\"]+)");

                QRegularExpressionMatch m = re.match(content);
                if (m.hasMatch())
                {
                    QString s = m.captured(1);
                    if (!result.contains(s))
                        result.append(s);
                }
            }
            q.clear();
            q.finish();
        }
        else
        {
            QSqlError e = q.lastError();
            qWarning() << "error:" << e;
        }
    }

    emit receivedPresenceBuddyList(result);
}
