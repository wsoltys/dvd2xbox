/*
** Copyright(C) 2003 WiSo (www.wisonauts.org)
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
**(at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111 - 1307, USA.
*/


#include "helper.h"



int	HelperX::dvdsize=0;
//char* HelperX::homepath=NULL;

////////////////////////////////////
// hack for Drivestatus
// prevents app from crash
/*
using namespace XISO9660;

CIoSupport			m_IO;
CCdInfo*			m_pCdInfo=NULL;
int	dvdsize=0;

LONGLONG getusedDS(char *path)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	LONGLONG llValue = 0;
	LONGLONG llResult = 0;
	LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				// Recursion
				llValue += getusedDS( sourcefile );
				//llResult += llValue;
			}
			else
			{
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					llValue += liSize.QuadPart;
				}
				
				//size += (wfd.nFileSizeHigh * (MAXDWORD+1)) + wfd.nFileSizeLow;
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return llValue;
}

void GetDriveState(WCHAR *m_scdstat,int& type)
{
	DWORD m_cdstat = m_IO.GetTrayState();
	dvd_reader_t*	dvd;
	int	size;

	static CCdInfo*		m_pCdInfo;
	CCdIoSupport cdio;
	//	Delete old CD-Information
	if ( m_pCdInfo != NULL ) {
		delete m_pCdInfo;
		m_pCdInfo = NULL;
	}
	//	Detect new CD-Information
	m_pCdInfo = cdio.GetCdInfo();

	WCHAR temp[40];
	switch(m_cdstat)
	{
		case TRAY_OPEN:
 			wcscpy(m_scdstat,L"DVD: Tray Open");
 			break;
 		case DRIVE_NOT_READY:
			type = 0;
 			wcscpy(m_scdstat,L"DVD: Drive Init");
 			break;
 		case TRAY_CLOSED_NO_MEDIA:
 			wcscpy(m_scdstat,L"DVD: No Disc");
 			break;
 		case TRAY_CLOSED_MEDIA_PRESENT:
			if(!type)
			{
				dvdsize = 0;
				m_IO.Remount("D:","Cdrom0");
				if (_access("D:\\default.xbe",00)!=-1)
				{
					type = GAME;
					dvdsize = getusedDS("D:\\")/1048576;
					wsprintfW(temp,L"DVD: XBOX Software %d MB",dvdsize);
				} else if(_access("D:\\VIDEO_TS",00)!=-1)
				{
					type = DVD;
					dvd = DVDOpen("\\Device\\Cdrom0");
					dvdsize = getusedDS("D:\\")/1048576;
					DVDClose(dvd);
					wsprintfW(temp,L"DVD: Video %d MB",dvdsize);
				} else if( m_pCdInfo->IsIso9660( 1 ) || m_pCdInfo->IsIso9660Interactive( 1 ) )
				{
					iso9660* m_pIsoReader;
					m_pIsoReader = new iso9660();
					HANDLE fd;
					if((fd=m_pIsoReader->OpenFile("\\VCD\\ENTRIES.VCD"))!=INVALID_HANDLE_VALUE)
					{
						type = VCD;
						wsprintfW(temp,L"DVD: VCD");
						m_pIsoReader->CloseFile(fd);
					} else if((fd=m_pIsoReader->OpenFile("\\SVCD\\ENTRIES.SVD"))!=INVALID_HANDLE_VALUE)
					{
						type = SVCD;
						wsprintfW(temp,L"DVD: SVCD");
						m_pIsoReader->CloseFile(fd);
 					} else 
					{
						type = ISO;
						//wsprintfW(temp,L"DVD: ISO CD %d",m_pCdInfo->GetIsoSize(2));
						wsprintfW(temp,L"DVD: ISO");
						
					}
					delete m_pIsoReader;
					m_pIsoReader = NULL;
 				} else if(m_pCdInfo->IsAudio( 1 )) 
				{
						type = CDDA;
						wsprintfW(temp,L"DVD: Audio CD");
					
				} else
				{
					type = UNKNOWN;
					wsprintfW(temp,L"DVD: unknown");
				}
				wcscpy(m_scdstat,temp);
				m_IO.Remount("D:","Cdrom0");
			}
			break;
		default:
			type = 0;
			wcscpy(m_scdstat,L"DVD: Drive Init");
	}

}

*/

HelperX::HelperX()
{
	c_init = CDPLAYX_NONE;
	//writeLog = false;
	
	/*
	char path[1024];
	
	m_IO.GetXbePath(path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	homepath = new char[strlen(path)+5];
	strcpy(homepath,path);
	strcat(homepath,"\\");
	*/
}

HelperX::~HelperX()
{
}


void HelperX::showList(float x,float y,int hl,CXBFont &font,char *items[])
{
	int i=0;
	float tmpy=0;
	DWORD dwColor;
	WCHAR text[100];
	if(((hl-1) > getnList(items)) || ((hl-1) <= 0 ))
	{
		hl=1;
	}

	while(items[i] != NULL)
	{
		tmpy = i*font.m_fFontHeight;
		if(i != (hl-1))
		{
			dwColor = 0xffffffff;
		} else {
			dwColor = 0xffffff00;
		}
		wsprintfW(text,L"%S",items[i]);
		font.DrawText( x, y+tmpy, dwColor, text );
		i++;
	}

}

int  HelperX::getnList(char *items[])
{
	int i=0;
	while(items[i] != NULL)
	{
		i++;
	}
	return i;
}

void HelperX::processList(XBGAMEPAD gp, XBIR_REMOTE ir,int& x, int& y)
{
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) || (ir.wPressedButtons == XINPUT_IR_REMOTE_UP)) {
		x--;
	}
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (ir.wPressedButtons == XINPUT_IR_REMOTE_DOWN)) {
		x++;
	}
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT) || (ir.wPressedButtons == XINPUT_IR_REMOTE_LEFT)) {
		y--;
	}
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT) || (ir.wPressedButtons == XINPUT_IR_REMOTE_RIGHT)) {
		y++;
	}
}


bool HelperX::pressA(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_A]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressB(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_B]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressX(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_X]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressY(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_Y]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressLTRIGGER(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressRTRIGGER(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressBLACK(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressWHITE(XBGAMEPAD gp)
{
	if((gp.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE]))
	{
		return true;
	}
	return false;
}

bool HelperX::pressBACK(XBGAMEPAD gp)
{
	if((gp.wPressedButtons & XINPUT_GAMEPAD_BACK))
	{
		return true;
	}
	return false;
}

bool HelperX::pressSTART(XBGAMEPAD gp)
{
	if((gp.wPressedButtons & XINPUT_GAMEPAD_START))
	{
		return true;
	}
	return false;
}

bool HelperX::IRpressBACK(XBIR_REMOTE ir)
{
	if((ir.wPressedButtons == XINPUT_IR_REMOTE_BACK))
	{
		return true;
	}
	return false;
}

bool HelperX::IRpressSELECT(XBIR_REMOTE ir)
{
	if((ir.wPressedButtons == XINPUT_IR_REMOTE_SELECT))
	{
		return true;
	}
	return false;
}


void HelperX::RebootToDash()
{
	m_IO.Unmount("D:");
	m_IO.Mount("D:","Harddisk0\\Partition2");
	LD_LAUNCH_DASHBOARD LaunchDash;
	LaunchDash.dwReason = XLD_LAUNCH_DASHBOARD_MAIN_MENU;
	LaunchDash.dwContext = 0;
	LaunchDash.dwParameter1 = 0;
	LaunchDash.dwParameter2 = 0;
	// todo: should come from ini file
	XLaunchNewImage("D:\\evoxdash.xbe", (PLAUNCH_DATA)(&LaunchDash) );
}

int HelperX::readIni(char* file)
{
	FILE* fh;
	int read;

	if((fh=fopen(file,"rt"))==NULL)
		return 1;
	read = fread(inidump,sizeof(char),INI_SIZE,fh);
	fclose(fh);
	if(read < 10)
		return 1;
	rootptr = new simplexml(inidump);
	return 0;
}

const char* HelperX::getIniValue(const char* root, const char* key)
{
	if((ptr = rootptr->child(root))==NULL)
		return "not found";
	return ptr->child(key)->value();
}


bool HelperX::getalphanum(char* pattern) 
{
	int i=0;
	char *temp;
	temp = new char[strlen(pattern)];
	if(temp==NULL)
		return false;
	strcpy(temp,"\0");
	while(pattern[i] != NULL)
	{
		if(isalnum(__toascii(pattern[i])))
		{
			sprintf(temp,"%s%c",temp,pattern[i]);
		}
		i++;
	}
	strcpy(pattern,temp);
	delete[] temp;
	return true;
}

bool HelperX::getFatxName(char* pattern)
{
	int i=0;
	int fatx=42;
	char temp[60];
	//temp = new char[strlen(pattern)];
	//if(temp==NULL)
	//	return false;
	strcpy(temp,"\0");
	while(pattern[i] != NULL)
	{
		if(isalnum(pattern[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern[i]))
		{
			sprintf(temp,"%s%c",temp,pattern[i]);
		}
		i++;
	}
	memset(pattern,0,fatx+1);
	if(strlen(temp) > fatx)
	{
		char* c;
		c = strrchr(temp,'.');
		if(c)
		{
			strncpy(pattern,temp,fatx-4);
			strncat(pattern,c,4);
		} else {
			strncpy(pattern,temp,fatx);
			//strcat(pattern,"\0");
			pattern[fatx] = '\0';
		}
	} else {
		strcpy(pattern,temp);
	}
	//delete[] temp;
	return true;
}

bool HelperX::getfreeDiskspace(char* drive,char* size)
{
	ULARGE_INTEGER ulFreeAvail;
	if( !GetDiskFreeSpaceEx( drive, NULL, NULL, &ulFreeAvail ) )
		return false;
	sprintf(size,"%6d MB free",(int)(ulFreeAvail.QuadPart/1048576));
	return true;
}

bool HelperX::getfreeDS(char* drive,int& size)
{
	ULARGE_INTEGER ulFreeAvail;
	if( !GetDiskFreeSpaceEx( drive, NULL, NULL, &ulFreeAvail ) )
		return false;
	size = int(ulFreeAvail.QuadPart/1048576);
	return true;
}

int HelperX::getfreeDSMB(char* drive)
{
	ULARGE_INTEGER ulFreeAvail;
	if( !GetDiskFreeSpaceEx( drive, NULL, NULL, &ulFreeAvail ) )
		return 0;
	return int(ulFreeAvail.QuadPart/1048576);
}


int HelperX::getusedDSul(char* drive)
{
	if(!dvdsize)
		dvdsize = int(CountDVDsize(drive)/1048576);
	return dvdsize;
}


int HelperX::getusedDSkb(char* path)
{
	if(GetFileAttributes(path)&FILE_ATTRIBUTE_DIRECTORY)
	{
		DPf_H("Found dir: %s",path);
		return int(CountDVDsize(path)/1024);
	} 
	DPf_H("Found file: %s",path);
	return GetFileSize (path, NULL);
}

bool HelperX::getusedDiskspace(char* drive,char* size)
{
	if(!dvdsize)
		dvdsize = int(CountDVDsize(drive)/1048576);
	sprintf(size,"%d MB",dvdsize);
	return true;
}

int HelperX::getFilesize(char* filename)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	LARGE_INTEGER liSize;
	LONGLONG llValue = 0;
	hFind = FindFirstFile( filename, &wfd );
	if( INVALID_HANDLE_VALUE == hFind )
	{
		llValue = 0;
	}
	else
	{
		liSize.LowPart = wfd.nFileSizeLow;
		liSize.HighPart = wfd.nFileSizeHigh;
		llValue = liSize.QuadPart/1024;
	}
	FindClose( hFind );
	return llValue;
}

char* HelperX::removeDrive(char* lpFileName,int c)
{
	if(!strncmp(lpFileName,"d:",2) || !strncmp(lpFileName,"e:",2) || !strncmp(lpFileName,"f:",2) || !strncmp(lpFileName,"g:",2))
		return lpFileName + c;
	else 
		return lpFileName;
}

LONGLONG HelperX::CountDVDsize(char *path)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	//char* sourcesearch = new char[1024];
	//char* sourcefile = new char[1024];
	LONGLONG llValue = 0;
	LONGLONG llResult = 0;
	LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				// Recursion
				llValue += CountDVDsize( sourcefile );
				//llResult += llValue;
			}
			else
			{
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					llValue += liSize.QuadPart;
				}
				
				//size += (wfd.nFileSizeHigh * (MAXDWORD+1)) + wfd.nFileSizeLow;
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	//delete sourcesearch;
	//sourcesearch = NULL;
	//delete sourcefile;
	//sourcefile = NULL;
	return llValue;
}

/*
bool HelperX::DelTree(char *path)
{
	//char sourcesearch[1024]="";
	//char sourcefile[1024]="";
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"\\*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
		RemoveDirectory( path );
	    return true;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,"\\");
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//strcat(sourcefile,"\\");
				// Recursion
				if(!DelTree( sourcefile ))
				{
					if(sourcesearch != NULL)
					{
						delete sourcesearch;
						sourcesearch = NULL;
					}
					if(sourcefile != NULL)
					{
						delete sourcefile;
						sourcefile = NULL;
					}
					return false;
				}
				DPf_H("Called deltree with: %hs",sourcefile);
				//if(!RemoveDirectory(sourcefile))
				//	return false;
				DPf_H("Called removedir with: %hs",sourcefile);
			
			}
			else
			{
				// delete file
				DPf_H("Called delfile with: %hs",sourcefile);
				if(!DeleteFile(sourcefile))
					return false;
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	RemoveDirectory( path );
	if(sourcesearch != NULL)
	{
		delete sourcesearch;
		sourcesearch = NULL;
	}
	if(sourcefile != NULL)
	{
		delete sourcefile;
		sourcefile = NULL;
	}
	return true;
}*/

void HelperX::LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters)
{
	CIoSupport helper;
	char temp[1024];
	if(!_strnicmp(szPath,"e:",2))
	{
		strcpy(temp,"Harddisk0\\Partition1");
		szPath+=2;
		strcat(temp,szPath);
	} else if(!_strnicmp(szPath,"f:",2))
	{
		strcpy(temp,"Harddisk0\\Partition6");
		szPath+=2;
		strcat(temp,szPath);
	} else if(!_strnicmp(szPath,"g:",2))
	{
		strcpy(temp,"Harddisk0\\Partition7");
		szPath+=2;
		strcat(temp,szPath);
	}
	helper.Unmount("D:");
	helper.Mount("D:",temp);

	DPf_H("Launching %s",temp);

	if (szParameters==NULL)
	{
		XLaunchNewImage(szXbe, NULL );
	}
	else
	{
		LAUNCH_DATA LaunchData;
		strcpy((CHAR*)LaunchData.Data,szParameters);

		XLaunchNewImage(szXbe, &LaunchData );
	}
}

/*
void HelperX::setLogFilename(char *file)
{
	strcpy(logFilename,file);
}


void HelperX::enableLog(bool value)
{
	writeLog=value;
}

void HelperX::WriteLog(WCHAR *message,...)
{
	//DPf_H("calling WriteLog %s",logFilename);
	if(logFilename == NULL)
		return;
	WCHAR expanded_message[1024];
	va_list tGlop;
	// Expand the varable argument list into the text buffer
	va_start(tGlop,message);
	if(vswprintf(expanded_message,message,tGlop)==-1)
	{
		// Fatal overflow, lets abort
		return;
	}
	va_end(tGlop);
	FILE *stream;
	char mchar[1024];
	if(!(stream = fopen(HelperX::logFilename,"a+")))
		return;
	wsprintf(mchar,"%S\n",expanded_message);
	fwrite(mchar,sizeof(char),strlen(mchar),stream);
	fclose(stream);
}
*/

bool HelperX::isdriveD(char* path)
{
	if(!_strnicmp(path,"d:",2))
		return true;
	else
		return false;
}

int HelperX::getXBETitle(char* file,WCHAR* m_GameTitle)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	wcscpy(m_GameTitle,L"");
	char *reverse = new char[strlen(file)+1];
	strcpy(reverse,file);
	if(_strnicmp(_strrev(reverse),"ebx.",4))
		return 0;
	delete[] reverse;
	stream  = fopen( file, "rb" );
	if(stream == NULL) {
	} else {
	
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,1,sizeof(HC),stream);
		fclose(stream);
		wcscpy(m_GameTitle, HC.TitleName);
		return 1;
	}	
    return 0;
}

void HelperX::addSlash(char* dest, char* source)
{
	if(source[strlen(source)-1] == '\\')
		strcpy(dest,source);
	else
	{
		strcpy(dest,source);
		strcat(dest,"\\");
	}	
}

void HelperX::addSlash(char* source)
{
	if(source[strlen(source)-1] != '\\')
		strcat(source,"\\");
}


/////////////////////////////////////////////////////////////////////////


void DPf_H(const char* pzFormat, ...)
{
  // WiSo: Comment in if you want debugging information 

  //char buf[512];

  //va_list arg;

  //va_start( arg, pzFormat );

  //vsprintf( buf, pzFormat, arg );
  //strcat(buf,"\n");
  //FILE *stream;
  //stream = fopen( "f:\\debug.txt", "a");
  //fputs(buf,stream);
  //fclose( stream );

  //va_end( arg );

}
