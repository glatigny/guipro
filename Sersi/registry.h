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

#include <windows.h>
#include <list>

bool regOpenKey(HKEY* hkey, const wchar_t* progname, bool read, bool write);
bool regCreateKey(const wchar_t* progname);
bool regRenameKey(const wchar_t* progname, const wchar_t* newprogname);
bool regCopyKey(const wchar_t* progname, const wchar_t* newprogname);
bool regDeleteKey(const wchar_t* progname);

wchar_t* GetStrOptionValue(const wchar_t* optionName, const wchar_t* progname);
wchar_t* GetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, wchar_t* value, DWORD value_size);

DWORD SetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, wchar_t* value, DWORD value_size);
DWORD SetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, wchar_t* value, DWORD value_size);

DWORD GetDwOptionValue(const wchar_t* optionName, const wchar_t* progname);
DWORD GetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, DWORD *value);

DWORD SetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, DWORD value);
DWORD SetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, DWORD value, HKEY* hk);

DWORD RemOption(const wchar_t* optionName, const wchar_t* progname);
DWORD RemOption(const wchar_t* optionName, const wchar_t* progname, HKEY* hk);

std::list<wchar_t*> listKeys();