/*
** Copyright (C) 2003 WiSo (www.wisonauts.org)
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
#ifndef CDPLAYX_HELPER
#define CDPLAYX_HELPER

#include <xtl.h>
#include <XBFont.h>
#include <io.h>
#include "IOSupport.h"
#include "xbApplicationEx.h"
#include "simplexml.h"
#include "iso9660.h"
#include <dvd_reader.h>
#include <undocumented.h>
#include "..\dvd2xbox\d2xsettings.h"
//#include "cdiosupport.h"




#define CDPLAYX_NONE	0
#define CDPLAYX_CDDA	1
#define CDPLAYX_MOD		2

#define CDPLAYX_OK		0
#define CDPLAYX_ERR		1
#define CDPLAYX_DONE	2

//#define UNKNOWN			1
//#define DVD				2
//#define GAME			3
//#define ISO				4
//#define CDDA			5
//#define VCD				6
//#define SVCD			7
//#define UDF				20
//#define SMBDIR			21
//#define FTP				22
//#define UDF2SMB			23



//#define  int64_t            __int64
//#define uint64_t   unsigned __int64

#define INI_SIZE	1024

//extern void GetDriveState(WCHAR *m_scdstat,int& type);
void DPf_H(const char* pzFormat, ...);


class HelperX
{
protected:
	CIoSupport			m_IO;
	int					c_init;
	char				inidump[INI_SIZE];
	simplexml			*rootptr;
	simplexml			*ptr;
	
	
	
	
	// Dir browser
	char				currentdir[1024];
	char				cprevdir[20][1024];
	char*				cDirs[100];
	char*				cFiles[100];
	int					cbrowse;
	int					crelbrowse;
	int					coffset;
	int					mdirscount;
	int					mfilescount;
	int					level;
	bool				renew;

public:

	//char			logFilename[1200];
	//int				writeLog;
	static int		dvdsize;
	//static char*	homepath;

	// Userinterface
	void showList(float x,float y,int hl,CXBFont &font,char *items[]);
	int  getnList(char *items[]);
	void processList(XBGAMEPAD gp, XBIR_REMOTE ir,int& x, int& y);
	//HDDBROWSEINFO processDirBrowser(int lines,char* path,XBGAMEPAD gp, XBIR_REMOTE ir);
	//bool showDirBrowser(int lines,float x,float y,DWORD fc,DWORD hlfc, CXBFont &font);
	//bool resetDirBrowser();
	// Input
	bool pressA(XBGAMEPAD gp);
	bool pressB(XBGAMEPAD gp);
	bool pressX(XBGAMEPAD gp);
	bool pressY(XBGAMEPAD gp);
	bool pressLTRIGGER(XBGAMEPAD gp);
	bool pressRTRIGGER(XBGAMEPAD gp);
	bool pressBACK(XBGAMEPAD gp);
	bool pressSTART(XBGAMEPAD gp);
	bool pressBLACK(XBGAMEPAD gp);
	bool pressWHITE(XBGAMEPAD gp);
	bool IRpressBACK(XBIR_REMOTE ir);
	bool IRpressSELECT(XBIR_REMOTE ir);
	
	void RebootToDash();
	int	 readIni(char* file);
	const char* getIniValue(const char* root,const char* key);
	bool getalphanum(char* pattern);
	bool getFatxName(char* pattern);
	bool getfreeDiskspace(char* drive,char* size);
	bool getusedDiskspace(char* drive,char* size);
	int getusedDSul(char* drive);
	int getusedDSkb(char* path);
	bool getfreeDS(char* drive,int& size);
	int getfreeDSMB(char* drive);
	//bool DelTree(char *path);
	int getFilesize(char* filename);
	char* removeDrive(char* lpFileName,int c=2);
	LONGLONG CountDVDsize(char *path);
	void LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters=NULL);
	//void setLogFilename(char *file);
	//void enableLog(bool value);
	//void WriteLog(WCHAR *message,...);
	bool isdriveD(char* path);
	int getXBETitle(char* file,WCHAR* m_GameTitle);
	void addSlash(char* dest, char* source);
	void addSlash(char* source);



	HelperX();
	~HelperX();

};

#endif