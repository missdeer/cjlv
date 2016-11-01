#ifndef PRESENCEWIDGET_H
#define PRESENCEWIDGET_H

#include <QWidget>

class PresenceModel;

class PresenceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PresenceWidget(QWidget *parent = 0);

signals:
    void databaseCreated(QString);

public slots:
    void onRefreshBuddyList();
    void onReceivedPresenceBuddyList(QStringList bl);
private:
    PresenceModel* m_model;
    QComboBox* m_cbBuddyList;
};

#endif // PRESENCEWIDGET_H
