<<<<<<< HEAD
#include "codeeditor.h"
#include "extensiondialog.h"
#include "ui_extensiondialog.h"

ExtensionDialog* g_extensionDialog = nullptr;

ExtensionDialog::ExtensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionDialog)
{
    ui->setupUi(this);

    CodeEditor* v = new CodeEditor(ui->codeEditorPlaceholder);
    v->initialize();

    QVBoxLayout* m_mainLayout = new QVBoxLayout;
    Q_ASSERT(m_mainLayout);
    m_mainLayout->setMargin(0);
    m_mainLayout->addWidget(v);
    ui->codeEditorPlaceholder->setLayout(m_mainLayout);
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
=======
#include "codeeditor.h"
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
>>>>>>> 2df8eddd55f0b7b4d1e9d8cad5fd14352e5eb91a
