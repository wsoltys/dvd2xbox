#include "d2xdrivestatus.h"

LONGLONG D2Xdstatus::dvdsize=0;
WCHAR D2Xdstatus::m_scdstat[128];
DWORD D2Xdstatus::mediaReady;
int	D2Xdstatus::type;
static CRITICAL_SECTION m_criticalSection;

D2Xdstatus::D2Xdstatus()
{
	m_dwLastTrayState=0;
	m_pCdInfo = NULL;
	type = 0;
	wsprintfW(m_scdstat,L"DVD: unknown");
	mediaReady = 0;
	InitializeCriticalSection(&m_criticalSection);
}

D2Xdstatus::~D2Xdstatus()
{
	DeleteCriticalSection(&m_criticalSection);
}

void D2Xdstatus::OnStartup()
{
	DebugOut("Starting drive status thread\n");
}

void D2Xdstatus::Process()
{
	while(!m_bStop && g_d2xSettings.detect_media_change)
	{
		Sleep(500);
		D2Xdstatus::GetDriveState();
	}
}

void D2Xdstatus::OnExit()
{
	DebugOut("Stopping drive status thread\n");
}

void D2Xdstatus::GetDriveState(WCHAR *w_scdstat,int& itype)
{
	EnterCriticalSection(&m_criticalSection);
	itype = type;
	wcscpy(w_scdstat,m_scdstat);
	LeaveCriticalSection(&m_criticalSection);
}

//void D2Xdstatus::GetDriveState(WCHAR *m_scdstat,int& type)
void D2Xdstatus::GetDriveState()
{
	//if(g_d2xSettings.detect_media_change)
	{
		DWORD m_cdstat = GetTrayState();
		switch(m_cdstat)
		{
			case DRIVE_OPEN:
				EnterCriticalSection(&m_criticalSection);
 				wcscpy(m_scdstat,L"DVD: Tray Open"); 
				mediaReady = DRIVE_OPEN;
				type = 0;
				D2Xdstatus::dvdsize = 0;
				LeaveCriticalSection(&m_criticalSection);
				p_ml.UnloadTexture("DVDxbeIcon");
 				break;
 			case DRIVE_NOT_READY:
				EnterCriticalSection(&m_criticalSection);
				mediaReady = DRIVE_NOT_READY;
				type = 0;
				D2Xdstatus::dvdsize = 0;
 				wcscpy(m_scdstat,L"DVD: Drive Init");
				LeaveCriticalSection(&m_criticalSection);

				p_ml.UnloadTexture("DVDxbeIcon");

 				break;
 			case DRIVE_CLOSED_NO_MEDIA:
				EnterCriticalSection(&m_criticalSection);
				mediaReady = DRIVE_CLOSED_NO_MEDIA;
 				wcscpy(m_scdstat,L"DVD: No Disc");
				LeaveCriticalSection(&m_criticalSection);
 				break;
			case DRIVE_READY:
 				break;
 			case DRIVE_CLOSED_MEDIA_PRESENT:
						
				if(!type)
					//DetectMedia(m_scdstat,type);
					DetectMedia();

				break;
			default:
				EnterCriticalSection(&m_criticalSection);
				mediaReady = DRIVE_NOT_READY;
				type = 0;
				D2Xdstatus::dvdsize = 0;
				wcscpy(m_scdstat,L"DVD: Drive Init");
				LeaveCriticalSection(&m_criticalSection);
				p_ml.UnloadTexture("DVDxbeIcon");
		}
	}
	/*else if(g_d2xSettings.detect_media)
	{
		DetectMedia(m_scdstat,type);
		g_d2xSettings.detect_media = 0;
	}*/
	
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
		if ((m_dwLastTrayState != TRAY_CLOSED_NO_MEDIA) && (m_dwLastTrayState != TRAY_CLOSED_MEDIA_PRESENT))
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

//void D2Xdstatus::DetectMedia(WCHAR *m_scdstat,int& type)
void D2Xdstatus::DetectMedia()
{
	WCHAR temp[40];
	int ttype = 0;
	D2Xdstatus::dvdsize = 0;
	m_IO.Remount("D:","Cdrom0");
	if(g_d2xSettings.enableUnlocker && p_u.Unlock()>0)
	{
		DebugOut("Seems to be a XBOX DVD\n");
		m_IO.Remount("D:","Cdrom0");
	}

	if (_access("D:\\default.xbe",00)!=-1)
	{
		ttype = GAME;
		g_d2xSettings.detected_media = GAME;
		dvdsize = countMB("D:\\");
		wsprintfW(temp,L"DVD: XBOX Software %d MB",(int)dvdsize);

		p_ml.LoadXBEIcon("D:\\default.xbe","DVDxbeIcon");

	} 
	else if(_access("D:\\VIDEO_TS",00)!=-1)
	{
		ttype = DVD;
		g_d2xSettings.detected_media = DVD;
		D2Xff factory;
		p_file = factory.Create(DVD);
		int i_ret = p_file->FileOpenRead("D:\\VIDEO_TS\\VIDEO_TS.VOB");
		dvdsize = countMB("D:\\");
		if(i_ret)
			p_file->FileClose();
		delete p_file;
		p_file = NULL;
		wsprintfW(temp,L"DVD: Video %d MB",(int)dvdsize);
	} 
	else 
	{
		CCdIoSupport cdio;

		if ( m_pCdInfo != NULL ) 
		{
			delete m_pCdInfo;
			m_pCdInfo = NULL;
		}
		//	Detect new CD-Information
		m_pCdInfo = cdio.GetCdInfo();
		if( m_pCdInfo->IsISOHFS(1) || m_pCdInfo->IsIso9660(1) || m_pCdInfo->IsIso9660Interactive(1) )
		{
			D2Xff factory;
			p_file = factory.Create(ISO);
			if(p_file->FileOpenRead("\\VCD\\ENTRIES.VCD"))
			{
				ttype = VCD;
				g_d2xSettings.detected_media = VCD;
				wsprintfW(temp,L"DVD: VCD");
				p_file->FileClose();
			} else if(p_file->FileOpenRead("\\SVCD\\ENTRIES.SVD"))
			{
				ttype = SVCD;
				g_d2xSettings.detected_media = SVCD;
				wsprintfW(temp,L"DVD: SVCD");
				p_file->FileClose();
 			} else 
			{
				ttype = ISO;
				g_d2xSettings.detected_media = ISO;
				wsprintfW(temp,L"DVD: ISO");
			
			}
			delete p_file;
			p_file = NULL;
 		} 
		else if(m_pCdInfo->IsUDF(1) || m_pCdInfo->IsUDFX(1) || m_pCdInfo->IsISOUDF(1))
		{
			ttype = UDF;
			g_d2xSettings.detected_media = UDF;
			dvdsize = countMB("D:\\");
			wsprintfW(temp,L"DVD: UDF %d MB",(int)dvdsize);
		} 
		else if(m_pCdInfo->IsAudio( 1 )) 
		{
			ttype = CDDA;
			g_d2xSettings.detected_media = CDDA;
			wsprintfW(temp,L"DVD: Audio CD");
		
		} 
		else
		{
			ttype = UNKNOWN_;
			wsprintfW(temp,L"DVD: unknown");
		}
	}
	EnterCriticalSection(&m_criticalSection);
	wcscpy(m_scdstat,temp);
	type = ttype;
	mediaReady = DRIVE_CLOSED_MEDIA_PRESENT;
	LeaveCriticalSection(&m_criticalSection);
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

DWORD D2Xdstatus::getMediaStatus()
{
	return mediaReady;
}