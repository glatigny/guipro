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

#ifndef PLUGIN_H
#define PLUGIN_H

#include "common.h"

#include "include/plugin.inc.h"

typedef struct pluginElement
{
	HMODULE module;
#ifdef _WIN64
	HMODULE module_x86;
#endif
	UINT8	id;
} pluginElem;

/* ----------------------------------------------------- */

bool installPlugin(const wchar_t* name);
bool removePlugin(const wchar_t* name);
bool removePlugin(pluginElem* plugin);
void uninstallPlugins();

pluginElem* getPlugin(const wchar_t* pluginName);
pluginElem* getPlugin(UINT id);
HMODULE getPluginModule(UINT8 id);

UINT activatePlugin(HMODULE module);
wchar_t* getPluginText(HMODULE module);

bool setPluginOption(const wchar_t* pluginName, const wchar_t* option, const wchar_t* value);
//void setPluginKey(const wchar_t* pluginName, UINT value);
//void setPluginMod(const wchar_t* pluginName, UINT value);

UINT getPluginActionId(HMODULE module, const wchar_t* function);
UINT getPluginActionId(const wchar_t* pluginName, const wchar_t* function);

bool callPluginAction(HMODULE module, UINT function, HWND hwnd);
bool callPluginAction(const wchar_t* pluginName, UINT function, HWND hwnd);

// void pluginHotkey(UINT id);
void pluginBalloon(pluginElem* plugin, UINT retId);

/* ----------------------------------------------------- */

#endif /* PLUGIN_H */
