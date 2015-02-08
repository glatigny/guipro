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

#include "config.h"
#include "common.h"
#include "main.h"
#include "hotkey.h"
#include "plugin.h"

#include <fstream>

mouseConfig g_mouse_options[MOUSE_ACT_NONE];
hkConfig* g_hkConfig = NULL;

/* ------------------------------------------------------------------------------------------------- */

int openConfig()
{
	clearConfig();

	wchar_t szConfigName[] = L"hulk.ini";
	wchar_t szFilename[FILENAME_MAX] = L"";

	// retrieve the path of the binary
	GetModuleFileName(g_hInst, szFilename, FILENAME_MAX);
	wchar_t* pos = wcsrchr(szFilename, L'\\');
	if( pos != NULL )
	{
		pos++;
		wcscpy_s(pos, szFilename - pos + SIZEOF_ARRAY(szFilename), szConfigName);
	}
	else
	{
		return 1;
	}

	wchar_t buff[LINE_MAX_SIZE];
	wchar_t *tmp;

	std::wifstream l_file(szFilename);
	if( l_file.is_open())
	{
		hkConfig* lastHkConfig = g_hkConfig;
		UINT mod = HK_ACT_CONFIG;
		UINT type = 0;
		wchar_t pluginName[MAX_PLUGIN_LEN];

		char memory[HK_ACT_NONE];
		memset(memory, 0, sizeof(memory));

		while( !l_file.eof() )
		{
			l_file.getline(buff, LINE_MAX_SIZE);
			
			if( buff[0] == L'[' )
			{
				UINT ex = mod;
				UINT exT = type;

#define CMP_HK(id) if( !wcsncmp(buff, HK_TXT_ ## id, wcslen(HK_TXT_ ## id)) ) { mod = HK_ACT_ ## id; type = TYPE_HK; }
#define CMP_MOUSE(id) if( !wcsncmp(buff, MOUSE_TXT_ ## id, wcslen(MOUSE_TXT_ ## id)) ) { mod = MOUSE_ACT_ ## id; type = TYPE_MOUSE; }
	
				CMP_HK( CONFIG )
				else CMP_HK( MINIMISE_POINTED )
				else CMP_HK( MINIMISE_CURRENT )
				else CMP_HK( CLOSE_POINTED )
				else CMP_HK( CLOSE_CURRENT )
				else CMP_HK( MAXIMISE_POINTED )
				else CMP_HK( MAXIMISE_CURRENT )
				else CMP_HK( ICONIZE_POINTED )
				else CMP_HK( ICONIZE_CURRENT )
				else CMP_HK( TRAYNIZE_POINTED )
				else CMP_HK( TRAYNIZE_CURRENT )
				else CMP_HK( UNICONIZE )
				else CMP_HK( ALWTOP_POINTED )
				else CMP_HK( ALWTOP_CURRENT )
				else CMP_HK( QUIT )
				else CMP_MOUSE( DRAG )
				else CMP_MOUSE( RESIZE )
				else CMP_MOUSE( SWITCH )
				else if( !wcsncmp(buff, HK_TXT_PLUGIN, wcslen(HK_TXT_PLUGIN)) )
				{
					mod = HK_ACT_PLUGIN;

					ZeroMemory(pluginName, sizeof(pluginName));
					size_t size1 = wcslen(buff);
					size_t size2 = wcslen(HK_TXT_PLUGIN);
					wcsncpy_s(pluginName, &buff[size2], size1-size2-1);
					wcscat_s(pluginName, L".dll");

					installPlugin(pluginName);
				}
#undef CMP_HK
#undef CMP_MOUSE
				
				if( ((ex != mod) || exT != type)&& (mod != HK_ACT_PLUGIN) )
				{
					if( (type == TYPE_HK) && (memory[mod] == 0) )
					{
						if( lastHkConfig == NULL )
						{
							g_hkConfig = (hkConfig*)malloc(sizeof(hkConfig));
							lastHkConfig = g_hkConfig;
						}
						else
						{
							lastHkConfig->next = (hkConfig*)malloc(sizeof(hkConfig));
							lastHkConfig = lastHkConfig->next;
						}

						lastHkConfig->action = IDH_HOTKEY_BASE + mod;
						lastHkConfig->key = 0;
						lastHkConfig->mod = MOD_WIN; // Set "Windows" key by default
						lastHkConfig->next = NULL;
					}
				}
			}
			else if( buff[0] != L';' )
			{
				tmp = wcsstr(buff, L"=");
				if( tmp != NULL )
				{
					tmp[0] = L'\0';

					if( (mod == HK_ACT_CONFIG) && (type == TYPE_HK) )
					{
// #define powa !
// This is a special technique in order to have a final ";" when using the macro.
// It could be very usefull if the number of option increase.
#define CONFIG_CMP(txt, opt) if( !wcscmp( buff, txt ) ) do { \
								if( (tmp[1] == L'y') || (tmp[1] == L'o') ) opt = true; \
								else opt = false; } while(0)

					//	CONFIG_CMP( L"switcher", g_option_switcher );
					//	CONFIG_CMP( L"drag", g_option_drag );
					//	CONFIG_CMP( L"resize", g_option_resize );
#undef CONFIG_CMP
					}
					else if( (mod == HK_ACT_PLUGIN) && (type == TYPE_HK) )
					{
						if( !wcscmp( buff, L"key" ) )
						{
							setPluginKey(pluginName, getHotKeyCode( &tmp[1] ));
						}
						else if( !wcscmp( buff, L"mod" ) )
						{
							setPluginMod(pluginName, getModifier( &tmp[1] ));
						}
						else
						{
							setPluginOption(pluginName, buff, &tmp[1]);
						}
					}
					else
					{
						if( type == TYPE_HK )
						{
							if( !wcscmp( buff, L"key" ) )
							{
								lastHkConfig->key = getHotKeyCode( &tmp[1] );
							}
							else if( !wcscmp( buff, L"mod" ) )
							{
								lastHkConfig->mod = getModifier( &tmp[1] );
							}
						}
						else if( type == TYPE_MOUSE )
						{
							if( !wcscmp( buff, L"btn" ) )
							{
								g_mouse_options[mod].btn = getMouseBtn( &tmp[1] );
							}
							else if( !wcscmp( buff, L"mod" ) )
							{
								g_mouse_options[mod].mod = getModifier( &tmp[1] );
							}
						}
					}
					tmp[0] = L'=';
				}
			}
		}
		l_file.close();
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

int registerConfig(int alert)
{
	LPWSTR l_registerErrors = (LPWSTR)malloc(sizeof(PWSTR) * MAX_LENGTH);
	memset(l_registerErrors, 0, sizeof(PWSTR) * MAX_LENGTH);
	if( registerHotkeys(l_registerErrors) > 0)
	{
		if( alert )
		{
			size_t l_size = (wcslen(ERR_HOTKEYS_MSG) + wcslen(l_registerErrors) + 2);
			LPWSTR l_message = (LPWSTR)malloc(sizeof(PWSTR) * l_size);
			wcscpy_s(l_message, l_size, ERR_HOTKEYS_MSG);
			wcscat_s(l_message, l_size, l_registerErrors);
			
			MessageBox(g_hwndMain, l_message, ERR_MSGBOX_TITLE, NULL);
			
			free(l_message);
			free(l_registerErrors);
		}
		return FALSE;
	}
	free(l_registerErrors);
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

int registerHotkeys(LPWSTR p_registerErrors)
{
	int error = 0;

	hkConfig* elem = g_hkConfig;
	while( elem != NULL )
	{
		if( elem->key > 0 )
		{
			if( !RegisterHK(elem->key, elem->mod, elem->action) )
			{
				error++;

				LPWSTR l_errkey = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
				wcscpy_s(l_errkey, MAX_ERRHKLEN, getInverseModifier(elem->mod));
				wcscat_s(l_errkey, MAX_ERRHKLEN, getInverseHotKeyCode(elem->key));

				if( (wcslen(p_registerErrors) + wcslen(l_errkey)) >= MAX_LENGTH)
				{
					return error;	
				}

				wcscat_s(p_registerErrors, MAX_LENGTH, l_errkey);
				wcscat_s(p_registerErrors, MAX_LENGTH, (wchar_t*)"\n");
			}
		}
		elem = elem->next;
	}

	error = registerHKPlugins(p_registerErrors, error);

	return error;
}

/* ------------------------------------------------------------------------------------------------- */

void clearConfig()
{
	memset(g_mouse_options, 0, sizeof(g_mouse_options));

	hkConfig* tmp = g_hkConfig;
	while( tmp != NULL )
	{
		g_hkConfig = tmp->next;
		if( tmp->key > 0 )
		{
			UnregisterHK(tmp->action);
		}
		free(tmp);
		tmp = g_hkConfig;
	}
	g_hkConfig = NULL;
}

/* ------------------------------------------------------------------------------------------------- */
