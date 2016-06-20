#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTableWidget>

class TabWidget : public QTabWidget
{
public:
    explicit TabWidget(QWidget *parent = 0);

    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path, bool installed);

    void filter(const QString& keyword);
public slots:
    void onCloseAll();
    void onCloseAllButThis();
    void onCloseCurrent();
    void onTabCloseRequested(int index);

    void onCopyCurrentCell();
    void onCopyCurrentRow();
    void onCopySelectedCells();
    void onCopySelectedRows();

    void onScrollToTop();
    void onScrollToBottom();

    void onReload();
    void onGotoById();
private:
    int findTab(const QString& path);
    int findTab(const QStringList& paths);
    int addTab(QWidget* w, const QString& text, const QString& tooltip);
};

#endif // TABWIDGET_H
