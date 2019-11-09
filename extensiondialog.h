#ifndef EXTENSIONDIALOG_H
#define EXTENSIONDIALOG_H

#include "extension.h"

class CodeEditor;
namespace Ui
{
    class ExtensionDialog;
}

class ExtensionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtensionDialog(QWidget *parent = 0);
    ~ExtensionDialog();
signals:
    void runExtension(ExtensionPtr);
private slots:
    void onTableViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void on_btnNewExtension_clicked();

    void on_btnDeleteExtension_clicked();

    void on_btnApplyModification_clicked();

    void on_btnTestExtension_clicked();

    void on_edtTitle_textChanged(const QString &arg1);

    void on_edtAuthor_textChanged(const QString &arg1);

    void on_cbField_currentIndexChanged(int index);

    void on_cbMethod_currentIndexChanged(int index);

    void on_edtComment_textChanged(const QString &arg1);

    void on_edtShortcut_textChanged(const QString &arg1);

    void on_edtCategory_textChanged(const QString &arg1);

private:
    Ui::ExtensionDialog *ui;
    CodeEditor *         m_contentEditor;
    ExtensionPtr         m_currentExtension;
    bool                 m_modified;
    bool                 m_notModifying;
    QString              m_defaultUserName;
};

extern ExtensionDialog *g_extensionDialog;

#endif // EXTENSIONDIALOG_H
