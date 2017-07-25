#include "stdafx.h"
#include "quickwidgetapi.h"

QuickWidgetAPI::QuickWidgetAPI(QObject *parent)
    : QObject(parent)
    , m_firstValue(1)
    , m_secondValue(1)
    , m_from(1)
    , m_to(1)
{

}

QuickWidgetAPI::~QuickWidgetAPI()
{

}

int QuickWidgetAPI::getSecondValue() const
{
    return m_secondValue;
}

int QuickWidgetAPI::getFirstValue() const
{
    return m_firstValue;
}

void QuickWidgetAPI::setFirstValue(int v)
{
    m_firstValue = v;
    emit valueChanged();
}

void QuickWidgetAPI::setSecondValue(int v)
{
    m_secondValue = v;
    emit valueChanged();
}

int QuickWidgetAPI::getTo() const
{
    return m_to;
}

void QuickWidgetAPI::setTo(int to)
{
    m_to = to;
}

int QuickWidgetAPI::getFrom() const
{
    return m_from;
}

void QuickWidgetAPI::setFrom(int from)
{
    m_from = from;
}
