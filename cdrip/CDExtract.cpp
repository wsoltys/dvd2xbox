/*
** Copyright (C) 1999 Albert L. Faber
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

//#include "StdAfx.h"
#include <xtl.h>
#include <Stdio.h>
#include <Stdlib.h>
#include "vector"
#include "CDRip.h"

using namespace std ;

#include "CDExtract.h"
#include "AspiDebug.h"
#include <math.h>

static BOOL g_bParanoiaJitterErrors = FALSE;



SENSEKEY	g_SenseKey;


// CONSTRUCTOR
CCDExtract::CCDExtract()
:CAspiCD()
{
	m_nPeakValue			= 0;
	m_pbtReadBuffer			= NULL;
	m_pbtOverlapBuffer		= NULL;
	m_pbtReadBufferCompare	= NULL;
	m_nBeginSector			= 0;
	m_nEndSector			= 0;
	m_nJitterPos			= 50;
	m_nJitterErrors			= 0;
	m_lSector				= 0;
	m_dwBytesToDo			= 0;

	m_pParanoia				= NULL;
	m_pParanoiaDrive		= NULL;

	m_nCurrentSpeed			= 0;
	m_nLastSpeedAdjust		= -1;

	m_pbAbort				= NULL;
}


// DESRUCTOR
CCDExtract::~CCDExtract()
{
	// de-allocate memory buffers
	delete [] m_pbtReadBuffer; m_pbtReadBuffer=NULL;

	// de-allocate Multiple read memory buffers
	delete [] m_pbtReadBufferCompare;m_pbtReadBufferCompare=NULL;

	// de-allocate memory buffers
	delete [] m_pbtOverlapBuffer; m_pbtOverlapBuffer=NULL;

	delete m_pParanoiaDrive;
	//if ( m_pParanoia  ) paranoia_free( m_pParanoia );

	m_pParanoia = NULL;
	m_pParanoiaDrive = NULL;

}

BOOL CCDExtract::SetupTrackExtract(int nBeginSector,int nEndSector)
{
	DebugPrintf("Entering CCDExtract::SetupTrackExtract");

	// Reset current sector
	m_lSector=0;


	m_nCurrentSpeed			= GetSpeed();
	m_nLastSpeedAdjust		= -1;

	// Allocate memory for a read buffers
	m_pbtReadBuffer=new BYTE[GetNumReadSectors() * CB_CDDASECTOR];

	// Allocate memory for multi read buffers 
	m_pbtReadBufferCompare=new BYTE[GetNumReadSectors() * CB_CDDASECTOR];

	// Allocate memory for the two read buffers
	if (GetNumCompareSectors()>0)
		m_pbtOverlapBuffer=new BYTE[GetNumCompareSectors() * CB_CDDASECTOR];
	else
		m_pbtOverlapBuffer=NULL;

	// Rip at leat 10 sectors
	if (nBeginSector+10>nEndSector)
		nEndSector=nBeginSector+10;

	// Set starting point
	m_nBeginSector=max((long)nBeginSector+(long)GetOffsetStart(),0);

	// Set end point
	m_nEndSector=(long)nEndSector+(long)GetOffsetEnd();


	// Call to enable CDDA (needed for certain drives)
	EnableCdda(TRUE);

	// Don't allow a CD change while recording
	PreventMediaRemoval (TRUE);

	SetScsiTimeOut(10);
	int	nTimeout=GetScsiTimeOut();

	DebugPrintf("Timeout is set to %d",nTimeout);


	// Clear number of jitter errors
	m_nJitterErrors=0;

	// Clear peak value
	m_nPeakValue=0;

	// Set First Read = TRUE
	m_bFirstRead=TRUE;
	

	m_dwBytesToDo=CB_CDDASECTOR*(m_nEndSector-m_nBeginSector);

	DebugPrintf("Leaving CCDExtract::SetupTrackExtract");

	return TRUE;
}

/*
BOOL CCDExtract::SetupTrackExtractParanoia( int nBeginSector,int nEndSector )
{
	DebugPrintf("Entering CCDExtract::SetupTrackExtractParanoia");

	if (m_pParanoia == NULL) {
		// first time -> allocate paranoia structure 
		m_pParanoiaDrive = new cdrom_drive;
		m_pParanoiaDrive->cdr = this;
		m_pParanoiaDrive->nsectors = GetNumReadSectors();
		m_pParanoia = paranoia_init( m_pParanoiaDrive );

	}

	int   nParanoiaMode = PARANOIA_MODE_FULL ^ PARANOIA_MODE_NEVERSKIP;


	switch ( GetParanoiaMode() )
	{
		case 0:
		    nParanoiaMode = PARANOIA_MODE_OVERLAP;
		break;
		case 1:
		    nParanoiaMode &= ~PARANOIA_MODE_VERIFY;
		break;
		case 2:
		    nParanoiaMode &= ~( PARANOIA_MODE_SCRATCH | PARANOIA_MODE_REPAIR );
		break;
		case 3:
			// use default setting
		break;
		default:
			//assert( FALSE );
		break;
	}	

	paranoia_modeset( m_pParanoia, nParanoiaMode );


	// Reset current sector
	m_lSector=0;

	// Allocate memory for a read buffers
	m_pbtReadBuffer=new BYTE[ GetNumReadSectors() * CB_CDDASECTOR ];

	// Allocate memory for multi read buffers 
	m_pbtReadBufferCompare=new BYTE[ GetNumReadSectors() * CB_CDDASECTOR ];

	// Allocate memory for the two read buffers
	if ( GetNumCompareSectors()>0 )
	{
		m_pbtOverlapBuffer=new BYTE[ GetNumCompareSectors() * CB_CDDASECTOR ];
	}
	else
	{
		m_pbtOverlapBuffer=NULL;
	}

	// Rip at leat 10 sectors
	if ( nBeginSector + 10 > nEndSector )
	{
		nEndSector = nBeginSector+10;
	}

	// Set starting point
	m_nBeginSector = max((long)nBeginSector+(long)GetOffsetStart(),0);

	// Set end point
	m_nEndSector = (long)nEndSector + (long)GetOffsetEnd();

	m_lSector = m_nBeginSector;

	paranoia_set_range( m_pParanoia, m_nBeginSector, m_nEndSector - 1  );


	// Call to enable CDDA (needed for certain drives
	EnableCdda(TRUE);

	// Don't allow a CD change while recording
	PreventMediaRemoval (TRUE);

	SetScsiTimeOut(10);
	int	nTimeout=GetScsiTimeOut();

	DebugPrintf("Timeout is set to %d",nTimeout);


	// Clear number of jitter errors
	m_nJitterErrors=0;

	// Clear peak value
	m_nPeakValue=0;

	// Set First Read = TRUE
	m_bFirstRead=TRUE;
	

	m_dwBytesToDo = CB_CDDASECTOR * ( m_nEndSector - m_nBeginSector );

	DebugPrintf("Leaving CCDExtract::SetupTrackExtractParanoia");

	return TRUE;
}
*/

BOOL CCDExtract::EndTrackExtract()
{
	DebugPrintf("Entering CCDExtract::EndTrackExtract");

	// de-allocate memory buffers
	delete [] m_pbtReadBuffer; m_pbtReadBuffer=NULL;
	delete [] m_pbtReadBufferCompare; m_pbtReadBufferCompare = NULL;

	// de-allocate memory buffers
	delete [] m_pbtOverlapBuffer; m_pbtOverlapBuffer=NULL;

	delete m_pParanoiaDrive;
	if ( m_pParanoia  )
	{
		//paranoia_free( m_pParanoia );
		m_pParanoia = NULL;
	}

	m_pParanoia = NULL;
	m_pParanoiaDrive = NULL;

	// Disable CDDA (needed for certain drives)
	EnableCdda( FALSE );

	// Allow a CD removal again
	PreventMediaRemoval( FALSE );


	m_pbAbort = NULL;

	DebugPrintf("Leaving CCDExtract::EndTrackExtract");
	return FALSE;
}



// now, figure out how many blocks to keep (jitter control) 
//  |-------- previous --------|                            
//                         |----------- buffer --------|    
//  we need to match the end of previous and start of buffer
//  so we do a match, and compute how many bytes of overlap 
//  to skip (bytes_to_skip)                                 

DWORD CCDExtract::CorrectJitter(BYTE* pbtOverlapPtr,BYTE* pbtReadPtr,DWORD dwBytesRead,BOOL& bJitterError)
{
	DebugPrintf("Entering CCDExtract::CorrectJitter");

	BOOL	bFound = FALSE;
	int		nOffset=0;
	int 	nCompareBytes=(int)GetNumCompareSectors()*(int)CB_CDDASECTOR;
	int		nMaxByteCompare=min(2*(int)GetNumOverlapSectors()*(int)CB_CDDASECTOR,dwBytesRead)-(int)nCompareBytes;
	int		nStartCompare=nMaxByteCompare/2;
	int 	nSkipBytes=nStartCompare;

	BYTE*	pbtLCmp=pbtReadPtr+nStartCompare;
	BYTE*	pbtHCmp=pbtReadPtr+nStartCompare;

	bJitterError=FALSE;

	if (pbtOverlapPtr==NULL)
	{
		DebugPrintf("Leaving CCDExtract::CorrectJitter, pbtOverlapPtr=NULL");
		return 0;
	}
	if (pbtReadPtr==NULL)
	{
		DebugPrintf("Leaving CCDExtract::CorrectJitter, pbtReadPtr==NULL");
		return 0;
	}
	if (dwBytesRead==0)
	{
		DebugPrintf("Leaving CCDExtract::CorrectJitter, dwBytesRead==0");
		return 0;
	}
	if (nMaxByteCompare<=0)
	{
		DebugPrintf("Leaving CCDExtract::CorrectJitter, nMaxByteCompare<=0");
		return 0;
	}

	for (nOffset = 0; nOffset<nStartCompare; nOffset+=4,pbtLCmp-=4,pbtHCmp+=4) 
	{
		if (memcmp( pbtOverlapPtr, pbtLCmp,nCompareBytes) == 0) 
		{
			nSkipBytes= nStartCompare-nOffset+nCompareBytes;
			bFound = TRUE;
			break;
		}
		if (memcmp( pbtOverlapPtr, pbtHCmp,nCompareBytes) == 0) 
		{
			nSkipBytes= nStartCompare+nOffset+nCompareBytes;
			bFound = TRUE;
			break;
		}

	}

	if (!bFound && dwBytesRead>nCompareBytes) 
	{
		bJitterError=TRUE;
		m_nJitterErrors++;
		DebugPrintf("Detected a jitter error in CCDExtract");
	}

	if (nMaxByteCompare)
		m_nJitterPos=(nSkipBytes-nCompareBytes)*100/nMaxByteCompare;
	else
		m_nJitterPos=50;

	//ASSERT( nSkipBytes<dwBytesRead);
	//ASSERT( nSkipBytes>=0);
	
	if (nSkipBytes<0)
		nSkipBytes=0;
	if (nSkipBytes>=dwBytesRead)
		nSkipBytes=dwBytesRead-1;

	DebugPrintf("Entering CCDExtract::CorrectJitter with nSkipBytes=%5d JitterPos=%5d",nSkipBytes,m_nJitterPos);
	return nSkipBytes;

/*
	if (!bFound) 
	{
		// Try using a more advanced jitter correction!!
		// Find max correlation
		nOffset=CB_CDDASECTOR*((nORD)GetNumOverlapSectors()-(nORD)GetNumCompareSectors());
		pbtLCmp=m_pbtReadBuffer+nOffset;
		pbtHCmp=m_pbtReadBuffer+nOffset;

		nORD	nMaxCorrPos=0;
		double	dMinDiff=1.0E99;

		// Look backwards
		for (i = 0; i < nOffset; i+=4) 
		{
			double dDiff=0.0;
			int j;

			for (j=0;j<CB_CDDASECTOR * GetNumCompareSectors();j++)
			{
				dDiff+=fabs(m_pbtOverlapBuffer[j]-pbtLCmp[j]);
			}

			if (dDiff<dMinDiff)
			{
				dMinDiff=dDiff;
				nBytesToSkip=nOffset-i + CB_CDDASECTOR * GetNumCompareSectors();
			}


			dDiff=0.0;

			for (j=0;j<CB_CDDASECTOR * GetNumCompareSectors();j++)
			{
				dDiff+=fabs(m_pbtOverlapBuffer[j]-pbtHCmp[j]);
			}
			if (dDiff<dMinDiff)
			{
				dMinDiff=dDiff;
				nBytesToSkip=nOffset+i + CB_CDDASECTOR * GetNumCompareSectors();
			}

			pbtLCmp-=4;
			pbtHCmp+=4;
		}

		bJitterError=TRUE;
		nBytesToSkip=nOffset;
		//AfxMessageBox(" Jitter correction failed, increase the Read Overlap value in the configure dialog box");
	}
*/
/*
#ifdef DEBUG_VERBOSE
	CString strTmp;
	strTmp.Format("Jitter Control: matched overlap %d , nominal value %d bytes \n ",nBytesToSkip-CB_CDDASECTOR * GetNumCompareSectors(), CB_CDDASECTOR * (GetNumOverlapSectors()-GetNumCompareSectors()));
	DebugPrintf(strTmp);
#endif
	return nBytesToSkip ;
*/
}



static char *callback_strings[15]={
	"wrote",
    "finished",
	"read",
	"verify",
	"jitter",
	"correction",
	"scratch",
	"scratch repair",
	"skip",
	"drift",
	"backoff",
	"overlap",
	"dropped",
	"duped",
	"transport error"
};

/*
void CCDExtract::paranoiaCallback( long inpos , int function )
{
	DebugPrintf("Callback sector %08d code %d string %s \n", inpos/2352 , function, (function>=-2&&function<=13?callback_strings[function+2]:"") );
	
	switch ( function )
	{
		case PARANOIA_CB_SKIP:
			g_bParanoiaJitterErrors = TRUE;
		break;
	}

}
*/
/*

CDEX_ERR CCDExtract::RipChunkParanoia( BYTE* pbtStream, LONG* pNumBytes, BOOL& bAbort )
{
	*pNumBytes = 0;

	m_pbAbort = &bAbort;
	g_bParanoiaJitterErrors = FALSE;


	if ( 0 == m_dwBytesToDo  )
	{
		SetPercentCompleted( 100 );
		return CDEX_RIPPING_DONE;
	}

	if (m_bFirstRead)
	{
		// Get starting sector which has to be copied
		m_lSector = m_nBeginSector;

		SetCDSpeed();

		// Spin up the CD Rom if necessary
		DWORD dwStart=::GetTickCount();
		while ( (::GetTickCount()-dwStart)<(DWORD)GetSpinUpTime()*1000)
		{
			// Read cd rom sector to Spin up the CD-ROM
			DebugPrintf("Reading Paranoia sector %d", m_lSector );

			ReadCdRomSector( m_pbtReadBuffer, m_lSector,1, FALSE );
		}

		m_bFirstRead = FALSE;
	}


	if ( m_pParanoia )
	{
		int nBlock = 0;

		DebugPrintf("Current, %8d Start %8d End %8d dwByteToDo %8d \n",
				m_lSector,
				m_nBeginSector,
				m_nEndSector,
				m_dwBytesToDo );

		for ( nBlock = 0; nBlock < GetNumReadSectors(); nBlock++ )
		{
			// function return only one sector
			short* buf = paranoia_read( m_pParanoia, &CCDExtract::paranoiaCallback, &bAbort );

			if ( NULL == buf )
				return CDEX_ERROR;

			*pNumBytes+= 2352;
			memcpy( pbtStream + nBlock * 2352, buf, 2352 );


			m_dwBytesToDo -= min( m_dwBytesToDo, 2352 );

			if ( 0 == m_dwBytesToDo  )
			{
				SetPercentCompleted( 100 );
				return CDEX_OK;
			}
		}

		if ( g_bParanoiaJitterErrors )
		{
			m_nJitterErrors++;
		}

		m_lSector+= GetNumReadSectors();
		int nSectorsToDo = (m_nEndSector - m_lSector);

		SetPercentCompleted((int)( 100.0- (DOUBLE)nSectorsToDo * 100.0/ ( m_nEndSector - m_nBeginSector ) ) );

		// Calculate Peak Value
		short* psValue=(short*)(pbtStream);

		// Determine peak value
		for ( DWORD dwSample=0;dwSample< *pNumBytes/2; dwSample++ )
		{
			m_nPeakValue=max( abs( psValue[dwSample] ),m_nPeakValue );
		}

		if ( g_bParanoiaJitterErrors )
			return CDEX_JITTER_ERROR;

		return CDEX_OK;
	}

	return CDEX_ERROR;
}
*/

CDEX_ERR CCDExtract::RipChunk( BYTE* pbtStream, LONG* pNumBytes, BOOL& bAbort )
{
	static LONG		lSectorEnd;
	static LONG		nSectorsToDo;
	static DWORD	dwSkipBytes=0;
	static DWORD	dwSkipBytesCompare=0;
	static DWORD	dwBytes=0;
	BOOL			bJitterCorr=FALSE;
	LONG			lOverlapSamples=GetNumOverlapSectors();


	DebugPrintf( "Entering CCDExtract::RipChunk" );

	if ( GetJitterCorrection() && ( GetNumOverlapSectors()>0 ) )
	{
		bJitterCorr=TRUE;
	}
	else
	{
		lOverlapSamples=0;
	}


	if ( NULL == pNumBytes )
	{
//		ASSERT( FALSE );
		return CDEX_ERROR;
	}
	else
	{
		*pNumBytes=0;
	}


	if ( NULL == pbtStream || NULL == m_pbtReadBuffer )
	{
//		ASSERT( FALSE );
		return CDEX_ERROR;
	}

	if ( m_bFirstRead )
	{
		// Get starting sector which has to be copied
		m_lSector = m_nBeginSector;

		// Get last sector which has to be copied
		lSectorEnd = m_nEndSector;

		// calculate # of sectors to read
		nSectorsToDo = (m_nEndSector - m_lSector);

		// Set the CD ripping speed
		SetCDSpeed();

		// Spin up the CD Rom if necessary
		DWORD dwStart=::GetTickCount();
		while ( (::GetTickCount()-dwStart)<(DWORD)GetSpinUpTime()*1000)
		{
			// Read cd rom sector to Spin up the CD-ROM
			ReadCdRomSector( m_pbtReadBuffer, m_lSector, 1, FALSE );
		}

	}

	// Loop through all the tracks, maybe something for a user thread in the near future
	// if (nSectorsToDo <=GetNumOverlapSectors()) 
	if ( 0 == m_dwBytesToDo )
	{
		SetPercentCompleted( 100 );
		return CDEX_RIPPING_DONE;
	}

	// Set recording percentage complete
	if ( (m_nEndSector - m_nBeginSector) != 0 )
	{
		SetPercentCompleted( (int)( 100.0 -(DOUBLE)nSectorsToDo * 100.0 / ( m_nEndSector - m_nBeginSector ) ) );
	}
	else
	{
		SetPercentCompleted( 0 );
	}

	BYTE* pbtWritePtr=NULL;
	DWORD dwBufferOffset = GetNumCompareSectors() * CB_CDDASECTOR;
	BOOL	bJitterError=FALSE;

	LONG nSectorsToRead= min( nSectorsToDo, GetNumReadSectors() );

	// Handle last sectors correctly
	// FIXME, CAN ALL DRIVERS HANDLE READING BEYOND THE LEAD LOCATION ?
	if ( nSectorsToRead <= GetNumOverlapSectors() )
	{
		// In this situation, we have to read the overlap area + the amount of sectors
		// that still have to be done.
		nSectorsToRead= min( GetNumOverlapSectors() +nSectorsToDo, GetNumReadSectors() );
	}

	DebugPrintf( "nSectorsToDo=%d nSectorsToRead=%d m_lSector=%d",nSectorsToDo,nSectorsToRead,m_lSector );

	// We are reading only m_nReadSector*CB_CDDASECTOR bytes
	DWORD dwBytesRead= nSectorsToRead * CB_CDDASECTOR;

	// Should we do the multiple reads
	BOOL bJitterErrorCompare = FALSE; 
	BOOL bReadMatch = FALSE;

	BOOL doMultiReads = FALSE;

#if 0
	BOOL doMultiReads = TRUE;

	if ((GetMultiReadEnable() == FALSE) ||
		(GetMultiRead() <= 0) ||
		((GetMultiReadFirstOnly() == TRUE) && (!m_bFirstRead)))
	{
		doMultiReads = FALSE;
	}
#endif

	// Read Multiple times and continue when match
	while ( FALSE == bReadMatch ) 
	{		
		// Default is to pass, will mark fail if found
		bReadMatch = TRUE;

		// Normal Read
		ReadChunk(nSectorsToRead, lOverlapSamples, bJitterCorr, dwBytesRead, bJitterError, dwSkipBytes, m_pbtReadBuffer);
		
		// Read the compare blocks
		for (int rereads = 0; (bReadMatch) && (doMultiReads) && (rereads < GetMultiRead()) ; rereads++) 
		{

			// Read the block
			ReadChunk(nSectorsToRead, lOverlapSamples, bJitterCorr, dwBytesRead, bJitterErrorCompare, dwSkipBytesCompare, m_pbtReadBufferCompare);

			// Compare
			for (int compareIndex = 0; (compareIndex + max(dwSkipBytes,dwSkipBytesCompare))  < dwBytesRead; compareIndex++)
			{				
				if (
					m_pbtReadBuffer[dwSkipBytes + compareIndex] != 
					m_pbtReadBufferCompare[dwSkipBytesCompare + compareIndex]
					)
				{
				bReadMatch = FALSE;
				continue;
				}
			}
		}

	}

	// If we did the multi read we need to copy the last block so the parameters that were
	// set in the read, like m_nJitterPos and m_nJitterErrors, will apply to the current
	// data
	if ( doMultiReads == TRUE )
	{
		memcpy(m_pbtReadBuffer,m_pbtReadBufferCompare,dwBytesRead);	
	}

	// Calculate Peak Value
	short* psValue=(short*)(m_pbtReadBuffer);

	// Determine peak value
	for ( DWORD dwSample=0; dwSample<dwBytesRead/2; dwSample++ )
	{
		m_nPeakValue=max( abs( psValue[ dwSample ] ),m_nPeakValue );
	}

	// Set beginning of 
	pbtWritePtr = m_pbtReadBuffer + dwSkipBytes;


	// Copy the compare region upfront 
	if ( GetNumCompareSectors() > 0 && m_pbtOverlapBuffer )
	{
		int nBytesToCopy=GetNumCompareSectors() * CB_CDDASECTOR;
		int nCopyOffset=dwBytesRead-nBytesToCopy;

		if (nCopyOffset<0)
		{
			nCopyOffset=0;
			nBytesToCopy=dwBytesRead;
		}

		// Some debug logging, error prone code in CDex 1.20 beta 7!
		DebugPrintf("Copy overlap buffer, nCopyOffset=%d nBytesToCopy=%d",nCopyOffset,nBytesToCopy);

		memcpy(m_pbtOverlapBuffer,m_pbtReadBuffer+nCopyOffset,nBytesToCopy);
	}

	// Set number of output bytes
	*pNumBytes=min( dwBytesRead - dwSkipBytes, m_dwBytesToDo );

	// Clip in case of last sector


	// Debug information
	DebugPrintf("copy data dwBytesRead=%d pNumBytes=%d dwSkipBytes=%d",dwBytesRead,*pNumBytes,dwSkipBytes);

	// Copy byte stream
	memcpy( pbtStream, pbtWritePtr, *pNumBytes );

	// Increase the sector offset
	m_lSector+=(nSectorsToRead-lOverlapSamples );


	// Lower the number of sectors still to do
	nSectorsToDo-= ( nSectorsToRead - lOverlapSamples );

	// Lower number of bytes todo
	m_dwBytesToDo-= *pNumBytes;

	DebugPrintf("Leaving CCDExtract::RipChunk");

	if ( bJitterError )
		return CDEX_JITTER_ERROR;

	// And close CD device
	return CDEX_OK;
}

void CCDExtract::GetLastJitterErrorPosition(DWORD& dwStartSector,DWORD& dwEndSector)
{
	dwStartSector=m_lSector-(GetNumReadSectors()-GetNumOverlapSectors());
	dwEndSector=m_lSector;
}


void CCDExtract::ReadChunk(const long nSectorsToRead, const long lOverlapSamples, const BOOL bJitterCorr, const DWORD dwBytesRead, BOOL &bJitterError, DWORD &dwSkipBytes, PBYTE readBuffer)
{
	// Read Chunk from CD-ROM
	if (((ReadCdRomSector(readBuffer,m_lSector,nSectorsToRead, FALSE ) )==FALSE) &&
		(GetAspiRetries()>0)
		)
	{
		// If there was an error slow down the CDROM and try again
		::Sleep(1000);
		SetCDSpeed( 1 );
		DebugPrintf("Switch to lower Read Speed\n");
		
		// Read previous tracks first to sync up again
		for (int nRetro=0;nRetro<GetAspiRetries();nRetro++)
		{
			// Reduce READ speed to 1
			SetCDSpeed( 1 );
			if ( FALSE == ReadCdRomSector(readBuffer,150+nRetro*10,10, FALSE ) )
			{
				DebugPrintf("ReadCdRomSector not OK in retry: %d\n",nRetro);
				::Sleep(GetAspiTimeOut());
			}

		}

		// Try it again
		ReadCdRomSector( readBuffer, m_lSector, nSectorsToRead, FALSE );
	}

	if ( !m_bFirstRead && bJitterCorr)
	{
		// Perform Jitter Correction
		dwSkipBytes = CorrectJitter( m_pbtOverlapBuffer, readBuffer, dwBytesRead, bJitterError );
	}
	else
	{
		dwSkipBytes=0;
		m_bFirstRead=FALSE; 
	}

	return;

}

// Interface for Monty's paranoia library:
// return the number of sectors?

long cdda_read(cdrom_drive *d, void *buffer, long beginsector, long sectors)
{
	int nRetries = 0;

	CCDExtract *cdr = (CCDExtract*)d->cdr;

	// when 100 sectors have be read without a problem, revert to default speed
	if (  ( ( beginsector - cdr->GetLastSectorSpeedAdjusted() ) > 500  ) &&
			(-1 != cdr->GetLastSectorSpeedAdjusted() ) )
	{
		if ( cdr->GetCurrentSpeed() != cdr->GetSpeed()  )
		{
			cdr->SetCDSpeed(  );
			cdr->GetLastSectorSpeedAdjusted( -1 );
		}

	}

	if ( ( FALSE == cdr->ReadCdRomSector( (BYTE*)buffer, beginsector, sectors, FALSE ) ) 
			&& ( nRetries< 10 ) )
	{
		// read error, reduce spead if possible
		int nSpeed = cdr->GetCurrentSpeed();

		if ( nSpeed > 1)
		{
			::Sleep(1000);
			cdr->GetLastSectorSpeedAdjusted( beginsector );
			cdr->SetCDSpeed( 1 );

		}

		// do a dummy read at the begin sector to flush cache buffer
		cdr->ReadCdRomSector( (BYTE*)buffer, cdr->GetBeginSector(), sectors, FALSE );

//		nRetries++;
		sectors = -1;
	}

	/* return -999 incase the abort button has been pressed */
	if ( cdr->GetAbort() )
	{
		sectors = -999;
	}
	
  return sectors;
}