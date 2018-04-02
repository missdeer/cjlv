#ifndef TABWIDGET_H
#define TABWIDGET_H

#include "extension.h"

class LogView;

class TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    explicit TabWidget(QWidget *parent = 0);

    void openZipBundle(const QString& zipBundle, const QString& crashInfo);
    void openZipBundle(const QString& path);
    void openRawLogFile(const QStringList& paths);
    void openFolder(const QString& path, bool installed);

    void filter(const QString& keyword);

    void inputKeyword();
    void clearKeyword();
    void enableRegexpMode(bool enabled);
    void searchFieldContent();
    void searchFieldID();
    void searchFieldDateTime();
    void searchFieldThread();
    void searchFieldCategory();
    void searchFieldSourceFile();
    void searchFieldMethod();
    void searchFieldLogFile();
    void searchFieldLine();
    void searchFieldLevel();
signals:
    void statusBarMessage(const QString&);

public slots:
    void onCloseAll();
    void onCloseAllButThis();
    void onCloseCurrent();
    void onTabCloseRequested(int index);

    void onCopyCurrentCell();
    void onCopyCurrentRow();
    void onCopySelectedCells();
    void onCopySelectedRows();
    void onCopyCurrentCellWithXMLFormatted();
    void onCopyCurrentRowWithXMLFormatted();
    void onCopySelectedCellsWithXMLFormatted();
    void onCopySelectedRowsWithXMLFormatted();

    void onAddCurrentRowToBookmark();
    void onRemoveCurrentRowFromBookmark();
    void onAddSelectedRowsToBookmark();
    void onRemoveSelectedRowsFromBookmark();
    void onRemoveAllBookmarks();
    void onGotoFirstBookmark();
    void onGotoPreviousBookmark();
    void onGotoNextBookmark();
    void onGotoLastBookmark();

    void onCopyFileName();
    void onCopyFileFullPath();
    void onOpenContainerFolder();
    void onCloneTab();

    void onScrollToTop();
    void onScrollToBottom();

    void onReload();
    void onGotoById();

    void onRunExtension(ExtensionPtr e);
    void onExtensionActionTriggered();
private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void onCurrentChanged(int index);
    void onLogViewRowCountChanged();
    
private:
    int findTab(const QString& path);
    int findTab(const QStringList& paths);
    int addTab(LogView *w, const QString& text, const QString& tooltip);
};

#endif // TABWIDGET_H
