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
    CodeEditor* v = new CodeEditor(ui->codeEditorPlaceholder);
    v->initialize();

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(v);
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

}

void ExtensionDialog::on_btnDeleteExtension_clicked()
{

}

void ExtensionDialog::on_btnApplyModification_clicked()
{

}

void ExtensionDialog::on_btnTestExtension_clicked()
{

}
