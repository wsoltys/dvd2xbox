#include "d2xdrivestatus.h"

LONGLONG D2Xdstatus::dvdsize=0;

D2Xdstatus::D2Xdstatus()
{
	m_dwLastTrayState=0;
}

D2Xdstatus::~D2Xdstatus()
{
}


void D2Xdstatus::GetDriveState(WCHAR *m_scdstat,int& type)
{
	if(g_d2xSettings.detect_media_change)
	{
		DWORD m_cdstat = GetTrayState();
		switch(m_cdstat)
		{
			case DRIVE_OPEN:
 				wcscpy(m_scdstat,L"DVD: Tray Open"); 
				type = 0;
				D2Xdstatus::dvdsize = 0;
 				break;
 			case DRIVE_NOT_READY:
				type = 0;
				D2Xdstatus::dvdsize = 0;
 				wcscpy(m_scdstat,L"DVD: Drive Init");
 				break;
 			case DRIVE_CLOSED_NO_MEDIA:
 				wcscpy(m_scdstat,L"DVD: No Disc");
 				break;
			case DRIVE_READY:
 				break;
 			case DRIVE_CLOSED_MEDIA_PRESENT:
						
				if(!type)
					DetectMedia(m_scdstat,type);

				break;
			default:
				type = 0;
				D2Xdstatus::dvdsize = 0;
				wcscpy(m_scdstat,L"DVD: Drive Init");
		}
	}
	else if(g_d2xSettings.detect_media)
	{
		DetectMedia(m_scdstat,type);
		g_d2xSettings.detect_media = 0;
	}
	
}

DWORD D2Xdstatus::GetTrayState()
{
	HalReadSMCTrayState(&m_dwTrayState,&m_dwTrayCount);

	if(m_dwTrayState == TRAY_CLOSED_MEDIA_PRESENT) 
	{
		if (m_dwLastTrayState != TRAY_CLOSED_MEDIA_PRESENT)
		{
			m_dwLastTrayState = m_dwTrayState;
			return DRIVE_CLOSED_MEDIA_PRESENT;
		}
		else
		{
			return DRIVE_READY;
		}
	} 
	else if(m_dwTrayState == TRAY_CLOSED_NO_MEDIA)
	{
		if (m_dwLastTrayState != TRAY_CLOSED_NO_MEDIA)
		{
			m_dwLastTrayState = m_dwTrayState;
			return DRIVE_CLOSED_NO_MEDIA;
		}
		else
		{
			return DRIVE_READY;
		}
	}
	else if(m_dwTrayState == TRAY_OPEN)
	{
		if (m_dwLastTrayState != TRAY_OPEN)
		{
			m_dwLastTrayState = m_dwTrayState;
			return DRIVE_OPEN;
		}
		else
		{
			return DRIVE_READY;
		}
	}
	else
	{
		m_dwLastTrayState = m_dwTrayState;
	}

	return DRIVE_NOT_READY;
}

void D2Xdstatus::DetectMedia(WCHAR *m_scdstat,int& type)
{
	WCHAR temp[40];
	D2Xdstatus::dvdsize = 0;
	m_IO.Remount("D:","Cdrom0");
	if (_access("D:\\default.xbe",00)!=-1)
	{
		type = GAME;
		dvdsize = countMB("D:\\");

		wsprintfW(temp,L"DVD: XBOX Software %d MB",(int)dvdsize);
	} else if(_access("D:\\VIDEO_TS",00)!=-1)
	{
		type = DVD;
		//dvd_reader_t*	dvd;
		//dvd = DVDOpen("\\Device\\Cdrom0");
		D2Xff factory;
		p_file = factory.Create(DVD);
		p_file->FileOpenRead("D:\\VIDEO_TS\\VIDEO_TS.VOB");
		dvdsize = countMB("D:\\");
		p_file->FileClose();
		delete p_file;
		p_file = NULL;
		//DVDClose(dvd);
		wsprintfW(temp,L"DVD: Video %d MB",(int)dvdsize);
	} else 
	{
		CCdIoSupport cdio;
		CCdInfo*		m_pCdInfo;
		//	Detect new CD-Information
		m_pCdInfo = cdio.GetCdInfo();
		if(m_pCdInfo->IsISOUDF(1) || m_pCdInfo->IsISOHFS(1) || m_pCdInfo->IsIso9660( 1 ) || m_pCdInfo->IsIso9660Interactive( 1 ) )
		{
			//iso9660 m_pIsoReader; 
			//HANDLE fd;
			D2Xff factory;
			p_file = factory.Create(ISO);
			//if((fd=m_pIsoReader.OpenFile("\\VCD\\ENTRIES.VCD"))!=INVALID_HANDLE_VALUE)
			if(p_file->FileOpenRead("\\VCD\\ENTRIES.VCD"))
			{
				type = VCD;
				wsprintfW(temp,L"DVD: VCD");
				p_file->FileClose();
				//m_pIsoReader.CloseFile();
			//} else if((fd=m_pIsoReader.OpenFile("\\SVCD\\ENTRIES.SVD"))!=INVALID_HANDLE_VALUE)
			} else if(p_file->FileOpenRead("\\SVCD\\ENTRIES.SVD"))
			{
				type = SVCD;
				wsprintfW(temp,L"DVD: SVCD");
				p_file->FileClose();
				//m_pIsoReader.CloseFile();
 			} else 
			{
				type = ISO;
				wsprintfW(temp,L"DVD: ISO");
			
			}
			delete p_file;
			p_file = NULL;
 		} else if(m_pCdInfo->IsAudio( 1 )) 
		{
			type = CDDA;
			wsprintfW(temp,L"DVD: Audio CD");
		
		} else
		{
			type = UNKNOWN_;
			wsprintfW(temp,L"DVD: unknown");
		}
	}
	wcscpy(m_scdstat,temp);
	m_IO.Remount("D:","Cdrom0");
}

// count used disk space
int D2Xdstatus::countMB(char* drive)
{
	if(!D2Xdstatus::dvdsize)
		D2Xdstatus::dvdsize = CountDVDsize(drive)/1048576;
	return int(D2Xdstatus::dvdsize);
}

LONGLONG D2Xdstatus::CountDVDsize(char *path)
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
				llValue += CountDVDsize( sourcefile );
			}
			else
			{
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					llValue += liSize.QuadPart;
				}
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}

	return llValue;
}