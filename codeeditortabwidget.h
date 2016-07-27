#ifndef CODEEDITORTABWIDGET_H
#define CODEEDITORTABWIDGET_H


class CodeEditor;

class CodeEditorTabWidget : public QTabWidget
{
public:
    explicit CodeEditorTabWidget(QWidget *parent = 0);

    void setContent(const QString& content);
    void gotoLine(const QString& fileName, int line = -1);

    bool copyable();
    void copy();
private slots:
    void onCopyFileName();
    void onCopyFileFullPath();
    void onOpenContainerFolder();

    void onCloseAll();
    void onCloseAllButThis();
    void onCloseCurrent();

    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);
    void onCustomContextMenuRequested(const QPoint &pos);
private:
    CodeEditor *createCodeEditor(const QString& text, const QString& tooltip);
};

#endif // CODEEDITORTABWIDGET_H
