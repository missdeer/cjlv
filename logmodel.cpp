#include "stdafx.h"
#include <tuple>
#include <lua.hpp>
#include <boost/scope_exit.hpp>
#include "quickwidgetapi.h"
#include "settings.h"
#include "logmodel.h"

static const QEvent::Type ROWCOUNT_EVENT = QEvent::Type(QEvent::User + 1);
static const QEvent::Type FINISHEDQUERY_EVENT = QEvent::Type(QEvent::User + 2);
static const int g_rowCountLimit = 200;

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

class ReadLineFromFile
{
    QFile* m_file;
    qint64 m_offset;
    qint64 m_fileSize;
    uchar* m_lineStartPos;
    uchar* m_mapStartPos;
    uchar* m_mapEndPos;
    const qint64 mapSize = 1024 * 1024; // 1M
public:
    ReadLineFromFile(const QString& fileName)
        : m_offset(0)
    {
        m_file = new QFile(fileName);
        if (!m_file->open(QIODevice::ReadOnly))
        {
            delete m_file;
            m_file = nullptr;
        }
        else
        {
            m_fileSize = m_file->size();
            if (m_fileSize)
            {
                m_lineStartPos = m_mapStartPos = m_file->map(m_offset, qMin(mapSize, m_fileSize - m_offset));
                m_mapEndPos = m_mapStartPos + qMin(mapSize, m_fileSize - m_offset);
            }
        }
    }

    ~ReadLineFromFile()
    {
        if (m_file)
        {
            m_file->unmap(m_mapStartPos);
            m_file->close();
            delete m_file;
            m_file = nullptr;
        }
    }

    QByteArray readLine()
    {
start_read:
        uchar * p = m_lineStartPos;
        while (p != m_mapEndPos && *p != '\r' && *p != '\n')
            p++;
        if (p != m_mapEndPos)
        {
            if (*p == '\r' && (p+1) != m_mapEndPos && *(p+1) == '\n')
                p++;
            QByteArray b(reinterpret_cast<const char*>(m_lineStartPos), p - m_lineStartPos);

            m_lineStartPos = p + 1;
            return b;
        }

        // re-map
        m_file->unmap(m_mapStartPos);
        m_offset += m_lineStartPos - m_mapStartPos;
        if (m_offset != m_fileSize)
        {
            m_lineStartPos = m_mapStartPos = m_file->map(m_offset, qMin(mapSize, m_fileSize - m_offset));
            m_mapEndPos = m_mapStartPos + qMin(mapSize, m_fileSize - m_offset);
            goto start_read;
        }
        return QByteArray();
    }

    bool atEnd()
    {
        return m_offset == m_fileSize;
    }
};


void LogModel::initialize()
{
    m_forceQuerying.store(false);
    qRegisterMetaType<QSharedPointer<LogItem>>("QSharedPointer<LogItem>");
    connect(this, &LogModel::logItemReady, this, &LogModel::onLogItemReady);
    qRegisterMetaType<QMap<int, QSharedPointer<LogItem>>>("QMap<int, QSharedPointer<LogItem>>");
    connect(this, &LogModel::logItemsReady, this, &LogModel::onLogItemsReady);
}

void LogModel::postInitialize()
{
    if (m_sqlite3Helper->isDatabaseOpened())
        loadFromDatabase();
}

LogModel::LogModel(QObject *parent, Sqlite3HelperPtr sqlite3Helper, QuickWidgetAPIPtr api)
    : QAbstractTableModel(parent)
    , m_sqlite3Helper(sqlite3Helper)
    , m_api(api)
    , m_L(nullptr)
    , m_searchField("content")
    , m_searchFieldOption("content")
    , m_rowCount(0)
    , m_currentTotalRowCount(0)
    , m_maxTotalRowCount(0)
    , m_toQueryOffset(-1)
    , m_regexpMode(false)
    , m_regexpModeOption(false)
    , m_luaMode(false)
    , m_allStanza(true)
{
    initialize();
}

LogModel::~LogModel()
{
    if (m_L)
    {
        lua_close(m_L);
    }
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

    if (role == Qt::BackgroundRole && index.row() % 2 == 0)
    {
        return QVariant(QColor(0xF8, 0xF8, 0xF8));
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= m_rowCount)
        return QVariant();

    auto it = m_logs.find(index.row());
    if (m_logs.end() == it || (*it)->level.isEmpty() || (*it)->logFile.isEmpty() || (*it)->source.isEmpty() )
    {
        int alignRow = ((index.row() < 100) ? index.row() : (index.row() - 100));
#ifndef QT_NO_DEBUG
        qDebug() << "do query index:" << alignRow;
#endif
        const_cast<LogModel&>(*this).query(alignRow);
        return QVariant();
    }

    const QSharedPointer<LogItem>& r = *it;

    QMap<int, QString> m = {
        {0, QString("%1").arg(r->id)},
        {1, r->time.toString("yyyy-MM-dd hh:mm:ss.zzz")},
        {2, r->level},
        {3, r->thread},
        {4, r->source},
        {5, r->category},
        {6, r->method},
        {7, r->content},
        {8, "." % r->logFile},
        {9, QString("%1").arg(r->line)},
    };
    auto mit = m.find(index.column());
    if (mit != m.end())
    {
        return QVariant(mit.value());
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
        QMap<int, QString> m = {
            {0, tr("Id")},
            {1, tr("Time")},
            {2, tr("Level")},
            {3, tr("Thread")},
            {4, tr("Source File")},
            {5, tr("Category")},
            {6, tr("Method")},
            {7, tr("Content")},
            {8, tr("Log File")},
            {9, tr("Line")},
        };
        auto it = m.find(section);
        if (it != m.end())
            return QVariant(it.value());
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
    m_lastFilterKeyword = keyword;
    if (keyword != m_keyword)
    {
        // get regexp mode option and search field option
        m_regexpMode = m_regexpModeOption;
        m_searchField = m_searchFieldOption;
    }
    bool regexpMode = m_regexpMode;
    QString searchField = m_searchField;
    QString kw = keyword;

    QStringList sl = keyword.split(">");
    if (sl.size() >= 2)
    {
        QString prefix = sl.at(0);
        qDebug() << "prefix:" << prefix << ", sl:" << sl;
        QStringList ss = prefix.split(":");
        if (ss.size() == 1 || (ss.size() == 2 && (ss.at(1) == "r" || ss.at(1) == "!r")))
        {
            prefix = ss.at(0);
            kw = keyword.mid(keyword.indexOf(">") + 1);

            if (ss.size() == 1 && prefix == "r")
            {
                regexpMode = true;
            }
            else if (ss.size() == 1 && prefix == "!r")
            {
                regexpMode = false;
            }
            else if (ss.size() == 1 && prefix == "sql")
            {
                // empty search field so that the keyword will be treated as SQL WHERE clause
                searchField.clear();
            }
            else
            {
                std::vector< std::tuple<QString, QString>> m = {
                    { "i", "id"},
                    { "d", "datetime"},
                    { "v", "level"},
                    { "t", "thread"},
                    { "s", "source"},
                    { "a", "category"},
                    { "m", "method"},
                    { "c", "content"},
                    { "f", "logfile"},
                    { "l", "line"},
                };

                for (auto t: m)
                {
                    if (std::get<0>(t) == prefix || (prefix.size() > 1 && std::get<1>(t).startsWith(prefix)))
                    {
                        searchField = std::get<1>(t);
                        if (ss.size() == 2 && ss.at(1) == "r")
                            regexpMode = true;
                        else if (ss.size() == 2 && ss.at(1) == "!r")
                            regexpMode = false;
                        break;
                    }
                }
            }
        }
    }
    doFilter(kw.trimmed(), searchField, regexpMode, false, true);
}

void LogModel::onSearchScopeChanged()
{
    m_allStanza = (m_api->getAStanza() && m_api->getRStanza() && m_api->getXStanza() && m_api->getEnableStanza() &&
                   m_api->getEnabledStanza() && m_api->getPresenceStanza() && m_api->getMessageStanza() && m_api->getIqStanza() &&
                   m_api->getSuccessStanza() && m_api->getStreamStreamStanza() && m_api->getStreamFeaturesStanza() && m_api->getAuthStanza() &&
                   m_api->getStartTlsStanza() && m_api->getProceedStanza());

    m_fullRange = (m_api->getFirstValue() == 1 && m_api->getSecondValue() == m_maxTotalRowCount);

    QString globalScope = "logs";
    if (!m_fullRange)
    {
        m_sqlite3Helper->execDML("DROP VIEW IF EXISTS inRange;");
        QString sql = QString("CREATE VIEW inRange AS SELECT * FROM logs WHERE id>=%1 AND id<=%2;").arg(m_api->getFirstValue()).arg(m_api->getSecondValue());
        qDebug() << sql;
        m_sqlite3Helper->execDML(sql);
        globalScope = "inRange";
    }

    m_sqlite3Helper->execDML("DROP VIEW IF EXISTS allStanza");
    m_sqlite3Helper->execDML("CREATE VIEW allStanza AS SELECT * FROM " + globalScope + " WHERE content LIKE '%send:<%' OR content LIKE '%recv:<%';");
    m_sqlite3Helper->execDML("DROP VIEW IF EXISTS sentStanza");
    m_sqlite3Helper->execDML("CREATE VIEW sentStanza AS SELECT * FROM " + globalScope + " WHERE content LIKE '%send:<%';");
    m_sqlite3Helper->execDML("DROP VIEW IF EXISTS receivedStanza");
    m_sqlite3Helper->execDML("CREATE VIEW receivedStanza AS SELECT * FROM " + globalScope + " WHERE content LIKE '%recv:<%';");

    QString dataSource = "receivedStanza";

    if (m_api->getSentStanza())
        dataSource = "sentStanza";

    if (m_api->getSentStanza() && m_api->getReceivedStanza())
        dataSource = "allStanza";

    QString sql = "CREATE VIEW customStanza AS SELECT * FROM " + dataSource + " WHERE 1=0";

    struct{
        std::function<bool()> f;
        QString s;
    }c[] ={
        { std::bind(&QuickWidgetAPI::getAStanza, m_api),              " OR content LIKE '%:<a %'"},
        { std::bind(&QuickWidgetAPI::getRStanza, m_api),              " OR content LIKE '%:<r %'"},
        { std::bind(&QuickWidgetAPI::getXStanza, m_api),              " OR content LIKE '%:<x %'"},
        { std::bind(&QuickWidgetAPI::getEnableStanza, m_api),         " OR content LIKE '%:<enable %'"},
        { std::bind(&QuickWidgetAPI::getEnabledStanza, m_api),        " OR content LIKE '%:<enabled %'"},
        { std::bind(&QuickWidgetAPI::getPresenceStanza, m_api),       " OR content LIKE '%:<presence %'"},
        { std::bind(&QuickWidgetAPI::getMessageStanza, m_api),        " OR content LIKE '%:<message %'"},
        { std::bind(&QuickWidgetAPI::getIqStanza, m_api),             " OR content LIKE '%:<iq %'"},
        { std::bind(&QuickWidgetAPI::getSuccessStanza, m_api),        " OR content LIKE '%:<success %'"},
        { std::bind(&QuickWidgetAPI::getStreamStreamStanza, m_api),   " OR content LIKE '%:<stream:stream %'"},
        { std::bind(&QuickWidgetAPI::getStreamFeaturesStanza, m_api), " OR content LIKE '%:<stream:features %'"},
        { std::bind(&QuickWidgetAPI::getAuthStanza, m_api),           " OR content LIKE '%:<auth %'"},
        { std::bind(&QuickWidgetAPI::getStartTlsStanza, m_api),       " OR content LIKE '%:<starttls %'"},
        { std::bind(&QuickWidgetAPI::getProceedStanza, m_api),        " OR content LIKE '%:<proceed %'"},
    };

    for (auto a : c)
    {
        if (a.f())
            sql.append(a.s);
    }
    sql.append(";");

    qDebug() << sql;
    m_sqlite3Helper->execDML("DROP VIEW IF EXISTS customStanza;");
    m_sqlite3Helper->execDML(sql);


    m_forceQuerying.store(true);
    onFilter(m_lastFilterKeyword);
}

int LogModel::getMaxTotalRowCount() const
{
    return m_maxTotalRowCount;
}

void LogModel::setRegexpMode(bool regexpMode)
{
    m_regexpModeOption = regexpMode;
    m_regexpMode = regexpMode;
}

void LogModel::setSearchField(const QString &searchField)
{
    m_searchFieldOption = searchField;
    m_searchField = searchField;
}

void LogModel::query(int offset)
{
    auto it = std::find_if(m_inQuery.begin(), m_inQuery.end(),
                           [offset](int i){ return (offset >= i && offset < i + 200); });

    if (m_inQuery.end() != it)
    {
#ifndef QT_NO_DEBUG
        qDebug() << __FUNCTION__ << "offset:" << offset ;
#endif
        return;
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
    QMap<int, QString> m = {
        {0, QString("%1").arg(r->id)},
        {1, r->time.toString("yyyy-MM-dd hh:mm:ss.zzz")},
        {2, r->level},
        {3, r->thread},
        {4, r->source},
        {5, r->category},
        {6, r->method},
        {7, r->content},
        {8, "." % r->logFile},
        {9, QString("%1").arg(r->line)},
    };
    auto mit = m.find(cell.column());
    if (mit != m.end())
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(mit.value());
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
    for(const QModelIndex& cell : cells)
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
    for(int row : rows)
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

void LogModel::copyCellWithXMLFormatted(const QModelIndex &cell)
{
    auto it = m_logs.find(cell.row());
    if (m_logs.end() == it)
    {
        return;
    }

    QSharedPointer<LogItem> r = *it;
    QMap<int, QString> m = {
        {0, QString("%1").arg(r->id)},
        {1, r->time.toString("yyyy-MM-dd hh:mm:ss.zzz")},
        {2, r->level},
        {3, r->thread},
        {4, r->source},
        {5, r->category},
        {6, r->method},
        {7, formatXML(r->content)},
        {8, "." % r->logFile},
        {9, QString("%1").arg(r->line)},
    };
    auto mit = m.find(cell.column());
    if (mit != m.end())
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(mit.value());
    }
}

void LogModel::copyRowWithXMLFormatted(int row)
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
                   .arg(formatXML(r->content));
    clipboard->setText(text);
}

void LogModel::copyCellsWithXMLFormatted(const QModelIndexList &cells)
{
    QString text;
    QString t;
    bool contentOnly = true;
    int lastRow = cells.at(0).row();
    for(const QModelIndex& cell : cells)
    {
        if (cell.column() != 7)
            contentOnly=false;
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
            if (contentOnly)
                t = formatXML(r->content);
            else
                t.append(QString("- %1").arg(formatXML(r->content)));
            break;
        default:
            break;
        }
    }
    text.append(t);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

void LogModel::copyRowsWithXMLFormatted(const QList<int> &rows)
{
    QString text;
    for(int row : rows)
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
                       .arg(formatXML(r->content));
        text.append(t);
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
}

const QString& LogModel::getLogContent(const QModelIndex& index)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
    return r->content;
}

QString LogModel::getLogSourceFile(const QModelIndex &index)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
#if defined(Q_OS_WIN)
    return QDir::toNativeSeparators(r->source);
#else
    return r->source;
#endif
}

QString LogModel::getLogFileName(const QModelIndex &index)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
    QString fileName;
    if (r->logFile == "log")
        fileName = "jabber.log";
    else
        fileName = "jabber.log." % r->logFile;

    auto itLog = std::find_if(m_logFiles.begin(), m_logFiles.end(),
                           [&fileName](const QString& logFile) { return logFile.contains(fileName); });
    if (m_logFiles.end() != itLog)
        return *itLog;
    Q_ASSERT(0);
    return fileName;
}

int LogModel::getLogFileLine(const QModelIndex &index, QString &fileName)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
    if (r->logFile == "log")
        fileName = "jabber.log";
    else
        fileName = "jabber.log." % r->logFile;

    auto itLog = std::find_if(m_logFiles.begin(), m_logFiles.end(),
                           [&fileName](const QString& logFile) { return logFile.contains(fileName); });
    if (m_logFiles.end() != itLog)
        fileName = *itLog;
    return r->line;
}

int LogModel::getId(const QModelIndex &index)
{
    auto it = m_logs.find(index.row());
    Q_ASSERT(m_logs.end() != it);
    QSharedPointer<LogItem> r = *it;
    return r->id;
}

void LogModel::runLuaExtension(ExtensionPtr e)
{
    // load lua script
    if (!m_L)
    {
        m_L = luaL_newstate();
        luaL_openlibs(m_L);
    }
    int error = luaL_loadbuffer(m_L, e->content().toStdString().c_str(), e->content().toStdString().size(), "match") ||
            lua_pcall(m_L, 0, 0, 0);
    if (error)
    {
        QString msg(lua_tostring(m_L, -1));
        lua_pop(m_L, 1);  /* pop error message from the stack */
        QWidget* p = QApplication::desktop()->screen();
        QMessageBox::warning(p, tr("Warning"), msg, QMessageBox::Ok);
        return;
    }
    m_sqlite3Helper->setLuaState(m_L);
    // regexp mode is ignored
    doFilter(e->content(), e->field(), false, true);
}

QString LogModel::formatXML(const QString &text)
{
    // try to format XML document
    int startPos = text.indexOf(QChar('<'));
    int endPos = text.lastIndexOf(QChar('>'));
    if (startPos > 0 && endPos > startPos)
    {
        QString header = text.mid(0, startPos);
        QString xmlIn = text.mid(startPos, endPos - startPos + 1);
#ifndef QT_NO_DEBUG
        qDebug() << "raw text:" << text;
        qDebug() << "xml in:" << xmlIn;
#endif
        QString xmlOut;

        QDomDocument doc;
        doc.setContent(xmlIn, false);
        QTextStream writer(&xmlOut);
        doc.save(writer, 4);

        header.append("\n");
        header.append(xmlOut);
        if (header.length() > text.length())
            return header;
    }

    return text;
}

void LogModel::saveRowsInFolder(const QList<int> &rows, const QString &folderName)
{
    QString text;
    for(int row : rows)
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
    // save text to file
    QFile f(folderName+"/jabber.log");
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QTextStream out(&f);
        out << text;
        f.close();
    }
}

QString LogModel::getSqlWhereClause(const QModelIndex& beginAnchor, const QModelIndex& endAnchor)
{
    auto itBeginAnchor = m_logs.find(beginAnchor.row());
    Q_ASSERT(m_logs.end() != itBeginAnchor);
    QSharedPointer<LogItem> br = *itBeginAnchor;

    auto itEndAnchor = m_logs.find(endAnchor.row());
    Q_ASSERT(m_logs.end() != itEndAnchor);
    QSharedPointer<LogItem> er = *itEndAnchor;

    int beginId = qMin(br->id, er->id);
    int endId = qMax(br->id, er->id);

    return QString("id>=%1 and id<=%2").arg(beginId).arg(endId);
}

void LogModel::saveRowsBetweenAnchorsInFolder(const QModelIndex &beginAnchor, const QModelIndex &endAnchor, const QString &folderName)
{
    auto itBeginAnchor = m_logs.find(beginAnchor.row());
    Q_ASSERT(m_logs.end() != itBeginAnchor);
    QSharedPointer<LogItem> br = *itBeginAnchor;

    auto itEndAnchor = m_logs.find(endAnchor.row());
    Q_ASSERT(m_logs.end() != itEndAnchor);
    QSharedPointer<LogItem> er = *itEndAnchor;

    if (!m_queryMutex.tryLock())
    {
        qDebug() << "obtaining lock failed";
        return;
    }

    BOOST_SCOPE_EXIT(this_) {
        this_->m_queryMutex.unlock();
    } BOOST_SCOPE_EXIT_END

    QString sql = generateSQLStatement(qMin(br->id, er->id), qMax(br->id, er->id));

    bool eof = false;
    int nRet = 0;
    sqlite3_stmt* pVM = nullptr;
    do {
        pVM = m_sqlite3Helper->compile(sql);
        if (!m_keyword.isEmpty() && sql.contains(QChar('?')))
            m_sqlite3Helper->bind(pVM, 1, m_keyword);
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            QFile f(folderName+"/jabber.log");
            if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
            {
                qDebug() << "opening file " << folderName << "/jabber.log failed";
                return;
            }

            QTextStream out(&f);
            while (!eof && !m_stopQuerying.load())
            {
                QSharedPointer<LogItem> log =  QSharedPointer<LogItem>(new LogItem);
                log->id = sqlite3_column_int(pVM, 0);   // 0 - id
                                                        // 1 - epoch
                log->time = QDateTime::fromString(QString((const char *)sqlite3_column_text(pVM, 2)), Qt::ISODate); //  2 - time
                log->level = (const char *)sqlite3_column_text(pVM, 3);
                log->thread = (const char *)sqlite3_column_text(pVM, 4);
                log->source = (const char *)sqlite3_column_text(pVM, 5);
                log->category = (const char *)sqlite3_column_text(pVM, 6);
                log->method = (const char *)sqlite3_column_text(pVM, 7);
                log->content = (const char *)sqlite3_column_text(pVM, 8);
                log->logFile = (const char *)sqlite3_column_text(pVM, 9);
                log->line = sqlite3_column_int(pVM, 10);

                if (log->level.isEmpty() || log->source.isEmpty() || log->logFile.isEmpty())
                {
                    qDebug() << "empty record, quit iterating now";
                    break;
                }

                // save to file
                out << QString("%1 %2 [%3] [%4] [%5] [%6] - %7\n")
                       .arg(log->time.toString("yyyy-MM-dd hh:mm:ss.zzz"))
                       .arg(log->level)
                       .arg(log->thread)
                       .arg(log->source)
                       .arg(log->category)
                       .arg(log->method)
                       .arg(log->content);
            }

            f.close();
            break;
        }
    }while(nRet == SQLITE_SCHEMA);
}

bool LogModel::getStatistic(const QString &tableName, QList<QSharedPointer<StatisticItem>> &sis)
{
    int limitCount = 15;
    for(int count = 0;;limitCount++)
    {
        bool eof = false;
        int nRet = 0;
        sqlite3_stmt* pVM = nullptr;
        QString sql = QString("SELECT * FROM %1 ORDER BY count DESC LIMIT %2;").arg(tableName).arg(limitCount);

        do {
            pVM = m_sqlite3Helper->compile(sql);
            nRet = m_sqlite3Helper->execQuery(pVM, eof);
            if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
            {
                count = 0;
                sis.clear();
                while (!eof)
                {
                    QSharedPointer<StatisticItem> si =  QSharedPointer<StatisticItem>(new StatisticItem);
                    si->content =(const char *)sqlite3_column_text(pVM, 1);
                    si->count = sqlite3_column_int(pVM, 2);
                    si->percent = ((double)si->count * 100)/((double)m_currentTotalRowCount);
                    sis.append(si);
                    count += si->count;
                    m_sqlite3Helper->nextRow(pVM, eof);
                }
                if (count * 3 <= m_currentTotalRowCount * 2)
                    break;

                if (count < m_currentTotalRowCount)
                {
                    QSharedPointer<StatisticItem> si =  QSharedPointer<StatisticItem>(new StatisticItem);
                    si->content = "Other";
                    si->count =  m_currentTotalRowCount - count;
                    si->percent = ((double)si->count * 100)/((double)m_currentTotalRowCount);
                    sis.append(si);
                }
                return true;
            } else {
                return !sis.empty();
            }
        }while(nRet == SQLITE_SCHEMA);
    }

    return false;
}

void LogModel::saveStatistic()
{
    m_sqlite3Helper->beginTransaction();
    for (auto i = m_levelCountMap.constBegin();
         i != m_levelCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO level_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting level_statistic into database failed!";
#endif
        }
    }
    m_levelCountMap.clear();

    for (auto i = m_threadCountMap.constBegin();
         i != m_threadCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO thread_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting thread_statistic into database failed!";
#endif
        }
    }
    m_threadCountMap.clear();

    for (auto i = m_sourceFileCountMap.constBegin();
         i != m_sourceFileCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO source_file_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting source_file_statistic into database failed!";
#endif
        }
    }
    m_sourceFileCountMap.clear();

    for (auto i = m_sourceLineCountMap.constBegin();
         i != m_sourceLineCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO source_line_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting source_line_statistic into database failed!";
#endif
        }
    }
    m_sourceLineCountMap.clear();

    for (auto i = m_categoryCountMap.constBegin();
         i != m_categoryCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO category_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting category_statistic into database failed!";
#endif
        }
    }
    m_categoryCountMap.clear();

    for (auto i = m_methodCountMap.constBegin();
         i != m_methodCountMap.constEnd();
         ++i)
    {
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO method_statistic (key, count) VALUES (:key, :count);");
        m_sqlite3Helper->bind(pVM, ":key", i.key());
        m_sqlite3Helper->bind(pVM, ":count", i.value());
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << " inserting method_statistic into database failed!";
#endif
        }
    }
    m_methodCountMap.clear();
    m_sqlite3Helper->endTransaction();
}

QString LogModel::getDataSource()
{
    if (!m_api->getStanzaOnly())
        return (m_fullRange ? "logs" : "inRange");

    if (!m_allStanza)
        return "customStanza";

    if (m_api->getSentStanza() && m_api->getReceivedStanza())
        return "allStanza";

    if (m_api->getSentStanza())
        return "sentStanza";

    return "receivedStanza";
}

bool LogModel::getLevelStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("level_statistic", sis);
}

bool LogModel::getThreadStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("thread_statistic", sis);
}

bool LogModel::getSourceFileStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("source_file_statistic", sis);
}

bool LogModel::getSourceLineStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("source_line_statistic", sis);
}

bool LogModel::getCategoryStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("category_statistic", sis);
}

bool LogModel::getMethodStatistic(QList<QSharedPointer<StatisticItem>> &sis)
{
    return getStatistic("method_statistic", sis);
}

void LogModel::onLogItemReady(int i,  QSharedPointer<LogItem> log)
{
#ifndef QT_NO_DEBUG
    qDebug() << __FUNCTION__ << i << log;
#endif
    m_logs[i] = log;
}

void LogModel::onLogItemsReady(QMap<int, QSharedPointer<LogItem> > logs)
{    
    for (auto i = logs.constBegin();i != logs.constEnd(); ++i)
    {
        m_logs[i.key()] = i.value();        
    }
}

void LogModel::doReload()
{
    RowCountEvent* e = new RowCountEvent;
    e->m_rowCount = 0;
    createDatabase();
    QDateTime t = QDateTime::currentDateTime();

    std::for_each(m_logFiles.rbegin(), m_logFiles.rend(),
                  [&](const QString& log) { e->m_rowCount += copyFromFileToDatabase(log); });
    m_currentTotalRowCount = e->m_rowCount;
    if (m_maxTotalRowCount < m_currentTotalRowCount)
    {
        m_maxTotalRowCount = m_currentTotalRowCount;

        connect(m_api.data(), &QuickWidgetAPI::sentStanzaChanged,           this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::receivedStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::aStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::rStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::xStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::presenceStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::enableStanzaChanged,         this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::enabledStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::messageStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::iqStanzaChanged,             this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::successStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::streamStreamStanzaChanged,   this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::streamFeaturesStanzaChanged, this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::authStanzaChanged,           this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::proceedStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::startTlsStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::valueChanged,                this, &LogModel::onSearchScopeChanged);
    }
    saveStatistic();

    qint64 q = t.secsTo(QDateTime::currentDateTime());
    createDatabaseIndex();
    qDebug() << "loaded elapsed " << q << " s";
    QCoreApplication::postEvent(this, e);
    emit dataLoaded();
}

void LogModel::loadFromDatabase()
{
    RowCountEvent* e = new RowCountEvent;
    e->m_rowCount = 0;

    // read records count from database
    bool eof = false;
    int nRet = 0;
    sqlite3_stmt* pVM = nullptr;
    do {
        pVM = m_sqlite3Helper->compile("SELECT COUNT(*) FROM logs;");
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            while (!eof)
            {
                e->m_rowCount = sqlite3_column_int(pVM, 0);
                m_sqlite3Helper->nextRow(pVM, eof);
            }
            break;
        }
    }while(nRet == SQLITE_SCHEMA);

    m_currentTotalRowCount = e->m_rowCount;
    if (m_maxTotalRowCount < m_currentTotalRowCount)
    {
        m_maxTotalRowCount = m_currentTotalRowCount;

        connect(m_api.data(), &QuickWidgetAPI::sentStanzaChanged,           this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::receivedStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::aStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::rStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::xStanzaChanged,              this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::presenceStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::enableStanzaChanged,         this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::enabledStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::messageStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::iqStanzaChanged,             this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::successStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::streamStreamStanzaChanged,   this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::streamFeaturesStanzaChanged, this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::authStanzaChanged,           this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::proceedStanzaChanged,        this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::startTlsStanzaChanged,       this, &LogModel::onSearchScopeChanged);
        connect(m_api.data(), &QuickWidgetAPI::valueChanged,                this, &LogModel::onSearchScopeChanged);
    }

    QCoreApplication::postEvent(this, e);
    emit dataLoaded();
}

void LogModel::generateSQLStatements(int offset, QString &sqlFetch, QString &sqlCount)
{    
    //qWarning() << __FUNCTION__ << m_keyword << m_searchField << m_regexpMode;
    qDebug() << "data source:" << getDataSource();
    if (g_settings->allLogLevelEnabled() || m_searchField == "level")
    {
        if (m_luaMode)
        {
            // lua match
            sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 MATCH 'dummy'").arg(m_searchField);
            sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 MATCH 'dummy' ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
            return;
        }

        if (m_keyword.isEmpty())
        {
            // no search, no filter
            sqlCount = "SELECT COUNT(*) FROM " + getDataSource();
            sqlFetch = QString("SELECT * FROM " + getDataSource() + " ORDER BY epoch LIMIT %1, 200;").arg(offset);
            return;
        }

        if (m_searchField.isEmpty())
        {
            // SQL WHERE clause extension
            sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1").arg(m_keyword);
            sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 ORDER BY epoch LIMIT %2, 200;").arg(m_keyword).arg(offset);
            return;
        }

        if (m_regexpMode)
        {
            // regexp filter
            sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 REGEXP ?").arg(m_searchField);
            sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 REGEXP ? ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
            return;
        }

        if (m_searchField == "datetime")
        {
            QString begin = "1970-01-01 00:00:00,000";
            QString end = "1970-12-31 23:59:59,999";
            if (m_keyword.length() < begin.length())
            {
                begin.replace(0, m_keyword.length(), m_keyword);
                end.replace(0, m_keyword.length(), m_keyword);
                sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2")
                        .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                        .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch());
                sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2 ORDER BY epoch LIMIT %3, 200;")
                        .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                        .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                        .arg(offset);
                return;
            }
        }

        // simple keyword, SQL LIKE fitler
        sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%'").arg(m_searchField);
        sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%' ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
        return;
    }

    if (m_luaMode)
    {
        // lua match
        sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 MATCH 'dummy' AND level GLOB 'dummy'").arg(m_searchField);
        sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 MATCH 'dummy' AND level GLOB 'dummy' ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
        return;
    }

    if (m_keyword.isEmpty())
    {
        // no search, no filter
        sqlCount = "SELECT COUNT(*) FROM " + getDataSource() + " WHERE level GLOB 'dummy'";
        sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE level GLOB 'dummy' ORDER BY epoch LIMIT %1, 200;").arg(offset);
        return;
    }

    if (m_searchField.isEmpty())
    {
        // SQL WHERE clause extension
        sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 AND level GLOB 'dummy'").arg(m_keyword);
        sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND level GLOB 'dummy' ORDER BY epoch LIMIT %2, 200;").arg(m_keyword).arg(offset);
        return;
    }

    if (m_regexpMode)
    {
        // regexp filter
        sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 REGEXP ? AND level GLOB 'dummy'").arg(m_searchField);
        sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 REGEXP ? AND level GLOB 'dummy' ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
        return;
    }


    if (m_searchField == "datetime")
    {
        QString begin = "1970-01-01 00:00:00,000";
        QString end = "1970-12-31 23:59:59,999";
        if (m_keyword.length() < begin.length())
        {
            begin.replace(0, m_keyword.length(), m_keyword);
            end.replace(0, m_keyword.length(), m_keyword);
            sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2 AND level GLOB 'dummy'")
                    .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                    .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch());
            sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2 AND level GLOB 'dummy' ORDER BY epoch LIMIT %3, 200;")
                    .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                    .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                    .arg(offset);
            return;
        }
    }

    // simple keyword, SQL LIKE fitler
    sqlCount = QString("SELECT COUNT(*) FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%' AND level GLOB 'dummy'").arg(m_searchField);
    sqlFetch = QString("SELECT * FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%' AND level GLOB 'dummy' ORDER BY epoch LIMIT %2, 200;").arg(m_searchField).arg(offset);
}

QString LogModel::generateSQLStatement(int from, int to)
{
    if (g_settings->allLogLevelEnabled() || m_searchField == "level")
    {
        if (m_luaMode)
        {
            // lua match
            return QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND id >= %2 AND id <= %3 MATCH 'dummy' ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
        }

        if (m_keyword.isEmpty())
        {
            // no search, no filter
            return QString("SELECT * FROM " + getDataSource() + " WHERE id >= %1 AND id <= %2 ORDER BY epoch LIMIT 400000;").arg(from).arg(to);
        }

        if (m_searchField.isEmpty())
        {
            // SQL WHERE clause extension
            return QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND id >= %2 AND id <= %3 ORDER BY epoch LIMIT 400000;").arg(m_keyword).arg(from).arg(to);
        }

        if (m_regexpMode)
        {
            // regexp filter
            return QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND id >= %2 AND id <= %3 REGEXP ? ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
        }

        if (m_searchField == "datetime")
        {
            QString begin = "1970-01-01 00:00:00,000";
            QString end = "1970-12-31 23:59:59,999";
            if (m_keyword.length() < begin.length())
            {
                begin.replace(0, m_keyword.length(), m_keyword);
                end.replace(0, m_keyword.length(), m_keyword);
                return QString("SELECT * FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2 AND id >= %3 AND id <= %4 ORDER BY epoch LIMIT 400000;")
                        .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                        .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                        .arg(from).arg(to);;
            }
        }
        // simple keyword, SQL LIKE fitler
        return QString("SELECT * FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%' AND id >= %2 AND id <= %3 ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
    }

    if (m_luaMode)
    {
        // lua match
        return QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND id >= %2 AND id <= %3 MATCH 'dummy' AND level GLOB 'dummy' ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
    }

    if (m_keyword.isEmpty())
    {
        // no search, no filter
        return QString("SELECT * FROM " + getDataSource() + " WHERE level GLOB 'dummy' AND id >= %1 AND id <= %2 ORDER BY epoch LIMIT 400000;").arg(from).arg(to);
    }

    if (m_searchField.isEmpty())
    {
        // SQL WHERE clause extension
        return QString("SELECT * FROM " + getDataSource() + " WHERE %1 AND level GLOB 'dummy' AND id >= %2 AND id <= %3 ORDER BY epoch LIMIT 400000;").arg(m_keyword).arg(from).arg(to);
    }

    if (m_regexpMode)
    {
        // regexp filter
        return QString("SELECT * FROM " + getDataSource() + " WHERE %1 REGEXP ? AND level GLOB 'dummy' AND id >= %2 AND id <= %3 ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
    }

    if (m_searchField == "datetime")
    {
        QString begin = "1970-01-01 00:00:00,000";
        QString end = "1970-12-31 23:59:59,999";
        if (m_keyword.length() < begin.length())
        {
            begin.replace(0, m_keyword.length(), m_keyword);
            end.replace(0, m_keyword.length(), m_keyword);
            return QString("SELECT * FROM " + getDataSource() + " WHERE epoch >= %1 AND epoch <= %2 AND level GLOB 'dummy' AND id >= %3 AND id <= %4 ORDER BY epoch LIMIT 400000;")
                    .arg(QDateTime::fromString(begin,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                    .arg(QDateTime::fromString(end,"yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch())
                    .arg(from).arg(to);;
        }
    }
    // simple keyword, SQL LIKE fitler
    return QString("SELECT * FROM " + getDataSource() + " WHERE %1 LIKE '%'||?||'%' AND level GLOB 'dummy' AND id >= %2 AND id <= %3 ORDER BY epoch LIMIT 400000;").arg(m_searchField).arg(from).arg(to);
}

void LogModel::doFilter(const QString &content, const QString &field, bool regexpMode, bool luaMode, bool saveOptions)
{
    if (m_keyword == content && !m_forceQuerying.load())
        return;

    // stop other query thread first
    m_stopQuerying.store(true);

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
        endRemoveRows();
    }

    m_luaMode = luaMode;
    bool regexpModeBackup = m_regexpMode;
    m_regexpMode = regexpMode;
    QString searchFieldBackup = m_searchField;
    m_searchField = field;
    m_keyword = content;
    qWarning() << __FUNCTION__ << content << field << regexpMode << m_keyword << m_searchField << m_regexpMode;
    query(0);

    QMutexLocker l(&m_dataMemberMutex);
    m_dataMemberCondition.wait(&m_dataMemberMutex, 500);
    if (!saveOptions)
    {
        m_regexpMode = regexpModeBackup;
        m_searchField = searchFieldBackup;
    }
}

void LogModel::doQuery(int offset)
{
    if (!m_queryMutex.tryLock())
    {
        qDebug() << "obtaining lock failed";
        m_toQueryOffset = offset;
        return;
    }

    BOOST_SCOPE_EXIT(this_) {
        this_->m_queryMutex.unlock();
    } BOOST_SCOPE_EXIT_END

    QString sqlCount;
    QString sqlFetch ;
    generateSQLStatements(offset, sqlFetch, sqlCount);
    m_dataMemberCondition.wakeAll();

    FinishedQueryEvent* e = new FinishedQueryEvent;
    e->m_offset = offset;
    e->m_size = 0;

    BOOST_SCOPE_EXIT(this_, e) {
        QCoreApplication::postEvent(this_, e);
    } BOOST_SCOPE_EXIT_END

    if (sqlFetch == m_sqlFetch && sqlCount == m_sqlCount && m_keyword.isEmpty() && !m_forceQuerying)
    {
        qDebug() << "search condition not changed";
        return;
    }
    m_sqlCount = sqlCount;
    m_sqlFetch = sqlFetch;

    qDebug() << __FUNCTION__ <<  m_sqlCount << m_sqlFetch << m_keyword << m_searchField;

    m_stopQuerying.store(false);

    if (m_stopQuerying.load())
        return;

    bool eof = false;
    int nRet = 0;
    sqlite3_stmt* pVM = nullptr;
    do {
        pVM = m_sqlite3Helper->compile(m_sqlCount);
        if (!m_keyword.isEmpty() && m_sqlCount.contains(QChar('?')))
            m_sqlite3Helper->bind(pVM, 1, m_keyword);
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            RowCountEvent* erc = new RowCountEvent;
            erc->m_rowCount = 0;
            while (!eof)
            {
                erc->m_rowCount = sqlite3_column_int(pVM, 0);
                m_sqlite3Helper->nextRow(pVM, eof);
            }

            qDebug() << __FUNCTION__ << " query row count: " << erc->m_rowCount;
            QCoreApplication::postEvent(this, erc);
            break;
        }
    }while(nRet == SQLITE_SCHEMA);

    if (m_stopQuerying.load())
        return;

    eof = false;
    nRet = 0;
    pVM = nullptr;
    do {
        pVM = m_sqlite3Helper->compile(m_sqlFetch);
        if (!m_keyword.isEmpty() && m_sqlFetch.contains(QChar('?')))
            m_sqlite3Helper->bind(pVM, 1, m_keyword);
        nRet = m_sqlite3Helper->execQuery(pVM, eof);
        if (nRet == SQLITE_DONE || nRet == SQLITE_ROW)
        {
            if (m_stopQuerying.load())
                return;
            QMap<int, QSharedPointer<LogItem>> logs;
            int logItemCount = 0;
            while (!eof && !m_stopQuerying.load())
            {
                QSharedPointer<LogItem> log =  QSharedPointer<LogItem>(new LogItem);
                log->id = sqlite3_column_int(pVM, 0);   // 0 - id
                                                        // 1 - epoch
                log->time = QDateTime::fromString(QString((const char *)sqlite3_column_text(pVM, 2)), Qt::ISODate); //  2 - time
                log->level = (const char *)sqlite3_column_text(pVM, 3);
                log->thread = (const char *)sqlite3_column_text(pVM, 4);
                log->source = (const char *)sqlite3_column_text(pVM, 5);
                log->category = (const char *)sqlite3_column_text(pVM, 6);
                log->method = (const char *)sqlite3_column_text(pVM, 7);
                log->content = (const char *)sqlite3_column_text(pVM, 8);
                log->logFile = (const char *)sqlite3_column_text(pVM, 9);
                log->line = sqlite3_column_int(pVM, 10);

                if (log->level.isEmpty() || log->source.isEmpty() || log->logFile.isEmpty())
                {
                    qDebug() << "empty record, quit iterating now";
                    break;
                }
                logs[ offset ] = log;
                offset++;

                e->m_size++;
                logItemCount++;
                if (logItemCount == g_rowCountLimit)
                {
                    emit logItemsReady(logs);
                    logs.clear();
                    logItemCount = 0;
                }
                m_sqlite3Helper->nextRow(pVM, eof);
            }
            if (logItemCount)
            {
                emit logItemsReady(logs);
                logs.clear();
                logItemCount = 0;
            }

            // reset force querying flag finally
            m_forceQuerying.store(false);
            break;
        }
    }while(nRet == SQLITE_SCHEMA);
}

bool LogModel::parseLine(const QString& line, QStringList& results)
{
    // manual parse
    int startPos = 0;
    int endPos = line.indexOf(' ');
    if (endPos <= startPos)
        return false;
    endPos = line.indexOf(' ', endPos + 1);
    if (endPos - startPos != 23) // yyyy-MM-dd hh:mm:ss,zzz  23 characters
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

    auto it = m_levelCountMap.find(results.at(1));
    if (m_levelCountMap.end() == it)
        m_levelCountMap.insert(results.at(1), 1);
    else
       (*it)++;

    it = m_threadCountMap.find(results.at(2));
    if (m_threadCountMap.end() == it)
        m_threadCountMap.insert(results.at(2), 1);
    else
        (*it)++;

    it = m_sourceLineCountMap.find(results.at(3));
    if (m_sourceLineCountMap.end() == it)
        m_sourceLineCountMap.insert(results.at(3), 1);
    else
        (*it)++;

    QString sourceFile = results.at(3);
    int index = sourceFile.indexOf(QChar('('));
    if (index > 0)
        sourceFile.remove(index, sourceFile.length() - index);
    it = m_sourceFileCountMap.find(sourceFile);
    if (m_sourceFileCountMap.end() == it)
        m_sourceFileCountMap.insert(sourceFile, 1);
    else
        (*it)++;

    it = m_categoryCountMap.find(results.at(4));
    if (m_categoryCountMap.end() == it)
        m_categoryCountMap.insert(results.at(4), 1);
    else
        (*it)++;

    it = m_methodCountMap.find(results.at(5));
    if (m_methodCountMap.end() == it)
        m_methodCountMap.insert(results.at(5), 1);
    else
        (*it)++;
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
        m_rowCount = dynamic_cast<RowCountEvent*>(e)->m_rowCount;
        qDebug() << "row count event:" << m_rowCount;
        if (m_rowCount != 0)
        {
            beginInsertRows(QModelIndex(), 0, m_rowCount-1);
            endInsertRows();
        }
        emit rowCountChanged();
        return true;
    case FINISHEDQUERY_EVENT:
    {
        int offset = dynamic_cast<FinishedQueryEvent*>(e)->m_offset;
        qDebug() << "remove offset " << offset;
        m_inQuery.removeAll(offset);

        int size = dynamic_cast<FinishedQueryEvent*>(e)->m_size;
        emit dataChanged(index(offset,0), index(offset+size, 0));

        if (m_toQueryOffset >= 0)
        {
            offset = m_toQueryOffset;
            m_toQueryOffset = -1;
            query(offset);
        }
    }
        return true;
    default:
        break;
    }

    return QObject::event(e);
}

void LogModel::createDatabaseIndex()
{
    m_sqlite3Helper->execDML("CREATE INDEX indexepoch ON logs (epoch);");
    m_sqlite3Helper->execDML("CREATE INDEX indextime ON logs ( time);");
    m_sqlite3Helper->execDML("CREATE INDEX indexlevel ON logs ( level);");
    m_sqlite3Helper->execDML("CREATE INDEX indexthread ON logs ( thread);");
    m_sqlite3Helper->execDML("CREATE INDEX indexsource ON logs ( source);");
    m_sqlite3Helper->execDML("CREATE INDEX indexcategory ON logs ( category);");
    m_sqlite3Helper->execDML("CREATE INDEX indexmethod ON logs ( method);");
    m_sqlite3Helper->execDML("CREATE INDEX indexcontent ON logs ( content);");
}

void LogModel::createDatabase()
{
    m_dbFile = g_settings->temporaryDirectory();
    if (m_dbFile.isEmpty())
    {
        m_dbFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        m_dbFile.append("/CiscoJabberLogs/database");
    }
    QDir dir(m_dbFile);
    if (!dir.exists())
        dir.mkpath(m_dbFile);
    m_dbFile.append("/");
    m_dbFile.append(QDateTime::currentDateTime().toString("ddd MMMM d yyyy hh-mm-ss.zzz"));
    m_dbFile.append(".db");

    QFile::remove(m_dbFile);

    if (!m_sqlite3Helper->createDatabase(m_dbFile))
    {
        return;
    }

    emit databaseCreated(m_dbFile);

    m_sqlite3Helper->execDML("CREATE TABLE logs(id INTEGER PRIMARY KEY AUTOINCREMENT,epoch INTEGER, time DATETIME,level TEXT,thread TEXT,source TEXT,category TEXT,method TEXT, content TEXT, log TEXT, line INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE level_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE thread_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE source_file_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE source_line_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE category_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
    m_sqlite3Helper->execDML("CREATE TABLE method_statistic(id INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, count INTEGER);");
}

int LogModel::copyFromFileToDatabase(const QString &fileName)
{
    ReadLineFromFile f(fileName);
    if (f.atEnd())
    {
        qDebug() << "opening log file failed " << fileName;
        return 0;
    }

    QFileInfo fi(fileName);
    QString suffix = fi.suffix();
    QStringList results;
    QString line = f.readLine();
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
    bool pendingRecord = false;

    m_sqlite3Helper->execDML("PRAGMA synchronous = OFF");
    m_sqlite3Helper->execDML("PRAGMA journal_mode = MEMORY");
    m_sqlite3Helper->beginTransaction();
    while(!f.atEnd())
    {
        QString lookAhead = f.readLine();
        lineNo++;
        results.clear();
        if (!parseLine(lookAhead, results))
        {
            // append to last line
            content.append(lookAhead);
            appendLine++;
        }
        else
        {
            // save to database
            sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO logs (time, epoch, level, thread, source, category, method, content, log, line) "
                "VALUES (:time, :epoch, :level, :thread, :source, :category, :method, :content, :log, :line );");
            m_sqlite3Helper->bind(pVM, ":time", dateTime);
            m_sqlite3Helper->bind(pVM, ":epoch", QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch());
            m_sqlite3Helper->bind(pVM, ":level", level);
            m_sqlite3Helper->bind(pVM, ":thread", thread);
            m_sqlite3Helper->bind(pVM, ":source", source);
            m_sqlite3Helper->bind(pVM, ":category", category);
            m_sqlite3Helper->bind(pVM, ":method", method);
            m_sqlite3Helper->bind(pVM, ":content", content);
            m_sqlite3Helper->bind(pVM, ":log", suffix);
            m_sqlite3Helper->bind(pVM, ":line", lineNo - appendLine);
            if (!m_sqlite3Helper->execDML(pVM)) {
        #ifndef QT_NO_DEBUG
                qDebug() << dateTime << level << thread << source << category << method << content << " inserting log into database failed!";
        #endif
            } else {
                recordCount++;
            }

            appendLine = 1;
            // parse lookAhead
            dateTime = results.at(0);
            level = results.at(1);
            thread = results.at(2);
            source = results.at(3);
            category = results.at(4);
            method = results.at(5);
            content = results.at(6);
            pendingRecord = true;
        }
    }
    if (pendingRecord)
    {
        // save the last record to database
        sqlite3_stmt* pVM = m_sqlite3Helper->compile("INSERT INTO logs (time, epoch, level, thread, source, category, method, content, log, line) "
            "VALUES (:time, :epoch, :level, :thread, :source, :category, :method, :content, :log, :line );");
        m_sqlite3Helper->bind(pVM, ":time", dateTime);
        m_sqlite3Helper->bind(pVM, ":epoch", QDateTime::fromString(dateTime, "yyyy-MM-dd hh:mm:ss,zzz").toMSecsSinceEpoch());
        m_sqlite3Helper->bind(pVM, ":level", level);
        m_sqlite3Helper->bind(pVM, ":thread", thread);
        m_sqlite3Helper->bind(pVM, ":source", source);
        m_sqlite3Helper->bind(pVM, ":category", category);
        m_sqlite3Helper->bind(pVM, ":method", method);
        m_sqlite3Helper->bind(pVM, ":content", content);
        m_sqlite3Helper->bind(pVM, ":log", suffix);
        m_sqlite3Helper->bind(pVM, ":line", lineNo +1 - appendLine);
        if (!m_sqlite3Helper->execDML(pVM)) {
#ifndef QT_NO_DEBUG
            qDebug() << dateTime << level << thread << source << category << method << content << " inserting log into database failed!" ;
#endif
        }
    }
    m_sqlite3Helper->endTransaction();

    return recordCount;
}
