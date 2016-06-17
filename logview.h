#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QTableView>
#include <QStringList>
#include <QMutex>

class LogModel;

class LogView : public QTableView
{
    Q_OBJECT
public:
    LogView(QWidget *parent = NULL);
    ~LogView();

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
    QString m_extractDir;
    QMutex m_mutex;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

    void extract(LogView* v, const QString& fileName, const QString& dirName);
};

#endif // LOGVIEW_H
