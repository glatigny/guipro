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

#include "registry.h"
#include "common.h"

#include <stdio.h>
#include <tchar.h>

/* ------------------------------------------------------------------------------------------------- */

bool regOpenKey(HKEY* hkey, const wchar_t* progname, bool read, bool write)
{
	REGSAM mode = 0;
	if( !read && !write )
	{
		mode = KEY_ALL_ACCESS;
	}
	else
	{
		if( read )
		{
			mode |= KEY_READ;
		}
	
		if( write )
		{
			mode |= KEY_WRITE;
		}
	}

	if( progname == NULL )
	{
		if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKEY, 0, mode, hkey) != ERROR_SUCCESS )
		{
			return false;
		}
	}
	else
	{
		wchar_t key[1024];
		wsprintf(key, L"%s\\%s", szKEY, progname);
		if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, mode, hkey) != ERROR_SUCCESS )
		{
			return false;
		}
	}
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool regCreateKey(const wchar_t* progname)
{
	HKEY hk;
	DWORD dwDisp = REG_CREATED_NEW_KEY;
	if( progname == NULL )
	{
		if( RegCreateKeyEx(HKEY_LOCAL_MACHINE, szKEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, &dwDisp) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	else
	{
		wchar_t key[1024];
		wsprintf(key, L"%s\\%s", szKEY, progname);
		if( RegCreateKeyEx(HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hk, &dwDisp) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	RegCloseKey(hk);
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool regRenameKey(const wchar_t* progname, const wchar_t* newprogname)
{
	if( _wcsicmp(progname, newprogname) )
	{
		if( regCopyKey(progname, newprogname) )
		{
			return regDeleteKey(progname);
		}
	}
	return false;
}
/* ------------------------------------------------------------------------------------------------- */

bool regCopyKey(const wchar_t* progname, const wchar_t* newprogname)
{
	HKEY hKey;
	HKEY hNewKey;

	if( !regOpenKey(&hKey, progname, true, false) )
		return false;

	if( !regCreateKey(newprogname) )
	{
		RegCloseKey(hKey);
		return false;
	}
	if( !regOpenKey(&hNewKey, newprogname, false, true) )
	{
		RegCloseKey(hKey);
		return false;
	}

	wchar_t szName[MAX_PATH];		// Buffer for registry names.
	DWORD dwcNameLen = MAX_PATH;	// Length of buffer.
	//DWORD dwcSubKeys = 0;			// Number of sub keys.
	//DWORD dwcMaxSubKey;			// Longest sub key size.
	//DWORD dwcMaxClass;			// Longest class string.
	DWORD dwcValues = 0;			// Number of values for this key.
	//DWORD dwcMaxValueName;		// Longest Value name.
	DWORD dwcMaxValueData;			// Longest Value data.
	//DWORD dwcSecDesc;				// Security descriptor.
	//FILETIME ftLastWriteTime;		// Last write time.

	ZeroMemory(szName, sizeof(wchar_t) * MAX_PATH);

	if( RegQueryInfoKey(hKey, szName, &dwcNameLen, NULL, NULL, NULL, NULL, &dwcValues, NULL, &dwcMaxValueData, NULL, NULL ) != ERROR_SUCCESS )
	{
		RegCloseKey(hNewKey);
		RegCloseKey(hKey);
		return false;
	}

	// allow for terminating nulls
	dwcMaxValueData++; 

	if (dwcValues)
	{
		DWORD dwcValueLen = dwcMaxValueData; 
		DWORD dwIndex = 0;
		DWORD dwType;
		LPBYTE pszData = (LPBYTE)malloc(dwcValueLen);

		if( pszData == NULL )
		{
			RegCloseKey(hKey);
			RegCloseKey(hNewKey);
			return false;
		}

		dwcNameLen = MAX_PATH;

		while( RegEnumValue(hKey, dwIndex++, szName, &dwcNameLen, NULL, &dwType, pszData, &dwcValueLen) == ERROR_SUCCESS )
		{
			RegSetValueEx(hNewKey, szName, NULL, dwType, pszData, dwcValueLen);
			dwcNameLen = MAX_PATH;
			dwcValueLen = dwcMaxValueData;
		}

		free(pszData);
	}
	
	RegCloseKey(hKey);
	RegCloseKey(hNewKey);
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

bool regDeleteKey(const wchar_t* progname)
{
	if( progname == NULL )
	{
		if( RegDeleteKey(HKEY_LOCAL_MACHINE, szKEY) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	else
	{
		wchar_t key[1024];
		wsprintf(key, L"%s\\%s", szKEY, progname);
		if( RegDeleteKey(HKEY_LOCAL_MACHINE, key) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	return true;
}

/* ------------------------------------------------------------------------------------------------- */

wchar_t* GetStrOptionValue(const wchar_t* optionName, const wchar_t* progname)
{
	return GetStrOptionValue(optionName, progname, NULL, NULL, 0);
}

/* -------------------------------------------------------- */

wchar_t* GetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, wchar_t* value, DWORD value_size)
{
	HKEY _hk;
	if( hk != NULL || regOpenKey(&_hk, progname, true, false) )
	{
		DWORD dwType = REG_SZ;

		if( value == NULL )
		{
			wchar_t optionValue[REG_KEY_LEN];
			ZeroMemory(optionValue, REG_KEY_LEN);
			DWORD size = REG_KEY_LEN;
			
			if(RegQueryValueEx( ((hk!=NULL)?(*hk):(_hk)) , optionName, 0, &dwType, (LPBYTE)optionValue, &size) == ERROR_SUCCESS)
			{
				if(hk == NULL) RegCloseKey(_hk);
				return _wcsdup(optionValue);
			}
		}
		else
		{
			if(RegQueryValueEx( ((hk!=NULL)?(*hk):(_hk)) , optionName, 0, &dwType, (LPBYTE)value, &value_size) == ERROR_SUCCESS)
			{
				if(hk == NULL) RegCloseKey(_hk);
				return L"";
			}
		}
	}
	if(hk == NULL) RegCloseKey(_hk);
	return NULL;
}

/* ------------------------------------------------------------------------------------------------- */

DWORD SetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, wchar_t* value, DWORD value_size)
{
	return SetStrOptionValue(optionName, progname, NULL, value, value_size);
}

/* -------------------------------------------------------- */

DWORD SetStrOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, wchar_t* value, DWORD value_size)
{
	HKEY _hk;
	if( hk != NULL || regOpenKey(&_hk, progname, false, true) )
	{
		DWORD dwType = REG_SZ;

		if( RegSetValueEx( ((hk!=NULL)?(*hk):(_hk)), optionName, 0, dwType, (LPBYTE)value, value_size) == ERROR_SUCCESS )
		{
			if(hk == NULL) RegCloseKey(_hk);
			return TRUE;
		}
	}
	if(hk == NULL) RegCloseKey(_hk);
	return FALSE;
}

/* ------------------------------------------------------------------------------------------------- */

DWORD GetDwOptionValue(const wchar_t* optionName, const wchar_t* progname)
{
	return GetDwOptionValue(optionName, progname, NULL, NULL);
}

/* -------------------------------------------------------- */

DWORD GetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, HKEY* hk, DWORD *value)
{
	HKEY _hk;
	if( hk != NULL || regOpenKey(&_hk, progname, true, false) )
	{
		DWORD dwType = REG_DWORD;
		DWORD size = sizeof(DWORD);

		if( value == NULL )
		{
			DWORD ret = 0;

			if( RegQueryValueEx( ((hk!=NULL)?(*hk):(_hk)) , optionName, 0, &dwType, (LPBYTE)&ret, &size) == ERROR_SUCCESS )
			{
				if(hk == NULL) RegCloseKey(_hk);
				return ret;
			}
		}
		else
		{
			if( RegQueryValueEx( ((hk!=NULL)?(*hk):(_hk)) , optionName, 0, &dwType, (LPBYTE)value, &size) == ERROR_SUCCESS )
			{
				if(hk == NULL) RegCloseKey(_hk);
				return TRUE;
			}	
		}
	}
	if(hk == NULL) RegCloseKey(_hk);
	return 0;
}

/* ------------------------------------------------------------------------------------------------- */

DWORD SetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, DWORD value)
{
	return SetDwOptionValue(optionName, progname, value, NULL);
}

/* -------------------------------------------------------- */

DWORD SetDwOptionValue(const wchar_t* optionName, const wchar_t* progname, DWORD value, HKEY* hk)
{
	HKEY _hk;
	if( hk != NULL || regOpenKey(&_hk, progname, false, true) )
	{
		DWORD dwType = REG_DWORD;

		if( RegSetValueEx( ((hk!=NULL)?(*hk):(_hk)), optionName, 0, dwType, (LPBYTE)&value, (DWORD)sizeof(DWORD)) == ERROR_SUCCESS )
		{
			if(hk == NULL) RegCloseKey(_hk);
			return TRUE;
		}
	}
	if(hk == NULL) RegCloseKey(_hk);
	return FALSE;
}

/* ------------------------------------------------------------------------------------------------- */

DWORD RemOption(const wchar_t* optionName, const wchar_t* progname)
{
	return RemOption(optionName, progname, NULL);
}

/* -------------------------------------------------------- */

DWORD RemOption(const wchar_t* optionName, const wchar_t* progname, HKEY* hk)
{
	HKEY _hk;
	if( hk != NULL || regOpenKey(&_hk, progname, false, true) )
	{
		if( RegDeleteValue( ((hk!=NULL)?(*hk):(_hk)) , optionName) )
		{
			if(hk == NULL) RegCloseKey(_hk);
			return TRUE;
		}
	}
	if(hk == NULL) RegCloseKey(_hk);
	return FALSE;
}

/* ------------------------------------------------------------------------------------------------- */

std::list<wchar_t*> listKeys()
{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

	std::list<wchar_t*> lst;

	HKEY hKey;

	if( regOpenKey(&hKey, NULL, true, false) )
	{
		DWORD		i, retCode;
		wchar_t		achClass[MAX_PATH] = L"";
		DWORD		cchClassName = MAX_PATH;
		DWORD		cSubKeys = 0;
		DWORD		cchMaxClass;
		DWORD		cValues, cchMaxValue, cbMaxValueData;
		DWORD		cbMaxSubKey;
		DWORD		cbSecurityDescriptor;
		FILETIME	ftLastWriteTime;

		// Get the class name and the value count. 
		retCode = RegQueryInfoKey(
						hKey,                    // key handle 
						achClass,                // buffer for class name 
						&cchClassName,           // size of class string 
						NULL,                    // reserved 
						&cSubKeys,               // number of subkeys 
						&cbMaxSubKey,            // longest subkey size 
						&cchMaxClass,            // longest class string 
						&cValues,                // number of values for this key 
						&cchMaxValue,            // longest value name 
						&cbMaxValueData,         // longest value data 
						&cbSecurityDescriptor,   // security descriptor 
						&ftLastWriteTime);       // last write time 

		if( cSubKeys )
		{
			wchar_t		achKey[MAX_KEY_LENGTH];
			DWORD		cbName;
			for( i = 0; i < cSubKeys; i++ )
			{ 
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyEx(hKey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime); 
				if (retCode == ERROR_SUCCESS) 
				{
					lst.insert( lst.end(), _wcsdup(achKey) );
				}
			}
		}
	}
	RegCloseKey(hKey);

	return lst;

#undef MAX_KEY_LENGTH
#undef MAX_VALUE_NAME
}