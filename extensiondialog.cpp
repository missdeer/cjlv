#include <QUuid>
#include <QMessageBox>
#include "codeeditor.h"
#include "extensionmodel.h"
#include "extensiondialog.h"
#include "ui_extensiondialog.h"

ExtensionDialog* g_extensionDialog = nullptr;

ExtensionDialog::ExtensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionDialog)
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
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

ExtensionDialog::~ExtensionDialog()
{
    delete ui;
}

void ExtensionDialog::on_btnNewExtension_clicked()
{
    m_currentExtension.reset(new Extension);
    m_currentExtension->setUuid(QUuid::createUuid().toString());
    ui->edtAuthor->clear();
    ui->edtTitle->clear();
    m_contentEditor->clear();
    ui->cbField->setCurrentIndex(0);
    ui->cbMethod->setCurrentIndex(0);
}

void ExtensionDialog::on_btnDeleteExtension_clicked()
{
    if (m_currentExtension)
    {
        ExtensionModel::instance()->removeExtension(m_currentExtension);
        m_currentExtension.reset();
    }
}

void ExtensionDialog::on_btnApplyModification_clicked()
{
    if (QMessageBox::question(this, tr("Confirm"), tr("Save changes?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
    {
        m_currentExtension->save();
    }
}

void ExtensionDialog::on_btnTestExtension_clicked()
{
    if (m_currentExtension)
    {
        m_currentExtension->run();
    }
}
