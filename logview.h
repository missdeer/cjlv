#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QTableView>
#include <QStringList>

class LogModel;

class LogView : public QTableView
{
    Q_OBJECT
public:
    LogView();

    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path);

    bool matched(const QString& path);
    bool matched(const QStringList& paths);
protected:

private slots:

private:
    LogModel* m_model;
    QString m_path;
};

#endif // LOGVIEW_H
