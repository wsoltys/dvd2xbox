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

#ifndef CCDENC_INCLUDED
#define CCDENC_INCLUDED


#include <xtl.h>
#include <stdio.h>
#include <vorbis\vorbisenc.h>

class CCDEnc
{
protected:
	FILE*	OGG;
	// Ogg
	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */
  
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	//vorbis_comment   vc; /* struct that stores all the user comments */

	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */

	int eos,ret;
	int i, founddata;

	BYTE*					tBuf;

public:
	int InitOgg(char* file,float quality,vorbis_comment vc);
	int OggEnc(int nNumBytesRead,BYTE* pbtStream);
	int OggClose();
	CCDEnc();
	~CCDEnc();


};
#endif