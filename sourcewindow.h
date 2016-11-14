#ifndef SOURCEWINDOW_H
#define SOURCEWINDOW_H

#include <QMainWindow>
#include "sourceviewtabwidget.h"

namespace Ui {
class SourceWindow;
}

class TabWidget;
class SourceViewTabWidget;

class SourceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceWindow(QWidget *parent = 0);
    ~SourceWindow();

    SourceViewTabWidget* getSourceViewTabWidget();
    void setMainTabWidget(TabWidget* w);
private:
    Ui::SourceWindow *ui;
};

#endif // SOURCEWINDOW_H
