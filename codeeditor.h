#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "ScintillaEdit.h"
#include "scintillaconfig.h"

class CodeEditor : public ScintillaEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = 0);
    void initialize();

    void setContent(const QString& content);
    void gotoLine(const QString& fileName, int line);
    void gotoLine(int line);
    bool matched(const QString& fileName);
    bool isContent() const;
    void setLanguage(const QString& lang);
private slots:
    void linesAdded(int linesAdded);
    void marginClicked(int position, int modifiers, int margin);
private:
    bool m_isContent;
    ScintillaConfig m_sc;
    QString m_fileName;
};

#endif // CODEEDITOR_H
