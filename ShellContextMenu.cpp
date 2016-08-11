#include "stdafx.h"
#include <boost/scope_exit.hpp>
#include "ShellContextMenu.h"


#define MIN_ID 1
#define MAX_ID 10000
	
void CShellContextMenu::ShowContextMenu(QMenu *menu, QWidget* parent, QPoint &pt, QString path)
{	
	ITEMIDLIST * id = 0;
	HRESULT result = SHParseDisplayName(path.replace(QChar('/'), QChar('\\')).toStdWString().c_str(), 0, &id, 0, 0);
	if (!SUCCEEDED(result) || !id)
        return;
    BOOST_SCOPE_EXIT(id) {
        LPMALLOC lpMalloc = NULL;
        SHGetMalloc (&lpMalloc);
        lpMalloc->Free (id);
    } BOOST_SCOPE_EXIT_END
	IShellFolder * ifolder = 0;

	LPCITEMIDLIST idChild = 0;
	result = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChild);
	if (!SUCCEEDED(result) || !ifolder)
        return ;
    BOOST_SCOPE_EXIT(ifolder) {
        ifolder->Release();
    } BOOST_SCOPE_EXIT_END

	IContextMenu * imenu = 0;
	result = ifolder->GetUIObjectOf((HWND)parent->winId(), 1, (const ITEMIDLIST **)&idChild, IID_IContextMenu, 0, (void**)&imenu);
	if (!SUCCEEDED(result) || !ifolder)
        return ;
    BOOST_SCOPE_EXIT(imenu) {
        imenu->Release();
    } BOOST_SCOPE_EXIT_END

	HMENU hMenu = CreatePopupMenu();
	if (!hMenu)
		return ;
	if (SUCCEEDED(imenu->QueryContextMenu(hMenu, 0, MIN_ID, MAX_ID, CMF_NORMAL)))
	{		
		QList<QAction *> actions = menu->actions();
		UINT_PTR id = MAX_ID + 1;
		::AppendMenuW(hMenu, MF_SEPARATOR, id++, 0);
		Q_FOREACH(QAction* action, actions)
		{
			if (action->text().isEmpty())
				::AppendMenuW(hMenu, MF_SEPARATOR, id++, 0);
			else
				::AppendMenuW(hMenu, MF_STRING, id++, action->text().toStdWString().c_str());
		}
		int iCmd = ::TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pt.x(), pt.y(), (HWND)parent->winId(), NULL);
		if (iCmd > 0)
		{
			if (iCmd > MAX_ID + 1)
			{
				QAction* action = actions.at(iCmd - MAX_ID - 1 -1);
				if (action)
					emit action->triggered();
			}
			else
			{
				CMINVOKECOMMANDINFOEX info = { 0 };
				info.cbSize = sizeof(info);
				info.fMask = CMIC_MASK_UNICODE;
				info.hwnd = (HWND)parent->winId();
				info.lpVerb  = MAKEINTRESOURCEA(iCmd - 1);
				info.lpVerbW = MAKEINTRESOURCEW(iCmd - 1);
				info.nShow = SW_SHOWNORMAL;
				imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);				
			}
		}
	}
	DestroyMenu(hMenu);
}
