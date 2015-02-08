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
#include "mouse.h"
#include "main.h"
#include "hotKey.h"
#include "config.h"

HHOOK hookMouseHandle;
UCHAR g_mouseUsed[VL_MOUSE_BTN_MAX];

/* ------------------------------------------------------------------------------------------------- */

__declspec(dllexport) LRESULT CALLBACK hookMouse(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
			if (g_mouseUsed[VL_MOUSE_BTN_LEFT] > 0 && mouseEvent(VL_MOUSE_BTN_LEFT, true, lParam))
				return 1;
			break;
		case WM_RBUTTONDOWN:
			if (g_mouseUsed[VL_MOUSE_BTN_RIGHT] > 0 && mouseEvent(VL_MOUSE_BTN_RIGHT, true, lParam))
				return 1;
			break;
		case WM_MBUTTONDOWN:
			if (g_mouseUsed[VL_MOUSE_BTN_MIDDLE] > 0 && mouseEvent(VL_MOUSE_BTN_MIDDLE, true, lParam))
				return 1;
			break;
		case WM_LBUTTONUP:
			if (g_mouseUsed[VL_MOUSE_BTN_LEFT] > 0 && mouseEvent(VL_MOUSE_BTN_LEFT, false, lParam))
				return 1;
			break;
		case WM_RBUTTONUP:
			if (g_mouseUsed[VL_MOUSE_BTN_RIGHT] > 0 && mouseEvent(VL_MOUSE_BTN_RIGHT, false, lParam))
				return 1;
			break;
		case WM_MBUTTONUP:
			if (g_mouseUsed[VL_MOUSE_BTN_MIDDLE] > 0 && mouseEvent(VL_MOUSE_BTN_MIDDLE, false, lParam))
				return 1;
			break;
		case WM_MOUSEWHEEL:
			if ((g_mouseUsed[VL_MOUSE_BTN_WHEEL_UP] > 0 || g_mouseUsed[VL_MOUSE_BTN_WHEEL_DOWN]> 0) && mouseWheelEvent(0, lParam))
				return 1;
			break;
		case WM_MOUSEHWHEEL:
			if ((g_mouseUsed[VL_MOUSE_BTN_WHEEL_LEFT] > 0 || g_mouseUsed[VL_MOUSE_BTN_WHEEL_RIGHT]> 0) && mouseWheelEvent(1, lParam))
				return 1;
			break;
		}
	}

	return CallNextHookEx(hookMouseHandle, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

UINT checkMouseKeys(UINT mouseType)
{
	UINT ret = 0;

	for (int i = 0; i < VL_ACT_LAST; i++)
	{
		if (g_vlConfig[i].action == 1 && g_vlConfig[i].type == VL_TYPE_MOUSE && g_vlConfig[i].key == mouseType)
		{
			int cpt_opt = 0;
			int cpt_key = 0;

#define OPT(a) ((g_vlConfig[i].mod & MOD_ ## a) != 0)
#define KEYSTATE(a) ((GetKeyState(VK_ ## a) & (1<<15)) != 0)
#define CHECK(a,b) if( OPT( a ) ) { cpt_opt++; if( KEYSTATE( L ## b )) { cpt_key++; } else if( KEYSTATE( R ## b )) cpt_key++; }

			CHECK(WIN, WIN);
			CHECK(CONTROL, CONTROL);
			CHECK(SHIFT, SHIFT);
			CHECK(ALT, MENU);

			if (cpt_opt > 0 && cpt_opt == cpt_key)
			{
				ret = getEventFromId(i + 1);
			}

#undef KEYSTATE
#undef OPT
			break;
		}
	}

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

UINT mouseEvent(UINT p_event, bool status, LPARAM lParam)
{
	UINT ret = 0;
	MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);
	UINT evt = 0;

	if (status)
	{
		evt = checkMouseKeys(p_event);
		if (evt > 0)
		{
			ret = 1;
			g_mouseUsed[p_event] = 2;
		}
	}
	else if (g_mouseUsed[p_event] > 1)
	{
		ret = 1;

		//
		for (int i = 0; i < VL_ACT_LAST; i++)
		{
			if (g_vlConfig[i].action == 1 && g_vlConfig[i].type == VL_TYPE_MOUSE && g_vlConfig[i].key == p_event)
			{
				evt = getEventFromId(i + 1);
				break;
			}
		}
		if (evt > 0)
			PostMessage(g_hwndMain, WM_HOTKEY, evt, NULL);

		//
		g_mouseUsed[p_event] = 1;
	}

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

UINT mouseWheelEvent(UINT direction, LPARAM lParam)
{
	UINT ret = 0;

	MSLLHOOKSTRUCT ms = *((MSLLHOOKSTRUCT *)lParam);
	short delta = HIWORD(ms.mouseData);
	UINT mouseType = 0;

	if (delta == WHEEL_DELTA)
	{
		if (direction == 0 && g_mouseUsed[VL_MOUSE_BTN_WHEEL_UP] > 0)
		{
			mouseType = VL_MOUSE_BTN_WHEEL_UP;
		}
		else if (direction == 1 && g_mouseUsed[VL_MOUSE_BTN_WHEEL_LEFT] > 0)
		{
			mouseType = VL_MOUSE_BTN_WHEEL_LEFT;
		}
	}
	else if (delta == -WHEEL_DELTA)
	{
		if (direction == 0 && g_mouseUsed[VL_MOUSE_BTN_WHEEL_DOWN] > 0)
		{
			mouseType = VL_MOUSE_BTN_WHEEL_DOWN;
		}
		else if (direction == 1 && g_mouseUsed[VL_MOUSE_BTN_WHEEL_RIGHT] > 0)
		{
			mouseType = VL_MOUSE_BTN_WHEEL_RIGHT;
		}
	}

	if (mouseType == 0)
		return ret;

	int evt = checkMouseKeys(mouseType);
	if (evt > 0)
	{
		ret = 1;
		PostMessage(g_hwndMain, WM_HOTKEY, evt, NULL);
	}
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

void installHookMouse()
{
	if (!hookMouseHandle)
	{
		hookMouseHandle = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)hookMouse, g_hInst, 0);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallHookMouse()
{
	if (hookMouseHandle)
	{
		memset(g_mouseUsed, 0, sizeof(g_mouseUsed));

		UnhookWindowsHookEx(hookMouseHandle);
		hookMouseHandle = NULL;
	}
}

/* ------------------------------------------------------------------------------------------------- */