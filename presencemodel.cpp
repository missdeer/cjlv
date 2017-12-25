#include "stdafx.h"
#include <boost/scope_exit.hpp>
#include "presencemodel.h"

PresenceModel::PresenceModel(QObject *parent, Sqlite3HelperPtr sqlite3Helper)
    : QAbstractTableModel(parent)
    , m_sqlite3Helper(sqlite3Helper)
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

    if (role == Qt::BackgroundRole && index.column() > 1 && index.column() %2 == 0)
    {
        return QVariant(QColor(224, 224, 224));
    }

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_presences.size())
        return QVariant();

    auto it = m_presences.at(index.row());

    switch (index.column())
    {
    case 0:
        return QVariant(it->id);
    case 1:
        return QVariant(it->time.toString("yyyy-MM-dd hh:mm:ss.zzz"));
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
                    if (role == Qt::DisplayRole)
                    {
                        QTextStream writer(&xmlOut);
                        doc.save(writer, 4);

                        return QVariant(xmlOut.trimmed());
                    }
                    if (role == Qt::ToolTipRole)
                    {
                        // extract x node
                        QDomElement it = doc.documentElement().firstChildElement("x");
                        if (!it.isNull())
                        {
                            QString var = it.attribute("var");
                            bool ok = false;
                            int value = var.toInt(&ok, 10);
                            if (ok)
                            {
                                QMap<int, QString> m = {
                                    {1, "Desktop Share"},
                                    {2, "VOIP"},
                                    {4, "Video"},
                                    {8, "On a Call"},
                                    {16, "In Webex Meeting"},
                                    {32, "In Outlook Meeting"},
                                    {64, "Tele Conf"},
                                    {256, "AES"},
                                    {512, "Sharing in Webex Meeting"},
                                };
                                QString res("Mask:");
                                for (auto kv = m.begin(); m.end() != kv; ++kv)
                                {
                                    if (value & kv.key())
                                    {
                                        res.append("\n    " + kv.value());
                                    }
                                }

                                if (res != "Mask:")
                                    return QVariant(res);
                            }
                        }
                    }
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

    QStringList jids;
    QList<QSharedPointer<PresenceItem>> results;

    bool eof = false;
    int nRet = 0;
    sqlite3_stmt* pVM = nullptr;

    do {
        pVM = m_sqlite3Helper->compile("SELECT id,time,content FROM logs WHERE content LIKE '%'||?||'%' ORDER BY epoch LIMIT 1, 200000;");
        m_sqlite3Helper->bind(pVM, 1, "Recv:<presence from=\""+jid);
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            QRegularExpression re("from=\"([^\"]+)");
            while (!eof)
            {
                QSharedPointer<PresenceItem> p =  QSharedPointer<PresenceItem>(new PresenceItem);
                p->id = sqlite3_column_int(pVM, 0);
                p->time = QDateTime::fromString(QString((const char *)sqlite3_column_text(pVM, 1)), Qt::ISODate);
                QString content((const char *)sqlite3_column_text(pVM, 2));
                // extract from JID

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

                m_sqlite3Helper->nextRow(pVM, eof);
            }
            break;
        }
    }while(nRet == SQLITE_SCHEMA);

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

    QStringList result;

    bool eof = false;
    int nRet = 0;
    sqlite3_stmt* pVM = nullptr;
    do {
        pVM = m_sqlite3Helper->compile("SELECT content FROM logs WHERE content LIKE '%'||?||'%' ORDER BY epoch LIMIT 1, 200000;");
        m_sqlite3Helper->bind(pVM, 1, "Recv:<presence from=");
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            // extract from JID
            QRegularExpression re("from=\"([^/\"]+)");
            while (!eof)
            {
                QString content((const char *)sqlite3_column_text(pVM, 0));

                QRegularExpressionMatch m = re.match(content);
                if (m.hasMatch())
                {
                    QString s = m.captured(1);
                    if (!result.contains(s))
                        result.append(s);
                }

                m_sqlite3Helper->nextRow(pVM, eof);
            }
            break;
        }
    }while(nRet == SQLITE_SCHEMA);

    emit receivedPresenceBuddyList(result);
}
