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
    void initEditorStyle(ScintillaEdit* sci);

    void applyLanguageStyle(ScintillaEdit* sci, const QString& configPath);
    void applyThemeStyle(ScintillaEdit* sci, const QString& themePath);
};

#endif // SCINTILLACONFIG_H
