#include "stdafx.h"

#include "ShellContextMenu.h"

#include "scopedguard.h"

#define MIN_ID 1
#define MAX_ID 10000

void CShellContextMenu::ShowContextMenu(QMenu *menu, QWidget *parent, QPoint &pt, QString path)
{
    ITEMIDLIST *id     = nullptr;
    HRESULT     result = SHParseDisplayName(QDir::toNativeSeparators(path).toStdWString().c_str(), 0, &id, 0, 0);
    if (!SUCCEEDED(result) || !id)
        return;
    ScopedGuard sg1([id]() { 
        LPMALLOC lpMalloc = nullptr;
        SHGetMalloc(&lpMalloc);
        lpMalloc->Free(id);
	});
    IShellFolder *ifolder = nullptr;

    LPCITEMIDLIST idChild = nullptr;
    result                = SHBindToParent(id, IID_IShellFolder, (void **)&ifolder, &idChild);
    if (!SUCCEEDED(result) || !ifolder)
        return;
    ScopedGuard sg2([ifolder](){ ifolder->Release();});

    IContextMenu *imenu = nullptr;
    result              = ifolder->GetUIObjectOf((HWND)parent->winId(), 1, (const ITEMIDLIST **)&idChild, IID_IContextMenu, 0, (void **)&imenu);
    if (!SUCCEEDED(result) || !ifolder)
        return;
    ScopedGuard sg3([imenu](){ imenu->Release();});

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;
    if (SUCCEEDED(imenu->QueryContextMenu(hMenu, 0, MIN_ID, MAX_ID, CMF_NORMAL)))
    {
        QList<QAction *> actions = menu->actions();
        UINT_PTR         id      = MAX_ID + 1;
        ::AppendMenuW(hMenu, MF_SEPARATOR, id++, nullptr);
        for (QAction *action : actions)
        {
            if (action->text().isEmpty())
                ::AppendMenuW(hMenu, MF_SEPARATOR, id++, nullptr);
            else
                ::AppendMenuW(hMenu, MF_STRING, id++, action->text().toStdWString().c_str());
        }
        int iCmd = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x(), pt.y(), (HWND)parent->winId(), nullptr);
        if (iCmd > 0)
        {
            if (iCmd > MAX_ID + 1)
            {
                QAction *action = actions.at(iCmd - MAX_ID - 1 - 1);
                if (action)
                    emit action->triggered();
            }
            else
            {
                CMINVOKECOMMANDINFOEX info = {0};
                info.cbSize                = sizeof(info);
                info.fMask                 = CMIC_MASK_UNICODE;
                info.hwnd                  = (HWND)parent->winId();
                info.lpVerb                = MAKEINTRESOURCEA(iCmd - 1);
                info.lpVerbW               = MAKEINTRESOURCEW(iCmd - 1);
                info.nShow                 = SW_SHOWNORMAL;
                imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);
            }
        }
    }
    DestroyMenu(hMenu);
}
