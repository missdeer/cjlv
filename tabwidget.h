#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTableWidget>

class TabWidget : public QTabWidget
{
public:
    explicit TabWidget(QWidget *parent = 0);

    void closeAll();
    void closeAllButThis();
    void closeCurrent();

public slots:
    void onTabCloseRequested(int index);
};

#endif // TABWIDGET_H
