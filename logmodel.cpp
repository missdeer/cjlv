#include <QtCore>
#include <QApplication>
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
#include <QClipboard>
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
    if (m_keyword == keyword)
        return;

    // stop other query thread first
    m_stopQuerying = true;

    if (!m_queryFuture.isFinished())
    {
        qDebug() << "wait for finished";
        m_queryFuture.waitForFinished();
        m_inQuery.clear();
    }

    // then start new query
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

    m_queryFuture = QtConcurrent::run(this, &LogModel::doQuery, offset);
}

void LogModel::copyCell(const QModelIndex& cell)
{
    auto it = m_logs.find(cell.row());
    if (m_logs.end() == it)
    {
        return;
    }

    QSharedPointer<LogItem> r = *it;
    QClipboard *clipboard = QApplication::clipboard();
    switch (cell.column())
    {
    case 0:
        clipboard->setText(QString("%1").arg(r->id));
        break;
    case 1:
        clipboard->setText(r->time.toString("yyyy-MM-dd hh:mm:ss.zzz"));
        break;
    case 2:
        clipboard->setText(r->level);
        break;
    case 3:
        clipboard->setText(r->thread);
        break;
    case 4:
        clipboard->setText(r->source);
        break;
    case 5:
        clipboard->setText(r->category);
        break;
    case 6:
        clipboard->setText(r->method);
        break;
    case 7:
        clipboard->setText(r->content);
        break;
    case 8:
        clipboard->setText("." + r->logFile);
        break;
    case 9:
        clipboard->setText(QString("%1").arg(r->line));
        break;
    default:
        break;
    }
}

void LogModel::copyRow(int row)
{
    auto it = m_logs.find(row);
    if (m_logs.end() == it)
    {
        return;
    }

    QSharedPointer<LogItem> r = *it;
    QClipboard *clipboard = QApplication::clipboard();
    QString text = QString("%1 %2 [%3] [%4] [%5] [%6] - %7")
                   .arg(r->time.toString("yyyy-MM-dd hh:mm:ss.zzz"))
                   .arg(r->level)
                   .arg(r->thread)
                   .arg(r->source)
                   .arg(r->category)
                   .arg(r->method)
                   .arg(r->content);
    clipboard->setText(text);
}

void LogModel::copyCells(const QModelIndexList& cells)
{
    QString text;
    QString t;
    int lastRow = cells.at(0).row();
    Q_FOREACH(const QModelIndex& cell, cells)
    {
        auto it = m_logs.find(cell.row());
        if (m_logs.end() == it)
        {
            continue;
        }

        if (cell.row() != lastRow)
        {
            lastRow = cell.row();
            text.append(t);
            text.append("\n");
            t.clear();
        }

        qDebug() << cell;

        QSharedPointer<LogItem> r = *it;
        switch (cell.column())
        {
        case 1:
            t.append(r->time.toString("yyyy-MM-dd hh:mm:ss.zzz"));
            text.append(" ");
            break;
        case 2:
            t.append(r->level);
            text.append(" ");
            break;
        case 3:
            t.append(QString("[%1] ").arg(r->thread));
            break;
        case 4:
            t.append(QString("[%1] ").arg(r->source));
            break;
        case 5:
            t.append(QString("[%1] ").arg(r->category));
            break;
        case 6:
            t.append(QString("[%1] ").arg(r->method));
            break;
        case 7:
            t.append(QString("- %1").arg(r->content));
            break;
        default:
            break;
        }
    }
    text.append(t);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void LogModel::copyRows(const QList<int>& rows)
{
    QString text;
    Q_FOREACH(int row, rows)
    {
        auto it = m_logs.find(row);
        if (m_logs.end() == it)
        {
            continue;
        }

        QSharedPointer<LogItem> r = *it;
        QString t = QString("%1 %2 [%3] [%4] [%5] [%6] - %7\n")
                       .arg(r->time.toString("yyyy-MM-dd hh:mm:ss.zzz"))
                       .arg(r->level)
                       .arg(r->thread)
                       .arg(r->source)
                       .arg(r->category)
                       .arg(r->method)
                       .arg(r->content);
        text.append(t);
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
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
    //qDebug() << "creating database";
    createDatabase();
    //qDebug() << "created database";
    QDateTime t = QDateTime::currentDateTime();
    Q_FOREACH(const QString& fileName, m_logFiles)
    {
       // qDebug() << "copying from file " << fileName;
        e->m_rowCount += copyFromFileToDatabase(fileName);
    }
    qint64 q = t.secsTo(QDateTime::currentDateTime());
    qDebug() << "loaded elapsed " << q << " s";
    QCoreApplication::postEvent(this, e);
}

void LogModel::doQuery(int offset)
{
    //qDebug() << __FUNCTION__ << offset;
    if (!m_queryMutex.tryLock())
    {
        qDebug() << "obtaining lock failed";
        return;
    }

    struct ScopedUnlock
    {
        QMutex& m_queryMutex_;
        explicit ScopedUnlock(QMutex& queryMutex)
            : m_queryMutex_(queryMutex)
        {
        }
        ~ScopedUnlock()
        {
            m_queryMutex_.unlock();
        }
    };
    ScopedUnlock sl(m_queryMutex);

    QString sqlCount;
    QString sqlFetch ;
    if (m_keyword.isEmpty())
    {
         sqlCount = "SELECT COUNT(*) FROM logs";
         sqlFetch = QString("SELECT * FROM logs ORDER BY epoch LIMIT %1, 200;").arg(offset);
    }
    else
    {
        QString field = g_settings.searchField();
        sqlCount = QString("SELECT COUNT(*) FROM logs WHERE %1 LIKE '%'||?||'%'").arg(field);
        sqlFetch = QString("SELECT * FROM logs WHERE %1 LIKE '%'||?||'%' ORDER BY epoch LIMIT %2, 200;").arg(field).arg(offset);
    }

    FinishedQueryEvent* e = new FinishedQueryEvent;
    e->m_offset = offset;
    e->m_size = 0;

    struct ScopedEventThrow
    {
        LogModel* target_;
        FinishedQueryEvent* e_;
        ScopedEventThrow(LogModel* t, FinishedQueryEvent* e)
            : target_(t), e_(e)
        {
        }
        ~ScopedEventThrow()
        {
            QCoreApplication::postEvent(target_, e_);
        }
    };
    ScopedEventThrow se(this, e);

    if (sqlFetch == m_sqlFetch && sqlCount == m_sqlCount && m_keyword.isEmpty())
    {
        return;
    }
    m_sqlCount = sqlCount;
    m_sqlFetch = sqlFetch;

    m_stopQuerying = false;

    QSqlDatabase db = QSqlDatabase::database(m_dbFile, true);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", m_dbFile);
        db.setDatabaseName(m_dbFile);
    }

    if (db.open())
    {
        if (m_stopQuerying)
            return;
        QSqlQuery q(db);
        q.prepare(m_sqlCount);
        if (!m_keyword.isEmpty())
            q.addBindValue(m_keyword);
        if (q.exec()) {
            RowCountEvent* erc = new RowCountEvent;
            erc->m_rowCount = 0;
            if (q.next())
            {
                erc->m_rowCount = q.value(0).toInt();
            }
            q.clear();
            q.finish();
            qDebug() << __FUNCTION__ << " query row count: " << erc->m_rowCount;
            QCoreApplication::postEvent(this, erc);
        }

        if (m_stopQuerying)
            return;
        q.prepare(m_sqlFetch);
        if (!m_keyword.isEmpty())
            q.addBindValue(m_keyword);
        if (q.exec()) {
            if (m_stopQuerying)
                return;
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
            while (q.next() && !m_stopQuerying) {
                QSharedPointer<LogItem> log =  QSharedPointer<LogItem>(new LogItem);
                log->id = q.value(idIndex).toInt();
                log->time =  q.value(dateTimeIndex).toDateTime();
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
}

bool LogModel::parseLine(const QByteArray& line, QStringList& results)
{
    //QRegularExpression re("^([0-9]{4}\\-[0-9]{2}\\-[0-9]{2}\\s[0-9]{2}:[0-9]{2}:[0-9]{2},[0-9]{3})\\s+([A-Z]{4,5})\\s+\\[(0x[0-9a-f]{8,16})\\]\\s+\\[([0-9a-zA-Z:\\s\\-\\_\\/\\\\\\(\\)\\.]+)\\]\\s+\\[([0-9a-zA-Z\\-\\_\\.]+)\\]\\s+\\[([0-9a-zA-Z:~<>\\-\\_\\.]+)\\]\\s+\\-\\s+(.+)$");
//    QRegularExpression re("^([^A-Z]+)([^\\s]+)\\s+(\\[[^\\]]+\\])\\s+(\\[[^\\]]+\\])\\s+(\\[[^\\]]+\\])\\s+(\\[[^\\]]+\\])\\s+\\-\\s+(.+)$");
//    QRegularExpressionMatch m = re.match(line);
//    if (m.hasMatch())
//    {
//        results.append( m.captured(1).trimmed());
//        results.append( m.captured(2));
//        results.append( m.captured(3));
//        results.append( m.captured(4));
//        results.append( m.captured(5));
//        results.append( m.captured(6));
//        results.append( m.captured(7));
//        return true;
//    }
//    return false;

    // manual parse
    int startPos = 0;
    int endPos = line.indexOf(' ');
    if (endPos <= startPos)
        return false;
    endPos = line.indexOf(' ', endPos + 1);
    if (endPos <= startPos)
        return false;
    results.append(line.mid(startPos, endPos - startPos)); // date time

    startPos = endPos;
    while(startPos < line.length() && line.at(startPos) == ' ')
        startPos++;
    endPos = line.indexOf(' ', startPos + 1);
    if (endPos < 0 || endPos - startPos > 6 )
        return false;
    results.append(line.mid(startPos, endPos - startPos)); // level

    for (int i = 0; i < 4; i++)
    {
        startPos = line.indexOf('[', endPos);
        if (startPos < 0)
            return false;
        endPos = line.indexOf(']', startPos + 1);
        if (endPos <= startPos)
            return false;
        results.append(line.mid(startPos + 1, endPos - startPos -1)); // thread, source, category, method
    }

    results.append(line.mid(endPos + 3, -1)); // content
    return true;
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
        query.exec("CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT,epoch INTEGER, time DATETIME,level TEXT,thread TEXT,source TEXT,category TEXT,method TEXT, content TEXT, log TEXT, line INTEGER);");
        query.exec("CREATE INDEX itime ON logs (epoch);");
        query.exec("CREATE INDEX it ON logs (epoch, time);");
        query.exec("CREATE INDEX il ON logs (epoch, level);");
        query.exec("CREATE INDEX ith ON logs (epoch, thread);");
        query.exec("CREATE INDEX is ON logs (epoch, source);");
        query.exec("CREATE INDEX ic ON logs (epoch, category);");
        query.exec("CREATE INDEX im ON logs (epoch, method);");
        query.exec("CREATE INDEX io ON logs (epoch, content);");
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
    QStringList results;
    QByteArray line = f.readLine();
    int lineNo = 1;
    if (!parseLine(line, results))
    {
        // append to last line
        content.append(line);
    }
    else
    {
        dateTime = results.at(0);
        level = results.at(1);
        thread = results.at(2);
        source = results.at(3);
        category = results.at(4);
        method = results.at(5);
        content = results.at(6);
    }

    int recordCount = 0;
    int appendLine = 1;
    QSqlQuery query(db);
    db.transaction();    
    while(!f.atEnd())
    {
        QByteArray lookAhead = f.readLine();
        lineNo++;
        results.clear();
        if (!parseLine(lookAhead, results))
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
            query.prepare("INSERT INTO logs (time, epoch, level, thread, source, category, method, content, log, line) "
                "VALUES (:time, :epoch, :level, :thread, :source, :category, :method, :content, :log, :line );");
            query.bindValue(":time", dateTime);
            query.bindValue(":epoch", QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch());
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
            dateTime = results.at(0);
            level = results.at(1);
            thread = results.at(2);
            source = results.at(3);
            category = results.at(4);
            method = results.at(5);
            content = results.at(6);
            //qDebug() << "parse look ahead";
        }
    }
    db.commit();

    f.close();

    return recordCount;
}
