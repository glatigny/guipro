/*
	PortAL - GUIPro Project ( http://guipro.sourceforge.net/ )

	Author : Glatigny Jérôme <jerome@obsidev.com> - http://www.obsidev.com/

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

// Include common first
#include "common.h"
#include <map>
// Then include others
#include "menu.h"
#include "main.h"
#include "resource.h"
#include "trayIcon.h"
#include "launch.h"
#include "config.h"
#include "iconManager.h"
#include "hotKey.h"

/* ------------------------------------------------------------------------------------------------- */

BOOL g_activeMenu = FALSE;

LOGFONT g_localFont;
SIZE g_iconSize;
DWORD g_menuBackColor;
DWORD g_menuBackTextColor;
DWORD g_ShotcutColor;
DWORD g_TextColor;
std::map<HMENU,PortalProg*> menus;

PortalMenuItem* g_PortalMenuItem = NULL;

/* ------------------------------------------------------------------------------------------------- */

void InitMenuVars()
{
	NONCLIENTMETRICS tNCMetrics;

	tNCMetrics.cbSize = sizeof(NONCLIENTMETRICS);

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &tNCMetrics, 0);

	g_localFont = tNCMetrics.lfMenuFont;
	g_iconSize.cx = GetSystemMetrics(SM_CXSMICON);
	g_iconSize.cy = GetSystemMetrics(SM_CYSMICON);
	//g_menuBackColor = GetSysColor(COLOR_HIGHLIGHT);
	g_menuBackColor = GetSysColor(COLOR_MENUHILIGHT);
	g_menuBackTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
	g_ShotcutColor = GetSysColor(COLOR_GRAYTEXT);
	g_TextColor = GetSysColor(COLOR_MENUTEXT);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowTrayMenu(PortalProg* p_menu, int param)
{
	if( g_activeMenu == TRUE )
	{
		return;
	}

	POINT pt;
	GetCursorPos(&pt);

	menus.clear();
	HMENU hmenu = MyCreateMenu(p_menu, param);

	if( hmenu )
	{
		menus[hmenu] = p_menu;

		if( param == PORTAL_MENU_SYSTRAY_AUTO )
		{
			MENUINFO tMenuInfo;
			ZeroMemory(&tMenuInfo, sizeof(tMenuInfo));
			tMenuInfo.cbSize = sizeof(tMenuInfo);
			tMenuInfo.fMask = MIM_STYLE;
			tMenuInfo.dwStyle = MNS_AUTODISMISS;
			SetMenuInfo(hmenu, &tMenuInfo);
		}
		
		SetForegroundWindow(g_hwndMain);
		
		UINT flags;
		if( param != PORTAL_MENU_HOTKEY ) 
		{
			flags = TPM_RIGHTBUTTON | TPM_RIGHTALIGN;
		} 
		else 
		{
			flags = TPM_RIGHTBUTTON | TPM_CENTERALIGN;
		}

		g_activeMenu = TRUE;

		BOOL ret = TrackPopupMenu(hmenu, flags, pt.x, pt.y, TPM_RETURNCMD, g_hwndMain, NULL);
		ret = (ret == 0);

		EndMenu();
		DestroyMenu(hmenu);
		DestroyMyMenu(ret);
	}
	menus.clear();
	FlushMemory();
	g_activeMenu = FALSE;
}

/* ------------------------------------------------------------------------------------------------- */

BOOL AddMenuItem(HMENU hMenu, UINT uItemID, UINT uType, UINT uState, PortalMenuItem* item)
{
	if( item == NULL ) return FALSE;
	MENUITEMINFO tMenuItemInfo;

	ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
	tMenuItemInfo.fType = uType;
	tMenuItemInfo.wID = uItemID;
	tMenuItemInfo.dwTypeData = item->text;
	tMenuItemInfo.dwItemData = (ULONG_PTR)item;
	if( uState != 0 )
	{
		tMenuItemInfo.fMask |= MIIM_STATE;
		tMenuItemInfo.fState = uState;
	}

	return InsertMenuItem(hMenu, uItemID, FALSE, &tMenuItemInfo);
}

/* ------------------------------------------------------------------------------------------------- */

BOOL AddSubMenu(HMENU hMenu, UINT uItemID, UINT uType, UINT uState, HMENU subMenu, PortalMenuItem* item)
{
	if( item == NULL ) return FALSE;
	MENUITEMINFO tMenuItemInfo;

	item->options |= MENU_SUB;

	ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA | MIIM_SUBMENU;
	tMenuItemInfo.fType = uType;
	tMenuItemInfo.wID = uItemID;
	tMenuItemInfo.dwTypeData = item->text;
	tMenuItemInfo.dwItemData = (ULONG_PTR)item;
	tMenuItemInfo.hSubMenu = subMenu;
	if( uState != 0 )
	{
		tMenuItemInfo.fMask |= MIIM_STATE;
		tMenuItemInfo.fState = uState;
	}

	return InsertMenuItem(hMenu, uItemID, FALSE, &tMenuItemInfo);
}

/* ------------------------------------------------------------------------------------------------- */

PortalMenuItem* newPortalItem(UINT id, wchar_t* text, DWORD options, PortalMenuItem* previous, wchar_t* sctext)
{
	PortalMenuItem *l_item = (PortalMenuItem*)malloc(sizeof(PortalMenuItem));
	
	if( l_item == NULL )
		return l_item;

	ZeroMemory( l_item, sizeof(PortalMenuItem) );
	if( previous == NULL )
	{
		if( g_PortalMenuItem != NULL )
		{
			DestroyMyMenu(1);
		}
		g_PortalMenuItem = l_item;
	}
	else
	{
		previous->next = l_item;
	}

	if( text )
	{
		l_item->text = _wcsdup( text );
	}
	else
	{
		l_item->text = _wcsdup( L"" );
	}

	if( sctext )
	{
		// Do not dup it, it's already one
		l_item->sctext = sctext;
	}

	l_item->id = id;
	l_item->next = NULL;
	l_item->options = options;

	return l_item;
}

/* ------------------------------------------------------------------------------------------------- */

PortalMenuItem* insertItemMenu(PortalProg* prog, PortalMenuItem* item, HMENU hMenu, int* configElem, int* nResult)
{
	UINT iconResource;
	wchar_t* char_progname = NULL;
	wchar_t* char_sctext = NULL;
	DWORD options;
#ifndef ICON_MANAGER
	SHFILEINFO tSHFileInfo;
#endif
	UINT type = MFT_OWNERDRAW, state = 0;

	if( (prog->progExe != NULL) || (prog->progs.size() > 0) )
	{
		iconResource = NULL;
		options = MENU_NORMAL;
#ifndef ICON_MANAGER
		ZeroMemory(&tSHFileInfo, sizeof(tSHFileInfo));

		if( (prog->icoPath != NULL) && (prog->icoPath[0] != L'\0') )
		{
			wchar_t* pos = wcschr( prog->icoPath, L',');
			if( pos > 0 )
			{
				wchar_t icoPath[512];
				int i = 0;
				wcsncpy_s( icoPath, 512, prog->icoPath, size_t(pos - prog->icoPath) );
				i = _wtoi( &pos[1] );
				ExtractIconEx( icoPath, i, NULL, &tSHFileInfo.hIcon, 1 );
			}
			else
				SHGetFileInfo( prog->icoPath , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
		}
		else
		{
			if( (prog->progExe != NULL) && (prog->progExe[0] != L'|') )
			{
				SHGetFileInfo( prog->progExe , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
			}
		}
#else
		UINT iconid = (UINT)PORTAL_ICON_RESOURCE;
		if( (prog->icoPath != NULL) && (prog->icoPath[0] != L'\0') )
		{
			iconid = preloadIcon(prog->icoPath);
		}
		else
		{
			if( (prog->progExe != NULL) && (prog->progExe[0] != L'|') )
			{
				iconid = preloadIcon(prog->progExe);
			}
		}
#endif
		char_progname = prog->progName;

		if( prog->options & PROG_OPTION_SHOWSC )
		{
			if( prog->hkey != 0 )
			{
				options |= MENU_SHOWSC;
				char_sctext = getHotkeyText(prog->modifier, prog->hkey);
			}
		}

		// Use the special tag "-" in order to create en line separator
		// Feature which could be improve (line separator with some text)
		if( prog->progExe && prog->progExe[0] == L'|' )
		{
			if( !wcscmp( prog->progExe, L"|sep" ) )
			{
				if( char_progname == NULL )
				{
					char_progname = L"-";
					type = MFT_SEPARATOR | MFT_OWNERDRAW;
					options = MENU_SEP;
				}
				else
				{
					options = MENU_TITLE;
					type = MFT_SEPARATOR | MFT_OWNERDRAW;
				}
			}
			else if( !wcscmp( prog->progExe, L"|quit" ) )
			{
				*configElem = (*configElem | PORTAL_CFG_OPT_QUIT);

				iconResource = IDI_MAIN_ICON;

				if( char_progname == NULL )
					char_progname = WC_PORTAL_QUIT_ITEM_TEXT;

				type = MFT_OWNERDRAW;
			}
			else if( !wcscmp( prog->progExe, L"|about" ) )
			{
				*configElem = (*configElem | PORTAL_CFG_OPT_ABOUT);

				iconResource = IDI_MAIN_ICON;

				if( char_progname == NULL )
					char_progname = WC_PORTAL_ABOUT_ITEM_TEXT;

				type = MFT_OWNERDRAW;
			}
		}
		else
		{
			type = MFT_OWNERDRAW;
		}

		if( prog->options & PROG_OPTION_DEFAULT )
		{
			state |= MF_DEFAULT;
		}

		if( prog->options & PROG_OPTION_BREAK )
		{
			type |= MFT_MENUBARBREAK;
		}

		item = newPortalItem( ( prog->uID + PORTAL_HK_ID ), char_progname, options, item, char_sctext);

		if( item == NULL )
		{
			*nResult = -1;
			return item;
		}

#ifndef ICON_MANAGER
		if( tSHFileInfo.hIcon != NULL )
		{
			item->icon = tSHFileInfo.hIcon;
		}
		else if( iconResource != NULL )
		{
			item->icon = LOADRESOURCEICON(iconResource);
		}
		else if( prog->progs.size() > 0 )
		{
			// Directory
			SHGetFileInfo(L"/", FILE_ATTRIBUTE_DIRECTORY, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES | SHGFI_SMALLICON);
			item->icon = tSHFileInfo.hIcon;
		}
#else
		item->iconid = PORTAL_ICON_RESOURCE;	
		if( iconid < PORTAL_ICON_RESOURCE  )
		{
			item->iconid = iconid;
		}
		else if( iconResource != NULL )
		{
			item->iconid = PORTAL_ICON_RESOURCE | iconResource;
		}
		else if( prog->progs.size() > 0 )
		{
			item->iconid = preloadIcon(L"/");
		}
#endif

		if( prog->progs.size() == 0 && !(prog->options & PROG_OPTION_BROWSE) )
		{
			AddMenuItem(hMenu, ( prog->uID + PORTAL_HK_ID ), type, state, item);
		}
		else
		{
			HMENU subMenu = MyCreateSubMenu( prog );
			AddSubMenu( hMenu, ( prog->uID + PORTAL_HK_ID ), type, state, subMenu, item); 
			while( item->next != NULL )
				item = item->next;
		}
	}

	// We do not free "char_sctext" because we didn't dump it in newPortalItem

	return item;
}

/* ------------------------------------------------------------------------------------------------- */

HMENU MyCreateMenu(PortalProg* p_menu, int param)
{
	int nResult = 0;
	int configElem = 0x0;
	HMENU hMenu = NULL;	
	PortalMenuItem* item = g_PortalMenuItem;

	if( item != NULL )
	{
		while( item->next != NULL )
			item = item->next;
	}

	// Create menu
	hMenu = CreatePopupMenu();
	if(hMenu == NULL)
	{
		return NULL;
	}

	if( p_menu )
	{
		menus[hMenu] = p_menu;

		for( PortalProgVector::iterator i = p_menu->progs.begin() ; i != p_menu->progs.end() ; i++)
		{
			item = insertItemMenu( (*i), item, hMenu, &configElem, &nResult );
			if( item == NULL )
				break;
		}
	}

	if( p_menu->progExe != NULL )
	{
		item = insertItemMenu( p_menu, item, hMenu, &configElem, &nResult );
	}

	if(( param == PORTAL_MENU_SYSTRAY_CMD ) && (configElem != PORTAL_CFG_OPT))
	{
		item = newPortalItem( (IDM_EXIT + 1), L"-", MENU_SEP, item, NULL);
		AddMenuItem(hMenu, (IDM_EXIT + 1), MFT_SEPARATOR | MFT_OWNERDRAW, 0, item);

		if(!(configElem & PORTAL_CFG_OPT_ABOUT))
		{
			// Creation of "about" item
			item = newPortalItem(IDM_ABOUT, WC_PORTAL_ABOUT_ITEM_TEXT, MENU_NORMAL, item, NULL);
			if( item )
			{
#ifndef ICON_MANAGER
				item->icon = LOADRESOURCEICON(IDI_MAIN_ICON);
#else
				item->iconid = PORTAL_ICON_RESOURCE | IDI_MAIN_ICON;
#endif
			}
			AddMenuItem(hMenu, IDM_ABOUT, MFT_OWNERDRAW, 0, item);
		}

		if(!(configElem & PORTAL_CFG_OPT_QUIT))
		{
			// Creation of "quit" item
			item = newPortalItem(IDM_EXIT, WC_PORTAL_QUIT_ITEM_TEXT, MENU_NORMAL, item, NULL);
			if( item )
			{
#ifndef ICON_MANAGER
				item->icon = LOADRESOURCEICON(IDI_MAIN_ICON);
#else
				item->iconid = PORTAL_ICON_RESOURCE | IDI_MAIN_ICON;
#endif
			}
			AddMenuItem(hMenu, IDM_EXIT, MFT_OWNERDRAW, 0, item);
		}
	}

	if( nResult != 0 )
	{
		DestroyMenu(hMenu);
		hMenu = NULL;
	}
	
	return hMenu;
}

/* ------------------------------------------------------------------------------------------------- */

HMENU MyCreateSubMenu(PortalProg* p_menu)
{
	if( p_menu == NULL )
		return NULL;

	HMENU hMenu = NULL;	
	hMenu = CreatePopupMenu();
	if(hMenu == NULL)
		return NULL;
	menus[hMenu] = p_menu;

	return hMenu;
}

/* ------------------------------------------------------------------------------------------------- */

void DestroyMyMenu(BOOL all)
{
	PortalMenuItem* item = g_PortalMenuItem;
	PortalMenuItem* nextItem;

	while( item != NULL )
	{
		nextItem = item->next;
#ifndef ICON_MANAGER
		if( item->icon )
			DestroyIcon( item->icon );
#endif
		free( item->text );
		if( item->sctext != NULL )
			free( item->sctext );
		free( item );
		item = nextItem;
	}
	if( all > 0 )
	{
		for(PortalProgVector::iterator i = g_portal_files.begin(); i != g_portal_files.end(); i++)
		{
			delete (*i);
		}
		g_portal_files.clear();
	}
	unloaderIconManager();
	g_PortalMenuItem = NULL;
}

/* ------------------------------------------------------------------------------------------------- */

LRESULT OnMenuCharItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	char chUser = (char)LOWORD(wParam); 
	HMENU menu = (HMENU)lParam;

#define MENU_CHAR_LNG	10

	if( menus.find(menu) != menus.end() )
	{
		int j = 0, k = -1, l = 0;
		bool n = false;

		wchar_t wcTemp1[MENU_CHAR_LNG], wcTemp2[MENU_CHAR_LNG];
		memset(wcTemp1, 0, sizeof(wchar_t)*MENU_CHAR_LNG);
		memset(wcTemp2, 0, sizeof(wchar_t)*MENU_CHAR_LNG);

		swprintf_s(wcTemp1, MENU_CHAR_LNG, L"&%c", chUser);

		if( isupper(chUser) )
			chUser = (char)tolower((int)chUser);
		else
			chUser = (char)toupper((int)chUser);
		
		swprintf_s(wcTemp2, MENU_CHAR_LNG, L"&%c", chUser);

		PortalProg* p = menus[menu];
		for( PortalProgVector::iterator i = p->progs.begin() ; i != p->progs.end() ; i++, l++)
		{
			if( (*i)->progName != NULL )
			{
				wchar_t* pos1 = wcsstr( (*i)->progName, wcTemp1);
				wchar_t* pos2 = wcsstr( (*i)->progName, wcTemp2);
				if( pos1 != NULL || pos2 != NULL )
				{
					j++;
					if( GetMenuState(menu, l, MF_BYPOSITION) & MF_HILITE )
					{
						if( k < 0 )
							k = l;
						n = true;
					}
					else if( k < 0 || n == true )
					{
						k = l;
						n = false;
					}
				}
			}
		}

		if( j == 1 )
		{
			return MAKELRESULT(k, MNC_EXECUTE);
		}
		else if( j > 1 )
		{
			return MAKELRESULT(k, MNC_SELECT);
		}
		else
		{
			wchar_t nname[120];
			k = -1;
			l = 0;
			for( PortalProgVector::iterator i = p->progs.begin() ; i != p->progs.end() ; i++, l++)
			{
				if( (*i)->progName != NULL )
				{
					swprintf(nname, MENU_CHAR_LNG, L"&%s", (*i)->progName);
					wchar_t* pos1 = wcsstr( nname, wcTemp1);
					wchar_t* pos2 = wcsstr( nname, wcTemp2);
					wchar_t* pos3 = wcschr( (*i)->progName, L'&');

					if( pos3 == NULL && (pos1 != NULL || pos2 != NULL) )
					{
						j++;
						if( GetMenuState(menu, l, MF_BYPOSITION) & MF_HILITE )
						{
							if( k < 0 )
								k = l;
							n = true;
						}
						else if( k < 0 || n == true )
						{
							k = l;
							n = false;
						}
					}
				}
			}

			if( j == 1 )
			{
				return MAKELRESULT(k, MNC_EXECUTE);
			}
			else if( j > 1 )
			{
				return MAKELRESULT(k, MNC_SELECT);
			}
		}
	}

#undef MENU_CHAR_LNG

	return MAKELRESULT(0, MNC_IGNORE);
}

/* ------------------------------------------------------------------------------------------------- */

void OnInitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HMENU hmenu = (HMENU)wParam;
	int count = GetMenuItemCount(hmenu);
	PortalProg *p_menu = menus[hmenu];
	if( count == 0 )
	{
		PortalMenuItem* item = g_PortalMenuItem;
		if( item != NULL )
		{
			while( item->next != NULL )
				item = item->next;
		}
		int nResult = 0;
		int configElem = 0x0;
		if( !(p_menu->options & PROG_OPTION_BROWSE) )
		{
			for( PortalProgVector::iterator i = p_menu->progs.begin() ; i != p_menu->progs.end() ; i++)
			{
				item = insertItemMenu( (*i), item, hmenu, &configElem, &nResult );
				if( item == NULL )
					break;
			}
		}
		else
		{
			bool subDirectory = (p_menu->dirPath != NULL && wcslen(p_menu->dirPath) > 0 );
			bool display = false;
#ifndef ICON_MANAGER
			SHFILEINFO tSHFileInfo;
#endif
			PortalProg* prog = NULL;
			size_t size = 0;

			wchar_t search[MAX_FILE_LEN];
			WIN32_FIND_DATA findData;
			HANDLE find = INVALID_HANDLE_VALUE;

			memset(search,0,sizeof(wchar_t)*MAX_FILE_LEN);
			memset(&findData,0,sizeof(WIN32_FIND_DATA));

			wcscpy_s(search,p_menu->progExe);
			wcscat_s(search,L"\\");
			if( subDirectory )
			{
				wcscat_s(search,L"*");
			}
			else if( p_menu->progParam )
			{
				wcscat_s(search,p_menu->progParam);
			}
			else
			{
				wcscat_s(search,L"*.*");
			}

			find = FindFirstFileW(search, &findData);
			if( find != INVALID_HANDLE_VALUE )
			{
				// Browse for directory
				// - Add directories with MyCreateSubMenu
				// - Add files with shell option
				do
				{
					display = false;
					if( !(findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN ) )
					{
						if(findData.dwFileAttributes & (FILE_ATTRIBUTE_NORMAL|FILE_ATTRIBUTE_ARCHIVE))
						{
							display = true;
						}
						if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY && subDirectory)
						{
							if( wcscmp(findData.cFileName, L"..") && wcscmp(findData.cFileName, L".") )
							{
								display = true;
							}
						}
					}
					if( display )
					{
						UINT uid = (UINT)g_portal_files.size() + PORTAL_FILE_ID;
						item = newPortalItem( uid, findData.cFileName, MENU_NORMAL, item, NULL);
						prog = new PortalProg();
						prog->dirPath = _wcsdup(p_menu->dirPath);
						prog->options = p_menu->options;
						prog->progName = _wcsdup(findData.cFileName);

						if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
						{
							size = sizeof(p_menu->progExe) + sizeof(findData.cFileName) + sizeof(L"\\");
							wchar_t* prog_exe = (wchar_t*)malloc( size );
							memset(prog_exe, 0, size);
							size /= sizeof(wchar_t);
							wcscpy_s(prog_exe, size, p_menu->progExe);
							wcscat_s(prog_exe, size, L"\\");
							wcscat_s(prog_exe, size, findData.cFileName);
							wcscat_s(prog_exe, size, L"\\");
							prog->progExe = prog_exe;

#ifndef ICON_MANAGER
							SHGetFileInfo( prog->progExe , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
							item->icon = tSHFileInfo.hIcon;
#else
							item->iconid = preloadIcon(L"/");
#endif

							HMENU subMenu = MyCreateSubMenu( prog );
							AddSubMenu( hmenu, uid, MFT_OWNERDRAW, 0, subMenu, item);
						}
						else
						{
							size = sizeof(p_menu->progExe) + sizeof(findData.cFileName);
							wchar_t* prog_exe = (wchar_t*)malloc( size );
							memset(prog_exe, 0, size);
							size /= sizeof(wchar_t);
							wcscpy_s(prog_exe, size, p_menu->progExe);
							wcscat_s(prog_exe, size, findData.cFileName);
							prog->progExe = prog_exe;

#ifndef ICON_MANAGER
							SHGetFileInfo( prog->progExe , 0, &tSHFileInfo, sizeof(tSHFileInfo), SHGFI_ICON | SHGFI_SMALLICON);
							item->icon = tSHFileInfo.hIcon;
#else
							item->iconid = preloadIcon(prog->progExe);
#endif

							AddMenuItem(hmenu, uid, MFT_OWNERDRAW, 0, item);
						}

						g_portal_files.insert( g_portal_files.end(), prog);
					}
				}
				while( FindNextFileW(find, &findData) != 0 );
			}
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

void OnUninitMenuPopup(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
}

/* ------------------------------------------------------------------------------------------------- */

void OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPMEASUREITEMSTRUCT ptMeasure = (LPMEASUREITEMSTRUCT)lParam;
	HDC hDC;
	SIZE tSize;
	HFONT hFont, hOldFont;
	PortalMenuItem* item = (PortalMenuItem*)ptMeasure->itemData;

	hDC = GetDC(hWnd);
	hFont = CreateFontIndirect(&g_localFont);
	hOldFont = (HFONT)SelectObject(hDC, hFont);

	ptMeasure->itemWidth = 0;
	if( item->options & (MENU_SUB | MENU_SHOWSC) && item->sctext != NULL)
	{
		GetTextExtentPoint32(hDC, item->sctext, (int)wcslen(item->sctext), &tSize);
		ptMeasure->itemWidth += tSize.cx + 15;
	}

	GetTextExtentPoint32(hDC, item->text, (int)wcslen(item->text), &tSize);

	SelectObject(hDC, hOldFont);
	DeleteObject(hFont);

	ptMeasure->itemWidth += tSize.cx + g_iconSize.cx + 54;

	if( item->options & MENU_SEP )
	{
		ptMeasure->itemHeight = 10;
	}
	else
	{
		ptMeasure->itemHeight = g_iconSize.cy + 8;
	}

	ReleaseDC(hWnd, hDC);
}

/* ------------------------------------------------------------------------------------------------- */

void OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LPDRAWITEMSTRUCT ptDrawItem = (LPDRAWITEMSTRUCT)lParam;
	HDC hDC;
	RECT tRect;
	RECT tRectText;
	RECT bar;
	HBRUSH hBrush;
	int lngText;
	PortalMenuItem* item = (PortalMenuItem*)ptDrawItem->itemData;
	bool isArrow = ((item->options & MENU_SUB) != 0);

	tRect = ptDrawItem->rcItem;

	tRectText = tRect;
	tRectText.left += g_iconSize.cx + 24;
	tRectText.top += 4;

	hDC = ptDrawItem->hDC;
	
	lngText = (int)wcslen(item->text);
	SetBkMode(hDC, OPAQUE);

	if( ptDrawItem->itemState & ODS_SELECTED )
	{
#ifdef MENU_DRAW_V1
		// Draw Item Menu: Version 1
		//
		SetBkColor(hDC, g_menuBackColor);
		SetTextColor(hDC, g_menuBackTextColor);

		hBrush = CreateSolidBrush(g_menuBackColor);
		FillRect(hDC, &tRect, hBrush);
#else
		// Draw Item Menu: Version 2
		//
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		FillRect(hDC, &tRect, hBrush);
		DeleteObject(hBrush);

		hBrush = CreateSolidBrush(g_menuBackColor);
		DWORD penColor = GetSysColor(COLOR_HIGHLIGHT);
		HPEN pen = CreatePen(PS_SOLID, 1, penColor);

		HGDIOBJ old_brush = SelectObject(hDC, hBrush);
		HGDIOBJ old_pen = SelectObject(hDC, pen);

		SetBkColor(hDC, g_menuBackColor);
		SetBkMode(hDC, TRANSPARENT);

		//Backup ClipBox for RoundRect clipping boxes
		RECT fullRect;
		GetClipBox(hDC, &fullRect);

		// Rounded
		BeginPath(hDC);
		RoundRect(hDC, tRect.left, tRect.top, tRect.right, tRect.bottom, 10, 10);
		EndPath(hDC);
		SelectClipPath(hDC, RGN_COPY);

		TRIVERTEX vertex[2];
		vertex[0].x = tRect.left;
		vertex[0].y = tRect.top;
		vertex[0].Red   = 0x9600;
		vertex[0].Green = 0xd900;
		vertex[0].Blue  = 0xf900;
		vertex[0].Alpha = 0xffff;

		vertex[1].x = tRect.right;
		vertex[1].y = tRect.bottom;
		vertex[1].Red   = 0x9600;
		vertex[1].Green = 0xd900;
		vertex[1].Blue  = 0xf900;
		vertex[1].Alpha = 0x0000;

		GRADIENT_RECT r;
		r.UpperLeft = 0;
		r.LowerRight = 1;

		GradientFill(hDC, vertex, 2, &r, 1, GRADIENT_FILL_RECT_V);

		// Rounded
		BeginPath(hDC);
		RoundRect(hDC, tRect.left+1, tRect.top+1, tRect.right-1, tRect.bottom-1, 10, 10);
		EndPath(hDC);
		SelectClipPath(hDC, RGN_COPY);

		vertex[0].x = tRect.left+1;
		vertex[0].y = tRect.top+1;
		vertex[0].Red   = 0xff00;
		vertex[0].Green = 0xff00;
		vertex[0].Blue  = 0xff00;
		vertex[1].x = tRect.right-1;
		vertex[1].y = tRect.bottom-1;

		GradientFill(hDC, vertex, 2, &r, 1, GRADIENT_FILL_RECT_V);

		SelectObject(hDC, old_brush);
		SelectObject(hDC, old_pen);
		DeleteObject(pen);

		BeginPath(hDC);
		Rectangle(hDC, fullRect.left, fullRect.top, fullRect.right, fullRect.bottom);
		EndPath(hDC);
		SelectClipPath(hDC, RGN_COPY);
#endif
	}
	else if( item->options & MENU_TITLE )
	{
		hBrush = CreateSolidBrush(GetBkColor(hDC));

		SIZE tSize;
		GetTextExtentPoint32(hDC, item->text, lngText, &tSize);

		tRectText = tRect;
		tRectText.left = (tRect.right/2) - (tSize.cx/2);
		tRectText.right = tRectText.left + tSize.cx + 10;
		tRectText.top += 2;

		FillRect(hDC, &tRect, hBrush);
	}
	else
	{
		hBrush = CreateSolidBrush(GetBkColor(hDC));
		FillRect(hDC, &tRect, hBrush);
	}

	DeleteObject(hBrush);

	if( item->options & MENU_TITLE )
	{
		bar.top = tRect.top + (tRect.bottom - tRect.top) / 2;
		bar.bottom = bar.top;

		bar.left = tRect.left + 5;
		bar.right = tRectText.left - 2;
		DrawEdge( hDC, &bar, EDGE_ETCHED, BF_BOTTOM);

		bar.left = tRectText.right + 2;
		bar.right = tRect.right - 5;
		DrawEdge( hDC, &bar, EDGE_ETCHED, BF_BOTTOM);
	}
	else if( !(ptDrawItem->itemState & ODS_SELECTED) )
	{
		bar.top = tRect.top;
		bar.bottom = tRect.bottom;
		bar.left = g_iconSize.cx + 16;
		bar.right = tRect.right;
		DrawEdge( hDC, &bar, EDGE_ETCHED, BF_LEFT);
	}

	if( item->options & MENU_SEP )
	{
		bar.top = tRect.top + 1 + (tRect.bottom - tRect.top) / 2;
		bar.bottom = bar.top;
		bar.left = tRect.left + g_iconSize.cx + 17;
		bar.right = tRect.right - 1;
		DrawEdge( hDC, &bar, EDGE_ETCHED, BF_BOTTOM);
	}
	else
	{
#ifndef ICON_MANAGER
		if( item->icon != NULL )
		{
			DrawIconEx(hDC, tRect.left + 8, tRect.top + 4, item->icon, g_iconSize.cx, g_iconSize.cy, 0, NULL, DI_NORMAL);
		}
#else
		if( item->iconid < PORTAL_ICON_RESOURCE )
		{
			HICON* icon = getIcon(item->iconid, wParam, lParam);
			if( icon != NULL )
			{
				DrawIconEx(hDC, tRect.left + 8, tRect.top + 4, *icon, g_iconSize.cx, g_iconSize.cy, 0, NULL, DI_NORMAL);
			}
		}
		else if( item->iconid > PORTAL_ICON_RESOURCE )
		{
			HICON icon = LOADRESOURCEICON( item->iconid - PORTAL_ICON_RESOURCE);
			DrawIconEx(hDC, tRect.left + 8, tRect.top + 4, icon, g_iconSize.cx, g_iconSize.cy, 0, NULL, DI_NORMAL);
		}
#endif
		DrawText(hDC, item->text, lngText, &tRectText, DT_LEFT | DT_BOTTOM);

		if( item->options & MENU_SHOWSC )
		{
			SetTextColor(hDC, g_ShotcutColor);
			RECT t = tRectText;
			t.right -= 10;
			if(isArrow) {
				t.right -= 10;
			}
			DrawText(hDC, item->sctext, (int)wcslen(item->sctext), &t, DT_RIGHT | DT_BOTTOM);
			SetTextColor(hDC, g_TextColor);
		}
	}
	SetBkMode(hDC, OPAQUE);

#ifndef MENU_DRAW_V1
	if(isArrow)
	{
		SetBkMode(hDC, TRANSPARENT);
		DWORD oldBkColor = GetBkColor(hDC);
		SetBkColor(hDC, RGB(255,255,255) );

		int arrowW  = 16;
		int arrowH  = 16;
		RECT tRectArrow;
		tRectArrow.right = tRectText.right;
		tRectArrow.left = tRectText.right - arrowW;
		tRectArrow.top = tRectText.top;
		tRectArrow.bottom = tRectText.bottom;
		
		RECT tmpArrowR;
		SetRect(&tmpArrowR, 0, 0, arrowW, arrowH);

		HDC arrowDC = CreateCompatibleDC(hDC);
		HDC fillDC  = CreateCompatibleDC(hDC);

		HBITMAP arrowBitmap    = CreateCompatibleBitmap(arrowDC, arrowW, arrowH);
		HBITMAP oldArrowBitmap = (HBITMAP)SelectObject(arrowDC, arrowBitmap);
		HBITMAP fillBitmap     = CreateCompatibleBitmap(fillDC, arrowW, arrowH);
		HBITMAP oldFillBitmap  = (HBITMAP)SelectObject(fillDC, fillBitmap);

		DrawFrameControl(arrowDC, &tmpArrowR, DFC_MENU, DFCS_MENUARROW);

		HBRUSH arrowBrush = CreateSolidBrush(RGB(0,0,0));

		FillRect(fillDC, &tmpArrowR, arrowBrush);
		BitBlt(hDC, tRectArrow.left, tRectArrow.top, arrowW, arrowH, fillDC, 0, 0, SRCINVERT);
		BitBlt(hDC, tRectArrow.left, tRectArrow.top, arrowW, arrowH, arrowDC, 0, 0, SRCAND);
		BitBlt(hDC, tRectArrow.left, tRectArrow.top, arrowW, arrowH, fillDC, 0, 0, SRCINVERT);

		SelectObject(fillDC, oldFillBitmap);
		DeleteObject(fillBitmap);
		SelectObject(arrowDC, oldArrowBitmap);
		DeleteObject(arrowBitmap);
		DeleteDC(fillDC);
		DeleteDC(arrowDC);

		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, oldBkColor);

		// Exclude the clip rect in order to not redraw the system arrow.
		ExcludeClipRect(hDC, tRectArrow.left, tRectArrow.top, tRectArrow.right, tRectArrow.bottom);
	}
#endif
}

/* ------------------------------------------------------------------------------------------------- */
