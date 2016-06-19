#ifndef LOGVIEW_H
#define LOGVIEW_H

#include <QWidget>
#include <QStringList>
#include <QMutex>
#include "scintillaconfig.h"

class LogModel;

QT_BEGIN_NAMESPACE
class QTableView;
class QSplitter;
QT_END_NAMESPACE


class LogView : public QWidget
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

    void copyCurrentCell();
    void copyCurrentRow();
    void copySelectedCells();
    void copySelectedRows();

    void scrollToTop();
    void scrollToBottom();
signals:
    void filter(const QString& keyword);
protected:

private slots:
    void onDoubleClicked(const QModelIndex &index);
private:
    QSplitter* m_verticalSplitter;
    QTableView *m_tableView;
    ScintillaEdit* m_codeEditor;
    LogModel* m_model;
    QString m_path;
    QString m_extractDir;
    QMutex m_mutex;
    ScintillaConfig m_sc;
    bool event(QEvent *e) Q_DECL_OVERRIDE;

    void extract(LogView* v, const QString& fileName, const QString& dirName);
};

#endif // LOGVIEW_H
