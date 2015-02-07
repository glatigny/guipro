/*
	HulK - GUIPro Project ( http://obsidev.github.io/guipro/ )

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

/* ------------------------------------------------------------------------------------------------- */

// Special borderless feature.
//  This option removes the border of the window when setting it to maximum size.
//  It could be weird but some people want it :)
void setBorderless(HWND p_hwnd)
{
	LONG_PTR dwStyle = GetWindowLongPtr( p_hwnd, GWL_STYLE );
	dwStyle = (dwStyle | WS_POPUP) & ~( WS_CAPTION | WS_MINIMIZEBOX | WS_OVERLAPPED );
	SetWindowLongPtr( p_hwnd, GWL_STYLE, (LONG)dwStyle);
}

/* ------------------------------------------------------------------------------------------------- */

void setBorderfull(HWND p_hwnd)
{
	LONG_PTR dwStyle = GetWindowLongPtr( p_hwnd, GWL_STYLE );
	dwStyle = (dwStyle | WS_CAPTION | WS_MINIMIZEBOX | WS_OVERLAPPED );
	SetWindowLongPtr( p_hwnd, GWL_STYLE, (LONG)dwStyle);
}

/* ------------------------------------------------------------------------------------------------- */
