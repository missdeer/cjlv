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

public slots:

private:
    PresenceModel* m_model;
};

#endif // PRESENCEWIDGET_H
