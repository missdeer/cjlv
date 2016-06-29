#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

#include <QDialog>

namespace Ui {
class PreferenceDialog;
}

class PreferenceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferenceDialog(QWidget *parent = 0);
    ~PreferenceDialog();

    virtual void accept();
private slots:
    void on_btnSelectTemporaryDirectory_clicked();

    void on_btnSelectSourceCodeDirectory_clicked();

    void on_buttonBox_accepted();

private:
    Ui::PreferenceDialog *ui;
};

#endif // PREFERENCEDIALOG_H