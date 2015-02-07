/*  Sersi - GUIPro Project ( http://guipro.sourceforge.net/ )

    Author : DarkSage  aka  Glatigny Jérôme <darksage@darksage.info>

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

#include "process.h"
#include "main.h"
#include "gui.h"
#include "registry.h"
#include "common.h"
#include "watch.h"
#include "sersiConf.h"

extern std::list<struct SersiProg*> plist;
extern bool check_config;

/* ------------------------------------------------------------------------------------------------- */

void process(const wchar_t* name)
{
	HKEY hk;
	if( regOpenKey(&hk, name, true, false) )
	{
		struct SersiConf* conf = loadConf(name);

		if( conf != NULL )
		{
			const wchar_t* s_szConfig = conf->exe;
			wchar_t szBuff[512] = L"";
			const int nMaxLen = REG_KEY_LEN - 1 - SIZEOF_ARRAY(s_szConfig);
			int n;

			bool relative = (s_szConfig[1] != ':');

			if( relative )
			{
				n = GetModuleFileName(NULL, szBuff, nMaxLen);
				if (n != 0 && n < nMaxLen) {
					while (n >= 0 && szBuff[n] != L'\\') n--;
					lstrcpyn(szBuff + n + 1, s_szConfig, SIZEOF_ARRAY(szBuff) - n);
				}
			} else
				lstrcpy(szBuff, s_szConfig);

			struct SersiProg *p;
			p = execute(name, szBuff, conf->params, conf->dir, conf->autoRestart, conf->dailyRestart);

			if( conf->dailyRestart )
				installWatch(NULL);
			if( conf->autoRestart )
				installWatch(p);

			unloadConf(conf);
		}
	}
	RegCloseKey(hk);
}

/* ------------------------------------------------------------------------------------------------- */

struct SersiProg* execute(const wchar_t* name, const wchar_t* progname, const wchar_t* progparam, const wchar_t* dirparam, bool autoRestart, bool dailyRestart)
{
	wchar_t* l_progparam = (wchar_t*)progparam;
	wchar_t* l_dirparam = (wchar_t*)dirparam;

	if( l_progparam == NULL || l_progparam[0] == L'\0' )
		l_progparam = NULL;

	if( l_dirparam == NULL || l_dirparam[0] == L'\0' )
		l_dirparam = NULL;

	if( !check_config )
	{
		SHELLEXECUTEINFOW* shi;
		shi = (SHELLEXECUTEINFOW*)malloc(sizeof(SHELLEXECUTEINFOW));

		ZeroMemory(shi, sizeof(SHELLEXECUTEINFOW));
		
		shi->cbSize = sizeof(SHELLEXECUTEINFOW);
		shi->fMask = SEE_MASK_NOCLOSEPROCESS;
		shi->hwnd = 0;

		shi->lpVerb = L"open";
		shi->lpFile = progname;
		shi->lpParameters = l_progparam;
		shi->lpDirectory = l_dirparam;
		shi->nShow = 0;
		
		ShellExecuteEx( shi );

		SersiProg* p = (SersiProg*)malloc(sizeof(SersiProg));
		p->name = _wcsdup(name);
		p->process = shi;
		p->autoRestart = autoRestart;
		p->dailyRestart = dailyRestart;

		plist.insert( plist.end(), p );

		return p;
	}
	else
	{
#ifdef WINDOWMODE
		insertItem(name, progname, l_dirparam, l_progparam, autoRestart, dailyRestart);
#else
		wprintf(L"Application %s\nOpen: %s\nParam: %s\nDir: %s\nAuto Restart: %s\nDailyRestart: %s\n\n", 
				name, progname, progparam, dirparam, autoRestart?L"yes":L"no", dailyRestart?L"yes":L"no");
#endif
		return NULL;
	}
}

/* ------------------------------------------------------------------------------------------------- */

void endAllChildren()
{
	for( std::list<SersiProg*>::iterator i = plist.begin() ; i != plist.end() ; i++ )
	{
		DWORD dwExitCode = 0;
		GetExitCodeProcess( (*i)->process->hProcess, &dwExitCode);
		if(dwExitCode == STILL_ACTIVE)
		{
			TerminateProcess( (*i)->process->hProcess, 0);
		}
		free( (*i)->process );
		free( (*i)->name );
		free( (*i) );
	}
	plist.clear();
}

/* ------------------------------------------------------------------------------------------------- */
