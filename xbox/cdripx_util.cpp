/*
** Copyright(C) 1999 Albert L. Faber
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

#include <stdio.h>
#include "cdripx_util.h"


// CWAV CONSTRUCTOR
CWAV::CWAV()
{
	m_pTmpWav = NULL;
	
	m_dwDataSize = 0;
	m_wChannels = 2;
	m_wBitsPerSample = 16;
	m_wSampleRate = 44100;
	m_dwSamplesToConvert = 0;
	
	m_pSndFile = NULL;
	memset(&m_wfInfo, 0x00, sizeof(SF_INFO));
}


// CWAV DESTRUCTOR
CWAV::~CWAV()
{
	delete m_pTmpWav;
	CloseStream();
}

int CWAV::CloseStream()
{
	if (m_pSndFile)
	{
		if (SndFileClose(m_pSndFile) !=0)
		{
			return 1;
		}
		m_pSndFile = NULL;
	}
	// No Errors
	return 0;       
}

// CWAV::OpenForRead Method
int CWAV::OpenForWrite(const char *strFileName, WORD wSampleRate, WORD wBitsPerSample, WORD wChannels)
{
	// Clear m_wfInfo struct
	memset(&m_wfInfo, 0, sizeof(SF_INFO));
	
	m_wChannels = m_wfInfo.channels = wChannels;
	m_wBitsPerSample = m_wfInfo.pcmbitwidth = wBitsPerSample;
	m_wfInfo.samples = 0;		// Number of samples are not know at this point
	m_wSampleRate = m_wfInfo.samplerate = wSampleRate;
	
	int nCompression = SF_FORMAT_PCM;
	int	nFormat = SF_FORMAT_WAV;
	
	// Set format options
	m_wfInfo.format =(nFormat | nCompression);
	
	// Open stream
	if (!(m_pSndFile = SndFileOpenWrite(strFileName, &m_wfInfo)))
	{
		return 1;
	}
	
	// return success
	return 0;
}

int CWAV::SaveDataChunk(BYTE*	pbtDataPtr, DWORD dwNumBytes)
{
	// Write chunk to disk
	return SndFileWrite(m_pSndFile, (SHORT*)pbtDataPtr, dwNumBytes/sizeof(SHORT))*sizeof(SHORT);
}	


