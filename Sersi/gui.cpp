/*  Sersi - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jerome <darksage@darksage.info>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "gui.h"
#include "service.h"
#include "common.h"
#include "resource.h"
#include "main.h"
#include "sersiConf.h"
#include "registry.h"
#include <shlobj.h>

#define IDC_BTN_INSTALL		1
#define IDC_BTN_UNINSTALL	2
#define IDC_BTN_STOP		3
#define IDC_BTN_START		4

#define TREE_IMG_ROOT		0
#define TREE_IMG_EXE		1
#define TREE_IMG_DIR		2
#define TREE_IMG_PARAM		3
#define TREE_IMG_RESTART	4

HINSTANCE g_hInst	= NULL;
HWND g_hwndMain		= NULL;
HWND hBtnInstall, hBtnUninstall, hBtnStart, hBtnStop, hBtnQuit, hList;
wchar_t* editBuffer = NULL;

/* ------------------------------------------------------------------------------------------------- */

#define CreateButton(text,x,y,w,h) CreateWindowEx(WS_EX_APPWINDOW , L"BUTTON", text, WS_CHILD | WS_VISIBLE, x, y, w, h, g_hwndMain, NULL, g_hInst, NULL)
#define CreateList(x,y,w,h) CreateWindowEx(WS_EX_CLIENTEDGE , WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE  | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS, x, y, w, h, g_hwndMain, (HMENU)ID_TREEVIEW, g_hInst, NULL)

/* ------------------------------------------------------------------------------------------------- */

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static UINT s_uTaskBarCreated = 0;

	switch(uMsg)
	{
		case WM_CREATE:
			g_hwndMain = hwnd;

			SET_RESSOURCE_ICON(ICON_BIG, IDI_MAIN_ICON);
			SET_RESSOURCE_ICON(ICON_SMALL, IDI_MAIN_ICON);

			createWindow();

			ShowWindow(g_hwndMain, 1);
			UpdateWindow(g_hwndMain);

			updateWindow();

			// Clean the memory in order to have a light application
			FlushMemory();
			break;

		case WM_COMMAND:
			switch(wParam)
			{
				case BN_CLICKED:
					if( (HWND)lParam == hBtnInstall )
					{
						InstallService();
						updateWindow();
					}
					else if( (HWND)lParam == hBtnUninstall )
					{
						UninstallService();
						updateWindow();
					}
					else if( (HWND)lParam == hBtnStart )
					{
						GuiProStartService();
						updateWindow();
					}
					else if( (HWND)lParam == hBtnStop )
					{
						GuiProStopService();
						updateWindow();
					}
					else if( (HWND)lParam == hBtnQuit )
					{
						PostQuitMessage(0);
					}
					break;
				
				case IDM_MENU_NEW_ELEM:
					{
						wchar_t* filename = BrowseForFile(NULL);
						if( filename != NULL )
						{
							wchar_t* progname = wcsrchr(filename, L'\\');
							progname++;
							if( regCreateKey(progname) )
							{
								SetStrOptionValue(GPS_KEY_EXE, progname, filename, (DWORD)(wcslen(filename) * sizeof(wchar_t)));
							}
							progname = NULL;
							free(filename);
							updateWindow();
						}
					}
					break;

				case IDM_MENU_REM_ELEM:
					{
						if( MessageBox(g_hwndMain, L"Confirm delete ?", L"Remove element", MB_YESNO) == IDYES )
						{
							struct tvItem* item = getTreeViewItem();
							regDeleteKey(item->name);
							free(item->name);
							free(item);
							updateWindow();
						}
					}
					break;

				case IDM_MENU_SET_PROGRAM:
					{
						struct tvItem* item = getTreeViewItem();
						wchar_t* oldFile = GetStrOptionValue(GPS_KEY_EXE, item->name);
						wchar_t* filename = BrowseForFile( oldFile );
						if( filename != NULL )
						{
							SetStrOptionValue(GPS_KEY_EXE, item->name, filename, (DWORD)(wcslen(filename) * sizeof(wchar_t)));
							updateWindow();
						}
						free(item->name);
						free(item);
						free(oldFile);
						free(filename);
					}
					break;

				case IDM_MENU_SET_DIRECTORY:
					{
						struct tvItem* item = getTreeViewItem();
						wchar_t* oldDir = GetStrOptionValue(GPS_KEY_DIR, item->name);
						wchar_t* directory = BrowseForDirectory( oldDir );
						if( directory != NULL )
						{
							SetStrOptionValue(GPS_KEY_DIR, item->name, directory, (DWORD)(wcslen(directory) * sizeof(wchar_t)));
							updateWindow();
						}
						free(item->name);
						free(item);
						free(oldDir);
						free(directory);
					}
					break;

				case IDM_MENU_REM_DIRECTORY:
					{
						struct tvItem* item = getTreeViewItem();
						RemOption(GPS_KEY_DIR, item->name);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;

				case IDM_MENU_SET_PARAMETER:
					{
						struct tvItem* item = getTreeViewItem();
						wchar_t* oldParam = GetStrOptionValue(GPS_KEY_PARAM, item->name);
						if( oldParam == NULL || oldParam[0] == L'\0' )
						{
							SetStrOptionValue(GPS_KEY_PARAM, item->name, L" ", (DWORD)sizeof(wchar_t));
							updateWindow();
						}
						else
						{
							free(oldParam);
						}

						HTREEITEM hitem = selectTreeViewItem(item->name, ITEM_PARAM);
						if( hitem ) {
							TreeView_SelectItem(hList, hitem);
							TreeView_EditLabel(hList, hitem ); 
						}

						free(item->name);
						free(item);
					}
					break;

				case IDM_MENU_REM_PARAMETER:
					{
						struct tvItem* item = getTreeViewItem();
						RemOption(GPS_KEY_PARAM, item->name);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;

				case IDM_MENU_REM_AUTORESTART:
					{
						struct tvItem* item = getTreeViewItem();
						RemOption(GPS_KEY_AUTORESTART, item->name);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;

				case IDM_MENU_SET_AUTORESTART:
					{
						struct tvItem* item = getTreeViewItem();
						SetDwOptionValue(GPS_KEY_AUTORESTART, item->name, 1);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;

				case IDM_MENU_REM_DAILYRESTART:
					{
						struct tvItem* item = getTreeViewItem();
						RemOption(GPS_KEY_DAILYRESTART, item->name);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;

				case IDM_MENU_SET_DAILYRESTART:
					{
						struct tvItem* item = getTreeViewItem();
						SetDwOptionValue(GPS_KEY_DAILYRESTART, item->name, 1);
						free(item->name);
						free(item);
						updateWindow();
					}
					break;
			}
			break;

		case WM_DESTROY:
			if( editBuffer != NULL )
				free(editBuffer);

			PostQuitMessage(0);
			break;

		case WM_NOTIFY:
			if( wParam == ID_TREEVIEW )
			{
				switch( ((LPNMHDR)lParam)->code )
				{
					case NM_RCLICK:
						{
							POINT point;
							TVHITTESTINFO ht = {0};
							GetCursorPos(&point);
							ht.pt.x = point.x;
							ht.pt.y = point.y;
							MapWindowPoints( HWND_DESKTOP, ((LPNMHDR)lParam)->hwndFrom, &ht.pt, 1);
							HTREEITEM item = TreeView_HitTest(((LPNMHDR)lParam)->hwndFrom, &ht);
							TreeViewMenu(item, ht.flags);		
							return 0;
						}
						break;

					case TVN_BEGINLABELEDIT:
						{
							LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO)lParam;
							
							//-> store text somewhere
							if( editBuffer != NULL )
								free(editBuffer);
							editBuffer = _wcsdup(ptvdi->item.pszText);

							if( (ptvdi->item.lParam == ITEM_ROOT) || (ptvdi->item.lParam == ITEM_PARAM) )
							{
								// Continu edition
								return FALSE;
							}
							else if( ptvdi->item.lParam == ITEM_DIR )
							{
								//-> Open the special box (cancel will be done just after)
								SendMessage(g_hwndMain, WM_COMMAND, IDM_MENU_SET_DIRECTORY, NULL);
								
							}
							else if( ptvdi->item.lParam == ITEM_EXE )
							{
								//-> Open the special box (cancel will be done just after)
								SendMessage(g_hwndMain, WM_COMMAND, IDM_MENU_SET_PROGRAM, NULL);
							}
							return TRUE;
						}
						break;

					case TVN_ENDLABELEDIT:
						{
							int ret = TRUE;
							LPNMTVDISPINFO ptvdi = (LPNMTVDISPINFO)lParam;
							
							//-> Validation part
							if( ptvdi->item.lParam == ITEM_ROOT )
							{
								if( ptvdi->item.pszText != NULL )
								{
									struct tvItem* item = getTreeViewItem();
									if( !regRenameKey(editBuffer, ptvdi->item.pszText) )
									{
										ret = FALSE;
									}
									free(item->name);
									free(item);
								}
								else
								{
									ret = FALSE;
								}
							}
							else if( ptvdi->item.lParam == ITEM_PARAM )
							{
								if( ptvdi->item.pszText != NULL )
								{
									struct tvItem* item = getTreeViewItem();
									SetStrOptionValue(GPS_KEY_PARAM, item->name, ptvdi->item.pszText, (DWORD)(wcslen(ptvdi->item.pszText) * sizeof(wchar_t)) );
									free(item->name);
									free(item);
								}
								else
								{
									ret = FALSE;
								}
							}

							if( editBuffer != NULL )
							{
								free(editBuffer);
								editBuffer = NULL;
							}
							return ret;
						}
						break;

					case TVN_KEYDOWN:
						{
							LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN) lParam;
							if( ptvkd->wVKey == VK_F2 )
							{
								HTREEITEM hitem = TreeView_GetSelection(hList);
								if( hitem )
								{
									TreeView_EditLabel(hList, hitem );
								}
								return 1;
							}
							else if( ptvkd->wVKey == VK_F5 )
							{
								updateWindow();
							}
						}
						break;
				}
			}
			return(DefWindowProc(hwnd, uMsg, wParam, lParam));

		default:
			return(DefWindowProc(hwnd, uMsg, wParam, lParam));
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void createWindow()
{
	hBtnInstall		= CreateButton(L"Install Service", 5, 5, 160, 25);
	hBtnUninstall	= CreateButton(L"Uninstall Service", 5, 40, 160, 25);
	hBtnStart		= CreateButton(L"Start Service", 5, 90, 160, 25);
	hBtnStop		= CreateButton(L"Stop Service", 5, 125, 160, 25);
	hBtnQuit		= CreateButton(L"Quit", 5, 180, 160, 25);

	InitCommonControls();
	hList			= CreateList(170, 5, 345, 260);

	HINSTANCE dllhinst;
	HICON hIcon;
	HIMAGELIST himgList;
	
	himgList = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_COLOR32 , 4, 4);
	ImageList_SetBkColor(himgList, GetSysColor(COLOR_WINDOW));

	dllhinst = LoadLibrary( L"shell32.dll" );
	// 0 : ROOT
	hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(13));
	ImageList_AddIcon(himgList, hIcon);
	// 1 : EXE
	hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(3));
	ImageList_AddIcon(himgList, hIcon);
	// 2 : DIR
	hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(4));
	ImageList_AddIcon(himgList, hIcon);
	// 3 : PARAM
	hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(22));
	ImageList_AddIcon(himgList, hIcon);
	// 4 : Restart
	hIcon = LoadIcon(dllhinst, MAKEINTRESOURCE(48));
	ImageList_AddIcon(himgList, hIcon);

	FreeLibrary(dllhinst);

	TreeView_SetImageList(hList, himgList, TVSIL_NORMAL);
}

/* ------------------------------------------------------------------------------------------------- */

void TreeViewMenu(HTREEITEM item, UINT flags)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hmenu = NULL;

	// New Item
	hmenu = CreatePopupMenu();

	if( item == NULL )
	{		
		// Nothing to add
		AppendMenu(hmenu, MF_STRING, IDM_MENU_NEW_ELEM, GPS_MENU_NEWELEMENT);
	}
	else
	{
		TreeView_SelectItem(hList, item);

		TVITEM tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_TEXT | TVIF_PARAM;
		tvi.cchTextMax = 255;
		tvi.pszText = (LPWSTR)malloc(sizeof(WCHAR)*255);

		if( TreeView_GetItem(hList, &tvi) )
		{
			//
			// Retreive the Name of the root element
			//
			LPWSTR name = NULL;
			if( tvi.lParam == ITEM_ROOT )
			{
				name = _wcsdup(tvi.pszText);
			}
			else if( tvi.lParam >= ITEM_EXE && tvi.lParam <= ITEM_DAILYRESTART )
			{
				HTREEITEM rootItem = TreeView_GetParent(hList, item);
				TVITEM tviRoot;
				tviRoot.hItem = rootItem;
				tviRoot.mask = TVIF_TEXT;
				tviRoot.cchTextMax = 255;
				tviRoot.pszText = (LPWSTR)malloc(sizeof(WCHAR)*255);
				if( TreeView_GetItem(hList, &tviRoot) )
				{
					name = _wcsdup(tviRoot.pszText);
				}
				free( tviRoot.pszText );
			}

			// Get Element in the list using his name.

			struct SersiConf* conf = loadConf(name);

			// ---

			if( tvi.lParam == ITEM_ROOT )
			{
				// ROOT
				AppendMenu(hmenu, MF_STRING, IDM_MENU_NEW_ELEM, GPS_MENU_NEWELEMENT);
				AppendMenu(hmenu, MF_SEPARATOR, 0, L"");
				AppendMenu(hmenu, MF_STRING, IDM_MENU_REM_ELEM, GPS_MENU_REMELEMENT);
				AppendMenu(hmenu, MF_SEPARATOR, 0, L"");
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_PROGRAM, GPS_MENU_SETPROG);
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_DIRECTORY, GPS_MENU_SETDIR);
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_PARAMETER, GPS_MENU_SETPARAM);
				AppendMenu(hmenu, MF_SEPARATOR, 0, L"");

				if( conf->autoRestart )
					AppendMenu(hmenu, MF_STRING, IDM_MENU_REM_AUTORESTART, GPS_MENU_REMAUTORESTART);
				else
					AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_AUTORESTART, GPS_MENU_SETAUTORESTART);

				if( conf->dailyRestart )
					AppendMenu(hmenu, MF_STRING, IDM_MENU_REM_DAILYRESTART, GPS_MENU_REMDAILYRESTART);
				else
					AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_DAILYRESTART, GPS_MENU_SETDAILYRESTART);
			}
			else if( tvi.lParam == ITEM_EXE )
			{
				// EXE
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_PROGRAM, GPS_MENU_SETPROG);
			}
			else if( tvi.lParam == ITEM_DIR )
			{
				// DIRECTORY
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_DIRECTORY, GPS_MENU_SETDIR);
				AppendMenu(hmenu, MF_STRING, IDM_MENU_REM_DIRECTORY, GPS_MENU_REMDIR);
			}
			else if( tvi.lParam == ITEM_PARAM )
			{
				// PARAM
				AppendMenu(hmenu, MF_STRING, IDM_MENU_SET_PARAMETER, GPS_MENU_SETPARAM);
				AppendMenu(hmenu, MF_STRING, IDM_MENU_REM_PARAMETER, GPS_MENU_REMPARAM);
			}

			free( tvi.pszText );
			unloadConf(conf);
		}
	}

	if( (UINT)TrackPopupMenu(hmenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, g_hwndMain, NULL) == 0)
	{
		// error
		DestroyMenu(hmenu);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void insertItem(const wchar_t* itemName, const wchar_t* progname, const wchar_t* dirname, const wchar_t* params, const DWORD autoRestart, const DWORD dailyRestart)
{
	TVINSERTSTRUCT tviis;
	HTREEITEM hitem;

	tviis.hInsertAfter = TVI_LAST;
	ZeroMemory(&(tviis.item), sizeof(TV_ITEM));
	tviis.item.mask  = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	tviis.hParent = TVI_ROOT;
	tviis.item.lParam = ITEM_ROOT;
	tviis.item.iImage = TREE_IMG_ROOT;
	tviis.item.iSelectedImage = TREE_IMG_ROOT;
	tviis.item.pszText = (LPWSTR)itemName;
	hitem = TreeView_InsertItem(hList, &tviis);

	tviis.hParent = hitem ;
	tviis.item.lParam = ITEM_EXE;
	tviis.item.iImage = TREE_IMG_EXE;
	tviis.item.iSelectedImage = TREE_IMG_EXE;
	tviis.item.pszText = (LPWSTR)progname;
	TreeView_InsertItem(hList, &tviis);

	if( dirname != NULL ) 
	{
		tviis.item.lParam = ITEM_DIR;
		tviis.item.iImage = TREE_IMG_DIR;
		tviis.item.iSelectedImage = TREE_IMG_DIR;
		tviis.item.pszText = (LPWSTR)dirname;
		TreeView_InsertItem(hList, &tviis);
	}

	if( params != NULL ) 
	{
		tviis.item.lParam = ITEM_PARAM;
		tviis.item.iImage = TREE_IMG_PARAM;
		tviis.item.iSelectedImage = TREE_IMG_PARAM;
		tviis.item.pszText = (LPWSTR)params;
		TreeView_InsertItem(hList, &tviis);
	}

	if( autoRestart == 1 )
	{
		tviis.item.lParam = ITEM_AUTORESTART;
		tviis.item.iImage = TREE_IMG_RESTART;
		tviis.item.iSelectedImage = TREE_IMG_RESTART;
		tviis.item.pszText = STR_AUTO_RESTART;
		TreeView_InsertItem(hList, &tviis);
	}

	if( dailyRestart == 1 )
	{
		tviis.item.lParam = ITEM_DAILYRESTART;
		tviis.item.iImage = TREE_IMG_RESTART;
		tviis.item.iSelectedImage = TREE_IMG_RESTART;
		tviis.item.pszText = STR_DAILY_RESTART;
		TreeView_InsertItem(hList, &tviis);
	}

	TreeView_Expand(hList, hitem, TVM_EXPAND);
}

/* ------------------------------------------------------------------------------------------------- */

struct tvItem* getTreeViewItem()
{
	struct tvItem* ret = NULL;
	HTREEITEM item = TreeView_GetSelection(hList);
	if( item )
	{
		TVITEM tvi;
		tvi.hItem = item;
		tvi.mask = TVIF_TEXT | TVIF_PARAM;
		tvi.cchTextMax = 255;
		tvi.pszText = (LPWSTR)malloc(sizeof(WCHAR)*255);

		if( TreeView_GetItem(hList, &tvi) )
		{
			//
			// Retreive the Name of the root element
			//
			ret = (struct tvItem*)malloc(sizeof(struct tvItem));
			ret->name = NULL;
			ret->param = tvi.lParam;
			if( tvi.lParam == ITEM_ROOT )
			{
				ret->name = _wcsdup(tvi.pszText);
			}
			else if( tvi.lParam >= ITEM_EXE && tvi.lParam <= ITEM_DAILYRESTART )
			{				
				HTREEITEM rootItem = TreeView_GetParent(hList, item);
				TVITEM tviRoot;
				tviRoot.hItem = rootItem;
				tviRoot.mask = TVIF_TEXT;
				tviRoot.cchTextMax = 255;
				tviRoot.pszText = (LPWSTR)malloc(sizeof(WCHAR)*255);
				if( TreeView_GetItem(hList, &tviRoot) )
				{
					ret->name = _wcsdup(tviRoot.pszText);
				}
				free( tviRoot.pszText );
			}
		}
	}
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

HTREEITEM selectTreeViewItem(wchar_t* progname, LPARAM type)
{
	HTREEITEM item = TreeView_GetRoot(hList);
	HTREEITEM progItem = NULL;
	if( item )
	{
		TVITEM tvi;
		tvi.mask = TVIF_TEXT | TVIF_PARAM;
		tvi.cchTextMax = 255;
		tvi.pszText = (LPWSTR)malloc(sizeof(WCHAR)*255);

		while( item )
		{
			tvi.hItem = item;
			if( TreeView_GetItem(hList, &tvi) )
			{
				if( !wcscmp(tvi.pszText, progname) )
				{
					progItem = item;
					item = NULL;
				}
				else
				{
					item = TreeView_GetNextSibling(hList, item);
				}
			}
			else
			{
				item = NULL;
			}
		}

		if( progItem )
		{
			if( tvi.lParam == type )
			{
				free( tvi.pszText );
				return progItem;
			}
			else
			{
				item = TreeView_GetChild(hList, progItem);
				while( item )
				{
					tvi.hItem = item;
					if( TreeView_GetItem(hList, &tvi) )
					{
						if( tvi.lParam == type )
						{
							free( tvi.pszText );
							return item;
						}
						else
						{
							item = TreeView_GetNextSibling(hList, item);
						}
					}
					else
					{
						item = NULL;
					}
				}
			}
		}

		free( tvi.pszText );
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch( uMsg )
	{
		case BFFM_INITIALIZED:
				if( lpData != NULL )
					SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			break;
	}
	return 0;
}

wchar_t* BrowseForDirectory(wchar_t* directory)
{
	BROWSEINFO lpbi = { 0 };
	lpbi.hwndOwner = g_hwndMain;
	lpbi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_NONEWFOLDERBUTTON;
	lpbi.lpfn = (BFFCALLBACK)BrowseCallbackProc;
	lpbi.pidlRoot = NULL;
	lpbi.lParam = (LPARAM)directory;
	
	LPITEMIDLIST pidl = SHBrowseForFolder(&lpbi);

	wchar_t path[MAX_PATH];
	if( SHGetPathFromIDList(pidl, path) )
	{
		CoTaskMemFree(pidl);
		return wcsdup(path);
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* BrowseForFile(wchar_t* filename)
{
	OPENFILENAME opf;
	wchar_t* szFile;
	
	szFile = (wchar_t*)malloc(sizeof(wchar_t) * 260);
	ZeroMemory(szFile, sizeof(wchar_t) * 260);

	ZeroMemory(&opf, sizeof(opf));
	opf.lStructSize = sizeof(opf);
	opf.hwndOwner = g_hwndMain;
	opf.lpstrFile = szFile;
	opf.nMaxFile = 260;
	opf.lpstrFilter = L"Exe (*.exe)\0*.exe\0All (*.*)\0*.*\0";
	opf.nFilterIndex = 1;
	opf.lpstrFileTitle = NULL;
	opf.nMaxFileTitle = 0;
	opf.lpstrInitialDir = filename;
	opf.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if( GetOpenFileName(&opf) != TRUE )
	{
		free(szFile);
		szFile = NULL;
	}

	return szFile;
}

/* ------------------------------------------------------------------------------------------------- */

void updateWindow()
{
	if( GuiProInServiceInstalled() )
	{
		EnableWindow( hBtnInstall, false );
		EnableWindow( hBtnUninstall, true );
		if( GuiProInServiceStarted() )
		{
			EnableWindow( hBtnStart, false );
			EnableWindow( hBtnStop, true );
		}
		else
		{
			EnableWindow( hBtnStart, true );
			EnableWindow( hBtnStop, false );
		}
	}
	else
	{
		EnableWindow( hBtnInstall, true );
		EnableWindow( hBtnUninstall, false );
		EnableWindow( hBtnStart, false );
		EnableWindow( hBtnStop, false );
	}
	
	TreeView_DeleteAllItems(hList);
	check_config = true;
	DoService();
	check_config = false;
}