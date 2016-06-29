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
