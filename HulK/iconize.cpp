/*  HulK - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.fr>

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

#include "common.h"
#include "iconize.h"
#include "main.h"
#include "trayIcon.h"
#include "resource.h"

TrayIconElem* g_iconizeTrays = NULL;

/* ------------------------------------------------------------------------------------------------- */

void Iconize(HWND l_hwnd, UINT option)
{
	WCHAR appName[256];

	if( l_hwnd == NULL || !IsWindow(l_hwnd) )
		return;

	InternalGetWindowText(l_hwnd, (LPWSTR)appName, 256);

	TrayIconElem *elem = g_iconizeTrays;

	if( elem == NULL )
	{
		g_iconizeTrays = (TrayIconElem*)malloc(sizeof(TrayIconElem));
		elem = g_iconizeTrays;
		elem->id = 0;
		elem->next = NULL;

		// Initialize the timer
		SetTimer(g_hwndMain, IDT_TIMERSYSTRAY, 5000, NULL);
	}
	else
	{
		if( g_iconizeTrays->hwnd != 0 )
		{
			char ids[ICONIZE_MAX_APPS];
			ZeroMemory(ids, sizeof(ids));
			while( elem->next != NULL )
			{
				ids[ elem->id ] = (char)0xFF;
				elem = elem->next;
			}
			ids[ elem->id ] = (char)0xFF;

			TrayIconElem *tmp = (TrayIconElem*)malloc(sizeof(TrayIconElem));

			int i;
			for( i = 0; i < ICONIZE_MAX_APPS; i++ )
			{
				if( ids[i] == 0 )
				{
					tmp->id = i;
					break;
				}
			}
			if( i == ICONIZE_MAX_APPS )
			{
				ShowBalloon(WC_HULK_ABOUT_TEXT_TITLE, WC_HULK_ERROR_MAX_APPS, NIIF_ERROR);
				free(tmp);
				return;
			}

			elem->next = tmp;
			elem = tmp;
			elem->next = NULL;
		}
	}

	elem->icon = NULL;
	elem->hwnd = l_hwnd;
	elem->option = option;
	elem->name = NULL;

	SendMessage(l_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
	ShowWindow( l_hwnd, SW_HIDE );

	if (!SendMessageTimeout(l_hwnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 250, (PDWORD_PTR)&elem->icon)) {
		DWORD dwErr = GetLastError();
		if (dwErr == 0 || dwErr == 1460) {
			elem->icon = (HICON)LoadImage(g_hInst, MAKEINTRESOURCE(IDI_TRAY_ICON), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		}
	}
	if (!elem->icon) 
		elem->icon = (HICON)(UINT_PTR)GetClassLongPtr(l_hwnd, GCLP_HICONSM);

	if( (option & ICONIZE_OPT_SYSTRAY) || (option == 0))
		AddTrayIcon( g_hwndMain, elem->id, elem->icon, (PCWSTR)appName);
	else
		elem->name = _wcsdup(appName);
}

/* ------------------------------------------------------------------------------------------------- */

void DeleteIconize(TrayIconElem *elem, TrayIconElem *previous, WPARAM wParam)
{
	if( IsWindow(elem->hwnd) )
	{
		// If we want to close an application with a sendmessage, the application have to be NOT minimized
		if( wParam == SC_CLOSE )
			SendMessage( elem->hwnd, WM_SYSCOMMAND, SC_RESTORE, NULL );
		else
			ShowWindow( elem->hwnd, SW_SHOW );

		if( wParam != NULL )
			SendMessage( elem->hwnd, WM_SYSCOMMAND, wParam, NULL );
	}

	if( elem->option & ICONIZE_OPT_SYSTRAY )
		DeleteTrayIcon( g_hwndMain, elem->id );

	DestroyIcon( elem->icon );
	free( elem->name );
	elem->name = NULL;

	if( elem == g_iconizeTrays )
	{
		g_iconizeTrays = elem->next;
		free( elem );
	}
	else
	{
		previous->next = elem->next;
		free( elem );
	}
}

/* ------------------------------------------------------------------------------------------------- */

void RemoveIconize(UINT id, WPARAM wParam)
{
	TrayIconElem *elem = g_iconizeTrays;
	TrayIconElem *previous = NULL;

	while( elem != NULL )
	{
		if( elem->id == id )
		{
			DeleteIconize(elem, previous, wParam);
			return;
		}

		previous = elem;
		elem = elem->next;
	}
}

/* ------------------------------------------------------------------------------------------------- */

void CheckAllIconize()
{
	TrayIconElem *elem = g_iconizeTrays;
	TrayIconElem *previous = NULL;

	while( elem != NULL )
	{
		if( !IsWindow(elem->hwnd) )
		{
			TrayIconElem *elemToRemove = elem;
			elem = elem->next;
			DeleteIconize(elemToRemove, previous, NULL);
		}
		else
		{
			previous = elem;
			elem = elem->next;
		}
	}

	// Stop the timer if the list is empty
	if( g_iconizeTrays == NULL )
		KillTimer(g_hwndMain, IDT_TIMERSYSTRAY);
}

/* ------------------------------------------------------------------------------------------------- */

void UnIconize(UINT id)
{
	RemoveIconize( id, SC_RESTORE );
}

/* ------------------------------------------------------------------------------------------------- */

void CloseIconize(UINT id)
{
	RemoveIconize( id, SC_CLOSE );
}

/* ------------------------------------------------------------------------------------------------- */

void UnIconizeLast()
{
	TrayIconElem *elem = g_iconizeTrays;
	if (elem != NULL )
	{
		while( elem->next != NULL )
			elem = elem->next;

		UnIconize( elem->id );
	}
}

/* ------------------------------------------------------------------------------------------------- */

void UnIconizeAll()
{
	TrayIconElem *elem = g_iconizeTrays;
	TrayIconElem *tmp;

	while( elem != NULL )
	{
		if( IsWindow(elem->hwnd) )
		{
			ShowWindow( elem->hwnd, SW_SHOW );
		}
		
		if( elem->option & ICONIZE_OPT_SYSTRAY )
			DeleteTrayIcon( g_hwndMain, elem->id );

		DestroyIcon( elem->icon );
		free( elem->name );
		elem->name = NULL;
		tmp = elem;
		elem = elem->next;
		free( tmp );
	}
	g_iconizeTrays = NULL;
	KillTimer(g_hwndMain, IDT_TIMERSYSTRAY);
}

/* ------------------------------------------------------------------------------------------------- */
