#include <QFileDialog>
#include "settings.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
#if defined(Q_OS_MAC)
    ui->buttonBox->setVisible(false);
#endif
    adjustSize();
    setFixedSize( size() );
    ui->edtTemporaryDirectory->setText(g_settings->temporaryDirectory());
    ui->edtSourceCodeDirectory->setText(g_settings->sourceDirectory());
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings->setTemporaryDirectory(ui->edtTemporaryDirectory->text());
    g_settings->setSourceDirectory(ui->edtSourceCodeDirectory->text());
    g_settings->save();
}

void PreferenceDialog::on_btnSelectTemporaryDirectory_clicked()
{
    QString t = ui->edtTemporaryDirectory->text();
    if (t.isEmpty())
        t = g_settings->temporaryDirectory();
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Temporary Directory"),
                                                    t,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;
    g_settings->setLastOpenedDirectory(dir);
    ui->edtTemporaryDirectory->setText(dir);
#if defined(Q_OS_MAC)
    g_settings->setTemporaryDirectory(ui->edtTemporaryDirectory->text());
    g_settings->save();
#endif
}

void PreferenceDialog::on_btnSelectSourceCodeDirectory_clicked()
{
    QString s = ui->edtSourceCodeDirectory->text();
    if (s.isEmpty())
        s = g_settings->sourceDirectory();
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select Cisco Jabber Source Code Directory"),
                                                    s,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;
    g_settings->setLastOpenedDirectory(dir);
    ui->edtSourceCodeDirectory->setText(dir);
#if defined(Q_OS_MAC)
    g_settings->setSourceDirectory(ui->edtSourceCodeDirectory->text());
    g_settings->save();
#endif
}

void PreferenceDialog::on_buttonBox_accepted()
{
    accept();
    QDialog::accept();
}
