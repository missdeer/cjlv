#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QStringBuilder>
#include "settings.h"

Settings* g_settings = nullptr;

Settings::Settings()
    : m_inMemoryDatabase(true)
    , m_searchOrFitler(false)
    , m_regexMode(false)
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

void Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "cisco.com", "Cisco Jabber Log Viewer");
    settings.setValue("searchOrFilter", m_searchOrFitler);
    settings.setValue("regexMode", m_regexMode);
    settings.setValue("searchField", m_searchField);
    settings.setValue("temporaryDirectory", m_temporaryDirectory);
    settings.setValue("sourceDirectory", m_sourceDirectory);
    settings.setValue("lastOpenedDirectory", m_lastOpenedDirectory);
    if (!m_everythingPath.isEmpty())
        settings.setValue("everythingPath", m_everythingPath);
    settings.sync();
}

void Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "cisco.com", "Cisco Jabber Log Viewer");
    m_searchOrFitler = settings.value("searchOrFilter", false).toBool();
    m_regexMode = settings.value("regexMode", false).toBool();
    m_searchField = settings.value("searchField", "content").toString();
    m_temporaryDirectory = settings.value("temporaryDirectory").toString();
    m_sourceDirectory = settings.value("sourceDirectory").toString();
    m_lastOpenedDirectory = settings.value("lastOpenedDirectory").toString();
    m_everythingPath = settings.value("everythingPath").toString();
    if (m_everythingPath.isEmpty())
        m_everythingPath = QApplication::applicationDirPath() % "/Everything.exe";
}




