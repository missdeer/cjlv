#include <QtCore>
#include <QHeaderView>
#include <QFileInfo>
#include "logmodel.h"
#include "logview.h"

LogView::LogView()
{
    m_model = new LogModel(this);
    setModel(m_model);
    horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
}

void LogView::openZipBundle(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

void LogView::openRawLogFile(const QStringList &paths)
{
    m_path = paths.join(":");
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

void LogView::openFolder(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

bool LogView::matched(const QString &path)
{
    return path == m_path;
}

bool LogView::matched(const QStringList &paths)
{
    return paths.join(":") == m_path;
}
