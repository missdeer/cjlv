#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings
{
public:
    Settings();
    ~Settings();

    void save();
    void load();

    bool searchOrFitler() const;
    void setSearchOrFitler(bool searchOrFitler);

    const QString & searchField() const;
    void setSearchField(const QString& searchField);

    const QString & temporaryDirectory() const;
    void setTemporaryDirectory(const QString &temporaryDirectory);

    const QString & lastOpenedDirectory() const;
    void setLastOpenedDirectory(const QString &lastOpenedDirectory);

    const QString & sourceDirectory() const;
    void setSourceDirectory(const QString &sourceDirectory);

    bool regexMode() const;
    void setRegexMode(bool regexMode);

    bool inMemoryDatabase() const;
    void setInMemoryDatabase(bool inMemoryDatabase);

    void initialize();

    const QString & everythingPath() const;
    void setEverythingPath(const QString& everythingPath);

    bool fatalEnabled() const;
    void setFatalEnabled(bool fatalEnabled);

    bool errorEnabled() const;
    void setErrorEnabled(bool errorEnabled);

    bool warnEnabled() const;
    void setWarnEnabled(bool warnEnabled);

    bool infoEnabled() const;
    void setInfoEnabled(bool infoEnabled);

    bool debugEnabled() const;
    void setDebugEnabled(bool debugEnabled);

    bool traceEnabled() const;
    void setTraceEnabled(bool traceEnabled);

    bool allLogLevelEnabled() const;
private:
    bool m_inMemoryDatabase;
    bool m_searchOrFitler;
    bool m_regexMode;
    bool m_fatalEnabled;
    bool m_errorEnabled;
    bool m_warnEnabled;
    bool m_infoEnabled;
    bool m_debugEnabled;
    bool m_traceEnabled;
    QString m_searchField;
    QString m_temporaryDirectory;
    QString m_lastOpenedDirectory;
    QString m_sourceDirectory;
    QString m_everythingPath;
};

extern Settings *g_settings;

#endif // SETTINGS_H
