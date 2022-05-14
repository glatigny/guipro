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

#include "plugin.h"
#include "main.h"
#include "hotkey.h"
#include <map>

#ifndef NIIF_USER
#define NIIF_USER       0x00000004
#endif

struct strCmp
{
	bool operator()(const wchar_t* s1, const wchar_t* s2) const
	{ return wcscmp(s1, s2) < 0; }
};

typedef std::map<wchar_t*, pluginElem*, strCmp> pluginMap;

pluginMap g_pluginList;
UINT8 g_pluginId = 1;

/* ------------------------------------------------------------------------------------------------- */

bool installPlugin(const wchar_t* name)
{
	// Already loaded
	if (g_pluginList.find((wchar_t*)name) != g_pluginList.end())
		return true;

	wchar_t full_name[MAX_FILE_LEN];
	
	wchar_t *sp = specialDirs(L"%hulk%\\");
	wcscpy_s(full_name, sp);
	free(sp);
	wcscat_s(full_name, name);
	wcscat_s(full_name, L".dll");

	HMODULE hkPlugin = LoadLibrary(full_name);
	if (hkPlugin == NULL)
		return false;

	pluginInitFct* initPluginFct = (pluginInitFct*)GetProcAddress( hkPlugin, "initPlugin" );
	if (initPluginFct == NULL)
	{
		FreeLibrary(hkPlugin);
		return false;
	}

	pluginElem* elem = (pluginElem*)malloc(sizeof(pluginElem));
	elem->module = hkPlugin;
	elem->id = g_pluginId++;
#ifdef _WIN64
	elem->module_x86 = NULL;
#endif

	UINT ret = initPluginFct(g_hwndMain, elem->id);
	if (ret == FALSE)
	{
		FreeLibrary(hkPlugin);
		free(elem);
		g_pluginId--;
		return false;
	}

#ifdef _WIN64
	wchar_t x86name[256];
	memset(x86name, 0, sizeof(x86name));
	wcscpy_s(x86name, name);
	wcscat_s(x86name, L"_x86");
	HMODULE hk86Plugin = LoadLibrary(x86name);
	if (hk86Plugin)
	{
		initPluginFct = (pluginInitFct*)GetProcAddress(hk86Plugin, "initPlugin");
		if (initPluginFct != NULL)
		{
			initPluginFct(g_hwndMain, elem->id);
		}
		elem->module_x86 = hk86Plugin;
	}
#endif

	g_pluginList[_wcsdup(name)] = elem;

	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool removePlugin(const wchar_t* name)
{
	pluginMap::iterator item = g_pluginList.find((wchar_t*)name);

	if( item != g_pluginList.end() )
	{
		removePlugin( item->second );
		free( item->second );
		free( item->first );
		g_pluginList.erase(item);
	}

	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool removePlugin(pluginElem* plugin)
{
	if (!plugin->module)
		return true;

	pluginVoidFct* fct = (pluginVoidFct*)GetProcAddress( plugin->module, "releasePlugin" );
	if( fct != NULL )
	{
		fct();
	}

	BOOL ret = FreeLibrary(plugin->module);
	plugin->module = NULL;

#ifdef _WIN64
	if (plugin->module_x86 != NULL)
	{
		fct = (pluginVoidFct*)GetProcAddress(plugin->module_x86, "releasePlugin");
		if (fct != NULL)
		{
			fct();
		}
		FreeLibrary(plugin->module_x86);
		plugin->module_x86 = NULL;
	}
#endif

	return (ret == TRUE);
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallPlugins()
{
	for (pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++)
	{
		removePlugin(i->second);
		free(i->second);
		free(i->first);
	}
	g_pluginList.clear();
}

/* ------------------------------------------------------------------------------------------------- */

pluginElem* getPlugin(const wchar_t* pluginName)
{
	pluginMap::iterator item = g_pluginList.find((wchar_t*)pluginName);
	if (item == g_pluginList.end())
		return NULL;
	return item->second;
}

/* ------------------------------------------------------------------------------------------------- */

pluginElem* getPlugin(UINT id)
{
	for (pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++)
	{
		if (i->second->id == id)
		{
			return i->second;
		}
	}
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

HMODULE getPluginModule(UINT8 id)
{
	for (pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++)
	{
		if (i->second->id == id)
		{
			return i->second->module;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

UINT activatePlugin(HMODULE module)
{
	pluginVoidFct* fct = (pluginVoidFct*)GetProcAddress( module, "activatePlugin" );
	if( fct != NULL )
	{
		return fct();
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getPluginText(HMODULE module)
{
	pluginRetStrFct* fct = (pluginRetStrFct*)GetProcAddress( module, "getText" );
	if( fct != NULL )
	{
		return fct();
	}

	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

bool setPluginOption(const wchar_t* pluginName, const wchar_t* option, const wchar_t* value)
{
	pluginMap::iterator item = g_pluginList.find((wchar_t*)pluginName);

	if( item != g_pluginList.end() )
	{
		pluginDblWcharFct* fct = (pluginDblWcharFct*)GetProcAddress( item->second->module, "setOption" );
		if( fct != NULL )
		{
			fct(option, value);
			return true;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getPluginActionId(HMODULE module, const wchar_t* function)
{
	pluginWcharFct* fct = (pluginWcharFct*)GetProcAddress(module, "getAction");
	if (fct != NULL)
	{
		return fct((wchar_t*)function);
	}
	return 0;
}

UINT getPluginActionId(const wchar_t* pluginName, const wchar_t* function)
{
	pluginMap::iterator item = g_pluginList.find((wchar_t*)pluginName);

	if (item != g_pluginList.end())
	{
		return getPluginActionId((HMODULE)item->second->module, function);
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

 bool callPluginAction(HMODULE module, UINT action, HWND hwnd)
{
	pluginUintHwndFct* fct = (pluginUintHwndFct*)GetProcAddress(module, "callAction");
	if (fct != NULL)
	{
		fct(action, hwnd);
		return true;
	}
	return false;
}

bool callPluginAction(const wchar_t* pluginName, UINT action, HWND hwnd)
{
	pluginMap::iterator item = g_pluginList.find((wchar_t*)pluginName);

	if (item != g_pluginList.end())
	{
		return callPluginAction((HMODULE)item->second->module, action, hwnd);
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

void pluginHotkey(UINT id)
{
	id -= IDH_HOTKEY_PLUGIN_BASE;

	for( pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++ )
	{
		if( i->second->id == id )
		{
			UINT ret = activatePlugin(i->second->module);
			pluginBalloon(i->second, ret);
			return;
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

void pluginBalloon(pluginElem* plugin, UINT retId)
{
	if( plugin )
	{
		if( (retId == PORTAL_PLUGIN_RETURN_TXT) || (retId == PORTAL_PLUGIN_ERROR_TXT) )
		{
			wchar_t* text = getPluginText(plugin->module);
			if( text != NULL )
			{
				ShowBalloon(WC_HULK_ABOUT_TEXT_TITLE, text, 
					(retId == PORTAL_PLUGIN_RETURN_TXT) ? NIIF_USER : NIIF_ERROR );

				free(text);
			}
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

