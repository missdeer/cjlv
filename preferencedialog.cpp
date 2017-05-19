#include "stdafx.h"
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
    ui->labelWinDBGPath->setVisible(false);
    ui->btnSelectWinDBGPath->setVisible(false);
    ui->edtWinDBG->setVisible(false);
#endif
    adjustSize();
    setFixedSize( size() );
    ui->edtTemporaryDirectory->setText(g_settings->temporaryDirectory());
    ui->edtSourceCodeDirectory->setText(g_settings->sourceDirectory());
    ui->edtCECId->setText(g_settings->cecId());
    ui->edtCECPassword->setText(g_settings->cecPassword());
#if defined(Q_OS_WIN)
    ui->edtWinDBG->setText(g_settings->windbgPath());
#endif
    ui->fontComboBox->setCurrentFont(QFont(g_settings->sourceViewFontFamily()));


    QDir dir(":/resource/themes");
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.asTheme";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    for( auto fi : list)
    {
        ui->cbSourceViewTheme->addItem(fi.baseName());
    }
    ui->cbSourceViewTheme->setCurrentText(g_settings->sourceViewTheme());
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    g_settings->setTemporaryDirectory(ui->edtTemporaryDirectory->text());
    g_settings->setSourceDirectory(ui->edtSourceCodeDirectory->text());
    g_settings->setSourceViewFontFamily(ui->fontComboBox->currentText());
    g_settings->setSourceViewTheme(ui->cbSourceViewTheme->currentText());
    g_settings->setCecId(ui->edtCECId->text());
    g_settings->setCecPassword(ui->edtCECPassword->text());
#if defined(Q_OS_WIN)
    g_settings->setWindbgPath(ui->edtWinDBG->text());
#endif
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


void PreferenceDialog::on_edtCECId_textChanged(const QString &)
{
#if defined(Q_OS_MAC)
    g_settings->setCecId(ui->edtCECId->text());
    g_settings->save();
#endif
}

void PreferenceDialog::on_edtCECPassword_textChanged(const QString &)
{
#if defined(Q_OS_MAC)
    g_settings->setCecPassword(ui->edtCECPassword->text());
    g_settings->save();
#endif
}

void PreferenceDialog::on_btnSelectWinDBGPath_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Find WinDBG.exe"),
                                                QString(),
                                                "WinDBG main executable (WinDBG.exe)");
    if (QFile::exists(path))
    {
        ui->edtWinDBG->setText(path);
    }
}

void PreferenceDialog::on_fontComboBox_currentFontChanged(const QFont &f)
{
#if defined(Q_OS_MAC)
    g_settings->setSourceViewFontFamily(f.family());
    g_settings->save();
#endif
}

void PreferenceDialog::on_cbSourceViewTheme_currentIndexChanged(const QString &theme)
{
#if defined(Q_OS_MAC)
    g_settings->setSourceViewTheme(theme);
    g_settings->save();
#endif
}
