#include <QStandardPaths>
#include <QDir>
#include "settings.h"

Settings g_settings;

Settings::Settings()
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
