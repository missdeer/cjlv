#include "stdafx.h"
#include <boost/scope_exit.hpp>
#include "presencemodel.h"

PresenceModel::PresenceModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    connect(this, &PresenceModel::gotPresences, &PresenceModel::onGetPrsences);
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

int PresenceModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_presences.size();
}

int PresenceModel::columnCount(const QModelIndex& /*parent*/) const
{
    return m_fullJIDList.size() + 2;
}

QVariant PresenceModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_presences.size())
        return QVariant();

    auto it = m_presences.at(index.row());

    switch (index.column())
    {
    case 0:
        return QVariant(it->id);
    case 1:
        return QVariant(it->time);
    default:
        if (index.column() -2< it->presences.size())
        {
            QString text = it->presences.at(index.column() - 2);
            if (!text.isEmpty())
            {
                int startPos = text.indexOf(QChar('<'));
                int endPos = text.lastIndexOf(QChar('>'));
                if (startPos > 0 && endPos > startPos)
                {
                    QString xmlIn = text.mid(startPos, endPos - startPos + 1);
                    QString xmlOut;

                    QDomDocument doc;
                    doc.setContent(xmlIn, false);
                    QTextStream writer(&xmlOut);
                    doc.save(writer, 4);

                    return QVariant(xmlOut.trimmed());
                }
            }
        }
    }

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
        default:
            if (section -2< m_fullJIDList.size())
                return QVariant(m_fullJIDList.at(section - 2));
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

void PresenceModel::onSelectedJIDChanged(const QString &text)
{
    // clear all
    if (!m_presences.empty())
    {
        beginRemoveRows(QModelIndex(), 0, m_presences.size() -1);
        m_presences.clear();
        endRemoveRows();
    }

    if (!m_fullJIDList.empty())
    {
        beginRemoveColumns(QModelIndex(), 2, m_fullJIDList.size() + 1);
        m_fullJIDList.clear();
        endRemoveColumns();
    }

    if (text != "--NONE--")
    {
        QtConcurrent::run(this, &PresenceModel::doQueryPresence, text);
    }
}

void PresenceModel::onGetPrsences(QStringList jidList, QList<QSharedPointer<PresenceItem> > presences)
{
    beginInsertColumns(QModelIndex(), 2, jidList.size() + 1);
    m_fullJIDList = jidList;
    endInsertColumns();

    beginInsertRows(QModelIndex(), 0, presences.size() - 1);
    m_presences = presences;
    endInsertRows();

    emit resizeTableCells(jidList.size() + 2, presences.size());
}


void PresenceModel::doQueryPresence(const QString &jid)
{
    QMutexLocker lock(&m_mutex);

    QSqlDatabase db = QSqlDatabase::database(m_dbFile, true);
    if (!db.isValid())
    {
        db = QSqlDatabase::addDatabase("QSQLITE", m_dbFile);
        db.setDatabaseName(m_dbFile);
    }

    if (!db.isOpen())
        db.open();

    QStringList jids;
    QList<QSharedPointer<PresenceItem>> results;
    if (db.isOpen())
    {
        QSqlQuery q(db);
        QString sqlFetch("SELECT id,time,content FROM logs WHERE content LIKE '%'||?||'%' ORDER BY epoch LIMIT 1, 200000;");
        q.prepare(sqlFetch);
        q.addBindValue("Recv:<presence from=\""+jid);
        if (q.exec())
        {
            int idIndex = q.record().indexOf("id");
            int dateTimeIndex = q.record().indexOf("time");
            int contentIndex = q.record().indexOf("content");
            while (q.next())
            {
                QSharedPointer<PresenceItem> p =  QSharedPointer<PresenceItem>(new PresenceItem);
                p->id = q.value(idIndex).toInt();
                p->time =  q.value(dateTimeIndex).toDateTime();
                QString content = q.value(contentIndex).toString();

                // extract from JID
                QRegularExpression re("from=\"([^\"]+)");

                QRegularExpressionMatch m = re.match(content);
                if (m.hasMatch())
                {
                    QString jid = m.captured(1);
                    int index = jids.indexOf(jid);
                    if (index < 0)
                    {
                        for (int i = 0; i < jids.size(); i++)
                            p->presences.append("");
                        jids.append(jid);
                    }
                    else
                    {
                        for (int i = 0; i < index; i++)
                            p->presences.append("");
                    }
                    p->presences.append(content);
                }
                results.append(p);
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

    if (!jids.empty() && !results.empty())
        emit gotPresences(jids, results);
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
