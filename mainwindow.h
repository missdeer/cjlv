#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QDragEnterEvent;
class QDropEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onStatusBarMessageChanges(const QString& msg);
private slots:
    void on_actionOpenZipLogBundle_triggered();

    void on_actionOpenRawLogFile_triggered();

    void on_actionOpenLogFolder_triggered();

    void on_actionOpenCurrentInstalledJabberLogFolder_triggered();

    void on_actionSearch_triggered();

    void on_actionFilter_triggered();

    void on_actionPreference_triggered();

    void on_actionAbout_triggered();

    void on_radioSearch_clicked();

    void on_radioFilter_clicked();

    void on_cbKeyword_editTextChanged(const QString &text);

    void on_cbKeyword_currentIndexChanged(const QString &text);

    void on_actionInputKeyword_triggered();

    void on_actionSearchFieldContent_triggered();

    void on_actionSearchFieldID_triggered();

    void on_actionSearchFieldDateTime_triggered();

    void on_actionSearchFieldThread_triggered();

    void on_actionSearchFieldCategory_triggered();

    void on_actionSearchFieldSourceFile_triggered();

    void on_actionSearchFieldMethod_triggered();

    void on_actionSearchFieldLogFile_triggered();

    void on_actionSearchFieldLine_triggered();

    void on_actionSearchFieldLevel_triggered();

    void on_actionClearKeyword_triggered();

    void on_actionEditExtensions_triggered();

    void on_actionRegexpMode_triggered();

private:
    Ui::MainWindow *ui;

    void dragEnterEvent(QDragEnterEvent *e);

    void dropEvent(QDropEvent *e);
};

#endif // MAINWINDOW_H
