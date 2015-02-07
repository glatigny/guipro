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

#include "watch.h"
#include "main.h"
#include "process.h"
#include <windows.h>

unsigned long	g_dailywatch_ThreadId;
HANDLE			g_dailywatch_threadHandle;
bool			g_dailyInProgress = false;

extern std::list<struct SersiProg*> plist;

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI threadDailyWatch(LPVOID lpthis)
{
	SYSTEMTIME st;
	SYSTEMTIME ref;
	GetLocalTime(&ref);

	for(;;)
	{
		Sleep(1000);
		GetLocalTime(&st);
		if( st.wDay != ref.wDay )
		{
			g_dailyInProgress = true;
			GetLocalTime(&ref);
			std::list<struct SersiProg*> restartList;
			restartList.clear();

			for( std::list<struct SersiProg*>::iterator i = plist.begin() ; i != plist.end() ; i++ )
			{
				if( (*i)->dailyRestart == true )
				{
					restartList.insert(restartList.end(), (*i) );
					DWORD dwExitCode = 0;
					GetExitCodeProcess( (*i)->process->hProcess, &dwExitCode);
					if(dwExitCode == STILL_ACTIVE)
					{
						TerminateProcess( (*i)->process->hProcess, 0);
						while(dwExitCode == STILL_ACTIVE)
						{
							GetExitCodeProcess( (*i)->process->hProcess, &dwExitCode);
							Sleep(500);
						}
					}
				}
			}

			for( std::list<struct SersiProg*>::iterator i = restartList.begin() ; i != restartList.end() ; i++ )
			{
				wchar_t* name = _wcsdup( (*i)->name );
				plist.remove( (*i) );
				process( name );
				free( (*i)->name );
				free( (*i)->process );
				free( (*i) );
				free( name );
			}
			restartList.clear();

			g_dailyInProgress = false;
		}
	}
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

DWORD WINAPI threadWatchEnd(LPVOID lpthis)
{
	struct SersiProg* p = (struct SersiProg*)lpthis;

	WaitForSingleObject(p->process->hProcess, INFINITE);

	if( !g_dailyInProgress )
	{
		wchar_t* name = _wcsdup( p->name );
		plist.remove( p );

		process( name );

		free( name );
		free( p->name );
		free( p->process );
	}

	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

bool installWatch(struct SersiProg* p)
{
	if( p == NULL )
	{
		if( g_dailywatch_threadHandle == NULL )
		{
			g_dailywatch_threadHandle = CreateThread(NULL, NULL, threadDailyWatch, NULL, NULL, &g_dailywatch_ThreadId);
			return true;
		}
	}
	else
	{
		CreateThread(NULL, NULL, threadWatchEnd, p, NULL, NULL);
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */

bool uninstallWatch(struct SersiProg* p)
{
	if( p == NULL )
	{
		if( g_dailywatch_threadHandle != NULL )
		{
			if( CloseHandle(g_dailywatch_threadHandle) )
			{
				g_dailywatch_threadHandle = NULL;
				return true;
			}
		}
	}
	else
	{
		// Nothing to do...
	}
	return false;
}

/* ------------------------------------------------------------------------------------------------- */