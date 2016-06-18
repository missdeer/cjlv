#include "settings.h"

Settings g_settings;

Settings::Settings()
{

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
