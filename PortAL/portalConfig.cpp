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

#include "portalConfig.h"

#ifdef USE_PATH_STORAGE
#include "pathStorage.h"
static pathStorage storage;
#endif

PortalProg::PortalProg() :
	progName(NULL), icoPath(NULL), progExe(NULL), progParam(NULL), dirPath(NULL),
	hkey(0), modifier(0), uID(0), options(0), overriding(false), events(PROG_EVENT_NONE),
	nextSameHotkey(NULL)
{
	progs.clear();
}

PortalProg::~PortalProg()
{
	for (std::vector<PortalProg*>::iterator i = progs.begin(); i != progs.end(); i++)
	{
		delete (*i);
	}
	progs.clear();

	free(progName);
#ifdef USE_PATH_STORAGE

#else
	free(icoPath);
	free(progExe);
	free(progParam);
	free(dirPath);
#endif

	nextSameHotkey = NULL;
}

/* ------------------------------------------------------------------------------------------------- */

void PortalProg::setIcoPath(wchar_t* path)
{
#ifdef USE_PATH_STORAGE
	icoPath = 0;
	if (path == NULL)
		return;
	if(path[0] != L'\0')
		icoPath = storage.add(path);
	free(path);
#else
	icoPath = path;
#endif
}

wchar_t* PortalProg::getIcoPath()
{
#ifdef USE_PATH_STORAGE
	if (icoPath == 0)
		return NULL;
	return storage.get(icoPath);
#else
	return icoPath;
#endif
}

bool PortalProg::isIcoPath()
{
#ifdef USE_PATH_STORAGE
	return (icoPath != 0);
#else
	return (icoPath != NULL) && (icoPath[0] != L'\0');
#endif
}

/* ------------------------------------------------------------------------------------------------- */

void PortalProg::setProgExe(wchar_t* path)
{
#ifdef USE_PATH_STORAGE
	progExe = 0;
	if (path == NULL)
		return;
	if (path[0] != L'\0')
		progExe = storage.add(path);
	free(path);
#else
	progExe = path;
#endif
}

wchar_t* PortalProg::getProgExe()
{
#ifdef USE_PATH_STORAGE
	return storage.get(progExe);
#else
	return progExe;
#endif
}

bool PortalProg::isProgExe()
{
#ifdef USE_PATH_STORAGE
	return (progExe != 0);
#else
	return (progExe != NULL);
#endif
}

/* ------------------------------------------------------------------------------------------------- */

void PortalProg::setProgParam(wchar_t* path)
{
#ifdef USE_PATH_STORAGE
	progParam = 0;
	if (path == NULL)
		return;
	if (path[0] != L'\0')
		progParam = storage.add(path);
	free(path);
#else
	progParam = path;
#endif
}

wchar_t* PortalProg::getProgParam()
{
#ifdef USE_PATH_STORAGE
	return storage.get(progParam);
#else
	return progParam;
#endif
}

bool PortalProg::isProgParam()
{
#ifdef USE_PATH_STORAGE
	return (progParam != 0);
#else
	return (progParam != NULL);
#endif
}

/* ------------------------------------------------------------------------------------------------- */

void PortalProg::setDirPath(wchar_t* path)
{
#ifdef USE_PATH_STORAGE
	dirPath = 0;
	if (path == NULL)
		return;
	if (path[0] != L'\0')
		dirPath = storage.add(path);
	free(path);
#else
	dirPath = path;
#endif
}

wchar_t* PortalProg::getDirPath()
{
#ifdef USE_PATH_STORAGE
	return storage.get(dirPath);
#else
	return dirPath;
#endif
}

bool PortalProg::isDirPath()
{
#ifdef USE_PATH_STORAGE
	return (dirPath != 0);
#else
	return (dirPath != NULL);
#endif
}

/* ------------------------------------------------------------------------------------------------- */

void PortalProg::run(bool single)
{
#ifdef USE_PATH_STORAGE
	bool internalCmd = false;
	if (progExe != 0)
	{
		wchar_t* t = getProgExe();
		internalCmd = (t[0] == '|');
		freeRes(t);

		if( !internalCmd )
			launch(this);
	}
#else
	if (progExe != NULL && progExe[0] != '|')
		launch(this);
#endif

	if (!single && nextSameHotkey)
		nextSameHotkey->run(single);

#ifdef USE_PATH_STORAGE
	if (progExe != 0 && internalCmd)
		launch(this);
#else
	if (progExe != NULL && progExe[0] == '|')
		launch(this);
#endif
}