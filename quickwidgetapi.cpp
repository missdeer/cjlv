#include "stdafx.h"
#include "quickwidgetapi.h"

QuickWidgetAPI::QuickWidgetAPI(QObject *parent)
    : QObject(parent)
    , m_firstValue(1)
    , m_secondValue(1)
    , m_from(1)
    , m_to(1)
    , m_stanzaOnly(false)
    , m_receivedStanza(true)
    , m_sentStanza(true)
    , m_xStanza(true)
    , m_rStanza(true)
    , m_aStanza(true)
    , m_enabledStanza(true)
    , m_enableStanza(true)
    , m_presenceStanza(true)
    , m_messageStanza(true)
    , m_iqStanza(true)
    , m_successStanza(true)
    , m_streamStreamStanza(true)
    , m_streamFeaturesStanza(true)
    , m_authStanza(true)
    , m_startTlsStanza(true)
    , m_proceedStanza(true)
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

bool QuickWidgetAPI::getStanzaOnly() const
{
    return m_stanzaOnly;
}

void QuickWidgetAPI::setStanzaOnly(bool stanzaOnly)
{
    m_stanzaOnly = stanzaOnly;
    emit stanzaOnlyChanged();
}

bool QuickWidgetAPI::getReceivedStanza() const
{
    return m_receivedStanza;
}

void QuickWidgetAPI::setReceivedStanza(bool receivedStanza)
{
    m_receivedStanza = receivedStanza;
    emit receivedStanzaChanged();
}

bool QuickWidgetAPI::getSentStanza() const
{
    return m_sentStanza;
}

void QuickWidgetAPI::setSentStanza(bool sentStanza)
{
    m_sentStanza = sentStanza;
    emit sentStanzaChanged();
}

bool QuickWidgetAPI::getXStanza() const
{
    return m_xStanza;
}

void QuickWidgetAPI::setXStanza(bool xStanza)
{
    m_xStanza = xStanza;
    emit xStanzaChanged();
}

bool QuickWidgetAPI::getRStanza() const
{
    return m_rStanza;
}

void QuickWidgetAPI::setRStanza(bool rStanza)
{
    m_rStanza = rStanza;
    emit rStanzaChanged();
}

bool QuickWidgetAPI::getAStanza() const
{
    return m_aStanza;
}

void QuickWidgetAPI::setAStanza(bool aStanza)
{
    m_aStanza = aStanza;
    emit aStanzaChanged();
}

bool QuickWidgetAPI::getEnabledStanza() const
{
    return m_enabledStanza;
}

void QuickWidgetAPI::setEnabledStanza(bool enabledStanza)
{
    m_enabledStanza = enabledStanza;
    emit enabledStanzaChanged();
}

bool QuickWidgetAPI::getEnableStanza() const
{
    return m_enableStanza;
}

void QuickWidgetAPI::setEnableStanza(bool enableStanza)
{
    m_enableStanza = enableStanza;
    emit enableStanzaChanged();
}

bool QuickWidgetAPI::getPresenceStanza() const
{
    return m_presenceStanza;
}

void QuickWidgetAPI::setPresenceStanza(bool presenceStanza)
{
    m_presenceStanza = presenceStanza;
    emit presenceStanzaChanged();
}

bool QuickWidgetAPI::getMessageStanza() const
{
    return m_messageStanza;
}

void QuickWidgetAPI::setMessageStanza(bool messageStanza)
{
    m_messageStanza = messageStanza;
    emit messageStanzaChanged();
}

bool QuickWidgetAPI::getIqStanza() const
{
    return m_iqStanza;
}

void QuickWidgetAPI::setIqStanza(bool iqStanza)
{
    m_iqStanza = iqStanza;
    emit iqStanzaChanged();
}

bool QuickWidgetAPI::getSuccessStanza() const
{
    return m_successStanza;
}

void QuickWidgetAPI::setSuccessStanza(bool successStanza)
{
    m_successStanza = successStanza;
    emit successStanzaChanged();
}

bool QuickWidgetAPI::getStreamStreamStanza() const
{
    return m_streamStreamStanza;
}

void QuickWidgetAPI::setStreamStreamStanza(bool streamStreamStanza)
{
    m_streamStreamStanza = streamStreamStanza;
    emit streamStreamStanzaChanged();
}

bool QuickWidgetAPI::getStreamFeaturesStanza() const
{
    return m_streamFeaturesStanza;
}

void QuickWidgetAPI::setStreamFeaturesStanza(bool streamFeaturesStanza)
{
    m_streamFeaturesStanza = streamFeaturesStanza;
    emit streamFeaturesStanzaChanged();
}

bool QuickWidgetAPI::getAuthStanza() const
{
    return m_authStanza;
}

void QuickWidgetAPI::setAuthStanza(bool authStanza)
{
    m_authStanza = authStanza;
    emit authStanzaChanged();
}

bool QuickWidgetAPI::getStartTlsStanza() const
{
    return m_startTlsStanza;
}

void QuickWidgetAPI::setStartTlsStanza(bool startTlsStanza)
{
    m_startTlsStanza = startTlsStanza;
    emit startTlsStanzaChanged();
}

bool QuickWidgetAPI::getProceedStanza() const
{
    return m_proceedStanza;
}

void QuickWidgetAPI::setProceedStanza(bool proceedStanza)
{
    m_proceedStanza = proceedStanza;
    emit proceedStanzaChanged();
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
