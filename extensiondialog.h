#ifndef EXTENSIONDIALOG_H
#define EXTENSIONDIALOG_H

#include <QDialog>
#include "extension.h"

class CodeEditor;
namespace Ui {
class ExtensionDialog;
}

class ExtensionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExtensionDialog(QWidget *parent = 0);
    ~ExtensionDialog();

private slots:
    void on_btnNewExtension_clicked();

    void on_btnDeleteExtension_clicked();

    void on_btnApplyModification_clicked();

    void on_btnTestExtension_clicked();

private:
    Ui::ExtensionDialog *ui;
    ExtensionPtr m_currentExtension;
    CodeEditor* m_contentEditor;
};

extern ExtensionDialog* g_extensionDialog;

#endif // EXTENSIONDIALOG_H
