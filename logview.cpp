#include <QtCore>
#include <QCoreApplication>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QVBoxLayout>
#include <QTableView>
#include <QSplitter>
#include <QDomDocument>
#include <QProgressDialog>
#include <QTimer>
#include <QRegularExpression>
#include <JlCompress.h>
#include "settings.h"
#include "logmodel.h"
#include "logview.h"

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results);

static const QEvent::Type EXTRACTED_EVENT = QEvent::Type(QEvent::User + 1);

class ExtractedEvent : public QEvent
{
public:
    ExtractedEvent() : QEvent(EXTRACTED_EVENT) {}
};

LogView::LogView(QWidget *parent)
    : QWidget (parent)
    , m_progressDialog(nullptr)
    , m_verticalSplitter(new QSplitter( Qt::Vertical, parent))
    , m_tableView(new QTableView(m_verticalSplitter))
    , m_codeEditorTabWidget(new CodeEditorTabWidget(m_verticalSplitter))
    , m_model(new LogModel(m_tableView))
    , m_lastId(-1)
    , m_lastColumn(-1)
{
    m_verticalSplitter->addWidget(m_tableView);
    m_verticalSplitter->addWidget(m_codeEditorTabWidget);

    QList<int> sizes;
    sizes << 0x7FFFF << 0;
    m_verticalSplitter->setSizes(sizes);

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_verticalSplitter);
    setLayout(m_mainLayout);

    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);

    connect(m_tableView, &QAbstractItemView::doubleClicked, this, &LogView::onDoubleClicked);
    connect(m_model, &LogModel::dataLoaded, this, &LogView::onDataLoaded);
    connect(m_model, &LogModel::rowCountChanged, this, &LogView::onRowCountChanged);
}

LogView::~LogView()
{
    if (!m_extractDir.isEmpty())
    {
        QDir dir(m_extractDir);
        dir.removeRecursively();
    }
}

void LogView::openZipBundle(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    m_extractDir = g_settings.temporaryDirectory();
    if (m_extractDir.isEmpty())
    {
        m_extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/CiscoJabberLogs";
    }
    m_extractDir.append("/" + fi.completeBaseName());
    QDir dir(m_extractDir);
    if (!dir.exists())
        dir.mkpath(m_extractDir);

    showProgressDialog();

    QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    showProgressDialog();

    m_model->loadFromFiles(paths);
}

void LogView::openFolder(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::Time);
    QStringList filters;
    filters << "jabber.log" << "jabber.log.*";
    dir.setNameFilters(filters);

    QStringList fileNames;
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        fileNames << fileInfo.filePath();
    }

    showProgressDialog();

    m_model->loadFromFiles(fileNames);
}

bool LogView::matched(const QString &path)
{
    return path == m_path;
}

bool LogView::matched(const QStringList &paths)
{
    return paths.join(":") == m_path;
}

void LogView::copyCurrentCell()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndex i = selected->currentIndex();
    m_model->copyCell(i);
}

void LogView::copyCurrentRow()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    int row = selected->currentIndex().row();
    m_model->copyRow(row);
}

void LogView::copySelectedCells()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    m_model->copyCells(l);
}

void LogView::copySelectedRows()
{
    QItemSelectionModel* selected =  m_tableView->selectionModel();
    if (!selected->hasSelection())
        return;
    QModelIndexList l = selected->selectedIndexes();
    QList<int> rows;
    Q_FOREACH(const QModelIndex& i, l)
    {
        rows.append(i.row());
    }
    auto t = rows.toStdList();
    t.unique();
    rows = QList<int>::fromStdList(t);
    m_model->copyRows(rows);
}

void LogView::scrollToTop()
{
    m_tableView->scrollToTop();
}

void LogView::scrollToBottom()
{
    m_tableView->scrollToBottom();
}

void LogView::gotoById(int i)
{
    m_tableView->scrollTo(m_model->index(i-1, 0));
}

void LogView::reload()
{
    m_model->reload();
}

int LogView::rowCount()
{
    return m_model->rowCount();
}

void LogView::showCodeEditorPane()
{
    const QList<int>& sizes = m_verticalSplitter->sizes();
    if (sizes.length() == 2 && sizes[1] < height()/10)
    {
        QList<int> resizes;
        resizes << height()/2 <<  height()/2;
        m_verticalSplitter->setSizes(resizes);
    }
}

void LogView::filter(const QString &keyword)
{
    if (keyword.isEmpty())
    {
        // record the selected cell
        QItemSelectionModel* selected =  m_tableView->selectionModel();
        if (!selected->hasSelection())
            return;
        QModelIndex i = selected->currentIndex();
        m_lastId = m_model->getId(i);
        m_lastColumn = i.column();
    }
    else
    {
        m_lastId = -1;
        m_lastColumn = -1;
    }
    m_model->onFilter(keyword);
}

void LogView::extractContent(const QModelIndex& index)
{
    showCodeEditorPane();

    const QString& text = m_model->getLogContent(index);
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
        m_codeEditorTabWidget->setContent(header);
    }
    else
    {
        m_codeEditorTabWidget->setContent(text);
    }
}

void LogView::openSourceFile(const QModelIndex &index)
{
    showCodeEditorPane();
    const QString& source = m_model->getLogSourceFile(index);

    QRegularExpression re("([^\\(]+)\\(([0-9]+)");
    QRegularExpressionMatch m = re.match(source);
    if (m.hasMatch())
    {
        QString s = m.captured(1);

        QString fileName(s);
        QStringList fileDirs;
        int i = s.lastIndexOf('/');
        if (i >= 0)
        {
            fileDirs = s.split('/');
            fileName = s.mid(i + 1, -1);
        }
        i = s.lastIndexOf('\\');
        if (i >= 0)
        {
            fileDirs = s.split('\\');
            fileName = s.mid(i + 1, -1);
        }

        QStringList results;
        if (!QuickGetFilesByFileName(fileName, results))
        {
            //qDebug() << fileDirs << fileName;
            if (fileDirs.length() == 1 || fileDirs.isEmpty())
            {
                QString t(fileName);
                while (t.at(0) < QChar('A') || t.at(0) > QChar('Z'))
                    t = t.remove(0,1);

                //qDebug() << "try " << t;
                if (!QuickGetFilesByFileName(t, results))
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
        //qDebug() << results << fileName;

        QDir srcDir(g_settings.sourceDirectory());
        Q_FOREACH(const QString& filePath, results)
        {
            QFileInfo fi(filePath);
            if (fileDirs.length() > 1)
            {
                if (fi.fileName() != fileName)
                {
                    qDebug()<< fi.fileName() << fileName;
                    continue;
                }
            }
            QDir dir(fi.filePath());
            if (!g_settings.sourceDirectory().isEmpty())
                while (dir != srcDir)
                    if (!dir.cdUp())
                        break;
            bool matched = true;
            Q_FOREACH(const QString& n, fileDirs)
            {
                if (!filePath.contains(n))
                {
                    matched = false;
                    //qDebug() << s << "is not matched" << filePath;
                    break;
                }
            }

            //qDebug() << "got file " << filePath << dir << srcDir;
            if (matched && (g_settings.sourceDirectory().isEmpty() || (!g_settings.sourceDirectory().isEmpty() && dir == srcDir)))
            {
                m_codeEditorTabWidget->gotoLine(filePath, m.captured(2).toInt());
                break;
            }
        }
    }
}

void LogView::openLog(const QModelIndex &index)
{
    showCodeEditorPane();
    const QString& logFile = m_model->getLogFileName(index);
    m_codeEditorTabWidget->gotoLine(logFile);
}

void LogView::gotoLogLine(const QModelIndex &index)
{
    showCodeEditorPane();
    QString logFile;
    int line = m_model->getLogFileLine(index, logFile);
    m_codeEditorTabWidget->gotoLine(logFile, line);
}

void LogView::onDoubleClicked(const QModelIndex& index)
{
    switch (index.column())// the content field
    {
    case 4:
        openSourceFile(index);
        break;
    case 7:
        extractContent(index);
        break;
    case 8:
        openLog(index);
        break;
    case 9:
        gotoLogLine(index);
        break;
    default:
        break;
    }
}

void LogView::onDataLoaded()
{
    closeProgressDialog();
}

void LogView::onRowCountChanged()
{
    if (m_lastId >= 0 && m_lastColumn >= 0)
    {
        m_tableView->scrollTo(m_model->index(m_lastId -1, m_lastColumn));
        m_tableView->selectRow(m_lastId -1);
    }
    emit rowCountChanged();
}

bool LogView::event(QEvent* e)
{
    QMutexLocker lock(&m_mutex);

    switch (int(e->type()))
    {
    case EXTRACTED_EVENT:
        {
            QDir dir(m_extractDir);
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            dir.setSorting(QDir::Name);
            QStringList filters;
            filters << "jabber.log" << "jabber.log.*";
            dir.setNameFilters(filters);

            QStringList fileNames;
            QFileInfoList list = dir.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                QFileInfo fileInfo = list.at(i);
                fileNames << fileInfo.filePath();
            }

            m_model->loadFromFiles(fileNames);
        }
        return true;
    default:
        return QObject::event(e);
    }
}

void LogView::extract(LogView* v, const QString& fileName, const QString& dirName)
{
    ExtractedEvent* e = new ExtractedEvent;
    JlCompress::extractDir(fileName, dirName);
    QCoreApplication::postEvent(v, e);
}

void LogView::showProgressDialog()
{
    if (!m_progressDialog)
    {
        m_progressDialog = new QProgressDialog(this);
        m_progressDialog->setLabelText("Loading log from files...");
        m_progressDialog->setWindowModality(Qt::WindowModal);
        m_progressDialog->setAutoClose(true);
        m_progressDialog->setAutoReset(true);
        m_progressDialog->setCancelButton(nullptr);
        m_progressDialog->setRange(0,0);
        m_progressDialog->setMinimumDuration(0);
    }
    m_progressDialog->show();
    qApp->processEvents();
}

void LogView::closeProgressDialog()
{
    if (m_progressDialog)
    {
        m_progressDialog->setValue(200);
        m_progressDialog->deleteLater();
        m_progressDialog=nullptr;
    }
}
