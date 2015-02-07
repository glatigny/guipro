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

#include "sersiConf.h"
#include "common.h"
#include "registry.h"

struct SersiConf* loadConf(const wchar_t* name)
{
	struct SersiConf* conf = NULL;

	HKEY hk;
	if( regOpenKey(&hk, name, true, false) )
	{
		wchar_t progname[REG_KEY_LEN];
		wchar_t progparam[REG_KEY_LEN];
		wchar_t dirparam[REG_KEY_LEN];
		DWORD autoRestart = 0;
		DWORD dailyRestart = 0;

		ZeroMemory(progname, sizeof(progname));
		ZeroMemory(progparam, sizeof(progparam));
		ZeroMemory(dirparam, sizeof(dirparam));

		if( GetStrOptionValue(GPS_KEY_EXE, NULL, &hk, progname, REG_KEY_LEN) != NULL )
		{
			GetStrOptionValue(GPS_KEY_PARAM, NULL, &hk, progparam, REG_KEY_LEN);
			GetStrOptionValue(GPS_KEY_DIR, NULL, &hk, dirparam, REG_KEY_LEN);

			GetDwOptionValue(GPS_KEY_AUTORESTART, NULL, &hk, &autoRestart);
			GetDwOptionValue(GPS_KEY_DAILYRESTART, NULL, &hk, &dailyRestart);

			conf = (struct SersiConf*)malloc(sizeof(struct SersiConf));
			ZeroMemory(conf, sizeof(struct SersiConf));
			conf->name = _wcsdup(name);
			conf->exe = _wcsdup(progname);
			if( dirparam[0] != '\0' )
				conf->dir = _wcsdup(dirparam);
			if( progparam[0] != '\0' )
				conf->params = _wcsdup(progparam);

			if( autoRestart == 1 )
				conf->autoRestart = true;
			else
				conf->autoRestart = false;

			if( dailyRestart == 1 )
				conf->dailyRestart = true;
			else
				conf->dailyRestart = false;
		}
	}
	RegCloseKey(hk);
	return conf;
}

void unloadConf(struct SersiConf* conf)
{
	free(conf->name);
	free(conf->exe);
	free(conf->dir);
	free(conf->params);
	free(conf);
}