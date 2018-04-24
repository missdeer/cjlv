#include "stdafx.h"
#include <keychain.h>
#include "settings.h"

using namespace QKeychain;

Settings* g_settings = nullptr;

Settings::Settings()
    : m_inMemoryDatabase(true)
    , m_searchOrFitler(false)
    , m_fatalEnabled(true)
    , m_errorEnabled(true)
    , m_warnEnabled(true)
    , m_infoEnabled(true)
    , m_debugEnabled(true)
    , m_traceEnabled(true)
{

}

Settings::~Settings()
{
    save();
}

void Settings::initialize()
{
    load();

    if (m_lastOpenedDirectory.isEmpty())
    {
#if defined(Q_OS_WIN)
        m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
        m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) % "/Library/Logs/Jabber";
#endif
        QDir dir(m_lastOpenedDirectory);
        if (!dir.exists())
        {
            m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        }
    }
}

const QString& Settings::prtTrackingSystemToken() const
{
    return m_prtTrackingSystemToken;
}

void Settings::setPrtTrackingSystemToken(const QString &prtTrackingSystemToken)
{
    m_prtTrackingSystemToken = prtTrackingSystemToken;
}

const QString &Settings::cecId() const
{
    return m_cecId;
}

void Settings::setCecId(const QString &cecId)
{
    m_cecId = cecId;
}

const QString &Settings::cecPassword() const
{
    return m_cecPassword;
}

void Settings::setCecPassword(const QString &cecPassword)
{
    m_cecPassword = cecPassword;
}

const QString &Settings::windbgPath() const
{
    return m_windbgPath;
}

void Settings::setWindbgPath(const QString &windbgPath)
{
    m_windbgPath = windbgPath;
}

const QString &Settings::sourceViewFontFamily() const
{
    return m_sourceViewFontFamily;
}

void Settings::setSourceViewFontFamily(const QString &sourceViewFontFamily)
{
    m_sourceViewFontFamily = sourceViewFontFamily;
}

const QString &Settings::sourceViewTheme() const
{
    return m_sourceViewTheme;
}

void Settings::setSourceViewTheme(const QString &sourceViewTheme)
{
    m_sourceViewTheme = sourceViewTheme;
}

int Settings::logTableColumnVisible() const
{
    return m_logTableColumnVisible;
}

void Settings::setLogTableColumnVisible(int logTableColumnVisible)
{
    m_logTableColumnVisible = logTableColumnVisible;
}

QNetworkProxy::ProxyType Settings::proxyType() const
{
    return m_proxyType;
}

void Settings::setProxyType(const QNetworkProxy::ProxyType &proxyType)
{
    m_proxyType = proxyType;
}

const QString &Settings::proxyHostName() const
{
    return m_proxyHostName;
}

void Settings::setProxyHostName(const QString &proxyHostName)
{
    m_proxyHostName = proxyHostName;
}

int Settings::proxyPort() const
{
    return m_proxyPort;
}

void Settings::setProxyPort(int proxyPort)
{
    m_proxyPort = proxyPort;
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "cisco.com", "Cisco Jabber Log Viewer");
    settings.setValue("searchOrFilter", m_searchOrFitler);
    settings.setValue("temporaryDirectory", m_temporaryDirectory);
    settings.setValue("sourceDirectory", m_sourceDirectory);
    settings.setValue("sourceViewFontFamily", m_sourceViewFontFamily);
    settings.setValue("sourceViewTheme", m_sourceViewTheme);
    settings.setValue("lastOpenedDirectory",QDir::toNativeSeparators(m_lastOpenedDirectory));
    if (!m_everythingPath.isEmpty())
        settings.setValue("everythingPath", m_everythingPath);
    if (!m_windbgPath.isEmpty())
        settings.setValue("windbgPath", m_windbgPath);
    settings.setValue("logTableColumnVisible", m_logTableColumnVisible);
    settings.setValue("proxyType", m_proxyType);
    settings.setValue("proxyHostName", m_proxyHostName);
    settings.setValue("proxyPort", m_proxyPort);
    settings.sync();

    WritePasswordJob job( QLatin1String("com.cisco.jabber.viewer") );
    job.setAutoDelete( false );
    QEventLoop loop;
    job.connect( &job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()) );

    job.setKey( "username");
    job.setTextData( m_cecId);
    job.start();
    loop.exec();

    job.setKey( "password");
    job.setTextData( m_cecPassword);
    job.start();
    loop.exec();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "cisco.com", "Cisco Jabber Log Viewer");
    m_searchOrFitler = settings.value("searchOrFilter", false).toBool();
    m_temporaryDirectory = settings.value("temporaryDirectory").toString();
    m_sourceDirectory = settings.value("sourceDirectory").toString();
    m_sourceViewFontFamily = settings.value("sourceViewFontFamily", QVariant("Source Code Pro")).toString();
    m_sourceViewTheme = settings.value("sourceViewTheme", QVariant("Default")).toString();
    m_lastOpenedDirectory = settings.value("lastOpenedDirectory").toString();
    m_everythingPath = settings.value("everythingPath").toString();
    if (m_everythingPath.isEmpty())
        m_everythingPath = QApplication::applicationDirPath() % "/Everything.exe";
    m_windbgPath = settings.value("windbgPath").toString();
    m_logTableColumnVisible = settings.value("logTableColumnVisible", 0x7FFFFFFF).toInt();
    m_proxyType = (QNetworkProxy::ProxyType)settings.value("proxyType", QNetworkProxy::NoProxy).toInt();
    m_proxyHostName = settings.value("proxyHostName").toString();
    m_proxyPort = settings.value("proxyPort", 0).toInt();

    ReadPasswordJob job( QLatin1String("com.cisco.jabber.viewer") );
    job.setAutoDelete( false );
    QEventLoop loop;
    job.connect( &job, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()) );

    job.setKey( "username");
    job.start();
    loop.exec();
    if (!job.textData().isEmpty())
        m_cecId = job.textData();

    job.setKey( "password");
    job.start();
    loop.exec();
    if (!job.textData().isEmpty())
        m_cecPassword = job.textData();
}




