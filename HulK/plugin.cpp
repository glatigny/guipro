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
UINT g_pluginId = 0;

/* ------------------------------------------------------------------------------------------------- */

bool installPlugin(wchar_t* name)
{
	if( g_pluginList.find(name) == g_pluginList.end() )
	{
		HMODULE hkPlugin = LoadLibrary(name);
		if( hkPlugin != NULL )
		{
			pluginInitFct* fct = (pluginInitFct*)GetProcAddress( hkPlugin, "initPlugin" );
			if( fct != NULL )
			{
				pluginElem* elem = (pluginElem*)malloc(sizeof(pluginElem));
				elem->module = hkPlugin;
				elem->id = g_pluginId++;
				elem->key = 0;
				elem->mod = MOD_WIN;

				g_pluginList[_wcsdup(name)] = elem;

				fct(g_hwndMain, elem->id);

				return true;
			}
		}
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool removePlugin(wchar_t* name)
{
	pluginMap::iterator item = g_pluginList.find(name);

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
	if( plugin->module )
	{
		pluginVoidFct* fct = (pluginVoidFct*)GetProcAddress( plugin->module, "releasePlugin" );
		if( fct != NULL )
		{
			fct();
		}
		BOOL ret = FreeLibrary(plugin->module);
		plugin->module = NULL;

		return (ret == TRUE);
	}
	if( plugin->key > 0 )
	{
		UnregisterHK(IDH_HOTKEY_PLUGIN_BASE + plugin->id);
	}
	return true;
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

bool setPluginOption(wchar_t* pluginName, wchar_t* option, wchar_t* value)
{
	pluginMap::iterator item = g_pluginList.find(pluginName);

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

void setPluginKey(wchar_t* pluginName, UINT value)
{
	pluginMap::iterator item = g_pluginList.find(pluginName);

	if( item != g_pluginList.end() )
	{
		item->second->key = value;
	}	
}

/* ------------------------------------------------------------------------------------------------- */

void setPluginMod(wchar_t* pluginName, UINT value)
{
	pluginMap::iterator item = g_pluginList.find(pluginName);

	if( item != g_pluginList.end() )
	{
		item->second->mod = value;
	}
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

pluginElem* pluginGetModule(UINT id)
{
	for( pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++ )
	{
		if( i->second->id == id )
		{
			return i->second;
		}
	}
	return NULL;
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

int registerHKPlugins(LPWSTR p_registerErrors, int error)
{
	for( pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++ )
	{
		if( i->second->key > 0 )
		{
			if( !RegisterHK(i->second->key, i->second->mod, IDH_HOTKEY_PLUGIN_BASE + i->second->id) )
			{
				error++;

				LPWSTR l_errkey = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
				wcscpy_s(l_errkey, MAX_ERRHKLEN, getInverseModifier(i->second->mod));
				wcscat_s(l_errkey, MAX_ERRHKLEN, getInverseHotKeyCode(i->second->key));
				
				if( (wcslen(p_registerErrors) + wcslen(l_errkey)) >= MAX_LENGHT)
				{
					return error;	
				}

				wcscat_s(p_registerErrors, MAX_LENGHT, l_errkey);
				wcscat_s(p_registerErrors, MAX_LENGHT, (wchar_t*)"\n");
			}
		}
	}
	return error;
}

/* ------------------------------------------------------------------------------------------------- */

void uninstallPlugins()
{
	for( pluginMap::iterator i = g_pluginList.begin(); i != g_pluginList.end(); i++ )
	{
		removePlugin( i->second );
		free( i->second );
		free( i->first );
	}
	g_pluginList.clear();
}

/* ------------------------------------------------------------------------------------------------- */
