#include <QHeaderView>
#include <QFileInfo>
#include "logmodel.h"
#include "logview.h"

LogView::LogView()
{
    LogModel* model = new LogModel(this);
    setModel(model);
    horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
}

void LogView::OpenZipBundle(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

void LogView::OpenRawLogFile(const QStringList &paths)
{
    m_path = paths.at(0);
    QFileInfo fi(m_path);
    setWindowTitle(fi.fileName());
}

void LogView::OpenFolder(const QString &path)
{
    m_path = path;
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}
