#include "d2xdrivestatus.h"

CCdInfo*	D2Xdstatus::m_pCdInfo = NULL;

D2Xdstatus::D2Xdstatus()
{
	//p_help = new HelperX;
}

D2Xdstatus::~D2Xdstatus()
{
	//delete p_help;
}


void D2Xdstatus::GetDriveState(WCHAR *m_scdstat,int& type)
{
	DWORD m_cdstat = m_IO.GetTrayState();

	WCHAR temp[40];
	switch(m_cdstat)
	{
		case TRAY_OPEN:
 			wcscpy(m_scdstat,L"DVD: Tray Open");
			type = 0;
 			break;
 		case DRIVE_NOT_READY:
			type = 0;
 			wcscpy(m_scdstat,L"DVD: Drive Init");
 			break;
 		case TRAY_CLOSED_NO_MEDIA:
 			wcscpy(m_scdstat,L"DVD: No Disc");
 			break;
 		case TRAY_CLOSED_MEDIA_PRESENT:
			{
			
		
			if(!type)
			{
				HelperX* p_help;
				p_help = new HelperX();
				HelperX::dvdsize = 0;
				m_IO.Remount("D:","Cdrom0");
				if (_access("D:\\default.xbe",00)!=-1)
				{
					type = GAME;
					//DPf_H("in dvd before count");
					dvdsize = p_help->getusedDSul("D:\\");
	
					wsprintfW(temp,L"DVD: XBOX Software %d MB",(int)dvdsize);
				} else if(_access("D:\\VIDEO_TS",00)!=-1)
				{
					type = DVD;
					dvd_reader_t*	dvd;
					dvd = DVDOpen("\\Device\\Cdrom0");
					dvdsize = p_help->getusedDSul("D:\\");
				
					DVDClose(dvd);
					wsprintfW(temp,L"DVD: Video %d MB",(int)dvdsize);
				} else 
				{
					CCdIoSupport* cdio;
					cdio = new CCdIoSupport();
					//	Delete old CD-Information
					if ( m_pCdInfo != NULL ) {
						delete m_pCdInfo;
						m_pCdInfo = NULL;
					}
					//	Detect new CD-Information
					m_pCdInfo = cdio->GetCdInfo();
					if( m_pCdInfo->IsIso9660( 1 ) || m_pCdInfo->IsIso9660Interactive( 1 ) )
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
						if(m_pIsoReader != NULL)
						{
							delete m_pIsoReader;
							m_pIsoReader = NULL;
						}
 					} else if(m_pCdInfo->IsAudio( 1 )) 
					{
						type = CDDA;
						wsprintfW(temp,L"DVD: Audio CD");
					
					} else
					{
						type = UNKNOWN;
						wsprintfW(temp,L"DVD: unknown");
					}
					if(cdio != NULL)
					{
						delete cdio;
						cdio = NULL;
					}
				}
				if(p_help != NULL)
				{
					delete p_help;
					p_help = NULL;
				}
				wcscpy(m_scdstat,temp);
				m_IO.Remount("D:","Cdrom0");
			}
			}
			break;
		default:
			type = 0;
			wcscpy(m_scdstat,L"DVD: Drive Init");
	}
	
}

LONGLONG D2Xdstatus::CountDVDsize(char *path)
{
	//char sourcesearch[1024]="";
	//char sourcefile[1024]="";
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
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
	    llValue = 0;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);
			//DPf_H("Found %s",sourcefile);

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
	delete sourcesearch;
	sourcesearch = NULL;
	delete sourcefile;
	sourcefile = NULL;
	return llValue;
}
