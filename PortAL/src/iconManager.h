/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef ICON_MANAGER_H
#define ICON_MANAGER_H

#include "menu.h"

#ifdef ICON_MANAGER

#include <vector>

/* ------------------------------------------------------------------------------------------------- */

typedef struct _iconLoaderThread {
	HANDLE mutex;
	HANDLE thread;
	std::vector<PortalMenuItem*>* items;
} iconLoaderThread;

typedef struct _iconLoader {
	wchar_t* filename;
	wchar_t* ext;
	HICON icon;
	UINT counter;
	iconLoaderThread* thread;
} iconLoader;

typedef std::vector<iconLoader*> iconsLoader;

/* ------------------------------------------------------------------------------------------------- */

UINT preloadIcon(wchar_t* filename);

HICON* getIcon(UINT id, LPDRAWITEMSTRUCT);
HICON* getIcon(wchar_t* filename);

void unloadIcon(wchar_t* filename);
void unloadIcon(UINT id);

void unloaderIconManager();

#endif /* ICON_MANAGER */

#endif /* ICON_MANAGER_H */