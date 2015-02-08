/*
	VulCan - GUIPro Project ( http://obsidev.github.io/guipro/ )

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

#include "menu.h"
#include "common.h"
#include "main.h"
#include "volume.h"
#include "resource.h"

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

	wchar_t* l_volume = NULL;
	int volume = getVolume();
	unloadVolumeInterface();

	if (volume == 0)
	{
		AppendMenu(hmenu, MF_STRING | MF_CHECKED, IDM_MUTE, L"Mute");
	}
	else
	{
		l_volume = (wchar_t*)malloc(sizeof(wchar_t) * 50);
		memset(l_volume, 0, sizeof(l_volume));
		wsprintf(l_volume, WC_VULCAN_VOLUME_MENU, volume);

		AppendMenu(hmenu, MF_STRING, IDM_MUTE, l_volume);
	}

	AppendMenu(hmenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hmenu, MF_STRING, IDM_ABOUT, L"About");
	AppendMenu(hmenu, MF_STRING, IDM_EXIT, L"Exit");

	SetForegroundWindow(g_hwndMain);
	UINT uMenuID = (UINT)TrackPopupMenu(hmenu,
		TPM_RIGHTBUTTON | TPM_RIGHTALIGN | TPM_NONOTIFY | TPM_RETURNCMD,
		pt.x, pt.y, 0, g_hwndMain, NULL);
	PostMessage(g_hwndMain, WM_NULL, 0, 0);

	// Cleaning
	DestroyMenu(hmenu);
	if (l_volume != NULL)
	{
		free(l_volume);
		l_volume = NULL;
	}

	// Manage event
	switch (uMenuID) {
		case IDM_EXIT:
			DestroyWindow(g_hwndMain);
			break;
		case IDM_STATUS:
			ShowVolumeBalloon(-1);
			break;
		case IDM_MUTE:
			changeVolumeMute();
			break;
		case IDM_ABOUT:
			ShowAbout();
			break;
		default:
			break;
	}

	FlushMemory();
}

/* ------------------------------------------------------------------------------------------------- */