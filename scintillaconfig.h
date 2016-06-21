#ifndef SCINTILLACONFIG_H
#define SCINTILLACONFIG_H

#include <QString>

class ScintillaEdit;

class ScintillaConfig
{
public:
    ScintillaConfig();
    void initScintilla(ScintillaEdit* sci);
    void initFolderStyle(ScintillaEdit* sci);
    void initEditorStyle(ScintillaEdit* sci, const QString &lang = "xml");

    void applyLanguageStyle(ScintillaEdit* sci, const QString& configPath);
    void applyThemeStyle(ScintillaEdit* sci, const QString& themePath);
    QString matchPatternLanguage(const QString &filename);
private:
    bool matchPattern(const QString &filename, const QString &pattern);
    bool matchSuffix(const QString &filename, const QString &suffix);

};

#endif // SCINTILLACONFIG_H
