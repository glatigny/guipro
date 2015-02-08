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

#include "config.h"
#include "common.h"
#include "main.h"
#include "hotkey.h"
#include "mouse.h"
#include "volume.h"

#include <fstream>

vlConfig g_vlConfig[VL_ACT_LAST];

/* ------------------------------------------------------------------------------------------------- */

int openConfig()
{
	clearConfig();

	wchar_t szConfigName[] = L"vulcan.ini";
	wchar_t szFilename[FILENAME_MAX] = L"";

	// retrieve the path of the binary
	GetModuleFileName(g_hInst, szFilename, FILENAME_MAX);
	wchar_t* pos = wcsrchr(szFilename, L'\\');
	if (pos != NULL)
	{
		pos++;
		wcscpy_s(pos, szFilename - pos + SIZEOF_ARRAY(szFilename), szConfigName);
	}
	else
	{
		return 1;
	}

	wchar_t buff[LINE_MAX_SIZE];
	wchar_t *tmp;

	std::wifstream l_file(szFilename);
	if (!l_file.is_open())
	{
		return 1;
	}

	UINT mod = VL_ACT_NONE;

	while (!l_file.eof())
	{
		l_file.getline(buff, LINE_MAX_SIZE);

		if (wcslen(buff) == 0)
			continue;

		if (buff[0] == L';')
			continue;

		tmp = wcsstr(buff, L"=");

		if (buff[0] == L'[')
		{
			mod = VL_ACT_NONE;

#define CMP_HK(id) if( !wcsncmp(buff, VL_TXT_ ## id, wcslen(VL_TXT_ ## id)) ) { mod = VL_ACT_ ## id; }

			CMP_HK(VOL_UP)
			else CMP_HK(VOL_DOWN)
			else CMP_HK(VOL_MUTE)

#undef CMP_HK
		}
		else if (mod == VL_ACT_NONE && tmp != NULL && tmp[1] != L'\0')
		{
			tmp[0] = L'\0';

			if (!wcscmp(buff, VL_OPT_VOLUME_STEP))
			{
				int step = _wtoi(&tmp[1]);
				if (step > 30)
					step = 30;
				if (step < 2)
					step = 1;
				g_volume_step = (float)(step / 100);
			}

			//
			tmp[0] = L'=';
		}
		else if (mod > VL_ACT_NONE && mod <= VL_ACT_LAST && tmp != NULL && tmp[1] != L'\0')
		{
			tmp[0] = L'\0';

			if (!wcscmp(buff, VL_TYPE_HK_TXT))
			{
				g_vlConfig[mod - 1].key = getHotKeyCode(&tmp[1]);
				if (g_vlConfig[mod - 1].key > 0)
					g_vlConfig[mod - 1].type = VL_TYPE_HK;
			}
			else if (!wcscmp(buff, VL_TYPE_MOUSE_TXT))
			{
				g_vlConfig[mod - 1].key = 0;

#define CMP_MOUSE(id) if (!wcscmp(&tmp[1], VL_MOUSE_TXT_ ## id)) { g_vlConfig[mod - 1].key = VL_MOUSE_BTN_ ## id; }

				CMP_MOUSE(LEFT)
				else CMP_MOUSE(RIGHT)
				else CMP_MOUSE(MIDDLE)
				else CMP_MOUSE(WHEEL_UP)
				else CMP_MOUSE(WHEEL_DOWN)

#undef CMP_MOUSE

				if (g_vlConfig[mod - 1].key > 0)
					g_vlConfig[mod - 1].type = VL_TYPE_MOUSE;
			}
			else if (!wcscmp(buff, L"mod"))
			{
				g_vlConfig[mod - 1].mod = getModifier(&tmp[1]);
			}

			//
			if (g_vlConfig[mod - 1].mod > 0 && g_vlConfig[mod - 1].key > 0)
			{
				g_vlConfig[mod - 1].action = 1;

				if (g_vlConfig[mod - 1].type == VL_TYPE_MOUSE)
				{
					// register the mouse button
					g_mouseUsed[g_vlConfig[mod - 1].key] = 1;
				}
			}

			//
			tmp[0] = L'=';
		}
	}
	l_file.close();

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void clearConfig()
{
	memset(g_vlConfig, 0, sizeof(g_vlConfig));
}

/* ------------------------------------------------------------------------------------------------- */

int registerConfig(int alert)
{
	LPWSTR l_registerErrors = (LPWSTR)malloc(sizeof(PWSTR) * MAX_LENGTH);
	memset(l_registerErrors, 0, sizeof(PWSTR) * MAX_LENGTH);
	if (registerHotkeys(l_registerErrors) > 0)
	{
		if (alert)
		{
			size_t l_size = (wcslen(ERR_HOTKEYS_MSG) + wcslen(l_registerErrors) + 2);
			LPWSTR l_message = (LPWSTR)malloc(sizeof(PWSTR) * l_size);
			wcscpy_s(l_message, l_size, ERR_HOTKEYS_MSG);
			wcscat_s(l_message, l_size, l_registerErrors);

			MessageBox(g_hwndMain, l_message, ERR_MSGBOX_TITLE, NULL);

			free(l_message);
			free(l_registerErrors);
		}
		return FALSE;
	}
	free(l_registerErrors);

	// Check for mouse hooking
	//
	for (int i = 0; i <= VL_MOUSE_BTN_MAX; i++)
	{
		if (g_mouseUsed[i] > 0)
		{
			installHookMouse();
			break;
		}
	}

	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

int registerHotkeys(LPWSTR p_registerErrors)
{
	int error = 0;
	int i = 0;

	for (i = 0; i < VL_ACT_LAST; i++)
	{
		if (g_vlConfig[i].action == 0)
			continue;

		if (g_vlConfig[i].type != VL_TYPE_HK)
			continue;

		if (g_vlConfig[i].key <= 0)
			continue;

		if (!myRegisterHotKey(g_vlConfig[i].key, g_vlConfig[i].mod, i, true))
		{
			error++;

			LPWSTR l_errkey = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
			wcscpy_s(l_errkey, MAX_ERRHKLEN, getInverseModifier(g_vlConfig[i].mod));
			wcscat_s(l_errkey, MAX_ERRHKLEN, getInverseHotKeyCode(g_vlConfig[i].key));

			if ((wcslen(p_registerErrors) + wcslen(l_errkey)) >= MAX_LENGTH)
			{
				return error;
			}

			wcscat_s(p_registerErrors, MAX_LENGTH, l_errkey);
			wcscat_s(p_registerErrors, MAX_LENGTH, (wchar_t*)"\n");
		}
	}

	return error;
}

/* ------------------------------------------------------------------------------------------------- */