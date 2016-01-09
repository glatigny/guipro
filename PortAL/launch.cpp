/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

	Author : Glatigny Jérôme <jerome@darksage.fr>

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
	wchar_t szParam[PARAMS_BUFFER_LNG] = L" ";
	wchar_t szDirName[512] = L"";
	const int nMaxLen = 255 - SIZEOF_ARRAY(s_szConfig);
	size_t n;

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

	bool relative = (s_szConfig[1] != ':') && wcsncmp(s_szConfig, L"app#", 4);

	if( relative )
	{
		n = GetModuleFileName(g_hInst, szBuff, nMaxLen);
		if (n > 0 && n < nMaxLen) {
			while (n >= 0 && szBuff[n] != L'\\')
				n--;
			lstrcpyn(szBuff + n + 1, s_szConfig, SIZEOF_ARRAY(szBuff) - (int)n);
		}
	}
	else
		wcscpy_s(szBuff, s_szConfig);
	
	if( progparams != NULL )
	{
		wcscat_s(szParam, progparams);

		// Handle special %clipboard% content in the parameters
		//
		wchar_t* cur = wcschr(szParam, L'%');
		if (cur != NULL && (!wcsncmp(cur, L"%clipboard%", 11) || !wcsncmp(cur, L"%clipboards%", 12)))
		{
			bool multiple = !wcsncmp(cur, L"%clipboards%", 12);
			// Get clipboard
			wchar_t* clipboardData = getClipboard(multiple);

			// Secure the clipboard length
			size_t pos = (size_t)(cur - szParam);
			size_t clipboardLength = 0;
			if (clipboardData != NULL)
				clipboardLength = wcslen(clipboardData);
			if (clipboardLength > 0 && (clipboardLength > PARAMS_BUFFER_LNG || (wcslen(szParam) - (multiple ? 12 : 11) + clipboardLength) > PARAMS_BUFFER_LNG))
			{
				free(clipboardData);
				clipboardData = NULL;
			}

			// Replace the %clipboard% by the clipboard (or just remove the content from the parameter string)
			wchar_t* tmp = _wcsdup(cur + (multiple ? 12 : 11));
			if (clipboardData != NULL)
			{
				wcscpy_s(cur, PARAMS_BUFFER_LNG - pos, clipboardData);
				size_t l = wcslen(clipboardData);
				pos += l;
				cur += l;
			}
			wcscpy_s(cur, PARAMS_BUFFER_LNG - pos, tmp);
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
		wcscpy_s(szDirName, dirname);
	}
	else
	{
		n = wcslen(szBuff);
		while( n >= 0 && (szBuff[n] != L'\\'))
			n--;
		wcsncpy_s(szDirName, szBuff, n + 1);
	}

	if( (g_portal->shellExecuteDefault && !(prog->options & PROG_OPTION_NOSHELL) ) || prog->options & PROG_OPTION_SHELL )
	{
		if (prog->options & PROG_OPTION_ELEVATION)
			ShellExecute(g_hwndMain, TEXT("runas"), szBuff, szParam, szDirName, SW_SHOW);
		else
			ShellExecute(g_hwndMain, TEXT("open"), szBuff, szParam, szDirName, SW_SHOW);
	}
	else if (!wcsncmp(szBuff, L"app#", 4))
	{
		launchRTApp(&szBuff[4]);
	}
	else
	{
		STARTUPINFO si = { sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi;

		// Elevation process
		//
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

			// If elevation failed, we use the shell execute
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

HRESULT launchRTApp(wchar_t* app)
{
#ifndef _USING_V110_SDK71_ // WIN8_SUPPORT
	if (windowsVersion < WINVER_8)
		return E_FAIL;

	HRESULT hr = E_FAIL;
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	if (!SUCCEEDED(hr))
		return hr;

	wchar_t* app_name = app;
#define IS_APP(a, b) if(!_wcsicmp(app, L ## a)) app_name = L ## b

	IS_APP("edge", "Microsoft.MicrosoftEdge_8wekyb3d8bbwe!MicrosoftEdge");
	else IS_APP("reader", "Microsoft.Reader_8wekyb3d8bbwe!Microsoft.Reader");
	else IS_APP("windows.photos", "Microsoft.Windows.Photos_8wekyb3d8bbwe!App");
	else IS_APP("windows.alarms", "Microsoft.WindowsAlarms_8wekyb3d8bbwe!App");
	else IS_APP("windows.calculator", "Microsoft.WindowsCalculator_8wekyb3d8bbwe!App");
	else IS_APP("windows.maps", "Microsoft.WindowsMaps_8wekyb3d8bbwe!App");
	else IS_APP("windows.scan", "Microsoft.WindowsScan_8wekyb3d8bbwe!App");
	else IS_APP("windows.store", "Microsoft.WindowsStore_8wekyb3d8bbwe!App");
	else IS_APP("zune.music", "Microsoft.ZuneMusic_8wekyb3d8bbwe!Microsoft.ZuneMusic");
	else IS_APP("zune.video", "Microsoft.ZuneVideo_8wekyb3d8bbwe!Microsoft.ZuneVideo");

#undef IS_APP

	CComPtr<IApplicationActivationManager> activationManager;
	//	hr = CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&activationManager));
	hr = CoCreateInstance(CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_IApplicationActivationManager, (LPVOID*)&activationManager);

	if (SUCCEEDED(hr))
	{
		hr = CoAllowSetForegroundWindow(activationManager, NULL);
	}

	if (SUCCEEDED(hr))
	{
		DWORD newProcessId;
		hr = activationManager->ActivateApplication(app_name, NULL, AO_NONE, &newProcessId);
	}

	CoUninitialize();
	FlushMemory();

	return hr;
#else /* _USING_V110_SDK71_ / WIN8_SUPPORT */
	return 0;
#endif /* _USING_V110_SDK71_ / WIN8_SUPPORT */
}

/* ------------------------------------------------------------------------------------------------- */

void fireEvent(UINT evt)
{
	for(PortalProgVector::iterator i = g_portal->flat.begin(); i != g_portal->flat.end(); i++ )
	{
		if ((*i)->events & evt)
		{
			(*i)->run(true);
		}
	}
}

/* ------------------------------------------------------------------------------------------------- */
