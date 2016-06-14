#include <QtCore>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <JlCompress.h>
#include "logmodel.h"
#include "logview.h"

LogView::LogView()
{
    m_model = new LogModel(this);
    setModel(m_model);
    horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
}

LogView::~LogView()
{
    Q_FOREACH(const QString& fileName, m_extractFiles)
    {
        QFile::remove(fileName);
    }
}

void LogView::openZipBundle(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());

    QString targetDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    targetDir.append("/CiscoJabberLogs/" + fi.completeBaseName());
    QDir dir(targetDir);
    if (!dir.exists())
        dir.mkpath(targetDir);
    m_extractFiles = JlCompress::extractDir(path, targetDir);

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
