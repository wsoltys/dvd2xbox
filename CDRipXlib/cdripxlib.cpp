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



#include <algorithm>
#include "cdripxlib.h"
#include "..\cdrip\cdrip.h"
#include "TCHAR.h"


// CCDRipX
CCDRipX::CCDRipX()
{
	m_init=false;
	//filename = NULL;
}

CCDRipX::~CCDRipX()
{
	Stop();
}

HRESULT CCDRipX::DeInit()
{
	if ( CDEX_OK != CR_DeInit() ) {
		DPf("CR_DeInit failed");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CCDRipX::Init()
{
	if ( CDEX_OK != CR_Init( "test.ini" ) ) {
		DPf("CR_Init failed");
		return E_FAIL;
	}
	DPf("CR_Init ok");
	m_nNumTracks = 0;
	DPf("num tracks ok");
	return S_OK;
}

HRESULT CCDRipX::playTrack(int nTrack)
{
	// Initialize to default values
	nBufferSize=0;
	nNumBytesRead=0;
	nTotalBytes=0;
	if(Ripperinit(nTrack))
		return E_FAIL;
	if(Playerinit())
		return E_FAIL;
	m_init = true;
	return S_OK;
}


int CCDRipX::Ripperinit(int ntrack)
{

	CR_ReadToc();
	DPf("Track %d,Sectors %d, DiscID %08x",CR_GetTocEntry(ntrack-1).btTrackNumber,CR_GetTocEntry(ntrack).dwStartSector-1-CR_GetTocEntry(ntrack-1).dwStartSector,CalculateDiscID());

	// Open ripper
	if (CR_OpenRipper(	&nBufferSize,
						CR_GetTocEntry(ntrack-1).dwStartSector,
						CR_GetTocEntry(ntrack).dwStartSector-1
						)!=CDEX_OK) {
							DPf("Open Ripper failed");
							return 1;
						}
	return 0;
}



int CCDRipX::Playerinit()
{
	DPf("Enter Playerinit");
	for(int i=0;i<WAVSTRM_PACKET_COUNT;i++) {
        m_pvSourceBuffer[i] = new BYTE[ nBufferSize ];
	}
    if( NULL == m_pvSourceBuffer[1] )
	{
        //DPf("could not allocate source buffer");
		return 1;
	}

	// Initialize packet status array
    ZeroMemory( m_adwStatus, sizeof(m_adwStatus) );
	m_hrOpenResult      = E_PENDING;
	DPf("leave Playerinit");
	return 0;
}

int CCDRipX::CreateStream()
{
	// Create an appropriate WAVEFORMATEX struct
    XAudioCreatePcmFormat( (WORD)2, (DWORD)44100,
		                   (WORD)16, &m_wfxSourceFormat );
	// Create the stream
    DSSTREAMDESC   dssd;
    ZeroMemory( &dssd, sizeof( dssd ) );
    dssd.dwMaxAttachedPackets = (DWORD)2;
    dssd.lpwfxFormat          = &m_wfxSourceFormat;

	hr = DirectSoundCreateStream( &dssd, &m_pDestXMO );
    if( FAILED(hr) )
    {
        //mDebug->Message(L"Could not create DS stream");
		m_hrOpenResult = E_FAIL;
        return 1;
    }
	m_hrOpenResult = S_OK;
	//mDebug->Message(L"create stream ok");
	return 0;
}

HRESULT CCDRipX::ProcessSound( DWORD dwPacketIndex )
{
	XMEDIAPACKET xmp = {0};
	xmp.pvBuffer     = (BYTE*)m_pvSourceBuffer [dwPacketIndex];
	xmp.dwMaxSize    = nNumBytesRead;
	xmp.pdwStatus    = &m_adwStatus[ dwPacketIndex ];

	// Submit for playback
	return m_pDestXMO->Process( &xmp, NULL );
}

BOOL CCDRipX::FindFreePacket( DWORD* pdwPacketIndex )
{
    for( DWORD dwPacketIndex = 0; dwPacketIndex < 2; dwPacketIndex++ )
    {
        if( XMEDIAPACKET_STATUS_PENDING != m_adwStatus[ dwPacketIndex ] )
        {
            if( pdwPacketIndex )
                (*pdwPacketIndex) = dwPacketIndex;

            return TRUE;
        }
    }

    return FALSE;
}


int CCDRipX::Process(DWORD* pTimeplayed /* =NULL */)
{
	DWORD   dwPacketIndex;
    HRESULT hr;
	BOOL bAbort=false;

	// if init == false, we don't have anything to do
	if(!m_init)
		return 0;

	if(m_hrOpenResult == E_PENDING)
	{
		DPf("Create new stream");
		if( CreateStream()  ) {
			//mDebug->Message(L"create stream failed");
            return 1;
		}
		m_hrOpenResult = S_OK;
	}
	// Find a free packet, re-fill with decoded data and submit to the stream.  
    // If there are no free packets, we don't have anything to do.
    while( FindFreePacket( &dwPacketIndex ) )
    {
	
		CDEX_ERR ripErr;


		if(( CDEX_RIPPING_DONE != (ripErr=CR_RipChunk(m_pvSourceBuffer[dwPacketIndex],&nNumBytesRead,bAbort))) )
		{
			if(CDEX_ERROR == ripErr) {
				DPf("CDEX_ERROR");
				break;
			}
			nTotalBytes+=nNumBytesRead;
		} else {
			Stop();
			return CDRIPX_DONE;
		}
		
        // Send the data to the destination XMO (DirectSound Stream)
        hr = ProcessSound ( dwPacketIndex );
        if( FAILED(hr) )
            return 1;
		if(pTimeplayed) 
		{
            (*pTimeplayed) = DWORD (nTotalBytes/(TRACKSPERSEC*CB_CDDASECTOR));
		}
    }
	return CDRIPX_OK;
}

void CCDRipX::Stop()
{
	if(!m_init)
		return;
	if(m_pDestXMO)
        m_pDestXMO->Discontinuity();
    CR_CloseRipper();
	if( m_pDestXMO )
        m_pDestXMO->Release();
	if(m_pvSourceBuffer!=NULL) {
		for(int i=0;i<WAVSTRM_PACKET_COUNT;i++) {
			delete[] m_pvSourceBuffer[i];
			m_pvSourceBuffer[i]=NULL;
		}
		DPf("m_pvSourceBuffer deleted");
	}
	m_init = false;
}

void CCDRipX::Pause(DWORD dwPause)
{
	if(!m_init)
		return;
	m_pDestXMO->Pause( dwPause );
}


int CCDRipX::GetNumTocEntries()
{
	if(!m_nNumTracks) 
	{
		if(E_FAIL==pGetTrackInfo())
			DPf("pGetTrackInfo failed");
	}
	return m_nNumTracks;
}

cdtoc CCDRipX::GetTrackInfo(int nTrack)
{
	return CDCon[nTrack-1];
}

HRESULT CCDRipX::pGetTrackInfo()
{
	LONG ntFra =0;
	FLOAT tdur;
	CR_ReadToc();
	m_nNumTracks =CR_GetNumTocEntries();
	if(m_nNumTracks<1)
		return E_FAIL;
	for (int i = 0; i < m_nNumTracks; i++) 
	{
		TOCENTRY TocEntry = CR_GetTocEntry(i);
		TOCENTRY TocEntry2 = CR_GetTocEntry(i+1);

		FLOAT dur = (float)(TocEntry.dwStartSector+150)/60/75;
		oCDCon[i].min = (int) dur;
		oCDCon[i].sec = (int) ((dur - oCDCon[i].min)*60);
		oCDCon[i].frame = (int) ((((dur - oCDCon[i].min)*60)-oCDCon[i].sec)*75);
		DPf("Track %2d *** Duration %d:%d.%d ",i+1,oCDCon[i].min,oCDCon[i].sec,oCDCon[i].frame);
		DPf("          *** Frames %d ",(oCDCon[i].min*60*75)+(oCDCon[i].sec*75)+oCDCon[i].frame);
		


		LONG sectors = TocEntry2.dwStartSector - TocEntry.dwStartSector;
		dur = (float)sectors/60/75;
		CDCon[i].min = (int) dur;
		CDCon[i].sec = (int) ((dur - CDCon[i].min)*60);
		CDCon[i].frame = (int) ((((dur - CDCon[i].min)*60)-CDCon[i].sec)*75);
		ntFra+=sectors;
	}
	
	tdur = (float)ntFra/60/75;
	CDCon[i+1].min = (int) tdur;
	CDCon[i+1].sec = (int) ((tdur - CDCon[i+1].min)*60);
	CDCon[i+1].frame = (int) ((((tdur - CDCon[i+1].min)*60)-CDCon[i+1].sec)*75);
	
	tdur = (float)(ntFra+150)/60/75;
	oCDCon[i].min = (int) tdur;
	oCDCon[i].sec = (int) ((tdur - oCDCon[i].min)*60);
	oCDCon[i].frame = (int) ((((tdur - oCDCon[i].min)*60)-oCDCon[i].sec)*75);
	DPf("Total min %d sek %d frames %d",oCDCon[i].min,oCDCon[i].sec,oCDCon[i].frame);

	return S_OK;
}



//////////////////////////////////////
// CDDB (still not complete)


int cddb_sum(int n)
{
	char	buf[12],*p;
	int	ret = 0;

	// For backward compatibility this algorithm must not change
	sprintf(buf, "%lu", n);
	for (p = buf; *p != '\0'; p++)
		ret += (*p - '0');

	return (ret);
}

/*
int cddb_sum(int n)
{
	int	ret;

	// For backward compatibility this algorithm must not change 

	ret = 0;

	while (n > 0) {
		ret = ret + (n % 10);
		n = n / 10;
	}

	return (ret);
}
*/

unsigned long CCDRipX::CalculateDiscID()
{
	int t = 0;
	int n = 0;
	
	DWORD	dwTwoSecOffset=0;
	int  m_nNumTracks =CR_GetNumTocEntries();

	//m_dwTotalSecs=0;

	// For backward compatibility this algorithm must not change
	dwTwoSecOffset=0;

	for (int i = 0; i < m_nNumTracks; i++) 
	{

		TOCENTRY TocEntry = CR_GetTocEntry(i);
		TOCENTRY TocEntry2 = CR_GetTocEntry(i+1);


		// org


		//n = n + cddb_sum((CDCon[i].min * 60) + CDCon[i].sec);
		
		
		
		// Keep in mind the two seconds offset
		//DWORD dwSectors = m_TrackInfo[i].GetStartSector()+dwTwoSecOffset;
		LONG dwSectors = TocEntry.dwStartSector + dwTwoSecOffset;
		

		n += cddb_sum(dwSectors/TRACKSPERSEC);

		// Keep in mind the two seconds offset
		//DWORD dwSectorsNext = m_TrackInfo[i+1].GetStartSector()+dwTwoSecOffset;
		LONG dwSectorsNext = TocEntry2.dwStartSector + dwTwoSecOffset;
		

		t += (dwSectorsNext/TRACKSPERSEC-dwSectors/TRACKSPERSEC);
	}

	
	//t = ((ntMin * 60) + ntSek) - (( CDCon[0].min * 60) + CDCon[0].sec);

	//dwRet=( (n % 0xff) << 24 | t << 8 | (DWORD)(m_nNumTracks));

	// Get total playing time
	//m_dwTotalSecs=(GetEndSector(m_nNumTracks)+1+150)/75;
	
	return ((n % 0xff) << 24 | t << 8 | (DWORD)m_nNumTracks);
}

///////////////////////////////
// Debug

void DPf(const char* pzFormat, ...)
{
  // WiSo: Comment in if you want debugging information
  /*
  char buf[512];
  va_list arg;

  va_start( arg, pzFormat );

  vsprintf( buf, pzFormat, arg );
  strcat(buf,"\n");
  FILE *stream;
  stream = fopen( "f:\\apps\\wisotest\\debug2.txt", "a");
  fputs(buf,stream);
  fclose( stream );

  va_end( arg );
	*/
}







#ifdef _WITHENC
////////////////////////////////////////////////////////////////////////
// Encoder


void GetCDRipInfo(int& nSampleFreq,int& nChannels,int& nBitsPerChannel)
{
	// Initialize to default values
	nSampleFreq=44100;
	nChannels=2;
	nBitsPerChannel=16;
}

int CCDRipX::InitOgg(int ntrack,char* file,float bq)
{
	if (Ripperinit(ntrack) ) {
		DPf("Cannot openripper Ogg");
		return CDRIPX_ERR;
	}
	DPf("Ripperinit ok");
	/*if(filename != NULL)
		delete[] filename;
	filename = new char[strlen(file)+1];*/
	strcpy(filename,file);
	base_quality = bq;
	pbtStream=new BYTE[nBufferSize];
	//enc = new CCDEnc();
	vorbis_comment_init(&vc);
	DPf("InitOgg ok");
	rip_in_progress = false;
	return CDRIPX_OK;
}

int CCDRipX::AddOggComment(char* key,char* value)
{
	vorbis_comment_add_tag(&vc,key,value);
	return CDRIPX_OK;
}


int CCDRipX::RipToOgg(		int&			nPercent,
							int&			nPeakValue,
							int&			nJitterErrors,
							int&			nJitterPos )
{
	if(rip_in_progress) {
		CDEX_ERR ripErr;
		// Initialize incoming paramters
		nPercent		= 0;
		nJitterErrors	= 0;
		nPeakValue		= 0;

		BOOL			bAbort=false;


		if(( CDEX_RIPPING_DONE != (ripErr=CR_RipChunk(pbtStream,&nNumBytesRead, bAbort )))) {
			
			// Check for jitter errors
			if ( CDEX_JITTER_ERROR == ripErr )
			{
				DWORD dwStartSector,dwEndSector;

				// Get info where jitter error did occur
				CR_GetLastJitterErrorPosition(dwStartSector,dwEndSector);

			}

			// Check if an error did occur
			if ( CDEX_ERROR == ripErr )
			{
				return CDRIPX_ERR;
			}


			enc.OggEnc(nNumBytesRead,pbtStream);

			// Get progress indication
			nPercent = CR_GetPercentCompleted();

			// Get relative jitter position
			nJitterPos = CR_GetJitterPosition();

			// Get the Peak Value
			nPeakValue = CR_GetPeakValue();
		
			// Get the number of jitter errors
			nJitterErrors = CR_GetNumberOfJitterErrors();
		} else {
			// Close the Ripper session
			CR_CloseRipper();

			enc.OggClose();
			vorbis_comment_clear(&vc);
			if(pbtStream)
                delete[] pbtStream;
			pbtStream=NULL;
			rip_in_progress=false;
			return CDRIPX_DONE;
		}

	} else {	
		if((enc.InitOgg(filename,base_quality,vc)) != CDRIPX_OK)
		{
			DPf("Init failed");
			return CDRIPX_ERR;
		}
		DPf("Ogg header wrote");
		rip_in_progress = true;
	}

	return CDRIPX_OK;
}

// Lame
int CCDRipX::InitLame(int ntrack,char* file)
{
	if (Ripperinit(ntrack) ) {
		DPf("Cannot openripper Lame");
		return CDRIPX_ERR;
	}
	DPf("Ripperinit ok");
	/*if(filename != NULL)
		delete[] filename;
	filename = new char[strlen(file)+1];*/
	//strcpy(filename,file);
	pbtStream=new BYTE[nBufferSize];
	//enc = new CCDEnc();
	DPf("InitLame ok");
	//rip_in_progress = false;
	if((enc.InitLame(file)) != CDRIPX_OK)
	{
		DPf("Init failed");
		return CDRIPX_ERR;
	}
	//rip_in_progress = true;
	return CDRIPX_OK;
}


void CCDRipX::AddLameTag(int key,const char* value)
{	
	enc.AddLameTag(key,value);
}


int CCDRipX::RipToLame(		int&			nPercent,
							int&			nPeakValue,
							int&			nJitterErrors,
							int&			nJitterPos )
{
	//if(rip_in_progress) {
		CDEX_ERR ripErr;
		// Initialize incoming paramters
		nPercent		= 0;
		nJitterErrors	= 0;
		nPeakValue		= 0;

		BOOL			bAbort=false;


		if(( CDEX_RIPPING_DONE != (ripErr=CR_RipChunk(pbtStream,&nNumBytesRead, bAbort )))) {
			
			// Check for jitter errors
			if ( CDEX_JITTER_ERROR == ripErr )
			{
				DWORD dwStartSector,dwEndSector;

				// Get info where jitter error did occur
				CR_GetLastJitterErrorPosition(dwStartSector,dwEndSector);

			}

			// Check if an error did occur
			if ( CDEX_ERROR == ripErr )
			{
				return CDRIPX_ERR;
			}


			enc.LameEnc(nNumBytesRead,pbtStream);

			// Get progress indication
			nPercent = CR_GetPercentCompleted();

			// Get relative jitter position
			nJitterPos = CR_GetJitterPosition();

			// Get the Peak Value
			nPeakValue = CR_GetPeakValue();
		
			// Get the number of jitter errors
			nJitterErrors = CR_GetNumberOfJitterErrors();
		} else {
			// Close the Ripper session
			CR_CloseRipper();

			enc.LameClose();
			if(pbtStream)
                delete[] pbtStream;
			pbtStream=NULL;
			rip_in_progress=false;
			return CDRIPX_DONE; 
		}

	/*} else {	
		if((enc.InitLame(filename)) != CDRIPX_OK)
		{
			DPf("Init failed");
			return CDRIPX_ERR;
		}
		rip_in_progress = true;
	}*/

	return CDRIPX_OK;
}

// Wav
int CCDRipX::InitWav(int ntrack,char* file)
{
	if (Ripperinit(ntrack) ) {
		DPf("Cannot openripper Lame");
		return CDRIPX_ERR;
	}
	DPf("Ripperinit ok");
	/*if(filename != NULL)
		delete[] filename;
	filename = new char[strlen(file)+1];*/
	//strcpy(filename,file);
	pbtStream=new BYTE[nBufferSize];
	//enc = new CCDEnc();
	DPf("InitLame ok");
	//rip_in_progress = false;
	if((enc.InitWav(file)) != CDRIPX_OK)
	{
		DPf("Init failed");
		return CDRIPX_ERR;
	}
	//rip_in_progress = true;
	return CDRIPX_OK;
}


int CCDRipX::RipToWav(		int&			nPercent,
							int&			nPeakValue,
							int&			nJitterErrors,
							int&			nJitterPos )
{
	//if(rip_in_progress) {
		CDEX_ERR ripErr;
		// Initialize incoming paramters
		nPercent		= 0;
		nJitterErrors	= 0;
		nPeakValue		= 0;

		BOOL			bAbort=false;


		if(( CDEX_RIPPING_DONE != (ripErr=CR_RipChunk(pbtStream,&nNumBytesRead, bAbort )))) {
			
			// Check for jitter errors
			if ( CDEX_JITTER_ERROR == ripErr )
			{
				DWORD dwStartSector,dwEndSector;

				// Get info where jitter error did occur
				CR_GetLastJitterErrorPosition(dwStartSector,dwEndSector);

			}

			// Check if an error did occur
			if ( CDEX_ERROR == ripErr )
			{
				return CDRIPX_ERR;
			}


			enc.WavEnc(nNumBytesRead,pbtStream);

			// Get progress indication
			nPercent = CR_GetPercentCompleted();

			// Get relative jitter position
			nJitterPos = CR_GetJitterPosition();

			// Get the Peak Value
			nPeakValue = CR_GetPeakValue();
		
			// Get the number of jitter errors
			nJitterErrors = CR_GetNumberOfJitterErrors();
		} else {
			// Close the Ripper session
			CR_CloseRipper();

			enc.WavClose();
			if(pbtStream)
                delete[] pbtStream;
			pbtStream=NULL;
			rip_in_progress=false;
			return CDRIPX_DONE; 
		}

	return CDRIPX_OK;
}

#endif