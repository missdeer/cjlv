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

private:
    bool m_searchOrFitler;
    QString m_searchField;
    QString m_temporaryDirectory;
};

extern Settings g_settings;

#endif // SETTINGS_H
