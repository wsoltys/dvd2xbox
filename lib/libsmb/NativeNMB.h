/*
    This file is part of the smb++ library
    Copyright (C) 2000  Nicolas Brodu
    nicolas.brodu@free.fr

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program, see the file COPYING; if not, write
    to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge,
    MA 02139, USA.
*/

/*
	Implements NameQueryInterface using the native code
*/
#ifndef NATIVE_NMB_H
#define NATIVE_NMB_H
#include "defines.h"
#ifndef USE_SAMBA

#include "NameQueryInterface.h"
#ifdef _WIN32
#ifdef _XBOX
#include <xtl.h>
#undef errno
#else
#include <winsock2.h>
#endif
#else
#include <netdb.h>
#endif

class NMBIO;

class NativeNMB : public NameQueryInterface
{
private:
	NMBIO* nmbio;
	hostent returnValue;

public:
	NativeNMB();
	~NativeNMB();
	virtual hostent *gethostbyname(const char *name, bool groupquery=false);
	virtual int setNBNSAddress(uint32 addr);
	virtual int setNBNSAddress(const char *addr);
};

#endif
#endif
