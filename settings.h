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

private:
    bool m_searchOrFitler;
    QString m_searchField;
};

extern Settings g_settings;

#endif // SETTINGS_H
