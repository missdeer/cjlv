#include "stdafx.h"
#include "ShellContextMenu.h"


#define MIN_ID 1
#define MAX_ID 10000

class CItemIdListReleaser {
	public:
	explicit CItemIdListReleaser(ITEMIDLIST* i) : p_i(i) {}
	~CItemIdListReleaser() { 
		LPMALLOC lpMalloc = NULL;
		SHGetMalloc (&lpMalloc);
		lpMalloc->Free (p_i);
		}
	private:
	ITEMIDLIST* p_i;
};
class CComInterfaceReleaser {
	public:
	explicit CComInterfaceReleaser(IUnknown* i) : p_i(i) {}
	~CComInterfaceReleaser() { p_i->Release();}
	private:
	IUnknown* p_i;
};
	
void CShellContextMenu::ShowContextMenu(QMenu *menu, QWidget* parent, QPoint &pt, QString path)
{	
	ITEMIDLIST * id = 0;
	HRESULT result = SHParseDisplayName(path.replace(QChar('/'), QChar('\\')).toStdWString().c_str(), 0, &id, 0, 0);
	if (!SUCCEEDED(result) || !id)
		return;
	CItemIdListReleaser idReleaser (id);
	IShellFolder * ifolder = 0;

	LPCITEMIDLIST idChild = 0;
	result = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChild);
	if (!SUCCEEDED(result) || !ifolder)
		return ;
	CComInterfaceReleaser ifolderReleaser (ifolder);

	IContextMenu * imenu = 0;
	result = ifolder->GetUIObjectOf((HWND)parent->winId(), 1, (const ITEMIDLIST **)&idChild, IID_IContextMenu, 0, (void**)&imenu);
	if (!SUCCEEDED(result) || !ifolder)
		return ;
	CComInterfaceReleaser menuReleaser(imenu);

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
