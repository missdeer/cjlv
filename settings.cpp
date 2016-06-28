#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include "settings.h"

Settings g_settings;

Settings::Settings()
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

Settings::~Settings()
{
    save();
}

bool Settings::searchOrFitler() const
{
    return m_searchOrFitler;
}

void Settings::setSearchOrFitler(bool searchOrFitler)
{
    m_searchOrFitler = searchOrFitler;
}

QString Settings::searchField() const
{
    return m_searchField;
}

void Settings::setSearchField(const QString& searchField)
{
    m_searchField = searchField;
}

QString Settings::temporaryDirectory() const
{
    return m_temporaryDirectory;
}

void Settings::setTemporaryDirectory(const QString &temporaryDirectory)
{
    m_temporaryDirectory = temporaryDirectory;
}

QString Settings::lastOpenedDirectory() const
{
    return m_lastOpenedDirectory;
}

void Settings::setLastOpenedDirectory(const QString &lastOpenedDirectory)
{
    m_lastOpenedDirectory = lastOpenedDirectory;
}

QString Settings::sourceDirectory() const
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
}
