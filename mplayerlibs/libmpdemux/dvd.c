/*****
 *
 * This file is part of libcss.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****/

#if defined(__NetBSD__) || defined(__OpenBSD__) || defined(__linux__)
# include "dvd_ioctl.c"
#elif defined(__sun)
# include "dvd_uscsi.c"
#else
//# warning No dvd device implementation available for your platform
//# include "dvd_null.c"

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <sys/ioctl.h>
#include <fcntl.h>

#include "dvd.h"
//#include "winioctl.h"
#include "dvdioctl.h"

/*
 * Internal state for a DVD device using Linux/xBSD dvd ioctls
 */
struct dvd_device {
    HANDLE		fd;	/* dvd device filedesc for dvd ioctl */
};
//#include "dvd_ioctl.h"
#if defined(_XBOX)
#include "Undocumented.h"
#endif

dvd_device_t *DVDOpenDeviceFD (HANDLE dev_fd);
dvd_device_t *DVDOpenDevice (char *dev_name)
{
    HANDLE fd;
    dvd_device_t *dvd;

#if defined (_XBOX)    
	ANSI_STRING filename;
	OBJECT_ATTRIBUTES attributes;
	IO_STATUS_BLOCK status;
	NTSTATUS error;
	//DWORD dummy;

	RtlInitAnsiString(&filename, dev_name);
	InitializeObjectAttributes(&attributes, &filename, OBJ_CASE_INSENSITIVE, NULL);

	if (!NT_SUCCESS(error = NtCreateFile(&fd, GENERIC_READ |
		SYNCHRONIZE | FILE_READ_ATTRIBUTES, &attributes, &status, NULL, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT)))
	{
		return NULL;
	}
#else
	fd = CreateFile(dev_name,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		0,
		0
		);
	if( fd == INVALID_HANDLE_VALUE )
		return NULL;
#endif
    dvd = DVDOpenDeviceFD(fd);
    if (!dvd)
	CloseHandle(fd);
    return dvd;
}

dvd_device_t *DVDOpenDeviceFD (HANDLE dev_fd)
{
    dvd_device_t *dvd;

    dvd = malloc(sizeof(dvd_device_t));
    if (dvd)
	dvd->fd = dev_fd;
    return dvd;
}

int DVDCloseDevice (dvd_device_t *dvd)
{
    if (dvd) {
	CloseHandle(dvd->fd);
	free(dvd);
    }
    return 0;
}

int DVDReadStruct (dvd_device_t *dvd, dvd_struct *s)
{
#ifdef __FreeBSD__
    return ioctl(dvd->fd, DVDIOCREADSTRUCTURE, s); 
#else
	DWORD dwBytesRead;
	DVD_READ_STRUCTURE st;
	static char buffer[2048+4];
	PVOID OutputBuffer;
	DWORD OutputBufferLen;

	st.BlockByteOffset.QuadPart = 0;

	switch( s->type )
	{
	case DVD_STRUCT_PHYSICAL:
		st.Format = DvdPhysicalDescriptor;
		st.LayerNumber = s->physical.layer_num;
		OutputBuffer = buffer;
		OutputBufferLen = 20;
		break;
	case DVD_STRUCT_COPYRIGHT:
		st.Format = DvdCopyrightDescriptor;
		st.LayerNumber = s->copyright.layer_num;
		OutputBuffer = buffer;
		OutputBufferLen = 8;
		break;
	case DVD_STRUCT_DISCKEY:
		st.SessionId = s->disckey.agid;
		st.Format = DvdDiskKeyDescriptor;
		OutputBuffer = buffer;
		OutputBufferLen = 2048+4;
		break;
	case DVD_STRUCT_BCA:
		st.Format = DvdBCADescriptor;
		OutputBuffer = buffer;
		OutputBufferLen = 188+4;
		break;
	case DVD_STRUCT_MANUFACT:
		st.Format = DvdManufacturerDescriptor;
		st.LayerNumber = s->manufact.layer_num;
		OutputBuffer = buffer;
		OutputBufferLen = 2048+4;
		break;
	}

	if( !DeviceIoControl(
		dvd->fd,
		IOCTL_DVD_READ_STRUCTURE,
		&st,
		sizeof(st),
		OutputBuffer,
		OutputBufferLen,
		&dwBytesRead,
		NULL ) )
		return -1;

	switch( s->type )
	{
	case DVD_STRUCT_PHYSICAL:
		break;
	case DVD_STRUCT_COPYRIGHT:
		s->copyright.cpst = buffer[4];
		s->copyright.rmi = buffer[5];
		break;
	case DVD_STRUCT_DISCKEY:
		memcpy(s->disckey.value, &(buffer[4]), 2048);
		break;
	case DVD_STRUCT_BCA:
		memcpy(s->bca.value, &(buffer[4]), 188);
		break;
	case DVD_STRUCT_MANUFACT:
		memcpy(s->manufact.value, &(buffer[4]), 2048);
		break;
	}

	return 0;

//    return ioctl(dvd->fd, DVD_READ_STRUCT, s); 
#endif
}

static char buff[256];
int DVDAuth (dvd_device_t *dvd, dvd_authinfo *a)
{
#ifdef __FreeBSD__
    return ioctl(dvd->fd, DVDIOCREPORTKEY, a); 
#else

	DWORD dwErr;
	DWORD dwBytesRead;
	DWORD dwIoctl;
	PVOID InputBuffer;
	PVOID OutputBuffer;
	DWORD InputBufferSize;
	DWORD OutputBufferSize;
	DVD_KEY_TYPE Type;

	DWORD bl, kl;

	ULONG agid;
	char buffer[40];
	DVD_COPY_PROTECT_KEY* key = (DVD_COPY_PROTECT_KEY*)&buffer;

//sprintf(buff, "dvd: %p\n", dvd);
//OutputDebugString(buff);
	
	memset(buffer, 0, 40);

//	OutputDebugString("DVDAuth\n");

	switch(a->type)
	{
	case DVD_LU_SEND_AGID:
//		OutputDebugString("DVDAuth: DVD_LU_SEND_AGID\n");
		dwIoctl = IOCTL_DVD_START_SESSION;
		InputBuffer = NULL;
		InputBufferSize = 0;
		OutputBuffer = &agid;
		OutputBufferSize = sizeof(ULONG);
		break;

	case DVD_INVALIDATE_AGID:
//		OutputDebugString("DVDAuth: DVD_INVALIDATE_AGID\n");
		dwIoctl = IOCTL_DVD_END_SESSION;
		InputBuffer = &agid;
		InputBufferSize = sizeof(ULONG);
		OutputBuffer = NULL;
		OutputBufferSize = 0;
		agid = a->lsa.agid;
		break;

	case DVD_LU_SEND_ASF:
//		OutputDebugString("DVDAuth: DVD_LU_SEND_ASF\n");
		return -1;
		break;

	case DVD_LU_SEND_KEY1:
//		OutputDebugString("DVDAuth: DVD_LU_SEND_KEY1\n");
		Type = DvdBusKey1;
		kl = 5;
		bl = 8;
		break;

	case DVD_LU_SEND_CHALLENGE:
//		OutputDebugString("DVDAuth: DVD_LU_SEND_CHALLENGE\n");
		Type = DvdChallengeKey;
		kl = 10;
		bl = 12;
		break;

	case DVD_LU_SEND_TITLE_KEY:
//		OutputDebugString("DVDAuth: DVD_LU_SEND_TITLE_KEY\n");
		Type = DvdTitleKey;
		kl = 6;
		bl = 8;
		key->Parameters.TitleOffset.QuadPart = a->lstk.lba * 2048;
		break;

	case DVD_HOST_SEND_CHALLENGE:
//		OutputDebugString("DVDAuth: DVD_HOST_SEND_CHALLENGE\n");
		Type = DvdChallengeKey;
		kl = 10;
		bl = 12;
		memcpy(key->KeyData, a->hsc.chal, kl);
		break;

	case DVD_HOST_SEND_KEY2:
//		OutputDebugString("DVDAuth: DVD_HOST_SEND_KEY2\n");
		Type = DvdBusKey2;
		kl = 5;
		bl = 8;
		memcpy(key->KeyData, a->hsk.key, kl);
		break;
	}

	if( a->type != DVD_LU_SEND_AGID && a->type != DVD_INVALIDATE_AGID )
	{
		// finish the stuff
		switch (a->type)
		{
			case DVD_LU_SEND_KEY1:
			case DVD_LU_SEND_CHALLENGE:
			case DVD_LU_SEND_TITLE_KEY:
				dwIoctl = IOCTL_DVD_READ_KEY;
				break;
			case DVD_HOST_SEND_CHALLENGE:
			case DVD_HOST_SEND_KEY2:
				dwIoctl = IOCTL_DVD_SEND_KEY;
				break;
		}

//sprintf(buff, "key: %p\n", &key);
//OutputDebugString(buff);
		key->KeyLength = sizeof(DVD_COPY_PROTECT_KEY)+bl;
		key->SessionId = a->hsc.agid;
		key->KeyType = Type;
		key->KeyFlags = 0;

		InputBuffer = key;
		InputBufferSize = key->KeyLength;
		OutputBuffer = key;
		OutputBufferSize = key->KeyLength;
	}

	if( !(dwErr = DeviceIoControl(
		dvd->fd,
		dwIoctl,
		InputBuffer,
		InputBufferSize,
		OutputBuffer,
		OutputBufferSize,
		&dwBytesRead,
		NULL ) ) )
	{
		dwErr = GetLastError();
		return -1;
	}

	switch(a->type)
	{
	case DVD_LU_SEND_AGID:
		a->lsa.agid = agid;
		break;
	case DVD_INVALIDATE_AGID:
	case DVD_LU_SEND_ASF:
		break;

	case DVD_LU_SEND_KEY1:
		memcpy(a->lsk.key, key->KeyData, kl);
		break;

	case DVD_LU_SEND_CHALLENGE:
		memcpy(a->lsc.chal, key->KeyData, kl);
		break;

	case DVD_LU_SEND_TITLE_KEY:
		memcpy(a->lstk.title_key, key->KeyData, kl);
		break;

	case DVD_HOST_SEND_CHALLENGE:
	case DVD_HOST_SEND_KEY2:
		break;
	}


	return 0;

#endif
}

#endif

