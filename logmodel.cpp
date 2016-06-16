#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QRegularExpression>
#include <QtConcurrent>
#include "logmodel.h"

static const QEvent::Type ROWCOUNT_EVENT = QEvent::Type(QEvent::User + 1);

class RowCountEvent : public QEvent
{
public:
    RowCountEvent() : QEvent(ROWCOUNT_EVENT) {}
    int m_rowCount;
};

LogModel::LogModel(QObject *parent)
    : QAbstractTableModel(parent)
{

}

LogModel::~LogModel()
{
    QSqlDatabase db = QSqlDatabase::database(m_dbFile, false);
    if (db.isValid() && db.isOpen())
        db.close();

    QFile::remove(m_dbFile);
}

QModelIndex LogModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
            return QModelIndex();

    return createIndex(row, column);
}

int LogModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_rowCount;
}

int LogModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 8;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_logs.size())
        return QVariant();

    QSharedPointer<LogItem> r = m_logs.at(index.row());
    switch (index.column())
    {
    case 0:
        return QVariant(r->id);
    case 1:
        return QVariant(r->time);
    case 2:
        return QVariant(r->level);
    case 3:
        return QVariant(r->thread);
    case 4:
        return QVariant(r->source);
    case 5:
        return QVariant(r->category);
    case 6:
        return QVariant(r->method);
    case 7:
        return QVariant(r->content);
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags LogModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        default:
            break;
        }

    }

    return QVariant();
}

void LogModel::loadFromFiles(const QStringList& fileNames)
{
    m_logFiles = fileNames;
    reload();
}

void LogModel::reload()
{
    QtConcurrent::run(this, &LogModel::doReload);
}

void LogModel::doReload()
{
    createDatabase();
    QDateTime t = QDateTime::currentDateTime();
    Q_FOREACH(const QString& fileName, m_logFiles)
    {
        copyFromFileToDatabase(fileName);
    }
    qint64 q = t.secsTo(QDateTime::currentDateTime());
    qDebug() << "loaded elapsed " << q << " s";
    RowCountEvent* e = new RowCountEvent;
    e->m_rowCount = 0;
    QSqlDatabase db = QSqlDatabase::database(m_dbFile, false);
    if (db.open())
    {
        QSqlQuery query(db);
        e->m_rowCount = query.lastInsertId().toInt();
    }
    QCoreApplication::postEvent(this, e);
}

bool LogModel::event(QEvent *e)
{
    QMutexLocker lock(&m_mutex);

    switch (int(e->type()))
    {
    case ROWCOUNT_EVENT:
        m_rowCount = ((RowCountEvent*)e)->m_rowCount;
        emit dataChanged(index(0, 0), index(m_rowCount -1, 0));
        return true;
    default:
        break;
    }

    return QObject::event(e);
}

void LogModel::createDatabase()
{
    m_dbFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_dbFile.append("/CiscoJabberLogs/database");
    QDir dir(m_dbFile);
    if (!dir.exists())
        dir.mkpath(m_dbFile);
    m_dbFile.append("/");
    m_dbFile.append(QDateTime::currentDateTime().toString("ddd MMMM d yyyy hh-mm-ss.zzz"));
    m_dbFile.append(".db");

    QFile::remove(m_dbFile);

    QSqlDatabase db = QSqlDatabase::database(m_dbFile, true);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", m_dbFile);
        db.setDatabaseName(m_dbFile);
    }

    if (db.open())
    {
        QSqlQuery query(db);
        query.exec("CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT,time TEXT,level TEXT,thread TEXT,source TEXT,category TEXT,method TEXT, content TEXT);");
        query.exec("CREATE INDEX it ON logs (time);");
        query.exec("CREATE INDEX is ON logs (source);");
        query.exec("CREATE INDEX ic ON logs (category);");
        query.exec("CREATE INDEX im ON logs (method);");
        query.exec("CREATE INDEX io ON logs (content);");
    }
}

void LogModel::copyFromFileToDatabase(const QString &fileName)
{
    QSqlDatabase db = QSqlDatabase::database(m_dbFile, false);
    if (!db.isValid() || !db.isOpen())
    {
        qDebug() << "opening database file failed " << m_dbFile;
        return;
    }

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening log file failed " << fileName;
        return;
    }

    QRegularExpression re("^([0-9]{4}\\-[0-9]{2}\\-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3})\\s+([A-Z]{4,5})\\s+\\[(0x[0-9a-f]{8,16})\\]\\s+\\[([0-9a-zA-Z:\\-\\/\\\\\\(\\)\\.]+)\\]\\s+\\[([0-9a-zA-Z\\-\\_\\.]+)\\]\\s+\\[([0-9a-zA-Z:\\-\\_\\.]+)\\]\\s+\\-\\s+(.+)$");

    QByteArray line = f.readLine();
    QRegularExpressionMatch m = re.match(line);
    if (!m.hasMatch())
    {
        // append to last line
        content.append(line);
    }
    else
    {
        dateTime = m.captured(1);
        level = m.captured(2);
        thread = m.captured(3);
        source = m.captured(4);
        category = m.captured(5);
        method = m.captured(6);
        content = m.captured(7);
    }

    db.transaction();
    while(!f.atEnd())
    {
        QByteArray lookAhead = f.readLine();
        m = re.match(lookAhead);
        if (! m.hasMatch())
        {
            // append to last line
            content.append(lookAhead);
            //qDebug() << "append look ahead" << QString(lookAhead);
        }
        else
        {
            //qDebug() << "save line";
            QSqlQuery query(db);
            // save to database
            query.prepare("INSERT INTO logs (time, level, thread, source, category, method, content) "
                "VALUES (:time, :level, :thread, :source, :category, :method, :content );");
            query.bindValue(":time", dateTime);
            query.bindValue(":level", level);
            query.bindValue(":thread", thread);
            query.bindValue(":source", source);
            query.bindValue(":category", category);
            query.bindValue(":method", method);
            query.bindValue(":content", content);
            if (!query.exec()) {
        #ifdef _DEBUG
                qDebug() << dateTime << level << thread << source << category << method << content << " inserting log into database failed!" << query.lastError();
        #endif
            }
            query.clear();

            // parse lookAhead
            dateTime = m.captured(1);
            level = m.captured(2);
            thread = m.captured(3);
            source = m.captured(4);
            category = m.captured(5);
            method = m.captured(6);
            content = m.captured(7);
            //qDebug() << "parse look ahead";
        }
    }
    db.commit();

    f.close();
}
