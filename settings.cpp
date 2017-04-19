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

QString Settings::cecId() const
{
    return m_cecId;
}

void Settings::setCecId(const QString &cecId)
{
    m_cecId = cecId;
}

QString Settings::cecPassword() const
{
    return m_cecPassword;
}

void Settings::setCecPassword(const QString &cecPassword)
{
    m_cecPassword = cecPassword;
}

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "cisco.com", "Cisco Jabber Log Viewer");
    settings.setValue("searchOrFilter", m_searchOrFitler);
    settings.setValue("temporaryDirectory", m_temporaryDirectory);
    settings.setValue("sourceDirectory", m_sourceDirectory);
    settings.setValue("lastOpenedDirectory",QDir::toNativeSeparators(m_lastOpenedDirectory));
    if (!m_everythingPath.isEmpty())
        settings.setValue("everythingPath", m_everythingPath);
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
    m_lastOpenedDirectory = settings.value("lastOpenedDirectory").toString();
    m_everythingPath = settings.value("everythingPath").toString();
    if (m_everythingPath.isEmpty())
        m_everythingPath = QApplication::applicationDirPath() % "/Everything.exe";

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




