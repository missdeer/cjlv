#include <QApplication>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <Everything.h>
#include <everything_ipc.h>

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results)
{
    HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS,0);
    if (!everything_hwnd)
    {
        QWidgetList wl = QApplication::topLevelWidgets();
        QMessageBox::warning(wl.isEmpty() ? NULL : wl.at(0),
                             "Warning",
                             "Everything is not running, thus Cisco Jabber Log Viewer can't find files.",
                             QMessageBox::Ok);
        return false;
    }
    // find
    Everything_SetSearch(fileName.toStdWString().c_str());
    Everything_Query(TRUE);

    for(DWORD i=0;i<Everything_GetNumResults();i++)
    {
        WCHAR path[MAX_PATH] = {0};
        Everything_GetResultFullPathName(i, path, MAX_PATH);
        results.append(QString::fromStdWString(path));
    }
    Everything_Reset();

    return true;
}
