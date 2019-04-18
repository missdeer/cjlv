#ifndef FINISHEDQUERYEVENT_H
#define FINISHEDQUERYEVENT_H

#include <QEvent>

static const QEvent::Type FINISHEDQUERY_EVENT = QEvent::Type(QEvent::User + 2);

class FinishedQueryEvent : public QEvent
{
public:
    FinishedQueryEvent() : QEvent(FINISHEDQUERY_EVENT){}
    int m_offset;
    int m_size;
};

#endif // FINISHEDQUERYEVENT_H
