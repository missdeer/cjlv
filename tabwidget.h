#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTableWidget>

class TabWidget : public QTabWidget
{
public:
    explicit TabWidget(QWidget *parent = 0);

    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path);

public slots:
    void closeAll();
    void closeAllButThis();
    void closeCurrent();
    void onTabCloseRequested(int index);

private:
    int findTab(const QString& path);
    int findTab(const QStringList& paths);
    void addTab(QWidget* w, const QString& text, const QString& tooltip);
};

#endif // TABWIDGET_H
