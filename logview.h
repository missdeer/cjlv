#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QTableView>
#include <QStringList>

class LogView : public QTableView
{
    Q_OBJECT
public:
    LogView();

    void OpenZipBundle(const QString& path);
    void OpenRawLogFile(const QStringList& paths);
    void OpenFolder(const QString& path);
protected:

private slots:

private:
    QString m_path;
};

#endif // LOGVIEW_H
