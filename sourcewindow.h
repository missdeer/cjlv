#ifndef SOURCEWINDOW_H
#define SOURCEWINDOW_H

#include <QMainWindow>
#include "sourceviewtabwidget.h"

namespace Ui {
class SourceWindow;
}

class SourceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SourceWindow(QWidget *parent = 0);
    ~SourceWindow();

    void gotoLine(const QString &logFile, const QString& sourceFile, int line = -1);
private:
    Ui::SourceWindow *ui;
};

#endif // SOURCEWINDOW_H
