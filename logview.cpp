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

void LogView::OpenZipBundle(const QString &path)
{
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

void LogView::OpenRawLogFile(const QStringList &paths)
{
    QString path = paths.at(0);
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}

void LogView::OpenFolder(const QString &path)
{
    QFileInfo fi(path);
    setWindowTitle(fi.fileName());
}
