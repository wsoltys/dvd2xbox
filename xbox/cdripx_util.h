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
#ifndef CDRIPX_UTIL_INCLUDED
#define CDRIPX_UTIL_INCLUDED

#include <xtl.h>

extern "C"{
	#include "../libsndfile/sndfile.h"
	SNDFILE*	SndFileOpenRead(const char *path, SF_INFO *sfinfo);
	SNDFILE*	SndFileOpenWrite(const char *path, SF_INFO *sfinfo);
	off_t		SndFileSeek(SNDFILE *sndfile, off_t offset, int whence);
	int			SndFileClose(SNDFILE *sndfile);
	size_t		SndFileRead(SNDFILE *sndfile, short *ptr, size_t items);
	size_t		SndFileWrite(SNDFILE *sndfile, short *ptr, size_t items);

}


// Single byte alignment
#pragma pack( push, 1 )

class CWAV 
{
private:
	DWORD	m_dwDataSize;				// Size of data section
	WORD	m_wSampleRate;				// Samples per sec per channel
	WORD	m_wChannels;				// Number of channels (1=MONO, 2=STEREO)
	WORD	m_wBitsPerSample;			// Should be 8 or 16 bits per sample
	char	*m_strFileName;				// Just for debugging purposes
	DWORD	m_dwSamplesToConvert;
	CWAV*	m_pTmpWav;


private:
	SNDFILE*	m_pSndFile;
	SF_INFO		m_wfInfo;

public:

	// CONSTRUCTORS
	CWAV();

	// DESTRUCTOR
	~CWAV();

	// METHODS
	int	OpenForWrite(const char *strFileName,WORD wSampleRate,WORD wBitsPerSample,WORD wNumChannels);
	int	CloseStream();
	int	SaveDataChunk(BYTE* pbtDataPtr,DWORD dwNumBytes);


};

#pragma pack(pop,1)


#endif