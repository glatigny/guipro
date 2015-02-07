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

#ifndef PLUGIN_H
#define PLUGIN_H

#include "common.h"

#include "plugin.inc.h"

typedef struct pluginElement
{
	HMODULE module;
	UINT	key;
	UINT	mod;
	UINT	id;
} pluginElem;

/* ----------------------------------------------------- */

extern UINT g_pluginId;

/* ----------------------------------------------------- */

bool installPlugin(wchar_t* name);
bool removePlugin(wchar_t* name);
bool removePlugin(pluginElem* plugin);

UINT activatePlugin(HMODULE module);
wchar_t* getPluginText(HMODULE module);

bool setPluginOption(wchar_t* pluginName, wchar_t* option, wchar_t* value);
void setPluginKey(wchar_t* pluginName, UINT value);
void setPluginMod(wchar_t* pluginName, UINT value);

pluginElem* pluginGetModule(UINT id);
void pluginBalloon(pluginElem* plugin, UINT retId);

void pluginHotkey(UINT id);
int registerHKPlugins(LPWSTR p_registerErrors, int error);
void uninstallPlugins();

/* ----------------------------------------------------- */

#endif /* PLUGIN_H */
