/*
	PortAL - GUIPro Project ( http://obsidev.github.io/guipro/ )

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

#ifndef CONFIG_H
#define CONFIG_H

#include <windows.h>
#include <vector>
#include "portalConfig.h"

/*-- STRUCTURES --*/

/*-- VARIABLES --*/
extern PortalConfig* g_portal;
extern PortalProgVector g_portal_files;

/*-- FUNCTIONS --*/

int menuGetNbElem();
PortalProg* menuGetElem(int pos);

int menuGetNbFiles();
PortalProg* menuGetFile(int pos);

int openConfig();

int registerConfig(int alert);

int registerHotkeys(LPWSTR p_registerErrors);

void unregisterHotkeys();

DWORD WINAPI threadFileNotification(LPVOID lpthis);
bool installFileNotification();
bool uninstallFileNotification();

#endif /* CONFIG_H */
