#include <QtCore>
#include <QCoreApplication>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QtConcurrent>
#include <QVBoxLayout>
#include <QTableView>
#include <JlCompress.h>
#include "logmodel.h"
#include "logview.h"

static const QEvent::Type EXTRACTED_EVENT = QEvent::Type(QEvent::User + 1);

class ExtractedEvent : public QEvent
{
public:
    ExtractedEvent() : QEvent(EXTRACTED_EVENT) {}
};

LogView::LogView(QWidget *parent)
    :QWidget (parent)
{
    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_tableView = new QTableView(this);
    m_mainLayout->addWidget(m_tableView);
    setLayout(m_mainLayout);

    m_model = new LogModel(this);
    m_tableView->setModel(m_model);
    m_tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);

    connect(this, &LogView::filter, m_model, &LogModel::onFilter);
    connect(m_model, &LogModel::forceRepaint, this, &LogView::onForceRepaint);
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

    m_extractDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_extractDir.append("/CiscoJabberLogs/" + fi.completeBaseName());
    QDir dir(m_extractDir);
    if (!dir.exists())
        dir.mkpath(m_extractDir);
    QtConcurrent::run(this, &LogView::extract, this, path, m_extractDir);
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

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

}

void LogView::copyCurrentRow()
{

}

void LogView::copySelectedCells()
{

}

void LogView::copySelectedRows()
{

}

void LogView::onForceRepaint()
{
    QSize size = parentWidget()->parentWidget()->size();
    size.setHeight(size.height() + 1);
    parentWidget()->parentWidget()->resize(size);
    size.setHeight(size.height() - 1);
    parentWidget()->parentWidget()->resize(size);
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
