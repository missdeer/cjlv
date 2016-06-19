#ifndef SCINTILLACONFIG_H
#define SCINTILLACONFIG_H

class ScintillaEdit;

class ScintillaConfig
{
public:
    ScintillaConfig();
    void initScintilla(ScintillaEdit* sci);
    void initFolderStyle(ScintillaEdit* sci);
    void initEditorStyle(ScintillaEdit* sci, const QString& filename);

    void applyLanguageStyle(ScintillaEdit* sci, const QString& configPath);
    void applyThemeStyle(ScintillaEdit* sci, const QString& themePath);
};

#endif // SCINTILLACONFIG_H
