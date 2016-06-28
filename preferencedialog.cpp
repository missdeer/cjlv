#include <QFileDialog>
#include "settings.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);
    setFixedSize( size() );
    ui->edtTemporaryDirectory->setText(g_settings.temporaryDirectory());
    ui->edtSourceCodeDirectory->setText(g_settings.sourceDirectory());
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings.setTemporaryDirectory(ui->edtTemporaryDirectory->text());
    g_settings.setSourceDirectory(ui->edtSourceCodeDirectory->text());
}

void PreferenceDialog::on_btnSelectTemporaryDirectory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                    g_settings.lastOpenedDirectory(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;
    g_settings.setLastOpenedDirectory(dir);
    ui->edtTemporaryDirectory->setText(dir);
}

void PreferenceDialog::on_btnSelectSourceCodeDirectory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Open Cisco Jabber Log Folder"),
                                                    g_settings.lastOpenedDirectory(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dir.isEmpty())
        return;
    g_settings.setLastOpenedDirectory(dir);
    ui->edtSourceCodeDirectory->setText(dir);
}

void PreferenceDialog::on_buttonBox_accepted()
{
    accept();
    QDialog::accept();
}
