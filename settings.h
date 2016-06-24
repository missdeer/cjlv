#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    Settings();

    bool searchOrFitler() const;
    void setSearchOrFitler(bool searchOrFitler);

    QString searchField() const;
    void setSearchField(const QString& searchField);

    QString temporaryDirectory() const;
    void setTemporaryDirectory(const QString &temporaryDirectory);

    QString lastOpenedDirectory() const;
    void setLastOpenedDirectory(const QString &lastOpenedDirectory);

    QString sourceDirectory() const;
    void setSourceDirectory(const QString &sourceDirectory);

    bool regexMode() const;
    void setRegexMode(bool regexMode);

private:
    bool m_searchOrFitler;
    bool m_regexMode;
    QString m_searchField;
    QString m_temporaryDirectory;
    QString m_lastOpenedDirectory;
    QString m_sourceDirectory;
};

extern Settings g_settings;

#endif // SETTINGS_H
