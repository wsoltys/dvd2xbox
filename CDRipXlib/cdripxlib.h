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
#ifndef CDRIPX_INCLUDED
#define CDRIPX_INCLUDED


#include <xtl.h>
#include <stdio.h>

#ifdef _WITHENC
#include "cdencoder.h"
#include <vorbis\vorbisenc.h>
#endif

//-----------------------------------------------------------------------------
// Name: class CCDRipX
// Desc: CDDA Player
//
// Init()
//   initialize and reads TOC
// DeInit()
//   deinitialize cdrip interface
// GetNumTocEntries()
//   returns the Number of Tracks
// GetTrackInfo(6)
//   returns track 6 in minutes,seconds and frames
// playTrack(7)
//   plays track 7
// Process( &pTimeplayed  )
//   processes the audio data. need to be called in a loop. calling DirectSoundDoWork() 
//   afterwards is mandatory. pTimeplayed returns the time played in seconds
// Pause(DSSTREAMPAUSE_PAUSE)
//   paused the stream. DSSTREAMPAUSE_RESUME resumes it
// Stop()
//   stops playing and clears memory (hopefully)
//
//
// Encoder (only included when _WITHENC is defined in CDPlayX and CDRipXlib
//
// InitOgg(8,"Track08.ogg",0.5)
//    marks track 8 for ripping, sets filename to Track08.ogg and base_quality to 0.5
//	  (range 0.0 lo to 1.0 hi)
//
// AddOggComment(key,value)
//    adds key value pairs to the ogg comment header
//
// RipToOgg(nPercent,nPeakValue,nJitterErrors,nJitterPos)
//    rips Track and encodes it do ogg.
//    nPercent: shows how many is done
//
// See sample app CDPlayX for example (www.wisonauts.org)
//-----------------------------------------------------------------------------

// Define the maximum amount of packets we will ever submit to the renderer
const DWORD WAVSTRM_PACKET_COUNT = 2;

#define TRACKSPERSEC 75
#define CB_CDDASECTOR 2352
#define TRACK_LIST 100

#define CDRIPX_OK		0
#define CDRIPX_ERR		1
#define CDRIPX_DONE		2

struct cdtoc {
	int	min;
	int	sec;
	int	frame;
};



class CCDRipX
{
protected:
	
	LONG					nNumBytesRead;
	LONG					nTotalBytes;
	IDirectSoundStream*     m_pDestXMO;
	WAVEFORMATEX            m_wfxSourceFormat; 
	BYTE*	                m_pvSourceBuffer[WAVSTRM_PACKET_COUNT]; 
	HRESULT					hr;
	HRESULT                 m_hrOpenResult; 
	DWORD                   m_adwStatus[2];
	BYTE*					pbtStream;
	LONG					nBufferSize;
	BOOL					m_init;
	cdtoc					CDCon[TRACK_LIST];
	int						m_nNumTracks;
	
	HRESULT					pGetTrackInfo();
	unsigned long			CalculateDiscID();
	int						Ripperinit(int ntrack);
	int						Playerinit();
	int						CreateStream();
	HRESULT					ProcessSound( DWORD dwPacketIndex );
	BOOL					FindFreePacket( DWORD* pdwPacketIndex );
	
#ifdef _WITHENC
	BOOL					rip_in_progress;
	CCDEnc					*enc;
	vorbis_comment			vc;
	char*					filename;
	float					base_quality;
#endif

public:
	// by Piet
	cdtoc					oCDCon[TRACK_LIST];
	// by Piet

	HRESULT					Init();
	HRESULT					DeInit();
	int						GetNumTocEntries();
	cdtoc					GetTrackInfo(int ntrack);
	HRESULT					playTrack(int nTrack);
	int						Process( DWORD* pTimeplayed = NULL );
	void					Pause(DWORD dwPause);
	void					Stop();

#ifdef _WITHENC

#define lame_artist		1
#define	lame_title		2
#define lame_album		3
#define lame_year		4
#define lame_comment	5
#define lame_track		6

	int						InitOgg(	int				ntrack,
										char*			file,
										float			bq);
	int						AddOggComment(char* key,char* value);	
	int						RipToOgg(	int&			nPercent,
										int&			nPeakValue,
										int&			nJitterErrors,
										int&			nJitterPos );

	int						InitLame(	int				ntrack,
										char*			file);
	void					AddLameTag(int key,const char* value);
	int						RipToLame(	int&			nPercent,
										int&			nPeakValue,
										int&			nJitterErrors,
										int&			nJitterPos );
#endif
		
	CCDRipX();
	~CCDRipX();

};
void DPf(const char* pzFormat, ...);


#endif