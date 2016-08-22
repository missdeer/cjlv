#include "stdafx.h"
#include "everythingwrapper.h"


HRESULT ResolveIt(HWND hwnd, LPCSTR lpszLinkFile, LPWSTR lpszPath, int iPathBufferSize)
{
    HRESULT hres;
    IShellLink* psl;
    WCHAR szGotPath[MAX_PATH];
    WCHAR szDescription[MAX_PATH];
    WIN32_FIND_DATA wfd;

    *lpszPath = 0; // Assume failure

    CoInitialize(NULL);
    // Get a pointer to the IShellLink interface. It is assumed that CoInitialize
    // has already been called.
    hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hres))
    {
        IPersistFile* ppf;

        // Get a pointer to the IPersistFile interface.
        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);

        if (SUCCEEDED(hres))
        {
            WCHAR wsz[MAX_PATH] = {0};

            // Ensure that the string is Unicode.
            MultiByteToWideChar(CP_ACP, 0, lpszLinkFile, -1, wsz, MAX_PATH);
            // Add code here to check return value from MultiByteWideChar
            // for success.

            // Load the shortcut.
            hres = ppf->Load(wsz, STGM_READ);

            if (SUCCEEDED(hres))
            {
                // Resolve the link.
                hres = psl->Resolve(hwnd, 0);

                if (SUCCEEDED(hres))
                {
                    // Get the path to the link target.
                    hres = psl->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA*)&wfd, SLGP_RAWPATH);

                    if (SUCCEEDED(hres))
                    {
                        // Get the description of the target.
                        hres = psl->GetDescription(szDescription, MAX_PATH);

                        if (SUCCEEDED(hres))
                        {
                            hres = StringCbCopy(lpszPath, iPathBufferSize, szGotPath);
                            if (SUCCEEDED(hres))
                            {
                                // Handle success
                            }
                            else
                            {
                                // Handle the error
                            }
                        }
                    }
                }
            }

            // Release the pointer to the IPersistFile interface.
            ppf->Release();
        }

        // Release the pointer to the IShellLink interface.
        psl->Release();
    }
    return hres;
}

bool isEverythingRunning()
{
    HWND everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS,0);
    return everything_hwnd != NULL;
}

void launchEverything(const QString& everythingFilePath)
{
    if (QFile::exists(everythingFilePath))
    {
        QString path = QDir::toNativeSeparators(everythingFilePath);
        ::ShellExecuteW(NULL, L"open", path.toStdWString().c_str(), NULL, NULL, SW_SHOWMINIMIZED);
    }
}

QString GetEverythingPath()
{
    HWND hWnd = FindWindow(EVERYTHING_IPC_WNDCLASS,0);
    if (hWnd)
    {
        int ret = (int)SendMessage(hWnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_IS_DESKTOP_SHORTCUT,0);
        if (!ret)
        {
            // create one
            SendMessage(hWnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_CREATE_DESKTOP_SHORTCUT,0);
        }

        QString path = QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) % "/Search Everything.lnk");

        WCHAR szTarget[MAX_PATH] = {0};
        HRESULT hr = ResolveIt(hWnd, path.toStdString().c_str(), szTarget, sizeof(szTarget)/sizeof(szTarget[0]));
        if (!ret)
        {
            // remove the one created right now
            SendMessage(hWnd,EVERYTHING_WM_IPC,EVERYTHING_IPC_DELETE_DESKTOP_SHORTCUT,0);
        }
        if (hr == S_OK)
        {
            QFileInfo f(QString::fromWCharArray(szTarget));
            return f.absoluteFilePath();
        }
    }

    return QString();
}

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
            QString everythingFilePath = QApplication::applicationDirPath() % "/Everything.exe";
            if (!QFile::exists(everythingFilePath))
            {
                QMessageBox::warning(p,
                                     "Warning",
                                     "Can't find the Everything executable, please re-install Cisco Jabber Log Viewer.",
                                     QMessageBox::Ok);
                return false;
            }
            ::ShellExecuteW(NULL, L"open", everythingFilePath.toStdWString().c_str(), NULL, NULL, SW_SHOWMINIMIZED);
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
