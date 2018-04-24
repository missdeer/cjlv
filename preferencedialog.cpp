#include "stdafx.h"
#include "settings.h"
#include "preferencedialog.h"
#include "ui_preferencedialog.h"

PreferenceDialog::PreferenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferenceDialog)
{
    ui->setupUi(this);

    std::map<QCheckBox*, int> m = {
        { ui->cbId,         0 },
        { ui->cbTime,       1 },
        { ui->cbLevel,      2 },
        { ui->cbThread,     3 },
        { ui->cbSourceFile, 4 },
        { ui->cbCategory,   5 },
        { ui->cbMethod,     6 },
        { ui->cbContent,    7 },
        { ui->cbLogFile,    8 },
        { ui->cbLine,       9 },
    };
    int visible = g_settings->logTableColumnVisible();
    for (auto p : m )
    {
        if ((0x01 << p.second) & visible)
            p.first->setChecked(true);
        else
            p.first->setChecked(false);
    }

#if defined(Q_OS_MAC)
    ui->buttonBox->setVisible(false);
    ui->labelWinDBGPath->setVisible(false);
    ui->btnSelectWinDBGPath->setVisible(false);
    ui->edtWinDBG->setVisible(false);
    connect(ui->cbId,         &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbTime,       &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbLevel,      &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbThread,     &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbSourceFile, &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbCategory,   &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbMethod,     &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbContent,    &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbLogFile,    &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
    connect(ui->cbLine,       &QCheckBox::stateChanged, this, &PreferenceDialog::onCheckBoxStateChanged);
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
    switch(g_settings->proxyType())
    {
    case QNetworkProxy::NoProxy:
        ui->rbNoneProxy->setChecked(true);
        break;
    case QNetworkProxy::Socks5Proxy:
        ui->rbSocksProxy->setChecked(true);
        break;
    case QNetworkProxy::HttpProxy:
        ui->rbHttpProxy->setChecked(true);
        break;
    default:
        break;
    }
    ui->edtProxy->setText(QString("%1:%2").arg(g_settings->proxyHostName()).arg(g_settings->proxyPort()));

    QDir dir(":/resource/themes");
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QStringList filters;
    filters << "*.asTheme";
    dir.setNameFilters(filters);

    QString currentTheme = g_settings->sourceViewTheme();
    QFileInfoList list = dir.entryInfoList();
    for( auto fi : list)
    {
        ui->cbSourceViewTheme->addItem(fi.baseName());
    }
    ui->cbSourceViewTheme->setCurrentText(currentTheme);
}

PreferenceDialog::~PreferenceDialog()
{
    delete ui;
}

void PreferenceDialog::accept()
{
    std::map<QCheckBox*, int> m = {
        { ui->cbId,         0 },
        { ui->cbTime,       1 },
        { ui->cbLevel,      2 },
        { ui->cbThread,     3 },
        { ui->cbSourceFile, 4 },
        { ui->cbCategory,   5 },
        { ui->cbMethod,     6 },
        { ui->cbContent,    7 },
        { ui->cbLogFile,    8 },
        { ui->cbLine,       9 },
    };

    int res = 0;
    for ( auto p : m )
    {
        if (p.first->isChecked())
        {
            res |= (0x01 << p.second);
        }
    }

    g_settings->setLogTableColumnVisible(res);
    g_settings->setTemporaryDirectory(ui->edtTemporaryDirectory->text());
    g_settings->setSourceDirectory(ui->edtSourceCodeDirectory->text());
    g_settings->setSourceViewFontFamily(ui->fontComboBox->currentText());
    g_settings->setSourceViewTheme(ui->cbSourceViewTheme->currentText());
    g_settings->setCecId(ui->edtCECId->text());
    g_settings->setCecPassword(ui->edtCECPassword->text());
#if defined(Q_OS_WIN)
    g_settings->setWindbgPath(ui->edtWinDBG->text());
#endif
    if (ui->rbNoneProxy->isChecked())
        g_settings->setProxyType(QNetworkProxy::NoProxy);
    else if (ui->rbSocksProxy->isChecked())
        g_settings->setProxyType(QNetworkProxy::Socks5Proxy);
    else if (ui->rbHttpProxy->isChecked())
        g_settings->setProxyType(QNetworkProxy::HttpProxy);
    QUrl u(ui->edtProxy->text());
    g_settings->setProxyHostName(u.host());
    g_settings->setProxyPort(u.port());
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

void PreferenceDialog::onCheckBoxStateChanged(int)
{
    std::map<QCheckBox*, int> m = {
        { ui->cbId,         0 },
        { ui->cbTime,       1 },
        { ui->cbLevel,      2 },
        { ui->cbThread,     3 },
        { ui->cbSourceFile, 4 },
        { ui->cbCategory,   5 },
        { ui->cbMethod,     6 },
        { ui->cbContent,    7 },
        { ui->cbLogFile,    8 },
        { ui->cbLine,       9 },
    };

    int res = 0;
    for ( auto p : m )
    {
        if (p.first->isChecked())
        {
            res |= (0x01 << p.second);
        }
    }
#if defined(Q_OS_MAC)
    g_settings->setLogTableColumnVisible(res);
    g_settings->save();
#endif
}
