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

#ifndef COMMON_H
#define COMMON_H

/* ------------------------------------------------------------------------------------------------- */

#define REG_KEY_LEN		256
#define MAX_SIZE		1024
#define BUFFER_SIZE		2048
#define SIZEOF_ARRAY(p)				(sizeof(p) /sizeof(p[0]))

/* ------------------------------------------------------------------------------------------------- */

#define STR_AUTO_RESTART		L"Auto Restart"
#define STR_DAILY_RESTART		L"Daily Restart"

/* ------------------------------------------------------------------------------------------------- */
#ifdef WINDOWMODE

#define ITEM_ROOT					0x00
#define ITEM_EXE					0x01
#define ITEM_DIR					0x02
#define ITEM_PARAM					0x03
#define ITEM_AUTORESTART			0x04
#define ITEM_DAILYRESTART			0x05

#define GPS_MENU_NEWELEMENT			L"New element"
#define GPS_MENU_REMELEMENT			L"Delete element"
#define GPS_MENU_SETPROG			L"Edit program"
#define GPS_MENU_SETDIR				L"Edit directory"
#define GPS_MENU_REMDIR				L"Remove directory"
#define GPS_MENU_SETPARAM			L"Edit parameter"
#define GPS_MENU_REMPARAM			L"Remove parameter"
#define GPS_MENU_SETAUTORESTART		L"Active Auto-restart"
#define GPS_MENU_REMAUTORESTART		L"Desactive Auto-restart"
#define GPS_MENU_SETDAILYRESTART	L"Active Daily-restart"
#define GPS_MENU_REMDAILYRESTART	L"Desactive Daily-restart"

#endif /* WINDOWMODE */

#define GPS_HEADER					L"\nGUIPro Services\nStart classical applications when computer start\n\n"
#define GPS_INSTALL_SUCCESS			L"Service installed successfully"
#define GPS_INSTALL_FAILURE			L"Service installed fail"
#define GPS_UNINSTALL_SUCCESS		L"Service uninstalled successfully"
#define GPS_UNINSTALL_FAILURE		L"Service uninstalled fail"

#define GPS_KEY_EXE					L"exe"
#define GPS_KEY_DIR					L"dir"
#define GPS_KEY_PARAM				L"param"
#define GPS_KEY_AUTORESTART			L"auto restart"
#define GPS_KEY_DAILYRESTART		L"daily restart"

#define MY_SERVICE_NAME				L"GuiPro Sersi"
#define MY_SERVICE_DISPLAYNAME		L"GuiPro Sersi"
#define MY_SERVICE_DESCRIPTION		L"Service application launcher. http://guipro.sourceforge.net"

#define szKEY	L"Software\\GUIPro Sersi"

/* ------------------------------------------------------------------------------------------------- */

void FlushMemory();

/* ------------------------------------------------------------------------------------------------- */

#endif