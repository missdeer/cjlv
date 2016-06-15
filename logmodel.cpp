#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include "logmodel.h"

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
    return 0;
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
    createDatabase();
    Q_FOREACH(const QString& fileName, m_logFiles)
    {
        copyFromFileToDatabase(fileName);
    }
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
        query.exec("CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT,time TEXT UNIQUE,level TEXT,thread TEXT,source TEXT,category TEXT,method TEXT, content TEXT);");
        query.exec("CREATE INDEX it ON logs (time);");
        query.exec("CREATE INDEX is ON logs (source);");
        query.exec("CREATE INDEX ic ON logs (category);");
        query.exec("CREATE INDEX im ON logs (method);");
        query.exec("CREATE INDEX io ON logs (content);");
    }
}

void LogModel::copyFromFileToDatabase(const QString &fileName)
{

}
