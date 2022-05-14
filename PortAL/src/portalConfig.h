/*
	PortAL - GUIPro Project ( http://glatigny.github.io/guipro/ )

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

#ifndef __PORTAL_CONFIG_H__
#define __PORTAL_CONFIG_H__

#include "common.h"
#include <windows.h>
#include <vector>

#define PROG_OPTION_NONE		(0x000)
#define PROG_OPTION_DEFAULT		(0x001)
#define PROG_OPTION_SHELL		(0x002)
#define PROG_OPTION_NOSHELL		(0x004)
#define PROG_OPTION_AUTOOPEN	(0x008)
#define PROG_OPTION_BROWSE		(0x010)
#define PROG_OPTION_SHOWSC		(0x020)
#define PROG_OPTION_BREAK		(0x040)
#define PROG_OPTION_ELEVATION	(0x080)

#define PROG_EVENT_NONE			(0x00)
#define PROG_EVENT_START		(0x01)
#define PROG_EVENT_QUIT			(0x02)
#define PROG_EVENT_LOCK			(0x04)
#define PROG_EVENT_UNLOCK		(0x08)

class PortalProg;

extern void launch(PortalProg* prog);

class PortalProg
{
	protected:
#ifdef USE_PATH_STORAGE
		size_t		icoPath;
		size_t		progExe;
		size_t		progParam;
		size_t		dirPath;
#else
		wchar_t*	icoPath;
		wchar_t*	progExe;
		wchar_t*	progParam;
		wchar_t*	dirPath;
#endif
	public:
		wchar_t*	progName;
		UINT		hkey;
		UINT		modifier;
		bool		overriding;
		UINT		uID;
		UINT		options;
		CHAR		events;
		PortalProg*	nextSameHotkey;
		std::vector<PortalProg*> progs;

		PortalProg();
		~PortalProg();

		void setIcoPath(wchar_t*);
		wchar_t* getIcoPath();
		bool isIcoPath();
		//
		void setProgExe(wchar_t*);
		wchar_t* getProgExe();
		bool isProgExe();
		//
		void setProgParam(wchar_t*);
		wchar_t* getProgParam();
		bool isProgParam();
		//
		void setDirPath(wchar_t*);
		wchar_t* getDirPath();
		bool isDirPath();

		inline void freeRes(wchar_t* content) {
#ifdef USE_PATH_STORAGE
			if (content != NULL)
				free(content);
#else
			UNREFERENCED_PARAMETER(content);
#endif
		};

		void run(bool);
};

typedef std::vector<PortalProg*> PortalProgVector;

class PortalConfig
{
	public:
		PortalProgVector	menus;
		PortalProgVector	hotkeys;
		PortalProgVector	flat;
		bool				shellExecuteDefault;
#ifdef ICON_MANAGER
		bool				iconloaderThread;
#endif

		PortalConfig()
		{
			menus.clear();
			hotkeys.clear();
			flat.clear();
			shellExecuteDefault = false;
#ifdef ICON_MANAGER
			iconloaderThread = false;
#endif
		}

		~PortalConfig()
		{
			for( PortalProgVector::iterator i = hotkeys.begin(); i != hotkeys.end(); i++ )
			{
				// Only remove special uID items.
				if( (*i)->uID == (UINT)(-1) )
					delete( *i );
			}
			for( PortalProgVector::iterator i = menus.begin(); i != menus.end(); i++ )
			{
				delete( *i );
			}
			menus.clear();
			hotkeys.clear();
			flat.clear();
		}
};

struct _PortalVariable {
	wchar_t* key;
	wchar_t* value;
} typedef PortalVariable;

typedef std::vector<PortalVariable*> PortalVariableVector;

#endif /* __PORTAL_CONFIG_H__ */
