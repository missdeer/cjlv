#ifndef CODEEDITORTABWIDGET_H
#define CODEEDITORTABWIDGET_H

#include <QTableWidget>

class CodeEditor;

class CodeEditorTabWidget : public QTabWidget
{
public:
    explicit CodeEditorTabWidget(QWidget *parent = 0);

    void setContent(const QString& content);
    void gotoLine(const QString& fileName, int line = -1);
private slots:
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
private:
    CodeEditor *createCodeEditor(const QString& text, const QString& tooltip);
};

#endif // CODEEDITORTABWIDGET_H
