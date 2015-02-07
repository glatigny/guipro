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

#include "service.h"
#include "common.h"

SERVICE_STATUS			ServiceStatus;
SERVICE_STATUS_HANDLE	hServiceStatus;
bool					ServiceStarted = false;

/* ------------------------------------------------------------------------------------------------- */

int InstallService()
{   
    WCHAR strDir[MAX_SIZE];
    SC_HANDLE hSCManager,hService;
    SERVICE_DESCRIPTION SD;

	GetModuleFileName(NULL, strDir, MAX_SIZE);

    hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    hService = CreateService(hSCManager, 
							MY_SERVICE_NAME, 
							MY_SERVICE_DISPLAYNAME,
							SERVICE_ALL_ACCESS, 
							SERVICE_WIN32_OWN_PROCESS, 
							SERVICE_AUTO_START,
							SERVICE_ERROR_NORMAL, 
							strDir, 
							NULL, NULL, NULL, NULL, NULL);

    SD.lpDescription = MY_SERVICE_DESCRIPTION;

    BOOL ret = ChangeServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, &SD);
    CloseServiceHandle(hService);

    return ret;
}

/* ------------------------------------------------------------------------------------------------- */

int UninstallService()
{
    SC_HANDLE hSCManager;
    SC_HANDLE hServ;
    
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    hServ = OpenService(hSCManager, MY_SERVICE_NAME, SERVICE_ALL_ACCESS);
    
	BOOL ret = DeleteService(hServ);
    
	CloseServiceHandle(hServ);
    CloseServiceHandle(hSCManager);
    
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

void GuiProStopService()
{
    SC_HANDLE SCMan;
    SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) ;

    SC_HANDLE schService;
    schService = OpenService(SCMan, MY_SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS );

	SERVICE_STATUS_PROCESS ssp;
	ControlService( schService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS) &ssp );

    CloseServiceHandle(schService);
    CloseServiceHandle(SCMan);
}

/* ------------------------------------------------------------------------------------------------- */

bool GuiProInServiceInstalled()
{
	bool ret = true;

    SC_HANDLE SCMan;
    SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) ;

    SC_HANDLE schService;
    schService = OpenService(SCMan, MY_SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS );

	if( schService == NULL )
	{
		ret = false;
	}

    CloseServiceHandle(schService);
    CloseServiceHandle(SCMan);

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

bool GuiProInServiceStarted()
{
	bool ret = false;

    SC_HANDLE SCMan;
    SCMan = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS) ;

    SC_HANDLE schService;
    schService = OpenService(SCMan, MY_SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS );

	if( schService == NULL )
	{
		CloseServiceHandle(SCMan);
		return false;
	}

	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	if ( !QueryServiceStatusEx( schService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded ) )
	{
		ret = false;
	}
	else
	{
		DWORD state = ssp.dwCurrentState;
		if( state == SERVICE_RUNNING /*|| state == SERVICE_START_PENDING || state == SERVICE_CONTINUE_PENDING*/ )
		{
			ret = true;
		}

		if( state == SERVICE_PAUSE_PENDING || state == SERVICE_PAUSED )
		{
			ret = true;
		}

		if( state == SERVICE_STOPPED || state == SERVICE_STOP_PENDING )
		{
			ret = false;
		}
	}

    CloseServiceHandle(schService);
    CloseServiceHandle(SCMan);

	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

void GuiProStartService()
{
    SC_HANDLE SCMan;
    SCMan = OpenSCManager(NULL, NULL, GENERIC_EXECUTE) ;

    SC_HANDLE MonServ;
	MonServ = OpenService(SCMan, MY_SERVICE_NAME, SC_MANAGER_ALL_ACCESS );

    StartService(MonServ, 0, NULL);

    CloseServiceHandle(MonServ);
    CloseServiceHandle(SCMan);
}

/* ------------------------------------------------------------------------------------------------- */

void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{ 
    ZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));
    ServiceStatus.dwServiceType = SERVICE_WIN32;
    ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    hServiceStatus = RegisterServiceCtrlHandler(MY_SERVICE_NAME, ServCtrlHandler);

    ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    SetServiceStatus(hServiceStatus, &ServiceStatus);

	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);

    ServiceStarted = true;
	DoService();
	while(ServiceStarted)
	{
		Sleep(60000);
	}
	endAllChildren();
}

/* ------------------------------------------------------------------------------------------------- */

void WINAPI ServCtrlHandler(DWORD SCCode)
{
	switch(SCCode)
	{
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			SetServiceStatus (hServiceStatus,&ServiceStatus);
		return;

		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			SetServiceStatus (hServiceStatus,&ServiceStatus);
		return;

		case SERVICE_CONTROL_STOP:
			endAllChildren();
			ServiceStatus.dwWin32ExitCode = 0;
			ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			ServiceStatus.dwCheckPoint = 0;
			ServiceStatus.dwWaitHint = 0;
			SetServiceStatus (hServiceStatus,&ServiceStatus);
			ServiceStarted = false;
		return;
	}
}