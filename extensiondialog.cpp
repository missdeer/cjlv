#include "stdafx.h"
#include "codeeditor.h"
#include "extensionmodel.h"
#include "extensiondialog.h"
#include "ui_extensiondialog.h"

ExtensionDialog* g_extensionDialog = nullptr;

ExtensionDialog::ExtensionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExtensionDialog),
    m_currentExtension(new Extension),
    m_modified(false),
    m_notModifying(false)
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
    ui->tableView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ExtensionDialog::onTableViewSelectionChanged);

    m_currentExtension->setFrom("Custom");
    m_currentExtension->setUuid(QUuid::createUuid().toString());
    m_currentExtension->setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));

    m_defaultUserName = qgetenv("USER");
    if (m_defaultUserName.isEmpty())
        m_defaultUserName = qgetenv("USERNAME");
}

ExtensionDialog::~ExtensionDialog()
{
    delete ui;
}

void ExtensionDialog::onTableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &/*deselected*/)
{
    QModelIndexList list = selected.indexes();
    for(const QModelIndex& index : list)
    {
        m_currentExtension = ExtensionModel::instance()->extension(index);
        if (!m_currentExtension)
            continue;
        ui->edtAuthor->setText(m_currentExtension->author());
        ui->edtTitle->setText(m_currentExtension->title());
        ui->edtComment->setText(m_currentExtension->comment());
        m_notModifying = true;
        ui->edtShortcut->setText(m_currentExtension->shortcut());
        ui->edtCategory->setText(m_currentExtension->category());
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
    m_currentExtension->changePathToCustomExtensionDirectory();
    ui->edtAuthor->setText(m_defaultUserName);
    ui->edtTitle->clear();
    ui->edtComment->clear();
    ui->edtShortcut->clear();
    ui->edtCategory->clear();
    m_contentEditor->setText("");
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

        m_currentExtension->destroy();
        ExtensionModel::instance()->removeExtension(m_currentExtension);
        m_currentExtension.reset();
        ui->edtAuthor->setText(m_defaultUserName);
        ui->edtTitle->clear();
        ui->edtComment->clear();
        ui->edtShortcut->clear();
        ui->edtCategory->clear();
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
        ExtensionPtr e(new Extension( *m_currentExtension.data()));
        m_currentExtension.swap(e);
        m_currentExtension->setCreatedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
        m_currentExtension->setUuid(QUuid::createUuid().toString());
        m_currentExtension->setFrom("Custom");
        m_currentExtension->changePathToCustomExtensionDirectory();
    }
    else
    {
        if (QMessageBox::question(this,
                                  tr("Confirm"),
                                  tr("Save changes?"),
                                  QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return;
    }

    if (ui->cbMethod->currentIndex() == 1 && ui->cbField->currentIndex() != 0)
    {
        QMessageBox::information(this,
                                 tr("Notice"),
                                 tr("Field must be empty when method is SQL WHERE clause."),
                                 QMessageBox::Ok);
        return;
    }

    m_currentExtension->setAuthor(ui->edtAuthor->text());
    m_currentExtension->setTitle(ui->edtTitle->text());
    m_currentExtension->setComment(ui->edtComment->text());
    m_currentExtension->setShortcut(ui->edtShortcut->text());
    m_currentExtension->setField(ui->cbField->currentText());
    m_currentExtension->setMethod(ui->cbMethod->currentText());
    m_currentExtension->setContent(m_contentEditor->getText(m_contentEditor->textLength() + 1));
    m_currentExtension->setLastModifiedAt(QDateTime::currentDateTime().toString(Qt::ISODate));
    m_currentExtension->setCategory(ui->edtCategory->text());
    if (!m_currentExtension->save())
        QMessageBox::warning(this, tr("Error"), tr("Saving extension failed."), QMessageBox::Ok);
    ExtensionModel::instance()->updateExtension(m_currentExtension);
    m_modified = false;
    m_contentEditor->setSavePoint();

}

void ExtensionDialog::on_btnTestExtension_clicked()
{
    if (m_currentExtension)
    {
        emit runExtension(m_currentExtension);
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
    {
        if (ui->cbField->currentIndex() == 0)
            ui->cbField->setCurrentIndex(7); // content
        m_contentEditor->setLanguage("lua");
    }
    else if (ui->cbMethod->currentText() == "Regexp" || ui->cbMethod->currentText() == "Keyword")
    {
        if (ui->cbField->currentIndex() == 0)
            ui->cbField->setCurrentIndex(7); // content
        m_contentEditor->setLanguage("null");
    }
    else
    {
        ui->cbField->setCurrentIndex(0);
        m_contentEditor->setLanguage("sql");
    }
}

void ExtensionDialog::on_edtComment_textChanged(const QString &/*arg1*/)
{
    m_modified = true;
}

void ExtensionDialog::on_edtShortcut_textChanged(const QString & text)
{
    if (!text.isEmpty())
    {
        QWidget* parent = parentWidget();
        Q_ASSERT(parent);
        auto actions = parent->findChildren<QAction*>();
        QList<QKeySequence> shortcuts;
        for(auto a: actions)
        {
            shortcuts.append(a->shortcuts());
        }

        if (!m_notModifying && shortcuts.contains(QKeySequence(text)))
        {
            QMessageBox::warning(this, tr("Shortcut is used"),
                                 QString("Shortcut %1 is used already, please choose another one.").arg(text),
                                 QMessageBox::Ok);
            ui->edtShortcut->clear();
            return;
        }
        m_notModifying = false;
    }
    m_modified = true;
}

void ExtensionDialog::on_edtCategory_textChanged(const QString &/*arg1*/)
{
    m_modified = true;
}
