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

#include "common.h"
#include "config.h"
#include "main.h"
#include "hotkey.h"
#include "plugin.h"
#include "xmlconfig.h"
#include "resource.h"

HulkConfig* g_hulk;

unsigned long	g_FileNotif_ThreadId;
HANDLE			g_FileNotif_threadHandle;

int registerHotkeys(LPWSTR p_registerErrors);
void unregisterHotkeys();
bool installFileNotification(const wchar_t* xmlfilename);
bool uninstallFileNotification();

/* ------------------------------------------------------------------------------------------------- */

wchar_t* getConfigurationFilename()
{
	wchar_t* xmlfilename = NULL;
	int args;
	wchar_t** argList = CommandLineToArgvW(GetCommandLineW(), &args);
	bool lookAppData = true;
	if (argList != NULL)
	{
		for (int i = 1; i < args; i++)
		{
			wchar_t* n = argList[i];
			if (!wcsncmp(n, L"-config", 7))
			{
				i++;
				if (i < args && xmlfilename == NULL)
					xmlfilename = specialDirs(argList[i]);
			}

			if (!wcsncmp(n, L"-local", 6))
			{
				lookAppData = false;
			}
		}
		LocalFree(argList);
	}

	if (xmlfilename == NULL && lookAppData)
	{
		xmlfilename = specialDirs(L"%appdata%\\GUIPro\\" WC_HULK_XML_FILENAME);
		if (!fileExists(xmlfilename))
		{
			free(xmlfilename);
			xmlfilename = NULL;
		}
	}

	if (xmlfilename == NULL)
	{
		xmlfilename = specialDirs(L"%hulk%\\" WC_HULK_XML_FILENAME);
	}

	return xmlfilename;
}

/* ------------------------------------------------------------------------------------------------- */


int openConfig()
{
	wchar_t* xmlfilename = NULL;

	int ret = FALSE;

	xmlfilename = getConfigurationFilename();
	HulkConfig* l_config = loadConfig(xmlfilename);
	if (l_config != NULL)
	{
		bool first = true;

		if (g_hulk)
		{
			first = false;

			uninstallFileNotification();
			unregisterHotkeys();
			/*
			// Store the old configuration
			HulkConfig* temp = g_hulk;
			*/
			// Clean the memory
			delete(g_hulk);
		}
		
		// change the config
		g_hulk = l_config;

		ret = TRUE;
		if (registerConfig(TRUE) == FALSE)
			ret = HULK_CONFIG_LOAD_PARTIAL;

		if (!first && g_loadingmessage != NULL) {
			free(g_loadingmessage);
			g_loadingmessage = NULL;
		}
	}

	if (xmlfilename)
	{
//		installFileNotification(xmlfilename);
		free(xmlfilename);
	}

	return ret;
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
			
			// If we do not have any tray icon ; we store the message in a global variable
			//
			if (!g_hIconTray)
			{
				g_loadingmessage = _wcsdup(l_message);
			}
			else
			{
				ShowBalloon(ERR_MSGBOX_TITLE, l_message, NIIF_ERROR);
			}
			// MessageBox(g_hwndMain, l_message, ERR_MSGBOX_TITLE, NULL);
			
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
	int error = 0;
	int i = 0;

	if (g_hulk == NULL)
		return error;

	if (g_hulk->hotkeys.size() == 0)
		return error;

	wchar_t* l_wc;

	for (hkConfigVector::iterator hk = g_hulk->hotkeys.begin(); hk != g_hulk->hotkeys.end(); ++hk, ++i)
	{
		if (RegisterHK((*hk)->key, (*hk)->mod, i))
		{
			continue;
		}

		error++;

		LPWSTR l_errkey = (LPWSTR)malloc(sizeof(PWSTR) * MAX_ERRHKLEN);
		l_wc = getInverseModifier((*hk)->mod);
		wcscpy_s(l_errkey, MAX_ERRHKLEN, l_wc);
		free(l_wc);
		l_wc = getInverseHotKeyCode((*hk)->key);
		wcscat_s(l_errkey, MAX_ERRHKLEN, l_wc);
		free(l_wc);

		if ((wcslen(p_registerErrors) + wcslen(l_errkey)) >= MAX_LENGTH)
		{
			free(l_errkey);
			return error;
		}

		wcscat_s(p_registerErrors, MAX_LENGTH, l_errkey);
		wcscat_s(p_registerErrors, MAX_LENGTH, (wchar_t*)"\n");

		free(l_errkey);
	}

	return error;
}

/* ------------------------------------------------------------------------------------------------- */

void unregisterHotkeys()
{
	if (!g_hulk)
		return;

	int cpt = 0;
	for (hkConfigVector::iterator hk = g_hulk->hotkeys.begin(); hk != g_hulk->hotkeys.end(); hk++)
	{
		UnregisterHK(cpt++);
	}
}

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI threadFileNotification(LPVOID lpthis)
{
#define THREAD_FILENAME_SIZE 512
	wchar_t* szBuff = NULL;
	wchar_t* szPath = NULL;

	int n = 0;
	if (lpthis != NULL) {
		szBuff = (wchar_t*)lpthis;
		n = (int)wcslen(szBuff);
	}

	// Get file path
	if (n == 0) {
		szBuff = (wchar_t*)malloc(sizeof(wchar_t) * THREAD_FILENAME_SIZE);
		n = GetModuleFileName(g_hInst, szBuff, THREAD_FILENAME_SIZE);
	}

	// Extract folder
	if (n != 0 && n < THREAD_FILENAME_SIZE) {
		while (n >= 0 && szBuff[n] != L'\\') n--;
		szPath = (wchar_t*)malloc(sizeof(wchar_t) * (n + 1));
		memset(szPath, 0, sizeof(wchar_t) * (n + 1));
		lstrcpyn(szPath, szBuff, n + 1);
	}
#undef THREAD_FILENAME_SIZE

	if (szBuff != NULL)
		free(szBuff);

	if (n == 0)
		return false;
	HANDLE g_hDir;
	FILE_NOTIFY_INFORMATION Buffer[8];
	memset(Buffer, 0, sizeof(Buffer));
	DWORD BytesReturned = 0;
	DWORD notifyFlags = FILE_NOTIFY_CHANGE_LAST_WRITE;

	g_hDir = CreateFile(szPath,
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (szPath != NULL)
		free(szPath);

	boolean modified = false;

	while (!modified && ReadDirectoryChangesW(g_hDir, &Buffer, sizeof(Buffer), FALSE, notifyFlags, &BytesReturned, NULL, NULL))
	{
		if (BytesReturned > 0)
		{
			int i = 0;
			do
			{
				if (!modified && !wcsncmp(Buffer[i].FileName, WC_HULK_XML_FILENAME, wcslen(WC_HULK_XML_FILENAME)))
				{
					modified = true;
					// Send the command via "PostMessage" because we are in a thread !
					PostMessage(g_hwndMain, WM_COMMAND, IDM_RELOAD, NULL);
				}
			} while (!modified && !Buffer[++i].NextEntryOffset);
		}

		if (!modified)
		{
			BytesReturned = 0;
			Sleep(500);
		}
	}

	CloseHandle(g_hDir);
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool installFileNotification(const wchar_t* xmlfilename)
{
	if (g_FileNotif_threadHandle != NULL)
	{
		DWORD dwWait = WaitForSingleObject(g_FileNotif_threadHandle, 0);
		if (dwWait == WAIT_TIMEOUT)
			g_FileNotif_threadHandle = NULL;
	}

	if (g_FileNotif_threadHandle == NULL)
	{
		wchar_t* filename = NULL;
		if (xmlfilename != NULL)
			filename = _wcsdup(xmlfilename);
		g_FileNotif_threadHandle = CreateThread(NULL, NULL, threadFileNotification, (LPVOID)filename, NULL, &g_FileNotif_ThreadId);
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool uninstallFileNotification()
{
	if (g_FileNotif_threadHandle != NULL && CloseHandle(g_FileNotif_threadHandle))
	{
		g_FileNotif_threadHandle = NULL;
		return true;
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

void clearConfig()
{
	memset(g_hulk->mouses, 0, sizeof(g_hulk->mouses));

	unregisterHotkeys();
	for (hkConfigVector::iterator hk = g_hulk->hotkeys.begin(); hk != g_hulk->hotkeys.end(); hk++)
	{
		free((*hk));
	}
	g_hulk->hotkeys.clear();

	free(g_hulk);
	g_hulk = NULL;
}

/* ------------------------------------------------------------------------------------------------- */
