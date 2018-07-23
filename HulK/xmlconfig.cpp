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
#include "xmlconfig.h"
#include "config.h"
#include "plugin.h"
#include "hotKey.h"
#include "pugixml/pugixml.hpp"

/* ------------------------------------------------------------------------------------------------- */

UINT isTrue(const wchar_t* cTemp, UINT ret = 0x0)
{
	if( (cTemp != NULL) && (cTemp[0] != L'\0') )
	{
		if( !_wcsicmp(cTemp, L"true") )
			return 0x1;
		if( !_wcsicmp(cTemp, L"yes") )
			return 0x1;
		if( ret == 0x1 ) {
			if( !_wcsicmp(cTemp, L"false") )
				return 0x0;
			if( !_wcsicmp(cTemp, L"no") )
				return 0x0;
		}
	}
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

void configSetAction(hkConfig* entry, const wchar_t* action, const wchar_t* plugin)
{
#define CMP_HK(id) if( !wcscmp(action, HK_CFG_TXT_ ## id) ) { entry->action = HK_ACT_ ## id; entry->plugin = 0; return; }
	CMP_HK(CONFIG)
	CMP_HK(QUIT)
	CMP_HK(MINIMIZE)
	CMP_HK(CLOSE)
	CMP_HK(MAXIMIZE)
	CMP_HK(ICONIZE)
	CMP_HK(TRAYNIZE)
	CMP_HK(UNICONIZE)
	CMP_HK(ALWTOP)
#undef CMP_HK

	if (plugin == NULL || wcslen(plugin) == 0)
		return;

	pluginElem* plg = getPlugin(plugin);
	if (plg == NULL)
		return;

	entry->action = getPluginActionId(plg->module, action);
	if (entry->action == 0)
		return;
	entry->plugin = plg->id;
}

/* ------------------------------------------------------------------------------------------------- */

HulkConfig* loadConfig(wchar_t* filename)
{
	if(filename == NULL)
		return NULL;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename);
	
	if( !result )
		return NULL;

	const wchar_t* l_wc = NULL;
	UINT8 l_ui8 = 0;
	hkConfig* l_hkconfig = NULL;

	//
	HulkConfig* config = new HulkConfig(); // (HulkConfig*)malloc(sizeof(HulkConfig)); //
	ZeroMemory(config->mouses, sizeof(mouseConfig) * 4);
//	ZeroMemory(config, sizeof(HulkConfig));

	//
	pugi::xml_node root = doc.first_child();

	//
	//
	//
	pugi::xml_node node = root.child(L"plugins");
	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	{
		if (!it->attribute(L"load").as_bool())
			continue;

		l_wc = it->attribute(L"name").value();

		// Load the plugin
		if (!installPlugin(l_wc))
			continue;

		// Handle the options
		for (pugi::xml_node option = it->first_child(); option; option = option.next_sibling())
		{
			setPluginOption(l_wc, option.attribute(L"name").value(), option.attribute(L"value").value());
		}
	}

	//
	//
	//
	node = root.child(L"mouse");
	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	{
		l_wc = it->name();
		UINT8 mouse_action = MOUSE_ACT_NONE;

		if (!wcscmp(l_wc, L"drag"))
		{
			mouse_action = MOUSE_ACT_DRAG;
		}
		else if (!wcscmp(l_wc, L"resize"))
		{
			mouse_action = MOUSE_ACT_RESIZE;
		}
		else if (!wcscmp(l_wc, L"switch"))
		{
			mouse_action = MOUSE_ACT_SWITCH;
		}

		if (mouse_action == MOUSE_ACT_NONE)
			continue;

		config->mouses[mouse_action].btn = 0;
		config->mouses[mouse_action].mod = 0;

		l_ui8 = getMouseBtn( it->attribute(L"btn").value() );
		if (l_ui8 == 0)
			continue;

		config->mouses[mouse_action].btn = l_ui8;
		config->mouses[mouse_action].mod = getModifier(it->attribute(L"mod").as_string(L"win"));
	}

	//
	//
	//
	node = root.child(L"hotkeys");
	for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	{
		l_wc = it->attribute(L"action").as_string(L"-");
		if (!wcscmp(l_wc, L"-"))
			continue;

		l_hkconfig = (hkConfig*)malloc(sizeof(hkConfig));
		ZeroMemory(l_hkconfig, sizeof(hkConfig));

		// action
		l_hkconfig->action = 0;

		// plugin
		configSetAction(l_hkconfig, l_wc, it->attribute(L"plugin").value());
		l_wc = NULL;

		if(l_hkconfig->action == 0)
		{
			free(l_hkconfig);
			l_hkconfig = NULL;
			continue;
		}

		// key
		//
		l_hkconfig->key = getHotKeyCode(it->attribute(L"key").as_string(L""));
		if (l_hkconfig->key == 0)
		{
			free(l_hkconfig);
			l_hkconfig = NULL;
			continue;
		}

		// mod
		l_hkconfig->mod = getModifier(it->attribute(L"mod").as_string(L"win"));

		// target
		l_wc = it->attribute(L"target").as_string(L"current");
		l_hkconfig->target = (!wcscmp(l_wc, L"pointed")) ? HK_TARGET_HWND_POINTED : HK_TARGET_HWND_CURRENT;
		l_wc = NULL;

		config->hotkeys.push_back(l_hkconfig);
	}

	doc.reset();
	return config;
}

/* ------------------------------------------------------------------------------------------------- */