#include "d2xdrivestatus.h"


D2Xdstatus::D2Xdstatus()
{
	m_dwLastTrayState=0;
}

D2Xdstatus::~D2Xdstatus()
{
}


void D2Xdstatus::GetDriveState(WCHAR *m_scdstat,int& type)
{
	DWORD m_cdstat = GetTrayState();

	WCHAR temp[40];
	switch(m_cdstat)
	{
		case DRIVE_OPEN:
 			wcscpy(m_scdstat,L"DVD: Tray Open"); 
			type = 0;
 			break;
 		case DRIVE_NOT_READY:
			type = 0;
 			wcscpy(m_scdstat,L"DVD: Drive Init");
 			break;
 		case DRIVE_CLOSED_NO_MEDIA:
 			wcscpy(m_scdstat,L"DVD: No Disc");
 			break;
		case DRIVE_READY:
 			break;
 		case DRIVE_CLOSED_MEDIA_PRESENT:
			{
			
		
			if(!type)
			{
				HelperX p_help;
				HelperX::dvdsize = 0;
				m_IO.Remount("D:","Cdrom0");
				if (_access("D:\\default.xbe",00)!=-1)
				{
					type = GAME;
					dvdsize = p_help.getusedDSul("D:\\");
	
					wsprintfW(temp,L"DVD: XBOX Software %d MB",(int)dvdsize);
				} else if(_access("D:\\VIDEO_TS",00)!=-1)
				{
					type = DVD;
					dvd_reader_t*	dvd;
					dvd = DVDOpen("\\Device\\Cdrom0");
					dvdsize = p_help.getusedDSul("D:\\");
				
					DVDClose(dvd);
					wsprintfW(temp,L"DVD: Video %d MB",(int)dvdsize);
				} else 
				{
					CCdIoSupport cdio;
					CCdInfo*		m_pCdInfo;
					//	Detect new CD-Information
					m_pCdInfo = cdio.GetCdInfo();
					if(m_pCdInfo->IsISOUDF(1) || m_pCdInfo->IsISOHFS(1) || m_pCdInfo->IsIso9660( 1 ) || m_pCdInfo->IsIso9660Interactive( 1 ) )
					{
						iso9660 m_pIsoReader; 
						HANDLE fd;
						if((fd=m_pIsoReader.OpenFile("\\VCD\\ENTRIES.VCD"))!=INVALID_HANDLE_VALUE)
						{
							type = VCD;
							wsprintfW(temp,L"DVD: VCD");
							m_pIsoReader.CloseFile();
						} else if((fd=m_pIsoReader.OpenFile("\\SVCD\\ENTRIES.SVD"))!=INVALID_HANDLE_VALUE)
						{
							type = SVCD;
							wsprintfW(temp,L"DVD: SVCD");
							m_pIsoReader.CloseFile();
 						} else 
						{
							type = ISO;
							wsprintfW(temp,L"DVD: ISO");
						
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