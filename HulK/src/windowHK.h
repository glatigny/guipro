/*
	HulK - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny J�r�me <jerome@obsi.dev>

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

#ifndef WINDOW_HK_H
#define WINDOW_HK_H

#include "common.h"

void minimizeWindow( HWND p_hwnd );
void closeWindow( HWND p_hwnd );
void maximizeWindow( HWND p_hwnd );
void iconizeWindow( HWND p_hwnd );
void traynizeWindow( HWND p_hwnd );
void alwaysOnTop( HWND p_hwnd );
void borderlessWindow( HWND p_hwnd );

void setWindow( HWND p_hwnd );

#endif /* WINDOW_HK_H */
