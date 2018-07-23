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

#ifndef PLUGIN_INC_H
#define PLUGIN_INC_H

/* ----------------------------------------------------- */

// SendMessage( g_ownerHwnd, WM_PLUGIN_EVENT, id / hwnd / param... , PORTAL_... );

#define WM_PLUGIN_EVENT		(WM_USER + 101)

#define PORTAL_MINIMIZE		(1)
#define PORTAL_MAXIMISE		(2)
#define PORTAL_CLOSE		(3)
#define PORTAL_ICONIZE		(4)
#define PORTAL_TRAYNIZE		(5)
#define PORTAL_UNICONIZE	(6)
#define PORTAL_ONTOP		(7)
#define PORTAL_BALLOON		(8)

/* ----------------------------------------------------- */

// activatePlugin() should return one:

#define PORTAL_PLUGIN_ERROR			(0)
// Ask for the getText() function in order to show a message in the balloon
#define PORTAL_PLUGIN_ERROR_TXT		(1)

#define PORTAL_PLUGIN_OK			(200)
// Ask for the getText() function in order to show a message in the balloon
#define PORTAL_PLUGIN_RETURN_TXT	(201)

/* ----------------------------------------------------- */

typedef UINT (pluginInitFct)(HWND p_owner, UINT id);
typedef UINT (pluginWcharFct)(const wchar_t*);
typedef UINT (pluginDblWcharFct)(const wchar_t*, const wchar_t*);
typedef UINT (pluginUintHwndFct)(UINT, HWND);
typedef UINT (pluginVoidFct)(void);
typedef wchar_t* (pluginRetStrFct)(void);

#endif /* PLUGIN_INC_H */