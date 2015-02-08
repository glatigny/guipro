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

#include "common.h"
#include "HotKey.h"
#include "main.h"
#include "resource.h"
#include "config.h"
#include <stdio.h>

HHOOK hookKeyboardHandle;
vlHk overrideKeys[VL_ACT_LAST];

/* ------------------------------------------------------------------------------------------------- */

bool myRegisterHotKey(UINT key, UINT mod, int id, bool overriding)
{
	int evt = getEventFromId(id + 1);
	if (RegisterHotKey(g_hwndMain, evt, mod, key))
		return true;
	
	if (overriding && mod == MOD_WIN)
		return addOverrideKey(key, id);
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool myUnregisterHotKey(int id)
{
	if (delOverrideKey(id))
		return true;

	return (UnregisterHotKey(g_hwndMain, id) == TRUE);
}

/* ------------------------------------------------------------------------------------------------- */

UINT getHotKeyCode(const wchar_t* p_ascii)
{
	if (!p_ascii || p_ascii[0] == L'\0')
		return 0;

	wchar_t* l_ascii = _wcsdup(p_ascii);
	_wcsupr_s(l_ascii, wcslen(l_ascii) + 1);

	UINT l_return = 0;

	if (l_ascii[1] == L'\0')
	{
		if (((l_ascii[0] >= L'0') && (l_ascii[0] <= L'9')) || ((l_ascii[0] >= L'A') && (l_ascii[0] <= L'Z')))
		{
			l_return = l_ascii[0];
		}
		else
		{
			l_return = (VkKeyScanW(l_ascii[0])) & 0xff;
		}
	}
	else
	{
#define KEYSYMB(name,code) if( !_wcsicmp(l_ascii, L ## name) ) { l_return = VK_ ## code ; }
#define KEYSYMBRAW(name,code) if( !_wcsicmp(l_ascii, L ## name) ) { l_return = code ; }
		/* F1.. keys */
		if (l_ascii[0] == L'F')
		{
			l_return = VK_F1 + _wtoi(&l_ascii[1]) - 1;
		}
		/* NUMPAD */
		else if ((l_ascii[0] == L'N') && (l_ascii[1] == L'U') && (l_ascii[2] == L'M'))
		{
			if (l_ascii[3] == L'_')
			{
				KEYSYMB("NUM_LOCK", NUMLOCK)
				else KEYSYMB("NUM_DIV", DIVIDE)
				else KEYSYMB("NUM_MUL", MULTIPLY)
				else KEYSYMB("NUM_ADD", ADD)
				else KEYSYMB("NUM_SUB", SUBTRACT)
				else KEYSYMB("NUM_DECIMAL", DECIMAL)
				else if (l_ascii[4] >= L'0' && l_ascii[4] <= L'9') { l_return = VK_NUMPAD0 + (l_ascii[4] - L'0'); }
			}
			else
			{
				l_return = VK_NUMPAD0 + (l_ascii[3] - L'0');
			}
		}
		else KEYSYMB("SPACE", SPACE)
		else KEYSYMB("ENTER", RETURN)
		else KEYSYMB("TAB", TAB)
		else KEYSYMB("ESCAPE", ESCAPE)
		else KEYSYMB("HOME", HOME)
		else KEYSYMB("END", END)
		else KEYSYMB("INSERT", INSERT)
		else KEYSYMB("UP", UP)
		else KEYSYMB("DOWN", DOWN)
		else KEYSYMB("LEFT", LEFT)
		else KEYSYMB("RIGHT", RIGHT)
		else KEYSYMB("SCROLL_LOCK", SCROLL)
		else KEYSYMB("PRINT_SCREEN", PRINT)
		else KEYSYMB("PAGE_UP", PRIOR)
		else KEYSYMB("PAGE_DOWN", NEXT)
		else KEYSYMB("BACK", BACK)
		else KEYSYMB("CLEAR", CLEAR)
		else KEYSYMB("MENU", MENU)
		else KEYSYMB("PAUSE", PAUSE)
		else KEYSYMB("CAPITAL", CAPITAL)
		else KEYSYMB("PRIOR", PRIOR)
		else KEYSYMB("NEXT", NEXT)
		else KEYSYMB("SELECT", SELECT)
		else KEYSYMB("PRINT", PRINT)
		else KEYSYMB("EXECUTE", EXECUTE)
		else KEYSYMB("SNAPSHOT", SNAPSHOT)
		else KEYSYMBRAW("DELETE", VK_DELETE)
		else KEYSYMB("HELP", HELP)
		else KEYSYMB("APPS", APPS)
		else KEYSYMB("SLEEP", SLEEP)
		else KEYSYMB("MULTIPLY", MULTIPLY)
		else KEYSYMB("ADD", ADD)
		else KEYSYMB("SEPARATOR", SEPARATOR)
		else KEYSYMB("SUBTRACT", SUBTRACT)
		else KEYSYMBRAW("DECIMAL", VK_DECIMAL)
		else KEYSYMB("DIVIDE", DIVIDE)
		else KEYSYMB("NUMLOCK", NUMLOCK)
		else KEYSYMB("SCROLL", SCROLL)
		else KEYSYMB("BROWSER_BACK", BROWSER_BACK)
		else KEYSYMB("BROWSER_FORWARD", BROWSER_FORWARD)
		else KEYSYMB("BROWSER_REFRESH", BROWSER_REFRESH)
		else KEYSYMB("BROWSER_STOP", BROWSER_STOP)
		else KEYSYMB("BROWSER_SEARCH", BROWSER_SEARCH)
		else KEYSYMB("BROWSER_FAVORITES", BROWSER_FAVORITES)
		else KEYSYMB("BROWSER_HOME", BROWSER_HOME)
		else KEYSYMB("VOLUME_MUTE", VOLUME_MUTE)
		else KEYSYMB("VOLUME_DOWN", VOLUME_DOWN)
		else KEYSYMB("VOLUME_UP", VOLUME_UP)
		else KEYSYMB("MEDIA_NEXT_TRACK", MEDIA_NEXT_TRACK)
		else KEYSYMB("MEDIA_PREV_TRACK", MEDIA_PREV_TRACK)
		else KEYSYMB("MEDIA_STOP", MEDIA_STOP)
		else KEYSYMB("MEDIA_PLAY_PAUSE", MEDIA_PLAY_PAUSE)
		else KEYSYMB("LAUNCH_MAIL", LAUNCH_MAIL)
		else KEYSYMB("LAUNCH_MEDIA_SELECT", LAUNCH_MEDIA_SELECT)
		else KEYSYMB("LAUNCH_APP1", LAUNCH_APP1)
		else KEYSYMB("LAUNCH_APP2", LAUNCH_APP2)
		else KEYSYMB("OEM_1", OEM_1)
		else KEYSYMB("OEM_2", OEM_2)
		else KEYSYMB("OEM_3", OEM_3)
		else KEYSYMB("OEM_4", OEM_4)
		else KEYSYMB("OEM_5", OEM_5)
		else KEYSYMB("OEM_6", OEM_6)
		else KEYSYMB("OEM_7", OEM_7)
		else KEYSYMB("OEM_8", OEM_8)
		else KEYSYMB("OEM_PLUS", OEM_PLUS)
		else KEYSYMB("OEM_COMMA", OEM_COMMA)
		else KEYSYMB("OEM_MINUS", OEM_MINUS)
		else KEYSYMB("OEM_PERIOD", OEM_PERIOD)

#undef KEYSYMB
#undef KEYSYMBRAW
	}

	if (l_return == 0)
	{
		l_return = _wtoi(l_ascii);
	}

	if (l_ascii != p_ascii)
		free(l_ascii);

	return l_return;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getInverseHotKeyCode(UINT key)
{
	if (key == 0)
		return NULL;

	char* l_return = NULL;

	if (((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')))
	{
		l_return = (char*)malloc(sizeof(char) * 2);
		l_return[0] = (char)key;
		l_return[1] = '\0';
	}
	else if ((key >= VK_NUMPAD0) && (key <= VK_NUMPAD9))
	{
		l_return = (char*)malloc(sizeof(char) * 8);
		sprintf_s(l_return, 8, "NUMPAD%d", key - VK_NUMPAD0);
	}
	else if ((key >= VK_F1) && (key <= VK_F24))
	{
		l_return = (char*)malloc(sizeof(char) * 4);
		sprintf_s(l_return, 4, "F%d", key - VK_F1 + 1);
	}
#define KEYSYMB(name,code) if( key == VK_ ## code ) { l_return = _strdup(name); }

	else KEYSYMB("NUM_LOCK", NUMLOCK)
	else KEYSYMB("NUM_DIV", DIVIDE)
	else KEYSYMB("NUM_MUL", MULTIPLY)
	else KEYSYMB("NUM_ADD", ADD)
	else KEYSYMB("NUM_SUB", SUBTRACT)
	else KEYSYMB("NUM_DECIMAL", DECIMAL)
	else KEYSYMB("SPACE", SPACE)
	else KEYSYMB("ENTER", RETURN)
	else KEYSYMB("TAB", TAB)
	else KEYSYMB("ESCAPE", ESCAPE)
	else KEYSYMB("HOME", HOME)
	else KEYSYMB("END", END)
	else KEYSYMB("INSERT", INSERT)
	else KEYSYMB("UP", UP)
	else KEYSYMB("DOWN", DOWN)
	else KEYSYMB("LEFT", LEFT)
	else KEYSYMB("RIGHT", RIGHT)
	else KEYSYMB("SCROLL_LOCK", SCROLL)
	else KEYSYMB("PRINT_SCREEN", PRINT)
	else KEYSYMB("PAGE_UP", PRIOR)
	else KEYSYMB("PAGE_DOWN", NEXT)
	else KEYSYMB("BACK", BACK)
	else KEYSYMB("CLEAR", CLEAR)
	else KEYSYMB("MENU", MENU)
	else KEYSYMB("PAUSE", PAUSE)
	else KEYSYMB("CAPITAL", CAPITAL)
	else KEYSYMB("PRIOR", PRIOR)
	else KEYSYMB("NEXT", NEXT)
	else KEYSYMB("SELECT", SELECT)
	else KEYSYMB("PRINT", PRINT)
	else KEYSYMB("EXECUTE", EXECUTE)
	else KEYSYMB("SNAPSHOT", SNAPSHOT)
	else KEYSYMB("DELETE", DELETE)
	else KEYSYMB("HELP", HELP)
	else KEYSYMB("APPS", APPS)
	else KEYSYMB("SLEEP", SLEEP)
	else KEYSYMB("MULTIPLY", MULTIPLY)
	else KEYSYMB("ADD", ADD)
	else KEYSYMB("SEPARATOR", SEPARATOR)
	else KEYSYMB("SUBTRACT", SUBTRACT)
	else KEYSYMB("DECIMAL", DECIMAL)
	else KEYSYMB("DIVIDE", DIVIDE)
	else KEYSYMB("NUMLOCK", NUMLOCK)
	else KEYSYMB("SCROLL", SCROLL)
	else KEYSYMB("BROWSER_BACK", BROWSER_BACK)
	else KEYSYMB("BROWSER_FORWARD", BROWSER_FORWARD)
	else KEYSYMB("BROWSER_REFRESH", BROWSER_REFRESH)
	else KEYSYMB("BROWSER_STOP", BROWSER_STOP)
	else KEYSYMB("BROWSER_SEARCH", BROWSER_SEARCH)
	else KEYSYMB("BROWSER_FAVORITES", BROWSER_FAVORITES)
	else KEYSYMB("BROWSER_HOME", BROWSER_HOME)
	else KEYSYMB("VOLUME_MUTE", VOLUME_MUTE)
	else KEYSYMB("VOLUME_DOWN", VOLUME_DOWN)
	else KEYSYMB("VOLUME_UP", VOLUME_UP)
	else KEYSYMB("MEDIA_NEXT_TRACK", MEDIA_NEXT_TRACK)
	else KEYSYMB("MEDIA_PREV_TRACK", MEDIA_PREV_TRACK)
	else KEYSYMB("MEDIA_STOP", MEDIA_STOP)
	else KEYSYMB("MEDIA_PLAY_PAUSE", MEDIA_PLAY_PAUSE)
	else KEYSYMB("LAUNCH_MAIL", LAUNCH_MAIL)
	else KEYSYMB("LAUNCH_MEDIA_SELECT", LAUNCH_MEDIA_SELECT)
	else KEYSYMB("LAUNCH_APP1", LAUNCH_APP1)
	else KEYSYMB("LAUNCH_APP2", LAUNCH_APP2)
	else KEYSYMB("OEM_1", OEM_1)
	else KEYSYMB("OEM_2", OEM_2)
	else KEYSYMB("OEM_3", OEM_3)
	else KEYSYMB("OEM_4", OEM_4)
	else KEYSYMB("OEM_5", OEM_5)
	else KEYSYMB("OEM_6", OEM_6)
	else KEYSYMB("OEM_7", OEM_7)
	else KEYSYMB("OEM_8", OEM_8)
	else KEYSYMB("OEM_PLUS", OEM_PLUS)
	else KEYSYMB("OEM_COMMA", OEM_COMMA)
	else KEYSYMB("OEM_MINUS", OEM_MINUS)
	else KEYSYMB("OEM_PERIOD", OEM_PERIOD)

#undef KEYSYMB

	if (l_return == NULL)
		return NULL;

	wchar_t* l_wreturn = AnsiToUnicode(l_return);
	free(l_return);

	return l_wreturn;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getModifier(const wchar_t* p_ascii)
{
	UINT mod = 0;

	if (p_ascii && wcslen(p_ascii) > 0)
	{
		for (int i = (int)wcslen(p_ascii) - 1; i >= 0; i--)
		{
			if ((p_ascii[i] == L'a') || (p_ascii[i] == L'A'))
			{
				mod |= MOD_ALT;
			}
			else if ((p_ascii[i] == L'c') || (p_ascii[i] == L'C'))
			{
				mod |= MOD_CONTROL;
			}
			else if ((p_ascii[i] == L's') || (p_ascii[i] == L'S'))
			{
				mod |= MOD_SHIFT;
			}
			else if ((p_ascii[i] == L'w') || (p_ascii[i] == L'W'))
			{
				mod |= MOD_WIN;
			}
		}

		return mod;
	}

	return MOD_WIN;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getInverseModifier(UINT mod)
{
	if (mod == 0)
		return NULL;

	LPWSTR l_mod = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
	size_t l_size = MAX_ERRHKLEN;
	memset(l_mod, 0, l_size);

	if (mod & MOD_WIN)
	{
		wcscat_s(l_mod, l_size, L"Win+");
	}

	if (mod & MOD_CONTROL)
	{
		wcscat_s(l_mod, l_size, L"Ctrl+");
	}

	if (mod & MOD_ALT)
	{
		wcscat_s(l_mod, l_size, L"Alt+");
	}

	if (mod & MOD_SHIFT)
	{
		wcscat_s(l_mod, l_size, L"Shift+");
	}

	return l_mod;
}

/* ------------------------------------------------------------------------------------------------- */

__declspec(dllexport) LRESULT CALLBACK hookKeyboard(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		bool winKeyPressed = (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0);
		KBDLLHOOKSTRUCT& kllhs = *(KBDLLHOOKSTRUCT*)lParam;
		vlHk* hk = NULL;

		if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) && winKeyPressed)
		{
			hk = getOverrideHotkey(kllhs.vkCode);
			if (hk != NULL)
			{
				// Get Focus
				SetActiveWindow(g_hwndMain);
				SetForegroundWindow(g_hwndMain);
				SetFocus(g_hwndMain);

				int evt = getEventFromId(hk->id);
				// Simulate hotkey
				PostMessage(g_hwndMain, WM_HOTKEY, evt, NULL);

				// Abort the other hotkey but sending a new key press (abort the window key too)
				// keybd_event may be an old function but...
				keybd_event(VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | 0, 0);
				keybd_event(VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

				return 1;
			}
		}

		if (wParam == WM_KEYUP && winKeyPressed)
		{
			// This is not the key you're looking for
			hk = getOverrideHotkey(kllhs.vkCode);
			if (hk != NULL)
				return 1;
		}
	}
	return CallNextHookEx(hookKeyboardHandle, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

vlHk* getOverrideHotkey(DWORD key, bool create)
{
	int i = 0;

	for (i = 0; i < VL_ACT_LAST; i++)
	{
		if (overrideKeys[i].key == key)
			return &(overrideKeys[i]);
	}

	if (!create)
		return NULL;

	for (i = 0; i < VL_ACT_LAST; i++)
	{
		if (overrideKeys[i].id == 0)
			return &(overrideKeys[i]);
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

int getEventFromId(int id)
{
	switch (id) {
		case VL_ACT_VOL_UP:
			return IDH_HOTKEY_VOL_UP;
		case VL_ACT_VOL_DOWN:
			return IDH_HOTKEY_VOL_DOWN;
		case VL_ACT_VOL_MUTE:
			return IDH_HOTKEY_VOL_MUTE;
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

void installHookKeyboard()
{
	if (!hookKeyboardHandle)
	{
		hookKeyboardHandle = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)hookKeyboard, g_hInst, 0);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallHookKeyboard()
{
	if (hookKeyboardHandle)
	{
		UnhookWindowsHookEx(hookKeyboardHandle);
		hookKeyboardHandle = NULL;
	}
	memset(overrideKeys, 0, sizeof(overrideKeys));
}

/* ------------------------------------------------------------------------------------------------- */

bool addOverrideKey(DWORD c, int id)
{
	vlHk* hk = getOverrideHotkey(c);
	if (hk != NULL)
		return false;

	id++;
	if (overrideKeys[id].id == id)
		return false;

	if (!hookKeyboardHandle)
		installHookKeyboard();
	if (!hookKeyboardHandle)
		return false;

	overrideKeys[id].id = id;
	overrideKeys[id].key = c;
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool delOverrideKey(DWORD c)
{
	vlHk* hk = getOverrideHotkey(c);
	if (hk == NULL)
		return false;
	hk->id = 0;
	hk->key = 0;
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool delOverrideKey(int id)
{
	id++;
	if (hookKeyboardHandle && overrideKeys[id].id == id)
	{
		overrideKeys[id].id = 0;
		overrideKeys[id].key = 0;

		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */