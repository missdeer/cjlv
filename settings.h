#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDir>
#include <QFile>
#include <QNetworkProxy>

class Settings
{
public:
    Settings() = default;
    ~Settings();

    void save();
    void load();

    bool searchOrFitler() const
    {
        return m_searchOrFitler;
    }
    void setSearchOrFitler(bool searchOrFitler)
    {
        m_searchOrFitler = searchOrFitler;
    }

    const QString &temporaryDirectory() const
    {
        return m_temporaryDirectory;
    }
    void setTemporaryDirectory(const QString &temporaryDirectory)
    {
        QDir d(temporaryDirectory);
        if (!d.exists(temporaryDirectory))
            d.mkpath(temporaryDirectory);
        if (d.exists(temporaryDirectory))
            m_temporaryDirectory = temporaryDirectory;
        else
            m_temporaryDirectory.clear();
    }

    const QString &lastOpenedDirectory() const
    {
        return m_lastOpenedDirectory;
    }
    void setLastOpenedDirectory(const QString &lastOpenedDirectory)
    {
        m_lastOpenedDirectory = lastOpenedDirectory;
    }

    const QString &sourceDirectory() const
    {
        return m_sourceDirectory;
    }
    void setSourceDirectory(const QString &sourceDirectory)
    {
        QDir d(sourceDirectory);
        if (!d.exists(sourceDirectory))
            d.mkpath(sourceDirectory);
        if (d.exists(sourceDirectory))
            m_sourceDirectory = sourceDirectory;
        else
            m_sourceDirectory.clear();
    }

    bool inMemoryDatabase() const
    {
        return m_inMemoryDatabase;
    }
    void setInMemoryDatabase(bool inMemoryDatabase)
    {
        m_inMemoryDatabase = inMemoryDatabase;
    }

    void initialize();

    const QString &everythingPath() const
    {
        return m_everythingPath;
    }
    void setEverythingPath(const QString &everythingPath)
    {
        m_everythingPath = everythingPath;
    }

    bool fatalEnabled() const
    {
        return m_fatalEnabled;
    }
    void setFatalEnabled(bool fatalEnabled)
    {
        m_fatalEnabled = fatalEnabled;
    }

    bool errorEnabled() const
    {
        return m_errorEnabled;
    }
    void setErrorEnabled(bool errorEnabled)
    {
        m_errorEnabled = errorEnabled;
    }

    bool warnEnabled() const
    {
        return m_warnEnabled;
    }
    void setWarnEnabled(bool warnEnabled)
    {
        m_warnEnabled = warnEnabled;
    }

    bool infoEnabled() const
    {
        return m_infoEnabled;
    }
    void setInfoEnabled(bool infoEnabled)
    {
        m_infoEnabled = infoEnabled;
    }

    bool debugEnabled() const
    {
        return m_debugEnabled;
    }
    void setDebugEnabled(bool debugEnabled)
    {
        m_debugEnabled = debugEnabled;
    }

    bool traceEnabled() const
    {
        return m_traceEnabled;
    }
    void setTraceEnabled(bool traceEnabled)
    {
        m_traceEnabled = traceEnabled;
    }

    bool allLogLevelEnabled() const
    {
        return m_fatalEnabled && m_errorEnabled && m_warnEnabled && m_infoEnabled && m_debugEnabled && m_traceEnabled;
    }
    bool multiMonitorEnabled() const
    {
        return m_multiMonitorEnabled;
    }
    void setMultiMonitorEnabled(bool multiMonitorEnabled)
    {
        m_multiMonitorEnabled = multiMonitorEnabled;
    }

    const QString &prtTrackingSystemToken() const;
    void           setPrtTrackingSystemToken(const QString &prtTrackingSystemToken);

    const QString &cecId() const;
    void           setCecId(const QString &cecId);

    const QString &cecPassword() const;
    void           setCecPassword(const QString &cecPassword);

    const QString &windbgPath() const;
    void           setWindbgPath(const QString &windbgPath);

    const QString &sourceViewFontFamily() const;
    void           setSourceViewFontFamily(const QString &sourceViewFontFamily);

    const QString &sourceViewTheme() const;
    void           setSourceViewTheme(const QString &sourceViewTheme);

    int  logTableColumnVisible() const;
    void setLogTableColumnVisible(int logTableColumnVisible);

    QNetworkProxy::ProxyType proxyType() const;
    void                     setProxyType(const QNetworkProxy::ProxyType &proxyType);

    const QString &proxyHostName() const;
    void           setProxyHostName(const QString &proxyHostName);

    int  proxyPort() const;
    void setProxyPort(int proxyPort);

    bool ftsEnabled() const;
    void setFtsEnabled(bool ftsEnabled);

private:
    bool                     m_inMemoryDatabase {true};
    bool                     m_searchOrFitler {false};
    bool                     m_fatalEnabled {true};
    bool                     m_errorEnabled {true};
    bool                     m_warnEnabled {true};
    bool                     m_infoEnabled {true};
    bool                     m_debugEnabled {true};
    bool                     m_traceEnabled {true};
    bool                     m_multiMonitorEnabled {true};
    bool                     m_ftsEnabled {true};
    int                      m_logTableColumnVisible;
    QNetworkProxy::ProxyType m_proxyType;
    QString                  m_proxyHostName;
    int                      m_proxyPort;
    QString                  m_temporaryDirectory;
    QString                  m_lastOpenedDirectory;
    QString                  m_sourceDirectory;
    QString                  m_everythingPath;
    QString                  m_prtTrackingSystemToken;
    QString                  m_cecId;
    QString                  m_cecPassword;
    QString                  m_windbgPath;
    QString                  m_sourceViewFontFamily;
    QString                  m_sourceViewTheme;
};

inline Settings *g_settings = nullptr;

#endif // SETTINGS_H
