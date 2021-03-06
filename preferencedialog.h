#ifndef PREFERENCEDIALOG_H
#define PREFERENCEDIALOG_H

namespace Ui
{
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
    void on_edtCECId_textChanged(const QString &);

    void on_edtCECPassword_textChanged(const QString &);

    void on_btnSelectWinDBGPath_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_cbSourceViewTheme_currentIndexChanged(const QString &theme);

    void onCheckBoxStateChanged(int state);
    void on_cbFTSSupport_stateChanged(int);

private:
    Ui::PreferenceDialog *ui;
};

#endif // PREFERENCEDIALOG_H
