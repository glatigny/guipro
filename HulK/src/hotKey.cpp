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

#include "common.h"
#include "HotKey.h"
#include "main.h"
#include "config.h"
#include <stdio.h>

/* ------------------------------------------------------------------------------------------------- */

bool RegisterHK(UINT key, UINT mod, int id)
{
	if (!RegisterHotKey(g_hwndMain, IDH_HOTKEY_BASE + id, mod, key))
	{
		return false;
	}
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool UnregisterHK(int id)
{
	if (!UnregisterHotKey(g_hwndMain, IDH_HOTKEY_BASE + id))
	{
		return false;
	}
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getHotKeyCode(const wchar_t* p_ascii)
{
	if( !p_ascii || p_ascii[0] == '\0' )
		return 0;

	char* tmp = UnicodeToAnsi(p_ascii);
	char* l_ascii = _strdup(tmp);
	_strupr_s(l_ascii, strlen(l_ascii) + 1);

	UINT l_return = 0;

	if( l_ascii[1] == '\0' )
	{
		if( ((l_ascii[0] >= '0') && (l_ascii[0] <= '9') ) || ((l_ascii[0] >= 'A') && (l_ascii[0] <= 'Z')) )
		{
			l_return = l_ascii[0];
		}
		else
		{
			l_return = (VkKeyScanA(l_ascii[0])) & 0xff;
		}
	}
	else
	{
#define KEYSYMB(name,code) if( !strcmp(l_ascii, name) ) { l_return = VK_ ## code ; }
		/* F1.. keys */
		if( l_ascii[0] == 'F' )
		{
			l_return = VK_F1 + atoi(&l_ascii[1]) - 1;
		}
		/* NUMPAD */
		else if( (l_ascii[0] == 'N') && (l_ascii[1] == 'U') && (l_ascii[2] == 'M') )
		{
			if( l_ascii[3] == '_' )
			{
				KEYSYMB("NUM_LOCK",NUMLOCK)
				else KEYSYMB("NUM_DIV",DIVIDE)
				else KEYSYMB("NUM_MUL",MULTIPLY)
				else KEYSYMB("NUM_ADD",ADD)
				else KEYSYMB("NUM_SUB",SUBTRACT)
				else KEYSYMB("NUM_DECIMAL",DECIMAL)
			}
			else
			{
				l_return = VK_NUMPAD0 + l_ascii[3] - '0';
			}
		}
		else KEYSYMB("SPACE",SPACE)
		else KEYSYMB("ENTER",RETURN)
		else KEYSYMB("TAB",TAB)
		else KEYSYMB("ESCAPE",ESCAPE)
		else KEYSYMB("HOME",HOME)
		else KEYSYMB("END",END)
		else KEYSYMB("INSERT",INSERT)
		else KEYSYMB("UP",UP)
		else KEYSYMB("DOWN",DOWN)
		else KEYSYMB("LEFT",LEFT)
		else KEYSYMB("RIGHT",RIGHT)
		else KEYSYMB("SCROLL_LOCK",SCROLL)
		else KEYSYMB("PRINT_SCREEN",PRINT)
		else KEYSYMB("PAGE_UP",PRIOR)
		else KEYSYMB("PAGE_DOWN",NEXT)
		else KEYSYMB("BACK",BACK)
		else KEYSYMB("CLEAR",CLEAR)
		else KEYSYMB("MENU",MENU)
		else KEYSYMB("PAUSE",PAUSE)
		else KEYSYMB("CAPITAL",CAPITAL)
		else KEYSYMB("PRIOR",PRIOR)
		else KEYSYMB("NEXT",NEXT)
		else KEYSYMB("SELECT",SELECT)
		else KEYSYMB("PRINT",PRINT)
		else KEYSYMB("EXECUTE",EXECUTE)
		else KEYSYMB("SNAPSHOT",SNAPSHOT)
		else KEYSYMB("DELETE",DELETE)
		else KEYSYMB("HELP",HELP)
		else KEYSYMB("APPS",APPS)
		else KEYSYMB("SLEEP",SLEEP)
		else KEYSYMB("MULTIPLY",MULTIPLY)
		else KEYSYMB("ADD",ADD)
		else KEYSYMB("SEPARATOR",SEPARATOR)
		else KEYSYMB("SUBTRACT",SUBTRACT)
		else KEYSYMB("DECIMAL",DECIMAL)
		else KEYSYMB("DIVIDE",DIVIDE)
		else KEYSYMB("NUMLOCK",NUMLOCK)
		else KEYSYMB("SCROLL",SCROLL)
		else KEYSYMB("BROWSER_BACK",BROWSER_BACK)
		else KEYSYMB("BROWSER_FORWARD",BROWSER_FORWARD)
		else KEYSYMB("BROWSER_REFRESH",BROWSER_REFRESH)
		else KEYSYMB("BROWSER_STOP",BROWSER_STOP)
		else KEYSYMB("BROWSER_SEARCH",BROWSER_SEARCH)
		else KEYSYMB("BROWSER_FAVORITES",BROWSER_FAVORITES)
		else KEYSYMB("BROWSER_HOME",BROWSER_HOME)
		else KEYSYMB("VOLUME_MUTE",VOLUME_MUTE)
		else KEYSYMB("VOLUME_DOWN",VOLUME_DOWN)
		else KEYSYMB("VOLUME_UP",VOLUME_UP)
		else KEYSYMB("MEDIA_NEXT_TRACK",MEDIA_NEXT_TRACK)
		else KEYSYMB("MEDIA_PREV_TRACK",MEDIA_PREV_TRACK)
		else KEYSYMB("MEDIA_STOP",MEDIA_STOP)
		else KEYSYMB("MEDIA_PLAY_PAUSE",MEDIA_PLAY_PAUSE)
		else KEYSYMB("LAUNCH_MAIL",LAUNCH_MAIL)
		else KEYSYMB("LAUNCH_MEDIA_SELECT",LAUNCH_MEDIA_SELECT)
		else KEYSYMB("LAUNCH_APP1",LAUNCH_APP1)
		else KEYSYMB("LAUNCH_APP2",LAUNCH_APP2)
		else KEYSYMB("OEM_1",OEM_1)
		else KEYSYMB("OEM_2",OEM_2)
		else KEYSYMB("OEM_3",OEM_3)
		else KEYSYMB("OEM_4",OEM_4)
		else KEYSYMB("OEM_5",OEM_5)
		else KEYSYMB("OEM_6",OEM_6)
		else KEYSYMB("OEM_7",OEM_7)
		else KEYSYMB("OEM_8",OEM_8)
		else KEYSYMB("OEM_PLUS",OEM_PLUS)
		else KEYSYMB("OEM_COMMA",OEM_COMMA)
		else KEYSYMB("OEM_MINUS",OEM_MINUS)
		else KEYSYMB("OEM_PERIOD",OEM_PERIOD)

#undef KEYSYMB
	}
	
	if( l_return == 0 )
	{
		l_return = atoi(l_ascii);
	}

	if( l_ascii != tmp )
		free(l_ascii);
	free(tmp);

	return l_return;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getInverseHotKeyCode(UINT key)
{
	if( key == 0 )
		return NULL;

	char* l_return = NULL;

	if( ((key >= 'A') && (key <= 'Z')) || ((key >= '0') && (key <= '9')) )
	{
		l_return = (char*)malloc(sizeof(char) * 2);
		l_return[0] = (char)key;
		l_return[1] = '\0';
	}
	else if( (key >= VK_NUMPAD0) && (key <= VK_NUMPAD9) )
	{
		l_return = (char*)malloc(sizeof(char) * 8);
		sprintf_s(l_return, 8, "NUMPAD%d", key - VK_NUMPAD0);
	}
	else if( (key >= VK_F1) && (key <= VK_F24) )
	{
		l_return = (char*)malloc(sizeof(char) * 4);
		sprintf_s(l_return, 4, "F%d", key - VK_F1 + 1);
	}
#define KEYSYMB(name,code) if( key == VK_ ## code ) { l_return = _strdup(name); }

	else KEYSYMB("NUM_LOCK",NUMLOCK)
	else KEYSYMB("NUM_DIV",DIVIDE)
	else KEYSYMB("NUM_MUL",MULTIPLY)
	else KEYSYMB("NUM_ADD",ADD)
	else KEYSYMB("NUM_SUB",SUBTRACT)
	else KEYSYMB("NUM_DECIMAL",DECIMAL)
	else KEYSYMB("SPACE",SPACE)
	else KEYSYMB("ENTER",RETURN)
	else KEYSYMB("TAB",TAB)
	else KEYSYMB("ESCAPE",ESCAPE)
	else KEYSYMB("HOME",HOME)
	else KEYSYMB("END",END)
	else KEYSYMB("INSERT",INSERT)
	else KEYSYMB("UP",UP)
	else KEYSYMB("DOWN",DOWN)
	else KEYSYMB("LEFT",LEFT)
	else KEYSYMB("RIGHT",RIGHT)
	else KEYSYMB("SCROLL_LOCK",SCROLL)
	else KEYSYMB("PRINT_SCREEN",PRINT)
	else KEYSYMB("PAGE_UP",PRIOR)
	else KEYSYMB("PAGE_DOWN",NEXT)
	else KEYSYMB("BACK",BACK)
	else KEYSYMB("CLEAR",CLEAR)
	else KEYSYMB("MENU",MENU)
	else KEYSYMB("PAUSE",PAUSE)
	else KEYSYMB("CAPITAL",CAPITAL)
	else KEYSYMB("PRIOR",PRIOR)
	else KEYSYMB("NEXT",NEXT)
	else KEYSYMB("SELECT",SELECT)
	else KEYSYMB("PRINT",PRINT)
	else KEYSYMB("EXECUTE",EXECUTE)
	else KEYSYMB("SNAPSHOT",SNAPSHOT)
	else KEYSYMB("DELETE",DELETE)
	else KEYSYMB("HELP",HELP)
	else KEYSYMB("APPS",APPS)
	else KEYSYMB("SLEEP",SLEEP)
	else KEYSYMB("MULTIPLY",MULTIPLY)
	else KEYSYMB("ADD",ADD)
	else KEYSYMB("SEPARATOR",SEPARATOR)
	else KEYSYMB("SUBTRACT",SUBTRACT)
	else KEYSYMB("DECIMAL",DECIMAL)
	else KEYSYMB("DIVIDE",DIVIDE)
	else KEYSYMB("NUMLOCK",NUMLOCK)
	else KEYSYMB("SCROLL",SCROLL)
	else KEYSYMB("BROWSER_BACK",BROWSER_BACK)
	else KEYSYMB("BROWSER_FORWARD",BROWSER_FORWARD)
	else KEYSYMB("BROWSER_REFRESH",BROWSER_REFRESH)
	else KEYSYMB("BROWSER_STOP",BROWSER_STOP)
	else KEYSYMB("BROWSER_SEARCH",BROWSER_SEARCH)
	else KEYSYMB("BROWSER_FAVORITES",BROWSER_FAVORITES)
	else KEYSYMB("BROWSER_HOME",BROWSER_HOME)
	else KEYSYMB("VOLUME_MUTE",VOLUME_MUTE)
	else KEYSYMB("VOLUME_DOWN",VOLUME_DOWN)
	else KEYSYMB("VOLUME_UP",VOLUME_UP)
	else KEYSYMB("MEDIA_NEXT_TRACK",MEDIA_NEXT_TRACK)
	else KEYSYMB("MEDIA_PREV_TRACK",MEDIA_PREV_TRACK)
	else KEYSYMB("MEDIA_STOP",MEDIA_STOP)
	else KEYSYMB("MEDIA_PLAY_PAUSE",MEDIA_PLAY_PAUSE)
	else KEYSYMB("LAUNCH_MAIL",LAUNCH_MAIL)
	else KEYSYMB("LAUNCH_MEDIA_SELECT",LAUNCH_MEDIA_SELECT)
	else KEYSYMB("LAUNCH_APP1",LAUNCH_APP1)
	else KEYSYMB("LAUNCH_APP2",LAUNCH_APP2)
	else KEYSYMB("OEM_1",OEM_1)
	else KEYSYMB("OEM_2",OEM_2)
	else KEYSYMB("OEM_3",OEM_3)
	else KEYSYMB("OEM_4",OEM_4)
	else KEYSYMB("OEM_5",OEM_5)
	else KEYSYMB("OEM_6",OEM_6)
	else KEYSYMB("OEM_7",OEM_7)
	else KEYSYMB("OEM_8",OEM_8)
	else KEYSYMB("OEM_PLUS",OEM_PLUS)
	else KEYSYMB("OEM_COMMA",OEM_COMMA)
	else KEYSYMB("OEM_MINUS",OEM_MINUS)
	else KEYSYMB("OEM_PERIOD",OEM_PERIOD)

#undef KEYSYMB
	
	if( l_return == NULL )
		return NULL;

	wchar_t* l_wreturn = AnsiToUnicode(l_return);
	free(l_return);

	return l_wreturn;
}

/* ------------------------------------------------------------------------------------------------- */

UINT16 getModifier(const wchar_t* p_ascii)
{
	UINT16 mod = 0;

	if( p_ascii )
	{
		for( int i = (int)wcslen(p_ascii) - 1 ; i >= 0 ; i-- )
		{
			if( (p_ascii[i] == L'a') || (p_ascii[i] == L'A' ) )
			{
				mod |= MOD_ALT;
			}
			else if( (p_ascii[i] == L'c') || (p_ascii[i] == L'C' ) )
			{
				mod |= MOD_CONTROL;
			}
			else if( (p_ascii[i] == L's') || (p_ascii[i] == L'S' ) )
			{
				mod |= MOD_SHIFT;
			}
			else if( (p_ascii[i] == L'w') || (p_ascii[i] == L'W' ) )
			{
				mod |= MOD_WIN;
			}
		}
	}

	return mod;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getInverseModifier(UINT mod)
{
	if( mod != 0 )
	{
		LPWSTR l_mod = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
		size_t l_size = MAX_ERRHKLEN;
		memset(l_mod, 0, l_size);
		
		if( mod & MOD_WIN )
		{
			wcscat_s(l_mod, l_size, L"Win+");
		}

		if( mod & MOD_CONTROL )
		{
			wcscat_s(l_mod, l_size, L"Ctrl+");
		}

		if( mod & MOD_ALT )
		{
			wcscat_s(l_mod, l_size, L"Alt+");
		}
		
		if( mod & MOD_SHIFT )
		{
			wcscat_s(l_mod, l_size, L"Shift+");
		}

		return l_mod;
	}
	else
	{
		return NULL;
	}
}

/* ------------------------------------------------------------------------------------------------- */

UINT8 getMouseBtn(const wchar_t* p_ascii)
{
	UINT8 btn = 0;

	if( p_ascii )
	{
		if( !wcscmp(p_ascii, L"left") )
			btn = MOUSE_BTN_LEFT;
		if( !wcscmp(p_ascii, L"right") )
			btn = MOUSE_BTN_RIGHT;
		if( !wcscmp(p_ascii, L"middle") )
			btn = MOUSE_BTN_MIDDLE;
		if( !wcscmp(p_ascii, L"wheel") )
			btn = MOUSE_BTN_WHEEL;
	}
	return btn;
}

/* ------------------------------------------------------------------------------------------------- */
