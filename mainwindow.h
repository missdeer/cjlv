#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenZipLogBundle_triggered();

    void on_actionOpenRawLogFile_triggered();

    void on_actionOpenLogFolder_triggered();

    void on_actionOpenCurrentInstalledJabberLogFolder_triggered();

    void on_actionClose_triggered();

    void on_actionCloseAll_triggered();

    void on_actionCloseAllButThis_triggered();

    void on_actionExit_triggered();

    void on_actionSearch_triggered();

    void on_actionFilter_triggered();

    void on_actionPreference_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
