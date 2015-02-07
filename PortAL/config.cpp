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

#include "common.h"
#include "config.h"
#include "main.h"
#include "hotKey.h"
#include "resource.h"
#include "xmlconfig.h"

PortalConfig* g_portal;
PortalProgVector g_portal_files;

unsigned long	g_FileNotif_ThreadId;
HANDLE			g_FileNotif_threadHandle;

/* ------------------------------------------------------------------------------------------------- */

int menuGetNbElem()
{
	return (int)g_portal->flat.size();
}

/* ------------------------------------------------------------------------------------------------- */

PortalProg* menuGetElem(int pos)
{
	return g_portal->flat[pos];
}

/* ------------------------------------------------------------------------------------------------- */

int menuGetNbFiles()
{
	return (int)g_portal_files.size();
}

/* ------------------------------------------------------------------------------------------------- */

PortalProg* menuGetFile(int pos)
{
	return g_portal_files[pos];
}

/* ------------------------------------------------------------------------------------------------- */

int openConfig()
{
	// File variables
	wchar_t szConfigName[] = WC_PORTAL_XML_FILENAME;
	const int nMaxLen = 255 - SIZEOF_ARRAY(szConfigName);
	wchar_t szBuff[255] = L"";
	wchar_t* xmlfilename = NULL;
	wchar_t* szFullDllName = NULL;

	int ret = FALSE;

	int args;
	wchar_t** argList = CommandLineToArgvW(GetCommandLineW(), &args);
	if( argList == NULL )
		return ret;

	for(int i = 1; i < args; i++)
	{
		wchar_t* n = argList[i];
		if( !wcsncmp(n, L"-config", 7) )
		{
			i++;
			if(i < args && xmlfilename == NULL)
				xmlfilename = specialDirs(argList[i]);
		}
	}
	LocalFree(argList);

	if( xmlfilename == NULL )
	{
		xmlfilename = specialDirs(L"%appdata%\\GUIPro\\"WC_PORTAL_XML_FILENAME);
		if(!fileExists(xmlfilename))
		{
			free(xmlfilename);
			xmlfilename = NULL;
		}
	}

	if( xmlfilename == NULL )
	{
		xmlfilename = specialDirs(L"%portal%\\"WC_PORTAL_XML_FILENAME);
	}

	PortalConfig* l_config = loadConfig(xmlfilename);
	if( l_config != NULL )
	{
		if( g_portal )
		{
			unregisterHotkeys();

			// Store the old configuration
			PortalConfig* temp = g_portal;

			// change the config
			g_portal = l_config;

#ifdef PORTAL_REFUSE_HOTKEY_ERROR
			// Register hotkeys
			if( registerConfig(TRUE) == FALSE )
			{
				unregisterHotkeys();
				// Error: restore the old configuration.
				g_portal = temp;
				delete(l_config);

				if( registerConfig(FALSE) == FALSE )
				{
					quitPortal();
				}
			}
			else
			{
				// Ok: Delete the old configuration.
				delete(temp);
				ret = TRUE;
			}
#else
			ret = TRUE;
			if (registerConfig(TRUE) == FALSE)
				ret = PORTAL_CONFIG_LOAD_PARTIAL;

			if (g_loadingmessage != NULL) {
				free(g_loadingmessage);
				g_loadingmessage = NULL;
			}
#endif
		}
		else
		{
#ifdef PORTAL_REFUSE_HOTKEY_ERROR
			if( registerConfig(TRUE) == FALSE )
			{
				quitPortal();
			}
			else
			{
				ret = TRUE;
			}
#else
			g_portal = l_config;
			ret = TRUE;
			if (registerConfig(TRUE) == FALSE)
				ret = PORTAL_CONFIG_LOAD_PARTIAL;
#endif
		}
	}

	if( xmlfilename )
		free(xmlfilename);

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

int registerConfig(int alert)
{
	LPWSTR l_registerErrors = (LPWSTR)malloc(sizeof(PWSTR) * MAX_LANGLEN);
	memset(l_registerErrors, 0, sizeof(PWSTR) * MAX_LANGLEN);
	if( registerHotkeys(l_registerErrors) > 0)
	{
		if( alert )
		{
			size_t l_size = (wcslen(ERR_HOTKEYS_MSG) + wcslen(l_registerErrors) + 2);
			LPWSTR l_message = (LPWSTR)malloc(sizeof(PWSTR) * l_size);
			wcscpy_s(l_message, l_size, ERR_HOTKEYS_MSG);
			wcscat_s(l_message, l_size, l_registerErrors);
			
			// If we do not have any tray icon ; we store the message in a global variable
			//
			if (g_IconTray.size() == 0)
			{
				g_loadingmessage = _wcsdup(l_message);
			}
			else
			{
				ShowBalloon(ERR_MSGBOX_TITLE, l_message, 0, NIIF_ERROR);
			}
			
			free(l_message);
		}
		free(l_registerErrors);
		return FALSE;
	}
	free(l_registerErrors);
	return TRUE;
}

/* ------------------------------------------------------------------------------------------------- */

int registerHotkeys(LPWSTR p_registerErrors)
{
	int cpt = 0;
	int error = 0;
	UINT mod = 0;
	bool overriding = false;

	if( g_portal )
	{
		for( PortalProgVector::iterator i = g_portal->hotkeys.begin(); i != g_portal->hotkeys.end(); i++ )
		{
			mod = (*i)->modifier;
			overriding = (*i)->overriding;

			if( !myRegisterHotKey( (*i)->hkey, mod, IDH_HOTKEY_MENU + (cpt++), overriding ) )
			{
				error++;

				LPWSTR l_errkey = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
				ZeroMemory(l_errkey, sizeof(PWSTR) * MAX_ERRHKLEN);

				LPWSTR tmp = getInvModifier((*i)->modifier);
				if( tmp != NULL )
				{
					wcscpy_s(l_errkey, MAX_ERRHKLEN, tmp);
					free(tmp);
				}

				tmp = getInvHotKeyCode((*i)->hkey);
				if( tmp != NULL )
				{
					wcscat_s(l_errkey, MAX_ERRHKLEN, tmp);
					free(tmp);
				}

				if( (wcslen(p_registerErrors) + wcslen(l_errkey)) >= MAX_LANGLEN)
				{
					return error;	
				}

				wcscat_s(p_registerErrors, MAX_LANGLEN, l_errkey);
				wcscat_s(p_registerErrors, MAX_LANGLEN, (wchar_t*)"\n");
			}
		}
	}

	return error;
}

/* ------------------------------------------------------------------------------------------------- */

void unregisterHotkeys()
{
	int cpt = 0;

	if( g_portal )
	{
		for( PortalProgVector::iterator i = g_portal->hotkeys.begin(); i != g_portal->hotkeys.end(); i++ )
		{
			myUnregisterHotKey( IDH_HOTKEY_MENU + (cpt++));
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI threadFileNotification(LPVOID lpthis)
{
	wchar_t szBuff[255];
	wchar_t szPath[255] = L"";
	// Get file path
	int n = GetModuleFileName(g_hInst, szBuff, 255);
	if (n != 0 && n < 255) {
		while (n >= 0 && szBuff[n] != L'\\') n--;
		lstrcpyn(szPath, szBuff, n+1);
	}

	HANDLE g_hDir;
	FILE_NOTIFY_INFORMATION Buffer[8];
	DWORD BytesReturned;
	DWORD notifyFlags = FILE_NOTIFY_CHANGE_LAST_WRITE;

	g_hDir = CreateFile( szPath,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL 
	);

	while( ReadDirectoryChangesW( g_hDir, &Buffer, sizeof(Buffer), FALSE, notifyFlags, &BytesReturned, NULL, NULL) )
	{
		int i = 0;
		do
		{
			if(! wcsncmp(Buffer[i].FileName, WC_PORTAL_XML_FILENAME, wcslen(WC_PORTAL_XML_FILENAME) ) )
			{
				// Send the command via "PostMessage" because we are in a thread !
				PostMessage(g_hwndMain, WM_COMMAND, IDM_RELOAD, NULL);
			}
		} while( !Buffer[++i].NextEntryOffset );
	}

	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool installFileNotification()
{
	if( g_FileNotif_threadHandle == NULL )
	{
		g_FileNotif_threadHandle = CreateThread(NULL, NULL, threadFileNotification, NULL, NULL, &g_FileNotif_ThreadId);
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool uninstallFileNotification()
{
	if( g_FileNotif_threadHandle != NULL )
	{
		if( CloseHandle(g_FileNotif_threadHandle) )
		{
			g_FileNotif_threadHandle = NULL;
			return true;
		}
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */