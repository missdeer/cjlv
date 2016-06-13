#include <QHeaderView>
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
    setWindowTitle(m_path);
}

void LogView::OpenRawLogFile(const QStringList &paths)
{
    m_path = paths.at(0);
    setWindowTitle(m_path);
}

void LogView::OpenFolder(const QString &path)
{
    m_path = path;
    setWindowTitle(m_path);
}
