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

#include "launch.h"
#include "main.h"
#include "common.h"
#include "config.h"

/* ------------------------------------------------------------------------------------------------- */

void launch(PortalProg* prog)
{
	wchar_t* progname = prog->progExe;
	wchar_t* progparams = prog->progParam;
	wchar_t* dirname = prog->dirPath;

	const wchar_t* s_szConfig = progname;
	wchar_t szBuff[512] = L"";
	wchar_t szParam[2048] = L"";
	wchar_t szDirName[512] = L"";
	const int nMaxLen = 255 - SIZEOF_ARRAY(s_szConfig);
	int n;

	if( progname[0] == '|' )
	{
		if( !wcscmp(progname, L"|quit" ) )
		{
			quitPortal();
		}
		else if( !wcscmp(progname, L"|about" ) )
		{
			ShowAbout(g_currentMenu);
		}
		return;
	}

	bool relative = (s_szConfig[1] != ':');

	if( relative )
	{
		n = GetModuleFileName(g_hInst, szBuff, nMaxLen);
		if (n != 0 && n < nMaxLen) {
			while (n >= 0 && szBuff[n] != L'\\') n--;
			lstrcpyn(szBuff + n + 1, s_szConfig, SIZEOF_ARRAY(szBuff) - n);
		}
	} else
		lstrcpy(szBuff, s_szConfig);
	
	if( progparams != NULL )
	{
		if( progparams[0] != ' ')
		{
			wchar_t szParamBuff[512] = L" ";
			lstrcpy(szParamBuff + 1, progparams);
			lstrcpy(szParam, szParamBuff);
		}
		else
		{
			lstrcpy(szParam, progparams);
		}

		// Handle special %clipboard% content in the parameters
		//
		wchar_t* pos = wcschr(szParam, L'%');
		if (pos != NULL && !wcsncmp(pos, L"%clipboard%", 11))
		{
			// Get clipboard
			wchar_t* clipboardData = getClipboard();

			// Secure the clipboard length
			int clipboardLength = 0;
			if (clipboardData != NULL)
				clipboardLength = wcslen(clipboardData);
			if (clipboardLength > 0 && (clipboardLength > 2048 || (wcslen(szParam) + clipboardLength) > 2048))
			{
				free(clipboardData);
				clipboardData = NULL;
			}

			// Replace
			wchar_t* tmp = _wcsdup(pos + 12);
			if (clipboardData != NULL)
			{
				lstrcpy(pos, clipboardData);
				pos += wcslen(clipboardData);
			}
			lstrcpy(pos, tmp);
			free(tmp);

			if (clipboardData != NULL)
			{
				free(clipboardData);
				clipboardData = NULL;
			}
		}
	}

	if( dirname != NULL && !(prog->options & PROG_OPTION_BROWSE) )
	{
		lstrcpy(szDirName, dirname);
	}
	else
	{
		n = lstrlen(szBuff);
		while( n >= 0 && (szBuff[n] != L'\\')) n--;
		lstrcpyn(szDirName, szBuff, n+1);
	}

	if( (g_portal->shellExecuteDefault && !(prog->options & PROG_OPTION_NOSHELL) ) || prog->options & PROG_OPTION_SHELL )
	{
		if (prog->options & PROG_OPTION_ELEVATION)
			ShellExecute(g_hwndMain, TEXT("runas"), szBuff, szParam, szDirName, SW_SHOW);
		else
			ShellExecute(g_hwndMain, TEXT("open"), szBuff, szParam, szDirName, SW_SHOW);
	}
	else
	{
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;

		if (prog->options & PROG_OPTION_ELEVATION)
		{
			BOOL runAsAdmin = false;
			PSID adminGroup = NULL;
			SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
			if (AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
			{
				CheckTokenMembership(NULL, adminGroup, &runAsAdmin);
			}
			if (adminGroup) {
				FreeSid(adminGroup);
				adminGroup = NULL;
			}

			if (!runAsAdmin)
			{
				SHELLEXECUTEINFO shexi;
				memset(&shexi, 0, sizeof(shexi));
				shexi.cbSize = sizeof(SHELLEXECUTEINFO);
				shexi.fMask = 0;
				shexi.hwnd = g_hwndMain;
				shexi.lpVerb = TEXT("runas");
				shexi.lpFile = szBuff;
				shexi.lpParameters = szParam;
				shexi.lpDirectory = szDirName;
				shexi.nShow = SW_NORMAL;

				ShellExecuteEx(&shexi);
				return;
			}
		}

		if (CreateProcess(szBuff, szParam, NULL, NULL, FALSE, 0, NULL, szDirName, &si, &pi))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */

void Autorun()
{
	for(PortalProgVector::iterator i = g_portal->flat.begin(); i != g_portal->flat.end(); i++ )
	{
		if( (*i)->options & PROG_OPTION_AUTORUN )
		{
			(*i)->run(true);
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */
