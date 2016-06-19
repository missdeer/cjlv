#include <QtCore>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QRegularExpression>
#include <QtConcurrent>
#include "settings.h"
#include "logmodel.h"

static const QEvent::Type ROWCOUNT_EVENT = QEvent::Type(QEvent::User + 1);
static const QEvent::Type FINISHEDQUERY_EVENT = QEvent::Type(QEvent::User + 2);

class RowCountEvent : public QEvent
{
public:
    RowCountEvent() : QEvent(ROWCOUNT_EVENT) {}
    int m_rowCount;
};

class FinishedQueryEvent : public QEvent
{
public:
    FinishedQueryEvent() : QEvent(FINISHEDQUERY_EVENT){}
    int m_offset;
    int m_size;
};

LogModel::LogModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_rowCount(0)
    , m_totalRowCount(0)
{
    qRegisterMetaType<QSharedPointer<LogItem>>("QSharedPointer<LogItem>");
    connect(this, &LogModel::logItemReady, this, &LogModel::onLogItemReady);
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
    return 10;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_totalRowCount)
        return QVariant();

    auto it = m_logs.find(index.row());
    if (m_logs.end() == it)
    {
#ifndef QT_NO_DEBUG
        qDebug() << "do query index:" << index.row();
#endif
        const_cast<LogModel&>(*this).query(index.row());
        return QVariant();
    }

    QSharedPointer<LogItem> r = *it;
    switch (index.column())
    {
    case 0:
        return QVariant(r->id);
    case 1:
        return QVariant(r->time.toString("yyyy-MM-dd hh:mm:ss.zzz"));
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
    case 8:
        return QVariant("." + r->logFile);
    case 9:
        return QVariant(r->line);
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

void LogModel::loadFromFiles(const QStringList& fileNames)
{
    m_logFiles = fileNames;
    reload();
}

void LogModel::reload()
{
    QtConcurrent::run(this, &LogModel::doReload);
}

void LogModel::onFilter(const QString &keyword)
{
    if (m_rowCount > 0)
    {
        beginRemoveRows(QModelIndex(), 0, m_rowCount-1);
        m_logs.clear();
        m_rowCount = 0;
        m_totalRowCount = 0;
        endRemoveRows();
    }

    m_keyword = keyword;
    query(0);
}

void LogModel::query(int offset)
{
    Q_FOREACH(int i, m_inQuery)
    {
        if (offset >= i && offset < i + 200)
        {
#ifndef QT_NO_DEBUG
            qDebug() << __FUNCTION__ << "offset:" << offset << ", i:" << i;
#endif
            return;
        }
    }

    m_inQuery.push_back(offset);

    QtConcurrent::run(this, &LogModel::doQuery, offset);
}

const QString& LogModel::getText(const QModelIndex& index)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
    return r->content;
}

void LogModel::onLogItemReady(int i,  QSharedPointer<LogItem> log)
{
#ifndef QT_NO_DEBUG
    qDebug() << __FUNCTION__ << i << log;
#endif
    m_logs[i] = log;
}

void LogModel::doReload()
{
    RowCountEvent* e = new RowCountEvent;
    e->m_rowCount = 0;
    createDatabase();
    QDateTime t = QDateTime::currentDateTime();
    Q_FOREACH(const QString& fileName, m_logFiles)
    {
        e->m_rowCount += copyFromFileToDatabase(fileName);
    }
    qint64 q = t.secsTo(QDateTime::currentDateTime());
    qDebug() << "loaded elapsed " << q << " s";
    QCoreApplication::postEvent(this, e);
}

void LogModel::doQuery(int offset)
{
    qDebug() << __FUNCTION__ << offset;
    QMutexLocker lock(&m_queryMutex);
    QString sqlCount;
    QString sqlFetch ;
    if (m_keyword.isEmpty())
    {
         sqlCount = "SELECT COUNT(*) FROM logs";
         sqlFetch = QString("SELECT * FROM logs ORDER BY datetime(time) DESC, line ASC LIMIT %1, 200;").arg(offset);
    }
    else
    {
        QString field = g_settings.searchField();
        sqlCount = QString("SELECT COUNT(*) FROM logs WHERE %1 LIKE '%'||?||'%'").arg(field);
        sqlFetch = QString("SELECT * FROM logs WHERE %1 LIKE '%'||?||'%' ORDER BY datetime(time) DESC, line ASC LIMIT %2, 200;").arg(field).arg(offset);
    }

    FinishedQueryEvent* e = new FinishedQueryEvent;
    e->m_offset = offset;
    e->m_size = 0;
    if (sqlFetch == m_sqlFetch && sqlCount == m_sqlCount && m_keyword.isEmpty())
    {
        QCoreApplication::postEvent(this, e);
        return;
    }
    m_sqlCount = sqlCount;
    m_sqlFetch = sqlFetch;

    QSqlDatabase db = QSqlDatabase::database(m_dbFile, true);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", m_dbFile);
        db.setDatabaseName(m_dbFile);
    }

    if (db.open())
    {
        QSqlQuery q(db);
        q.prepare(m_sqlCount);
        if (!m_keyword.isEmpty())
            q.addBindValue(m_keyword);
        if (q.exec()) {
            RowCountEvent* e = new RowCountEvent;
            e->m_rowCount = 0;
            if (q.next())
            {
                e->m_rowCount = q.value(0).toInt();
            }
            q.clear();
            q.finish();
            qDebug() << __FUNCTION__ << " query row count: " << e->m_rowCount;
            QCoreApplication::postEvent(this, e);
        }

        q.prepare(m_sqlFetch);
        if (!m_keyword.isEmpty())
            q.addBindValue(m_keyword);
        if (q.exec()) {
            int idIndex = q.record().indexOf("id");
            int dateTimeIndex = q.record().indexOf("time");
            int levelIndex = q.record().indexOf("level");
            int threadIndex = q.record().indexOf("thread");
            int sourceIndex = q.record().indexOf("source");
            int categoryIndex = q.record().indexOf("category");
            int methodIndex = q.record().indexOf("method");
            int contentIndex = q.record().indexOf("content");
            int logIndex = q.record().indexOf("log");
            int lineIndex = q.record().indexOf("line");
            while (q.next()) {
                QSharedPointer<LogItem> log =  QSharedPointer<LogItem>(new LogItem);
                log->id = q.value(idIndex).toInt();
                log->time = q.value(dateTimeIndex).toDateTime();
                log->level = q.value(levelIndex).toString();
                log->thread = q.value(threadIndex).toString();
                log->source = q.value(sourceIndex).toString();
                log->category = q.value(categoryIndex).toString();
                log->method = q.value(methodIndex).toString();
                log->content = q.value(contentIndex).toString();
                log->logFile = q.value(logIndex).toString();
                log->line = q.value(lineIndex).toInt();

                e->m_size++;
                emit logItemReady(offset++, log);
            }
            q.clear();
            q.finish();
        }
    }
    QCoreApplication::postEvent(this, e);
}

bool LogModel::event(QEvent *e)
{
    QMutexLocker lock(&m_eventMutex);

    switch (int(e->type()))
    {
    case ROWCOUNT_EVENT:
        if (m_rowCount != 0)
        {
            beginRemoveRows(QModelIndex(), 0, m_rowCount-1);
            endRemoveRows();
        }
        m_rowCount = ((RowCountEvent*)e)->m_rowCount;
        if (m_rowCount > m_totalRowCount)
            m_totalRowCount = m_rowCount;
        qDebug() << "row count event:" << m_rowCount;
        if (m_rowCount != 0)
        {
            beginInsertRows(QModelIndex(), 0, m_rowCount-1);
            endInsertRows();
        }
        return true;
    case FINISHEDQUERY_EVENT:
    {
        int offset = ((FinishedQueryEvent*)e)->m_offset;
        qDebug() << "remove offset " << offset;
        m_inQuery.removeAll(offset);

        int size = ((FinishedQueryEvent*)e)->m_size;
        emit dataChanged(index(offset,0), index(offset+size, 0));
    }
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
        query.exec("CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT,time TEXT,level TEXT,thread TEXT,source TEXT,category TEXT,method TEXT, content TEXT, log TEXT, line INTEGER);");
        query.exec("CREATE INDEX it ON logs (time, line);");
        query.exec("CREATE INDEX is ON logs (source);");
        query.exec("CREATE INDEX ic ON logs (category);");
        query.exec("CREATE INDEX im ON logs (method);");
        query.exec("CREATE INDEX io ON logs (content);");
    }
}

int LogModel::copyFromFileToDatabase(const QString &fileName)
{
    QSqlDatabase db = QSqlDatabase::database(m_dbFile, false);
    if (!db.isValid() || !db.isOpen())
    {
        qDebug() << "opening database file failed " << m_dbFile;
        return 0;
    }

    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))
    {
        qDebug() << "opening log file failed " << fileName;
        return 0;
    }

    QFileInfo fi(fileName);
    QString suffix = fi.suffix();
    QRegularExpression re("^([0-9]{4}\\-[0-9]{2}\\-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3})\\s+([A-Z]{4,5})\\s+\\[(0x[0-9a-f]{8,16})\\]\\s+\\[([0-9a-zA-Z:\\-\\_\\/\\\\\\(\\)\\.]+)\\]\\s+\\[([0-9a-zA-Z\\-\\_\\.]+)\\]\\s+\\[([0-9a-zA-Z:\\-\\_\\.]+)\\]\\s+\\-\\s+(.+)$");

    QByteArray line = f.readLine();
    int lineNo = 1;
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

    int recordCount = 0;
    int appendLine = 1;
    QSqlQuery query(db);
    db.transaction();    
    while(!f.atEnd())
    {
        QByteArray lookAhead = f.readLine();
        lineNo++;
        m = re.match(lookAhead);
        if (! m.hasMatch())
        {
            // append to last line
            content.append("\n");
            content.append(lookAhead);
            //qDebug() << "append look ahead" << QString(lookAhead);
            appendLine++;
        }
        else
        {
            //qDebug() << "save line";
            // save to database
            query.prepare("INSERT INTO logs (time, level, thread, source, category, method, content, log, line) "
                "VALUES (:time, :level, :thread, :source, :category, :method, :content, :log, :line );");
            query.bindValue(":time", dateTime);
            query.bindValue(":level", level);
            query.bindValue(":thread", thread);
            query.bindValue(":source", source);
            query.bindValue(":category", category);
            query.bindValue(":method", method);
            query.bindValue(":content", content);
            query.bindValue(":log", suffix);
            query.bindValue(":line", lineNo-appendLine);
            if (!query.exec()) {
        #ifdef _DEBUG
                qDebug() << dateTime << level << thread << source << category << method << content << " inserting log into database failed!" << query.lastError();
        #endif
            } else {
                recordCount++;
            }
            query.clear();

            appendLine = 1;
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

    return recordCount;
}
