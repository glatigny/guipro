/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@obsi.dev>

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

#include "menu.h"
#include "common.h"
#include "main.h"
#include "resource.h"
#include "iconize.h"
#include "config.h"

LOGFONT g_localFont;
SIZE g_iconSize;
DWORD g_menuBackColor;
DWORD g_menuBackTextColor;

/* ------------------------------------------------------------------------------------------------- */

void InitMenuVars()
{
	NONCLIENTMETRICS tNCMetrics;

	tNCMetrics.cbSize = sizeof(NONCLIENTMETRICS);

	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &tNCMetrics, 0);

	g_localFont = tNCMetrics.lfMenuFont;
	g_iconSize.cx = GetSystemMetrics(SM_CXSMICON);
	g_iconSize.cy = GetSystemMetrics(SM_CYSMICON);
	g_menuBackColor = GetSysColor(COLOR_HIGHLIGHT);
	g_menuBackTextColor = GetSysColor(COLOR_HIGHLIGHTTEXT);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowTrayMenu()
{
	POINT pt;
	GetCursorPos(&pt);

	HMENU hmenu = CreatePopupMenu();

	// Set Menu Info
	/* We change the menu style in order to give the same emplacement for checkmarks and bitmaps */
	MENUINFO tMenuInfo;
	ZeroMemory(&tMenuInfo, sizeof(tMenuInfo));
	tMenuInfo.cbSize = sizeof(tMenuInfo);
	tMenuInfo.fMask = MIM_STYLE;
	tMenuInfo.dwStyle = MNS_CHECKORBMP;
	SetMenuInfo(hmenu, &tMenuInfo);

	TrayIconElem *elem = g_iconizeTrays;
	TrayIconElem *previous = NULL;
	bool showSeparator = false;

	while( elem != NULL )
	{
		if( !IsWindow(elem->hwnd) )
		{
			TrayIconElem *corrupt = elem;
			elem = elem->next;
			DeleteIconize(corrupt, previous, NULL);
		}
		else 
		{
			if( (elem->option & ICONIZE_OPT_INTERNAL) && (elem->hwnd != 0) )
			{
				insertProg(hmenu, IDM_ICONIZE_MENU + elem->id, elem);
				showSeparator = true;
			}
			previous = elem;
			elem = elem->next;
		}
	}

	if( showSeparator )
		AppendMenu(hmenu, MF_SEPARATOR, NULL, NULL);

	AppendMenu(hmenu, MF_STRING, IDM_ABOUT, L"About");
	AppendMenu(hmenu, MF_STRING, IDM_EXIT, L"Exit");

	SetForegroundWindow(g_hwndMain);
	UINT uMenuID = (UINT)TrackPopupMenu(hmenu, 
		TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, 
		pt.x, pt.y, 0, g_hwndMain, NULL);
	PostMessage(g_hwndMain, WM_NULL, 0, 0);
	DestroyMenu(hmenu);

	switch (uMenuID) {
		case IDM_EXIT:
			DestroyWindow(g_hwndMain);
			break;
		case IDM_ABOUT:
			ShowAbout();
			break;
		default:
			if( uMenuID >= IDM_ICONIZE_MENU )
				UnIconize( uMenuID - IDM_ICONIZE_MENU);
			break;
	}
}

/* ------------------------------------------------------------------------------------------------- */

BOOL insertProg(HMENU hMenu, UINT uItemID, TrayIconElem *elem)
{
	MENUITEMINFO tMenuItemInfo;

	ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_DATA;
	tMenuItemInfo.wID = uItemID;
	tMenuItemInfo.fType = MFT_OWNERDRAW;
	tMenuItemInfo.dwTypeData = elem->name;
	tMenuItemInfo.dwItemData = (ULONG_PTR)elem;

	return InsertMenuItem(hMenu, uItemID, FALSE, &tMenuItemInfo);
}

/* ------------------------------------------------------------------------------------------------- */

void ShowIconizeMenu( UINT id )
{
	POINT pt;
	GetCursorPos(&pt);

	HMENU hmenu = CreatePopupMenu();

	// Set Menu Info
	/* We change the menu style in order to remove checkmarks */
	MENUINFO tMenuInfo;
	ZeroMemory(&tMenuInfo, sizeof(tMenuInfo));
	tMenuInfo.cbSize = sizeof(tMenuInfo);
	tMenuInfo.fMask = MIM_STYLE;
	tMenuInfo.dwStyle = MNS_NOCHECK;
	SetMenuInfo(hmenu, &tMenuInfo);

	// Creation of the Restore Item
	/* We create a menu item info in order to have a beautiful "restore" icon provided by Windows */
	MENUITEMINFO tMenuItemInfo;
	ZeroMemory(&tMenuItemInfo, sizeof(tMenuItemInfo));
	tMenuItemInfo.cbSize = sizeof(tMenuItemInfo);
	tMenuItemInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_BITMAP | MIIM_STATE;
	tMenuItemInfo.wID = IDM_OPEN_ICONIZE;
	tMenuItemInfo.hbmpItem = HBMMENU_POPUP_RESTORE;
	tMenuItemInfo.dwTypeData = L"Restore";
	tMenuItemInfo.fState = MFS_DEFAULT;

	InsertMenuItem(hmenu, IDM_OPEN_ICONIZE, FALSE, &tMenuItemInfo);

	// Modification for the Close Item
	/* We just modify the menu item info in order to have a beautiful "close" icon provided by Windows */
	tMenuItemInfo.wID = IDM_CLOSE_ICONIZE;
	tMenuItemInfo.hbmpItem = HBMMENU_POPUP_CLOSE;
	tMenuItemInfo.dwTypeData = L"Close";
	tMenuItemInfo.fState = MFS_ENABLED;

	InsertMenuItem(hmenu, IDM_CLOSE_ICONIZE, FALSE, &tMenuItemInfo);

	SetForegroundWindow(g_hwndMain);
	UINT uMenuID = (UINT)TrackPopupMenu(hmenu, 
		TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, 
		pt.x, pt.y, 0, g_hwndMain, NULL);
	PostMessage(g_hwndMain, WM_NULL, 0, 0);
	DestroyMenu(hmenu);

	switch (uMenuID) {
		case IDM_OPEN_ICONIZE:
			UnIconize( id );
			break;
		case IDM_CLOSE_ICONIZE:
			CloseIconize( id );
			break;
	}
}

/* ------------------------------------------------------------------------------------------------- */

void OnMeasureItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);

	LPMEASUREITEMSTRUCT ptMeasure = (LPMEASUREITEMSTRUCT)lParam;
	HDC hDC;
	SIZE tSize;
	HFONT hFont, hOldFont;
	TrayIconElem* item = (TrayIconElem*)ptMeasure->itemData;

	hDC = GetDC(hWnd);

	hFont = CreateFontIndirect(&g_localFont);

	hOldFont = (HFONT)SelectObject(hDC, hFont);

	GetTextExtentPoint32(hDC, item->name, (int)wcslen(item->name), &tSize);

	SelectObject(hDC, hOldFont);

	DeleteObject(hFont);

	ptMeasure->itemWidth = tSize.cx + g_iconSize.cx + 14;
	ptMeasure->itemHeight = g_iconSize.cy + 4;

	ReleaseDC(hWnd, hDC);
}

/* ------------------------------------------------------------------------------------------------- */

void OnDrawItem(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(wParam);

	LPDRAWITEMSTRUCT ptDrawItem = (LPDRAWITEMSTRUCT)lParam;
	HDC hDC;
	RECT tRect;
	RECT tRectText;
	HBRUSH hBrush;
	int lngText;
	TrayIconElem* item = (TrayIconElem*)ptDrawItem->itemData;

	tRect = ptDrawItem->rcItem;

	tRectText = tRect;
	tRectText.left += g_iconSize.cx + 14;
	tRectText.top += 2;

	hDC = ptDrawItem->hDC;
	
	lngText = (int)wcslen(item->name);

	if( ptDrawItem->itemState & ODS_SELECTED )
	{
		hBrush = CreateSolidBrush(g_menuBackColor);
		SetBkColor(hDC, g_menuBackColor);
		SetTextColor(hDC, g_menuBackTextColor);
	}
	else
	{
		hBrush = CreateSolidBrush(GetBkColor(hDC));
	}

	FillRect(hDC, &tRect, hBrush);

	DeleteObject(hBrush);

	if( item->icon != NULL )
	{
		DrawIconEx(hDC, tRect.left + 2, tRect.top + 2, item->icon, g_iconSize.cx, g_iconSize.cy, 0, NULL, DI_NORMAL);
	}

	DrawText(hDC, item->name, lngText, &tRectText, DT_BOTTOM);
}

/* ------------------------------------------------------------------------------------------------- */