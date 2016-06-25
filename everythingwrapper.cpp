#include <Windows.h>
#include <Shellapi.h>
#include <QApplication>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QFile>
#include <QDesktopWidget>
#include <Everything.h>
#include <everything_ipc.h>

bool QuickGetFilesByFileName(const QString& fileName, QStringList& results)
{
    HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS,0);
    if (!everything_hwnd)
    {
        QWidget* p =  QApplication::desktop()->screen();

        if (QMessageBox::question(p,
                             "Notice",
                             "Everything is not running, thus Cisco Jabber Log Viewer can't find files. Do you want to launch the Everything application shipped with Cisco Jabber Log Viewer?") == QMessageBox::Yes)
        {
            QString everythingFilePath = QApplication::applicationDirPath() + "/Everything.exe";
            if (!QFile::exists(everythingFilePath))
            {
                QMessageBox::warning(p,
                                     "Warning",
                                     "Can't find the Everything executable, please re-install Cisco Jabber Log Viewer.",
                                     QMessageBox::Ok);
                return false;
            }
            ::ShellExecuteW(NULL, L"open", everythingFilePath.toStdWString().c_str(), NULL, NULL, SW_SHOWNORMAL);
        }
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

    return !results.isEmpty();
}
