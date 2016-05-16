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

#include "windowHK.h"
#include "main.h"
#include "config.h"
#include "iconize.h"

/* ------------------------------------------------------------------------------------------------- */

void minimizeWindow( HWND p_hwnd )
{
	SendMessage(p_hwnd, WM_SYSCOMMAND, SC_MINIMIZE, NULL);
}

/* ------------------------------------------------------------------------------------------------- */

void closeWindow( HWND p_hwnd )
{
	SendMessage(p_hwnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
}

/* ------------------------------------------------------------------------------------------------- */

void maximizeWindow( HWND p_hwnd )
{
	LONG_PTR dwStyle = GetWindowLongPtr(p_hwnd, GWL_STYLE);
	if( dwStyle & WS_MAXIMIZE )
	{
		SendMessage(p_hwnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
	}
	else
	{
		SendMessage(p_hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, NULL);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void alwaysOnTop( HWND p_hwnd )
{
	LONG_PTR dwStyle = GetWindowLongPtr(p_hwnd, GWL_EXSTYLE);
	if( dwStyle & WS_EX_TOPMOST )
	{
		SetWindowPos(p_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	else
	{
		SetWindowPos(p_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
}

/* ------------------------------------------------------------------------------------------------- */

void iconizeWindow( HWND p_hwnd )
{
	Iconize( p_hwnd, ICONIZE_OPT_SYSTRAY);
}

/* ------------------------------------------------------------------------------------------------- */

void traynizeWindow( HWND p_hwnd )
{
	Iconize( p_hwnd, ICONIZE_OPT_INTERNAL);
}

/* ------------------------------------------------------------------------------------------------- */

void setWindow( HWND p_hwnd )
{
	if( p_hwnd != NULL )
	{
		SetForegroundWindow( p_hwnd );
		SetActiveWindow( p_hwnd );
		BringWindowToTop( p_hwnd );
	
		if( IsIconic( p_hwnd ) )
		{
			PostMessage( p_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0 );
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */
