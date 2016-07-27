#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
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
        m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/AppData/Local/Cisco/Unified Communications/Jabber/CSF/Logs";
#endif
#if defined(Q_OS_MAC)
        m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Library/Logs/Jabber";
#endif
        QDir dir(m_lastOpenedDirectory);
        if (!dir.exists())
        {
            m_lastOpenedDirectory = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
        }
    }
}

const QString &Settings::everythingPath() const
{
    return m_everythingPath;
}

void Settings::setEverythingPath(const QString& everythingPath)
{
    m_everythingPath = everythingPath;
}

bool Settings::fatalEnabled() const
{
    return m_fatalEnabled;
}

void Settings::setFatalEnabled(bool fatalEnabled)
{
    m_fatalEnabled = fatalEnabled;
}

bool Settings::errorEnabled() const
{
    return m_errorEnabled;
}

void Settings::setErrorEnabled(bool errorEnabled)
{
    m_errorEnabled = errorEnabled;
}

bool Settings::warnEnabled() const
{
    return m_warnEnabled;
}

void Settings::setWarnEnabled(bool warnEnabled)
{
    m_warnEnabled = warnEnabled;
}

bool Settings::infoEnabled() const
{
    return m_infoEnabled;
}

void Settings::setInfoEnabled(bool infoEnabled)
{
    m_infoEnabled = infoEnabled;
}

bool Settings::debugEnabled() const
{
    return m_debugEnabled;
}

void Settings::setDebugEnabled(bool debugEnabled)
{
    m_debugEnabled = debugEnabled;
}

bool Settings::traceEnabled() const
{
    return m_traceEnabled;
}

void Settings::setTraceEnabled(bool traceEnabled)
{
    m_traceEnabled = traceEnabled;
}

bool Settings::searchOrFitler() const
{
    return m_searchOrFitler;
}

void Settings::setSearchOrFitler(bool searchOrFitler)
{
    m_searchOrFitler = searchOrFitler;
}

const QString &Settings::searchField() const
{
    return m_searchField;
}

void Settings::setSearchField(const QString& searchField)
{
    m_searchField = searchField;
}

const QString &Settings::temporaryDirectory() const
{
    return m_temporaryDirectory;
}

void Settings::setTemporaryDirectory(const QString &temporaryDirectory)
{
    m_temporaryDirectory = temporaryDirectory;
}

const QString &Settings::lastOpenedDirectory() const
{
    return m_lastOpenedDirectory;
}

void Settings::setLastOpenedDirectory(const QString &lastOpenedDirectory)
{
    m_lastOpenedDirectory = lastOpenedDirectory;
}

const QString &Settings::sourceDirectory() const
{
    return m_sourceDirectory;
}

void Settings::setSourceDirectory(const QString &sourceDirectory)
{
    m_sourceDirectory = sourceDirectory;
}

bool Settings::regexMode() const
{
    return m_regexMode;
}

void Settings::setRegexMode(bool regexMode)
{
    m_regexMode = regexMode;
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
        m_everythingPath = QApplication::applicationDirPath() + "/Everything.exe";
}

bool Settings::inMemoryDatabase() const
{
    return m_inMemoryDatabase;
}

void Settings::setInMemoryDatabase(bool inMemoryDatabase)
{
    m_inMemoryDatabase = inMemoryDatabase;
}
