#ifndef ROWCOUNTEVENT_H
#define ROWCOUNTEVENT_H

#include <QEvent>

static const QEvent::Type ROWCOUNT_EVENT = QEvent::Type(QEvent::User + 1);

class RowCountEvent : public QEvent
{
public:
    RowCountEvent() : QEvent(ROWCOUNT_EVENT) {}
    int m_rowCount;
};

#endif // ROWCOUNTEVENT_H
