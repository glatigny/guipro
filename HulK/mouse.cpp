/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@darksage.fr>

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

#include "mouse.h"
#include "keyboard.h"
#include "main.h"
#include "hotKey.h"
#include "windowHK.h"
#include "config.h"
#include <list>

HHOOK hookMouseHandle;

bool movingInProgress = false;
bool resizeInProgress = false;
int mousePositionX = 0;
int mousePositionY = 0;
bool resizeStatusX = false;
bool resizeStatusY = false;

HWND mouseHwndPointed = NULL;
HWND mouseHwndMinBtnPointed = NULL;
RECT mouseApplicationRect;

/* ------------------------------------------------------------------------------------------------- */

__declspec(dllexport) LRESULT CALLBACK hookMouse(int nCode, WPARAM wParam, LPARAM lParam) 
{
	if( nCode >= 0 )
	{
		switch( wParam )
		{
			case WM_LBUTTONDOWN:
				if (mouseEvent(MOUSE_BTN_LEFT, true, lParam))
					return 1;
				break;

			case WM_RBUTTONDOWN:
				if (mouseEvent(MOUSE_BTN_RIGHT, true, lParam))
					return 1;
				break;

			case WM_MBUTTONDOWN:
				if (mouseEvent(MOUSE_BTN_MIDDLE, true, lParam))
					return 1;
				break;

			case WM_LBUTTONUP:
				if (mouseEvent(MOUSE_BTN_LEFT, false, lParam))
					return 1;
				break;

			case WM_RBUTTONUP:
				if (mouseEvent(MOUSE_BTN_RIGHT, false, lParam))
					return 1;
				break;

			case WM_MBUTTONUP:
				if (mouseEvent(MOUSE_BTN_MIDDLE, false, lParam))
					return 1;
				break;

			case WM_MOUSEMOVE:
				if( movingInProgress )
				{
					drag_process(lParam);
				}
				else if( resizeInProgress )
				{
					resize_process(lParam);
				}
				break;

			case WM_MOUSEWHEEL:
				if (mouseEvent(MOUSE_BTN_WHEEL, false, lParam))
					return 1;
				break;
		}
	}

	return CallNextHookEx(hookMouseHandle, nCode, wParam, lParam);
}

/* ------------------------------------------------------------------------------------------------- */

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	LONG_PTR dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
	LONG_PTR dwStyleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

	if( (dwStyle & WS_VISIBLE) && !(dwStyleEx & WS_EX_TOPMOST) )
	{
		HWND g_hwndShell = GetShellWindow();
		HWND g_deskWindow = GetDesktopWindow();
		HWND hwndOwner, hwndTmp = hwnd;

		do {
			hwndOwner = hwndTmp;
			hwndTmp = GetWindow(hwndTmp, GW_OWNER);
		} while (hwndTmp && (hwndTmp != g_hwndShell) && (hwndTmp != g_deskWindow)); // service messages

		if( (hwnd == hwndOwner) && ( hwndOwner != g_hwndShell ) )
		{
			((std::list<HWND>*)lParam)->push_back(hwndOwner);
		}
	}

	return (TRUE);
}

/* ------------------------------------------------------------------------------------------------- */

UINT mouseEvent(UINT p_event, bool status, LPARAM lParam)
{
	UINT ret = 0;

	if (g_hulk == NULL)
		return ret;

#define KEYSTATE(a) ((GetKeyState(VK_ ## a) & (1<<15)) != 0)
#define MOUSE_OPTION(a,b) ((g_hulk->mouses[MOUSE_ACT_ ## a].mod & MOD_ ## b) != 0)

	if( g_hulk->mouses[MOUSE_ACT_DRAG].btn == p_event )
	{
		if( ((!movingInProgress == status) &&
			(KEYSTATE(LMENU) == MOUSE_OPTION(DRAG, ALT)) && 
			(KEYSTATE(LCONTROL) == MOUSE_OPTION(DRAG, CONTROL)) && 
			(KEYSTATE(LSHIFT) == MOUSE_OPTION(DRAG, SHIFT)) && 
			(KEYSTATE(LWIN) == MOUSE_OPTION(DRAG, WIN)))
			|| (movingInProgress && !status))
		{
			if( MOUSE_OPTION(DRAG, ALT) || MOUSE_OPTION(DRAG, WIN) )
			{
				key_press_code(VK_RSHIFT);
				//keybd_event( VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | 0, 0 );
				//keybd_event( VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
			}

			if( status )
			{
				if( drag_activation(lParam) )
					ret = 1;
			}
			else
			{
				movingInProgress = false;
				ret = 1;
			}
		}
	}

	if(g_hulk->mouses[MOUSE_ACT_RESIZE].btn == p_event)
	{
		if( ((!resizeInProgress && status) &&
			(KEYSTATE(LMENU) == MOUSE_OPTION(RESIZE, ALT)) && 
			(KEYSTATE(LCONTROL) == MOUSE_OPTION(RESIZE, CONTROL)) && 
			(KEYSTATE(LSHIFT) == MOUSE_OPTION(RESIZE, SHIFT)) && 
			(KEYSTATE(LWIN) == MOUSE_OPTION(RESIZE, WIN)))
			|| (resizeInProgress && !status))
		{
			if( MOUSE_OPTION(RESIZE, ALT) || MOUSE_OPTION(RESIZE, WIN) )
			{
				key_press_code(VK_RSHIFT);
				//keybd_event( VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | 0, 0 );
				//keybd_event( VK_RSHIFT, 0x36, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 );
			}

			if( status )
			{
				if( resize_activation(lParam) )
					ret = 1;
			}
			else
			{
				resizeInProgress = false;
				ret = 1;
			}
		}
	}

	if(g_hulk->mouses[MOUSE_ACT_SWITCH].btn == p_event )
	{
		if( (KEYSTATE(LMENU) == MOUSE_OPTION(SWITCH, ALT)) && 
			(KEYSTATE(LCONTROL) == MOUSE_OPTION(SWITCH, CONTROL)) && 
			(KEYSTATE(LSHIFT) == MOUSE_OPTION(SWITCH, SHIFT)) && 
			(KEYSTATE(LWIN) == MOUSE_OPTION(SWITCH, WIN)))
		{
			if (MOUSE_OPTION(SWITCH, ALT))
				//keybd_event(VK_LMENU, 0, KEYEVENTF_KEYUP, 0);
				key_send_code(VK_LMENU, KEYEVENTF_KEYUP);
			if( MOUSE_OPTION(SWITCH, CONTROL) )
				//keybd_event(VK_LCONTROL, 0, KEYEVENTF_KEYUP, 0);
				key_send_code(VK_LCONTROL, KEYEVENTF_KEYUP);
			if( MOUSE_OPTION(SWITCH, SHIFT) )
				//keybd_event(VK_LSHIFT, 0, KEYEVENTF_KEYUP, 0);
				key_send_code(VK_LSHIFT, KEYEVENTF_KEYUP);

			change_current_window(lParam);
			ret = 1;
		}
	}

#undef KEYSTATE
#undef MOUSE_OPTION

	if( p_event == MOUSE_BTN_RIGHT )
	{
		MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);
		HWND destHwnd = WindowFromPoint(ms.pt);
		LRESULT dw;

		dw = SendMessage(destHwnd, WM_NCHITTEST, 0, MAKELPARAM((WORD)ms.pt.x, (WORD)ms.pt.y));
		if (dw == HTMINBUTTON)
		{
			if( status )
			{
				mouseHwndMinBtnPointed = destHwnd;
				ret = 1;
			}
			else
			{
				if( destHwnd == mouseHwndMinBtnPointed )
				{
					traynizeWindow(destHwnd);
					//iconizeWindow(destHwnd);
					ret = 1;
				}
				else
				{
					destHwnd = NULL;
				}
			}
		}
		else if( mouseHwndMinBtnPointed != NULL )
		{
			mouseHwndMinBtnPointed = NULL;
		}
	}

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

inline bool drag_activation(LPARAM lParam)
{
	MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);
	mousePositionX = ms.pt.x;
	mousePositionY = ms.pt.y;
	
	mouseHwndPointed = HulkGetPointedWindow();
	if( mouseHwndPointed )
	{
		// Drag activation
		movingInProgress = true;
		GetWindowRect( mouseHwndPointed, &mouseApplicationRect );

		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

inline void drag_process(LPARAM lParam)
{
	MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);

	SetWindowPos( mouseHwndPointed, NULL, 
		mouseApplicationRect.left + ms.pt.x - mousePositionX, 
		mouseApplicationRect.top + ms.pt.y - mousePositionY, 
		mouseApplicationRect.right - mouseApplicationRect.left, 
		mouseApplicationRect.bottom - mouseApplicationRect.top, 0);
}

/* ------------------------------------------------------------------------------------------------- */

inline bool resize_activation(LPARAM lParam)
{
	MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);

	mousePositionX = ms.pt.x;
	mousePositionY = ms.pt.y;
	
	mouseHwndPointed = HulkGetPointedWindow();
	if( mouseHwndPointed )
	{
		// Resize activation
		resizeInProgress = true;

		GetWindowRect( mouseHwndPointed, &mouseApplicationRect );

		int mouseInWindowX = mousePositionX - mouseApplicationRect.left;
		int mouseInWindowY = mousePositionY - mouseApplicationRect.top;
		
		int a = mouseApplicationRect.right - mouseApplicationRect.left;
		if( mouseInWindowX <= a/2 )
			resizeStatusX = false;
		else
			resizeStatusX = true;

		a = mouseApplicationRect.bottom - mouseApplicationRect.top;
		if( mouseInWindowY <= a/2 )
			resizeStatusY = false;
		else
			resizeStatusY = true;

		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

inline void resize_process(LPARAM lParam)
{
	MOUSEHOOKSTRUCT ms = *((MOUSEHOOKSTRUCT *)lParam);
	int newX1, newX2, newY1, newY2;

	if( resizeStatusX == false )
	{
		newX1 = mouseApplicationRect.left + ms.pt.x - mousePositionX;
		newX2 = mouseApplicationRect.right - mouseApplicationRect.left - ms.pt.x + mousePositionX;
	}
	else
	{
		newX1 = mouseApplicationRect.left;
		newX2 = mouseApplicationRect.right - mouseApplicationRect.left + ms.pt.x - mousePositionX;
	}

	if( resizeStatusY == false )
	{
		newY1 = mouseApplicationRect.top + ms.pt.y - mousePositionY;
		newY2 = mouseApplicationRect.bottom - mouseApplicationRect.top - ms.pt.y + mousePositionY;
	}
	else
	{
		newY1 = mouseApplicationRect.top;
		newY2 = mouseApplicationRect.bottom - mouseApplicationRect.top + ms.pt.y - mousePositionY;
	}

	if( newX2 < HULK_MIN_SIZE_PX ) newX2 = HULK_MIN_SIZE_PX;
	if( newY2 < HULK_MIN_SIZE_PX ) newY2 = HULK_MIN_SIZE_PX;

	SendMessage(mouseHwndPointed, WM_ENTERSIZEMOVE, 0, 0);
	SetWindowPos(mouseHwndPointed, NULL, newX1, newY1, newX2, newY2, 0);
	SendMessage(mouseHwndPointed, WM_EXITSIZEMOVE, 0, 0);
}

/* ------------------------------------------------------------------------------------------------- */

inline void change_current_window(LPARAM lParam)
{
	std::list<HWND> l_hwnd;
	EnumDesktopWindows(NULL, EnumWindowsProc, (LPARAM)&l_hwnd);

	int wheelData = (int)(((PMSLLHOOKSTRUCT)lParam)->mouseData) >> 16;

	HWND previous = NULL;
	HWND currentHwnd = GetForegroundWindow();

	if( wheelData < 0 )
	{
		std::list<HWND>::iterator it = l_hwnd.begin();
		it++;
		
		SetWindowPos( currentHwnd, HWND_BOTTOM, 0,0,0,0, SWP_NOSIZE | SWP_NOMOVE );
		
		setWindow( *it );
	}
	else
	{
		for( std::list<HWND>::iterator it = l_hwnd.begin(); it != l_hwnd.end(); it++ )
		{
			previous = (*it);
		}
		setWindow( previous );
	}
}

/* ------------------------------------------------------------------------------------------------- */

void installHookMouse()
{
	if( !hookMouseHandle )
	{
		hookMouseHandle  = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)hookMouse, g_hInst, 0);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallHookMouse()
{
	if( hookMouseHandle )
	{
		UnhookWindowsHookEx(hookMouseHandle);
		hookMouseHandle = NULL;
	}
}

/* ------------------------------------------------------------------------------------------------- */
