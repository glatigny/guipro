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

#ifndef PORTAL_CONFIG_H
#define PORTAL_CONFIG_H

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

#define PROG_EVENT_NONE			(0x000)
#define PROG_EVENT_START		(0x001)
#define PROG_EVENT_QUIT			(0x002)
#define PROG_EVENT_LOCK			(0x004)
#define PROG_EVENT_UNLOCK		(0x008)

class PortalProg;

extern void launch(PortalProg* prog);

class PortalProg
{
	public:
		wchar_t*	progName;
		wchar_t*	icoPath;
		wchar_t*	progExe;
		wchar_t*	progParam;
		wchar_t*	dirPath;
		UINT		hkey;
		UINT		modifier;
		bool		overriding;
		UINT		uID;
		UINT		options;
		CHAR		events;
		PortalProg*	nextSameHotkey;
		std::vector<PortalProg*> progs;

		PortalProg()
		{
			progName = NULL;
			icoPath = NULL;
			progExe = NULL;
			progParam = NULL;
			dirPath = NULL;
			hkey = 0;
			modifier = 0;
			overriding = false;
			uID = 0;
			options = 0;
			nextSameHotkey = NULL;
			progs.clear();
		}

		~PortalProg()
		{
			for( std::vector<PortalProg*>::iterator i = progs.begin(); i != progs.end(); i++ )
			{
				delete (*i);
			}
			progs.clear();

			free( progName );
			free( icoPath );
			free( progExe );
			free( progParam );
			free( dirPath );

			nextSameHotkey = NULL;
		}

		void run(bool single)
		{
			if( progExe != NULL && progExe[0] !=  '|' )
				launch( this );

			if( !single && nextSameHotkey )
				nextSameHotkey->run(single);

			if( progExe != NULL && progExe[0] ==  '|' )
				launch( this );
		}
};

typedef std::vector<PortalProg*> PortalProgVector;

class PortalConfig
{
	public:
		PortalProgVector	menus;
		PortalProgVector	hotkeys;
		PortalProgVector	flat;
		bool				shellExecuteDefault;

		PortalConfig()
		{
			menus.clear();
			hotkeys.clear();
			flat.clear();
			shellExecuteDefault = false;
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

#endif /* PORTAL_CONFIG_H */
