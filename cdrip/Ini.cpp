/*
** Copyright (C) 2001 Albert L. Faber
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/


#include "StdAfx.h"
#include "Ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni(  )
{
}


CIni::~CIni()
{
}



// **********************************************************************************

BOOL CIni::SetValue(const char * cSection, const char * cItem, const char * cVal)
{
	ASSERT( !m_strFileName.IsEmpty() );

	return WritePrivateProfileString( cSection, cItem, cVal, m_strFileName );
}

BOOL CIni::SetValue(const char * cSection, const char * cItem, const int iVal)
{
	CString strEntry;
	strEntry.Format( "%d", iVal );
	return SetValue( cSection, cItem, strEntry );
}

BOOL CIni::SetValue(const char * cSection, const char * cItem, const long lVal)
{
	return SetValue( cSection, cItem, (int) lVal );
}

BOOL CIni::SetValue(const char * cSection, const char * cItem, const DWORD dwVal)
{
	return SetValue( cSection, cItem, (int)dwVal );
}



CString CIni::GetValue( const char * cSection, const char * cItem, CString strDefault )
{
	ASSERT( !m_strFileName.IsEmpty() );

	CHAR lpszValue[ 1024 ] = {'\0',};

	::GetPrivateProfileString(	cSection,
								cItem,
								strDefault,
								lpszValue,
								sizeof( lpszValue),
								m_strFileName );

	return CString( lpszValue );
}

INT CIni::GetValue( const char * cSection, const char * cItem, const INT nDefault )
{
	INT nReturn = nDefault;

	CString strReturn = GetValue( cSection, cItem, "" );

	if ( !strReturn.IsEmpty() )
	{
		nReturn = atoi( (LPSTR)(LPCSTR)strReturn );
	}

	return nReturn;
}


LONG CIni::GetValue( const char * cSection, const char * cItem, const LONG nDefault )
{
	return (LONG) GetValue ( cSection, cItem, (INT) nDefault );
}

DWORD CIni::GetValue( const char * cSection, const char * cItem, const DWORD nDefault )
{
	return (DWORD) GetValue ( cSection, cItem, (INT) nDefault );
}



