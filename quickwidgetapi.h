#ifndef QUICKWIDGETAPI_H
#define QUICKWIDGETAPI_H

#include <QObject>

class QuickWidgetAPI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int from READ getFrom WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(int to READ getTo WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(int firstValue READ getFirstValue WRITE setFirstValue NOTIFY firstValueChanged)
    Q_PROPERTY(int secondValue READ getSecondValue WRITE setSecondValue NOTIFY secondValueChanged)
    Q_PROPERTY(bool stanzaOnly READ getStanzaOnly WRITE setStanzaOnly NOTIFY stanzaOnlyChanged)
    Q_PROPERTY(bool receivedStanza READ getReceivedStanza WRITE setReceivedStanza NOTIFY receivedStanzaChanged)
    Q_PROPERTY(bool sentStanza READ getSentStanza WRITE setSentStanza NOTIFY sentStanzaChanged)
    Q_PROPERTY(bool xStanza READ getXStanza WRITE setXStanza NOTIFY xStanzaChanged)
    Q_PROPERTY(bool rStanza READ getRStanza WRITE setRStanza NOTIFY rStanzaChanged)
    Q_PROPERTY(bool aStanza READ getAStanza WRITE setAStanza NOTIFY aStanzaChanged)
    Q_PROPERTY(bool enableStanza READ getEnableStanza WRITE setEnableStanza NOTIFY enableStanzaChanged)
    Q_PROPERTY(bool enabledStanza READ getEnabledStanza WRITE setEnabledStanza NOTIFY enabledStanzaChanged)
    Q_PROPERTY(bool presenceStanza READ getPresenceStanza WRITE setPresenceStanza NOTIFY presenceStanzaChanged)
    Q_PROPERTY(bool messageStanza READ getMessageStanza WRITE setMessageStanza NOTIFY messageStanzaChanged)
    Q_PROPERTY(bool iqStanza READ getIqStanza WRITE setIqStanza NOTIFY iqStanzaChanged)
    Q_PROPERTY(bool successStanza READ getSuccessStanza WRITE setSuccessStanza NOTIFY successStanzaChanged)
    Q_PROPERTY(bool streamStreamStanza READ getStreamStreamStanza WRITE setStreamStreamStanza NOTIFY streamStreamStanzaChanged)
    Q_PROPERTY(bool streamFeaturesStanza READ getStreamFeaturesStanza WRITE setStreamFeaturesStanza NOTIFY streamFeaturesStanzaChanged)
    Q_PROPERTY(bool authStanza READ getAuthStanza WRITE setAuthStanza NOTIFY authStanzaChanged)
    Q_PROPERTY(bool startTlsStanza READ getStartTlsStanza WRITE setStartTlsStanza NOTIFY startTlsStanzaChanged)
    Q_PROPERTY(bool proceedStanza READ getProceedStanza WRITE setProceedStanza NOTIFY proceedStanzaChanged)
public:
    explicit QuickWidgetAPI(QObject* parent = nullptr);
    ~QuickWidgetAPI();

    int getFrom() const;
    void setFrom(int from);

    int getTo() const;
    void setTo(int to);

    int getFirstValue() const;
    void setFirstValue(int v);

    int getSecondValue() const;
    void setSecondValue(int v);

    bool getStanzaOnly() const;
    void setStanzaOnly(bool stanzaOnly);

    bool getReceivedStanza() const;
    void setReceivedStanza(bool receivedStanza);

    bool getSentStanza() const;
    void setSentStanza(bool sentStanza);

    bool getXStanza() const;
    void setXStanza(bool xStanza);

    bool getRStanza() const;
    void setRStanza(bool rStanza);

    bool getAStanza() const;
    void setAStanza(bool aStanza);

    bool getEnabledStanza() const;
    void setEnabledStanza(bool enabledStanza);

    bool getEnableStanza() const;
    void setEnableStanza(bool enableStanza);

    bool getPresenceStanza() const;
    void setPresenceStanza(bool presenceStanza);

    bool getMessageStanza() const;
    void setMessageStanza(bool messageStanza);

    bool getIqStanza() const;
    void setIqStanza(bool iqStanza);

    bool getSuccessStanza() const;
    void setSuccessStanza(bool successStanza);

    bool getStreamStreamStanza() const;
    void setStreamStreamStanza(bool streamStreamStanza);

    bool getStreamFeaturesStanza() const;
    void setStreamFeaturesStanza(bool streamFeaturesStanza);

    bool getAuthStanza() const;
    void setAuthStanza(bool authStanza);

    bool getStartTlsStanza() const;
    void setStartTlsStanza(bool startTlsStanza);

    bool getProceedStanza() const;
    void setProceedStanza(bool proceedStanza);

signals:
    void fromChanged();
    void toChanged();
    void firstValueChanged();
    void secondValueChanged();
    void valueChanged();

    void stanzaOnlyChanged();
    void receivedStanzaChanged();
    void sentStanzaChanged();
    void xStanzaChanged();
    void rStanzaChanged();
    void aStanzaChanged();
    void enabledStanzaChanged();
    void enableStanzaChanged();
    void presenceStanzaChanged();
    void messageStanzaChanged();
    void iqStanzaChanged();
    void successStanzaChanged();
    void streamStreamStanzaChanged();
    void streamFeaturesStanzaChanged();
    void authStanzaChanged();
    void startTlsStanzaChanged();
    void proceedStanzaChanged();
private:
    int m_firstValue;
    int m_secondValue;
    int m_from;
    int m_to;
    bool m_stanzaOnly;
    bool m_receivedStanza;
    bool m_sentStanza;
    bool m_xStanza;
    bool m_rStanza;
    bool m_aStanza;
    bool m_enabledStanza;
    bool m_enableStanza;
    bool m_presenceStanza;
    bool m_messageStanza;
    bool m_iqStanza;
    bool m_successStanza;
    bool m_streamStreamStanza;
    bool m_streamFeaturesStanza;
    bool m_authStanza;
    bool m_startTlsStanza;
    bool m_proceedStanza;
};

#endif // QUICKWIDGETAPI_H
