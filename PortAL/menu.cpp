/*
	PortAL - GUIPro Project ( http://obsidev.github.io/guipro/ )

	Author : Glatigny J�r�me <jerome@obsidev.com> - http://www.obsidev.com/

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

#ifdef USE_GDI_MENU
/* Some useful doc
 *
 * http://www.nanoant.com/programming/themed-menus-icons-a-complete-vista-xp-solution
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ms534077%28v=vs.85%29.aspx
 * https://code.google.com/p/tortoisesvn/source/browse/tags/version-1.5.0/src/TortoiseShell/ContextMenu.cpp
 * https://msdn.microsoft.com/en-us/library/bb757020.aspx?s=6 ( https://msdn.microsoft.com/en-us/library/bb756947.aspx )
 * http://www.codeproject.com/Articles/16529/Simple-Menus-That-Display-Icons-Minimalistic-Appro
 */
#include <GdiPlus.h>
#pragma comment (lib, "Gdiplus.lib")
ULONG_PTR g_gdiplusToken = 0;
#endif

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
PortalMenuDesign* g_PortalMenuDesign = NULL;
#ifdef USE_GDI_MENU
boolean g_PortalMenuDesignSystem = false;
#endif

ColorPair cp1 = { 0x000000, 0xaeaeae };

PortalMenuDesign design1 = {
	1,			// border_size
	10,			// border_round
	0xf9d996,	// border_color
	NULL,		// base
	NULL,		// selected
	1,			// background_grandiant_direction
	0xffffff,	// background_gradiant_start
	0xf9d996	// background_gradiant_end
};
PortalMenuDesign design2 = {
	1,			// border_size
	0,			// border_round
	0x3c3c3c,	// border_color
	NULL,		// base
	&cp1,		// selected
	0,			// background_grandiant_direction
	0x000000,	// background_gradiant_start
	0x000000	// background_gradiant_end
};


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

#ifdef USE_GDI_MENU
	if(g_PortalMenuDesignSystem)
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
	}
#endif

	g_PortalMenuDesign = &design1;
}

/* ------------------------------------------------------------------------------------------------- */

void UnInitMenuVars()
{
#ifdef USE_GDI_MENU
	if (g_gdiplusToken)
		Gdiplus::GdiplusShutdown(g_gdiplusToken);
	g_gdiplusToken = 0;
#endif
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
	if (item == NULL) return FALSE;
	MENUITEMINFO tMenuItemInfo = { 0 };

	//ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
	tMenuItemInfo.fType = uType;
	tMenuItemInfo.wID = uItemID;
#ifdef USE_GDI_MENU
	if(g_PortalMenuDesignSystem && windowsVersion >= WINVER_VISTA)
	{
		if (!(uType & MFT_SEPARATOR))
		{
			// tMenuItemInfo.hbmpItem = (windowsVersion >= WINVER_VISTA) ? IconToBitmapPARGB32(item->iconid) : HBMMENU_CALLBACK;
			tMenuItemInfo.hbmpItem = IconToBitmapPARGB32(item->iconid);
			tMenuItemInfo.fMask = MIIM_ID | MIIM_BITMAP | MIIM_STRING | MIIM_DATA;
		}
		else
		{
			tMenuItemInfo.fType = uType & ~MFT_OWNERDRAW;
			tMenuItemInfo.fMask = MIIM_FTYPE | MIIM_ID | MIIM_DATA;
		}
	}
#endif
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
#ifdef USE_GDI_MENU

HBITMAP IconToBitmapPARGB32(UINT uIcon)
{
/*
	std::map<UINT, HBITMAP>::iterator bitmap_it = bitmaps.lower_bound(uIcon);
	if (bitmap_it != bitmaps.end() && bitmap_it->first == uIcon)
		return bitmap_it->second;
*/
	if (!g_gdiplusToken)
		return HBMMENU_CALLBACK;

	HICON* hIcon = NULL;
	if (uIcon < PORTAL_ICON_RESOURCE)
	{
		hIcon = getIcon(uIcon, NULL, NULL);
	}
	else if (uIcon > PORTAL_ICON_RESOURCE)
	{
		HICON icon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(uIcon), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); //LOADRESOURCEICON(uIcon - PORTAL_ICON_RESOURCE);
		hIcon = &icon;
	}

	if (hIcon == NULL)
		return HBMMENU_CALLBACK;
/*
	// Method 1
	Gdiplus::Bitmap icon(*hIcon);
	Gdiplus::Bitmap bmp(16, 16, PixelFormat32bppPARGB);
	Gdiplus::Graphics g(&bmp);
	g.DrawImage(&icon, 0, 0, 16, 16);

	HBITMAP hBmp = NULL;
	bmp.GetHBITMAP(Gdiplus::Color(255, 0, 0, 0), &hBmp);
/*/
	// Method 2
	ICONINFO ii;
	GetIconInfo(*hIcon, &ii);
	BITMAP bmp;
	GetObject(ii.hbmColor, sizeof(bmp), &bmp);

	Gdiplus::Bitmap temp(ii.hbmColor, NULL);
	Gdiplus::BitmapData lockedBitmapData;
	Gdiplus::Rect rc(0, 0, 16, 16);

	temp.LockBits(&rc, Gdiplus::ImageLockModeRead, temp.GetPixelFormat(), &lockedBitmapData);
	Gdiplus::Bitmap image(lockedBitmapData.Width, lockedBitmapData.Height, lockedBitmapData.Stride, PixelFormat32bppARGB, reinterpret_cast<BYTE *>(lockedBitmapData.Scan0));
	temp.UnlockBits(&lockedBitmapData);

	HBITMAP hBmp = NULL;
	image.GetHBITMAP(Gdiplus::Color(255, 0, 0, 0), &hBmp);
//*/
	if (uIcon > PORTAL_ICON_RESOURCE)
		DestroyIcon(*hIcon);
/*
	if (hBmp)
		bitmaps.insert(bitmap_it, std::make_pair(uIcon, hBmp));
*/
	return hBmp;
}

/* ------------------------------------------------------------------------------------------------- */

HBITMAP IconToBitmap(UINT uIcon)
{
/*
	std::map<UINT, HBITMAP>::iterator bitmap_it = bitmaps.lower_bound(uIcon);
	if (bitmap_it != bitmaps.end() && bitmap_it->first == uIcon)
		return bitmap_it->second;
*/
	HICON* hIcon = NULL;
	if (uIcon < PORTAL_ICON_RESOURCE)
	{
		hIcon = getIcon(uIcon, NULL, NULL);
	}
	else if (uIcon > PORTAL_ICON_RESOURCE)
	{
		HICON icon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(uIcon), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR); //LOADRESOURCEICON(uIcon - PORTAL_ICON_RESOURCE);
		hIcon = &icon;
	}

	if (hIcon == NULL)
		return NULL;

	RECT rect;

	rect.right = GetSystemMetrics(SM_CXMENUCHECK);
	rect.bottom = GetSystemMetrics(SM_CYMENUCHECK);

	rect.left = rect.top = 0;

	HWND desktop = GetDesktopWindow();
	if (desktop == NULL)
	{
		if (uIcon > PORTAL_ICON_RESOURCE)
			DestroyIcon(*hIcon);
		return NULL;
	}

	HDC screen_dev = GetDC(desktop);
	if (screen_dev == NULL)
	{
		if (uIcon > PORTAL_ICON_RESOURCE)
			DestroyIcon(*hIcon);
		return NULL;
	}

	// Create a compatible DC
	HDC dst_hdc = CreateCompatibleDC(screen_dev);
	if (dst_hdc == NULL)
	{
		if (uIcon > PORTAL_ICON_RESOURCE)
			DestroyIcon(*hIcon);
		::ReleaseDC(desktop, screen_dev);
		return NULL;
	}

	// Create a new bitmap of icon size
	HBITMAP bmp = CreateCompatibleBitmap(screen_dev, rect.right, rect.bottom);
	if (bmp == NULL)
	{
		if (uIcon > PORTAL_ICON_RESOURCE)
			DestroyIcon(*hIcon);
		DeleteDC(dst_hdc);
		ReleaseDC(desktop, screen_dev);
		return NULL;
	}

	// Select it into the compatible DC
	HBITMAP old_dst_bmp = (HBITMAP)SelectObject(dst_hdc, bmp);
	if (old_dst_bmp == NULL)
	{
		if (uIcon > PORTAL_ICON_RESOURCE)
			DestroyIcon(*hIcon);
		return NULL;
	}

	// Fill the background of the compatible DC with the white color
	// that is taken by menu routines as transparent
	SetBkColor(dst_hdc, RGB(255, 255, 255));
	ExtTextOut(dst_hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);

	// Draw the icon into the compatible DC
	DrawIconEx(dst_hdc, 0, 0, *hIcon, rect.right, rect.bottom, 0, NULL, DI_NORMAL);

	// Restore settings
	SelectObject(dst_hdc, old_dst_bmp);
	DeleteDC(dst_hdc);
	ReleaseDC(desktop, screen_dev);
	if (uIcon > PORTAL_ICON_RESOURCE)
		DestroyIcon(*hIcon);
/*
	if (bmp)
		bitmaps.insert(bitmap_it, std::make_pair(uIcon, bmp));
*/
	return bmp;
}
#endif

/* ------------------------------------------------------------------------------------------------- */

BOOL AddSubMenu(HMENU hMenu, UINT uItemID, UINT uType, UINT uState, HMENU subMenu, PortalMenuItem* item)
{
	if (item == NULL) return FALSE;
	MENUITEMINFO tMenuItemInfo;

	item->options |= MENU_SUB;

	ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA | MIIM_SUBMENU;
	tMenuItemInfo.fType = uType;
#ifdef USE_GDI_MENU
	if(g_PortalMenuDesignSystem && windowsVersion >= WINVER_VISTA)
	{
		tMenuItemInfo.fType = uType & ~(MFT_OWNERDRAW | MFT_MENUBARBREAK);
		tMenuItemInfo.fMask = MIIM_FTYPE | MIIM_ID | MIIM_BITMAP | MIIM_STRING | MIIM_DATA | MIIM_SUBMENU;
		//tMenuItemInfo.hbmpItem = (windowsVersion >= WINVER_VISTA) ? IconToBitmapPARGB32(item->iconid) : HBMMENU_CALLBACK;
		tMenuItemInfo.hbmpItem = IconToBitmapPARGB32(item->iconid);
	}
#endif
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

	if (p_menu && p_menu->progExe != NULL)
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
	if (count != 0)
		return;
	if (p_menu == NULL)
		return;

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
	RECT tRect, tRectText, bar;
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
//#ifdef MENU_DRAW_V1
		if (g_PortalMenuDesign == NULL)
		{
			// Draw Item Menu: Version 1
			//
			SetBkColor(hDC, g_menuBackColor);
			SetTextColor(hDC, g_menuBackTextColor);

			hBrush = CreateSolidBrush(g_menuBackColor);
			FillRect(hDC, &tRect, hBrush);
		}
		else
		{
//#else
			// Define the base color and background
			if (g_PortalMenuDesign->base != NULL)
			{
				SetBkColor(hDC, g_PortalMenuDesign->base->background);
				SetTextColor(hDC, g_PortalMenuDesign->base->textcolor);
			}

			if (g_PortalMenuDesign->selected != NULL)
			{
				SetTextColor(hDC, g_PortalMenuDesign->selected->textcolor);

				// When having round borders, we have to first display the default background
				if (g_PortalMenuDesign->border_round == 0)
					SetBkColor(hDC, g_PortalMenuDesign->selected->background);
			}

			// Draw Item Menu: Version 2
			//
			hBrush = CreateSolidBrush(GetBkColor(hDC));
			FillRect(hDC, &tRect, hBrush);
			DeleteObject(hBrush);

			if (g_PortalMenuDesign->selected != NULL)
			{
				hBrush = CreateSolidBrush(g_PortalMenuDesign->selected->background);
			}
			else
			{
				hBrush = CreateSolidBrush(g_menuBackColor);
				SetBkColor(hDC, g_menuBackColor);
			}

			DWORD penColor = GetSysColor(COLOR_HIGHLIGHT);
			HPEN pen = CreatePen(PS_SOLID, 1, penColor);

			HGDIOBJ old_brush = SelectObject(hDC, hBrush);
			HGDIOBJ old_pen = SelectObject(hDC, pen);

			SetBkMode(hDC, TRANSPARENT);

			// Backup ClipBox for RoundRect clipping boxes
			RECT fullRect;
			GetClipBox(hDC, &fullRect);

			// Create the zone for the rounded borders
			if (g_PortalMenuDesign->border_round > 0)
			{
				BeginPath(hDC);
				RoundRect(hDC, tRect.left, tRect.top, tRect.right, tRect.bottom, g_PortalMenuDesign->border_round, g_PortalMenuDesign->border_round);
				EndPath(hDC);
				SelectClipPath(hDC, RGN_COPY);
			}

			// Display the border, if there is something to display
			if (g_PortalMenuDesign->border_size > 0)
			{
				// Display the border
				HBRUSH borderBrush = CreateSolidBrush(g_PortalMenuDesign->border_color);
				FillRect(hDC, &tRect, borderBrush);
				DeleteObject(borderBrush);

				// Select the background depending the rounded borders
				if (g_PortalMenuDesign->border_round > 0)
				{
					BeginPath(hDC);
					RoundRect(hDC, tRect.left + g_PortalMenuDesign->border_size, tRect.top + g_PortalMenuDesign->border_size, tRect.right - g_PortalMenuDesign->border_size, tRect.bottom - g_PortalMenuDesign->border_size, g_PortalMenuDesign->border_round, g_PortalMenuDesign->border_round);
					EndPath(hDC);
					SelectClipPath(hDC, RGN_COPY);
				}
			}

			if (g_PortalMenuDesign->background_grandiant_direction > 0)
			{
				TRIVERTEX vertex[2];
				memset(vertex, 0, sizeof(vertex));

				GRADIENT_RECT r;
				r.UpperLeft = 0;
				r.LowerRight = 1;

				// Display the background
				vertex[0].x = tRect.left + g_PortalMenuDesign->border_size;
				vertex[0].y = tRect.top + g_PortalMenuDesign->border_size;
				vertex[0].Red = (COLOR16)GetRValue(g_PortalMenuDesign->background_gradiant_start) << 8;
				vertex[0].Green = (COLOR16)GetGValue(g_PortalMenuDesign->background_gradiant_start) << 8;
				vertex[0].Blue = (COLOR16)GetBValue(g_PortalMenuDesign->background_gradiant_start) << 8;
				vertex[0].Alpha = 0x0000; // Alpha is not used by GradientFill

				vertex[1].x = tRect.right - g_PortalMenuDesign->border_size;
				vertex[1].y = tRect.bottom - g_PortalMenuDesign->border_size;
				vertex[1].Red = (COLOR16)GetRValue(g_PortalMenuDesign->background_gradiant_end) << 8;
				vertex[1].Green = (COLOR16)GetGValue(g_PortalMenuDesign->background_gradiant_end) << 8;
				vertex[1].Blue = (COLOR16)GetBValue(g_PortalMenuDesign->background_gradiant_end) << 8;
				vertex[1].Alpha = 0x0000; // Alpha is not used by GradientFill

				if (g_PortalMenuDesign->background_grandiant_direction == 1)
					GradientFill(hDC, vertex, 2, &r, 1, GRADIENT_FILL_RECT_V);
				else
					GradientFill(hDC, vertex, 2, &r, 1, GRADIENT_FILL_RECT_H);

				SelectObject(hDC, old_brush);
				SelectObject(hDC, old_pen);
				DeleteObject(pen);

				BeginPath(hDC);
				Rectangle(hDC, fullRect.left, fullRect.top, fullRect.right, fullRect.bottom);
				EndPath(hDC);
				SelectClipPath(hDC, RGN_COPY);
			}
			else
			{
				tRect.left += g_PortalMenuDesign->border_size;
				tRect.top += g_PortalMenuDesign->border_size;
				tRect.right -= g_PortalMenuDesign->border_size;
				tRect.bottom -= g_PortalMenuDesign->border_size;

				FillRect(hDC, &tRect, hBrush);
			}
		}
//#endif
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
		if (g_PortalMenuDesign != NULL && g_PortalMenuDesign->base != NULL)
		{

		}

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
		DrawEdge(hDC, &bar, EDGE_ETCHED, BF_BOTTOM);

		bar.left = tRectText.right + 2;
		bar.right = tRect.right - 5;
		DrawEdge(hDC, &bar, EDGE_ETCHED, BF_BOTTOM);
	}
	else if( !(ptDrawItem->itemState & ODS_SELECTED) )
	{
		bar.top = tRect.top;
		bar.bottom = tRect.bottom;
		bar.left = g_iconSize.cx + 16;
		bar.right = tRect.right;
		DrawEdge(hDC, &bar, EDGE_ETCHED, BF_LEFT);
	}

	if( item->options & MENU_SEP )
	{
		bar.top = tRect.top + 1 + (tRect.bottom - tRect.top) / 2;
		bar.bottom = bar.top;
		bar.left = tRect.left + g_iconSize.cx + 17;
		bar.right = tRect.right - 1;
		DrawEdge(hDC, &bar, EDGE_ETCHED, BF_BOTTOM);
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

//#ifndef MENU_DRAW_V1
	if (g_PortalMenuDesign != NULL && isArrow)
	{
		SetBkMode(hDC, TRANSPARENT);
		DWORD oldBkColor = GetBkColor(hDC);
		SetBkColor(hDC, RGB(255,255,255));

		int arrowW = 16;
		int arrowH = 16;
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
//#endif
}

/* ------------------------------------------------------------------------------------------------- */
