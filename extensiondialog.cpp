#include <QUuid>
#include <QDateTime>
#include <QMessageBox>
#include "codeeditor.h"
#include "extensionmodel.h"
#include "extensiondialog.h"
#include "ui_extensiondialog.h"

ExtensionDialog* g_extensionDialog = nullptr;

ExtensionDialog::ExtensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionDialog),
    m_currentExtension(new Extension),
    m_modified(false)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    m_contentEditor = new CodeEditor(ui->codeEditorPlaceholder);
    m_contentEditor->initialize();

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(m_contentEditor);
    ui->codeEditorPlaceholder->setLayout(m_mainLayout);

    ui->tableView->setModel(ExtensionModel::instance());
    ui->tableView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ExtensionDialog::on_tableView_selectionChanged);

    m_currentExtension->setFrom("Custom");
    m_currentExtension->setUuid(QUuid::createUuid().toString());
    m_currentExtension->setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
}

ExtensionDialog::~ExtensionDialog()
{
    delete ui;
}

void ExtensionDialog::on_tableView_selectionChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    QModelIndexList list = selected.indexes();
    Q_FOREACH(const QModelIndex& index, list)
    {
        m_currentExtension = ExtensionModel::instance()->extension(index);
        if (!m_currentExtension)
            continue;
        ui->edtAuthor->setText(m_currentExtension->author());
        ui->edtTitle->setText(m_currentExtension->title());
        ui->cbField->setCurrentText(m_currentExtension->field());
        ui->cbMethod->setCurrentText(m_currentExtension->method());
        m_contentEditor->setText(m_currentExtension->content().toStdString().c_str());
        m_contentEditor->setSavePoint();
        m_modified = false;
        break;
    }
}

void ExtensionDialog::on_btnNewExtension_clicked()
{
    if (m_modified || m_contentEditor->modify())
    {
        if (QMessageBox::question(this, tr("Confirm"), tr("Discard modification?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }
    m_currentExtension.reset(new Extension);
    m_currentExtension->setFrom("Custom");
    m_currentExtension->setUuid(QUuid::createUuid().toString());
    m_currentExtension->setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
    ui->edtAuthor->clear();
    ui->edtTitle->clear();
    m_contentEditor->clear();
    m_contentEditor->setSavePoint();
    ui->cbField->setCurrentIndex(0);
    ui->cbMethod->setCurrentIndex(0);
    m_modified = false;
}

void ExtensionDialog::on_btnDeleteExtension_clicked()
{
    QItemSelectionModel* selection = ui->tableView->selectionModel();
    if (!selection || !selection->hasSelection())
        return;

    if (m_currentExtension)
    {
        if (m_modified || m_contentEditor->modify())
        {
            if (QMessageBox::question(this, tr("Confirm"), tr("Discard modification?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
                return;
        }
        if (QMessageBox::question(this, tr("Confirm"), tr("Delete the selected extension?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
        ExtensionModel::instance()->removeExtension(m_currentExtension);
        m_currentExtension->destroy();
        m_currentExtension.reset();
        ui->edtAuthor->clear();
        ui->edtTitle->clear();
        m_contentEditor->setText("");
        m_contentEditor->setSavePoint();
        ui->cbField->setCurrentIndex(0);
        ui->cbMethod->setCurrentIndex(0);
        m_modified = false;
    }
}

void ExtensionDialog::on_btnApplyModification_clicked()
{
    if (m_currentExtension->from() != "Custom")
    {
        if (QMessageBox::question(this,
                                  tr("Confirm"),
                                  tr("Current selected extension is built-in extension, do you want to fork a custom one?"),
                                  QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
        m_currentExtension->setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
        m_currentExtension->setUuid(QUuid::createUuid().toString());
        m_currentExtension->setFrom("Custom");
    }
    else
    {
        if (QMessageBox::question(this,
                                  tr("Confirm"),
                                  tr("Save changes?"),
                                  QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }

    m_currentExtension->setAuthor(ui->edtAuthor->text());
    m_currentExtension->setTitle(ui->edtTitle->text());
    m_currentExtension->setField(ui->cbField->currentText());
    m_currentExtension->setMethod(ui->cbMethod->currentText());
    m_currentExtension->setContent(m_contentEditor->getText(m_contentEditor->textLength()));
    m_currentExtension->setLastModifiedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
    m_currentExtension->save();
    ExtensionModel::instance()->updateExtension(m_currentExtension);
    m_modified = false;
    m_contentEditor->setSavePoint();

}

void ExtensionDialog::on_btnTestExtension_clicked()
{
    if (m_currentExtension)
    {
        m_currentExtension->run();
    }
}

void ExtensionDialog::on_edtTitle_textChanged(const QString &/*arg1*/)
{
    m_modified = true;
}

void ExtensionDialog::on_edtAuthor_textChanged(const QString &/*arg1*/)
{
    m_modified = true;
}

void ExtensionDialog::on_cbField_currentIndexChanged(int /*index*/)
{
    m_modified = true;
}

void ExtensionDialog::on_cbMethod_currentIndexChanged(int /*index*/)
{
    m_modified = true;
    if (ui->cbMethod->currentText() == "Lua")
        m_contentEditor->setLanguage("lua");
    else if (ui->cbMethod->currentText() == "Regexp" || ui->cbMethod->currentText() == "Keyword")
        m_contentEditor->setLanguage("null");
    else
        m_contentEditor->setLanguage("sql");
}
