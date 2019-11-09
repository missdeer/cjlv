#ifndef PRESENCEWIDGET_H
#define PRESENCEWIDGET_H

#include <QWidget>

#include "sqlite3helper.h"

class PresenceModel;

class PresenceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PresenceWidget(QWidget *parent = 0, Sqlite3HelperPtr sqlite3Helper = nullptr);

signals:
    void databaseCreated(QString);

public slots:
    void onRefreshBuddyList();
    void onReceivedPresenceBuddyList(QStringList bl);
    void onResizeTableCells(int columns, int rows);

private:
    PresenceModel *m_model;
    QComboBox *    m_cbBuddyList;
    QTableView *   m_presenceTableView;
};

#endif // PRESENCEWIDGET_H
