#ifndef SOURCEVIEWTABWIDGET_H
#define SOURCEVIEWTABWIDGET_H

#include <QObject>

class TabWidget;
class CodeEditorTabWidget;

class SourceViewTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit SourceViewTabWidget(QWidget *parent = 0);

signals:

public slots:
    void onCloseAll();
    void onCloseAllButThis();
    void onCloseCurrent();
    void onTabCloseRequested(int index);
    void onCurrentChanged(int index);

    void setMainTabWidget(TabWidget *w)
    {
        m_mainTabWidget = w;
    }
    void gotoLine(const QString &logFile, const QString &sourceFile, int line = -1);
    void setContent(const QString &logFile, const QString &text);
private slots:
    void onCopyFileName();
    void onCopyFileFullPath();
    void onOpenContainerFolder();

    void onCustomContextMenuRequested(const QPoint &pos);

private:
    TabWidget *          m_mainTabWidget;
    QStringList          m_logFiles;
    int                  findTab(const QString &path);
    int                  addTab(CodeEditorTabWidget *w, const QString &text, const QString &tooltip);
    CodeEditorTabWidget *getCodeEditorTabWidget(const QString &file);
};

#endif // SOURCEVIEWTABWIDGET_H
