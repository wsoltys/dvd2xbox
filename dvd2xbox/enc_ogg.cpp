/*
** Copyright(C) 2003 WiSo (www.wisonauts.de)
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


//#include <xtl.h>
#include "d2xaudioencoder.h"
//#include "..\dvd2xbox\d2xsettings.h"
#include <time.h>


int D2Xaenc::InitOgg(char* file)
{
	if((OGG = fopen(file,"wb+"))==NULL) {
		OutputDebugString("Cannot open dest file");
		return 0;
	}
	int eos = 0;
	OutputDebugString("Entering InitOgg2");
	//ogg
	vorbis_info_init(&vi);
	OutputDebugString("Vorbis init ok");
	int ret=vorbis_encode_init_vbr(&vi,2,44100,g_d2xSettings.ogg_quality);
	
	if(ret)
		return 0;
	OutputDebugString("vorbis encode init ok");
	/* add a comment */
	//vorbis_comment_init(&vc);
	//vorbis_comment_add_tag(&vc,"TRACK",file);

	/* set up the analysis state and auxiliary encoding storage */
	vorbis_analysis_init(&vd,&vi);
	OutputDebugString("vorbis analysis init ok");
	vorbis_block_init(&vd,&vb);
	OutputDebugString("vorbis block init ok");
  
	/* set up our packet->stream encoder */
	/* pick a random serial number; that way we can more likely build
	chained streams just by concatenation */
	srand(time(NULL));
	ogg_stream_init(&os,rand());

	{
		ogg_packet header;
		ogg_packet header_comm;
		ogg_packet header_code;
		vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
		ogg_stream_packetin(&os,&header); /* automatically placed in its own page */
		ogg_stream_packetin(&os,&header_comm);
		ogg_stream_packetin(&os,&header_code);

		/* This ensures the actual
		* audio data will start on a new page, as per spec
		*/
		while(!eos){
			int result=ogg_stream_flush(&os,&og);
			if(result==0)break;
			fwrite(og.header,1,og.header_len,OGG);
			fwrite(og.body,1,og.body_len,OGG);
		}
	}
	buffer = new BYTE[4096];
	OutputDebugString("InitOgg2 ok");
	return 1;
}

int D2Xaenc::OggEnc(int nNumBytesRead,BYTE* pbtStream)
{
			int eos				= 0;
			/* data to encode */
			LONG nBlocks = (int)(nNumBytesRead/4096);
			LONG nBytesleft = nNumBytesRead - nBlocks*4096;
			LONG block = 4096;

			//OutputDebugString("Ripped %d bytes; %d Blocks, %d left",nNumBytesRead,nBlocks,nBytesleft);
			for(int a=0;a<=nBlocks;a++) {
			
			if(a==nBlocks) {
				OutputDebugString("Enter left Bytes");
				block = nBytesleft;
			}
	
			
		
			 /* expose the buffer to submit data */
			float **buffer=vorbis_analysis_buffer(&vd,1024);
      
			
			/* uninterleave samples */
			memcpy(buffer,pbtStream,block);
			pbtStream+=4096;
			LONG realsamples = block/(2*2);
			signed char* b = (signed char*) buffer;
			for (int i=0; i<realsamples; i++) {
				int j = i << 2;
				buffer[0][i]=(((long)b[j+1]<<8)|(0x00ff&(int)b[j]))/32768.0f;
				buffer[1][i]=(((long)b[j+3]<<8)|(0x00ff&(int)b[j+2]))/32768.0f;
			}
			
			OutputDebugString("uninterleave samples complete");
			
		
			/* tell the library how much we actually submitted */
			vorbis_analysis_wrote(&vd,realsamples);
	
			OutputDebugString("Analysis wrote");
			/* vorbis does some data preanalysis, then divvies up blocks for
			more involved (potentially parallel) processing.  Get a single
			block for encoding now */
			while(vorbis_analysis_blockout(&vd,&vb)==1){

				OutputDebugString("analysis_blockout");
				/* analysis, assume we want to use bitrate management */
				vorbis_analysis(&vb,NULL);
				vorbis_bitrate_addblock(&vb);

				OutputDebugString("bitrate addblock");

				while(vorbis_bitrate_flushpacket(&vd,&op)){
	
					/* weld the packet into the bitstream */
					ogg_stream_packetin(&os,&op);
	
					OutputDebugString("stream packetin");
					/* write out pages (if any) */
					while(!eos){
						int result=ogg_stream_pageout(&os,&og);
						OutputDebugString("stream pageout");
						if(result==0)break;
						fwrite(og.header,1,og.header_len,OGG);
						fwrite(og.body,1,og.body_len,OGG);
	  
						/* this could be set above, but for illustrative purposes, I do
						it here (to show that vorbis does know where the stream ends) */
	  
						if(ogg_page_eos(&og))eos=1;
					}
				}
			}

		}
	return 1;
}

int D2Xaenc::OggClose()
{
	vorbis_analysis_wrote(&vd,0);
	int eos = 0;
	while(vorbis_analysis_blockout(&vd,&vb)==1){

				OutputDebugString("analysis_blockout");
				/* analysis, assume we want to use bitrate management */
				vorbis_analysis(&vb,NULL);
				vorbis_bitrate_addblock(&vb);

				OutputDebugString("bitrate addblock");

				while(vorbis_bitrate_flushpacket(&vd,&op)){
	
					/* weld the packet into the bitstream */
					ogg_stream_packetin(&os,&op);
	
					OutputDebugString("stream packetin");
					/* write out pages (if any) */
					while(!eos){
						int result=ogg_stream_pageout(&os,&og);
						OutputDebugString("stream pageout");
						if(result==0)break;
						fwrite(og.header,1,og.header_len,OGG);
						fwrite(og.body,1,og.body_len,OGG);
	  
						/* this could be set above, but for illustrative purposes, I do
						it here (to show that vorbis does know where the stream ends) */
	  
						if(ogg_page_eos(&og))eos=1;
					}
				}
	}
	/* clean up and exit.  vorbis_info_clear() must be called last */
	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	//vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);
  
	/* ogg_page and ogg_packet structs always point to storage in
	libvorbis.  They're never freed or manipulated directly */
	fclose(OGG);

	delete[] buffer;
	buffer=NULL;
	return 1;
}

void D2Xaenc::AddOggTag(int key,char* value)
{
	switch(key)
	{
		case ENC_ARTIST:
		vorbis_comment_add_tag(&vc,"Artist",value);
		break;
	case ENC_TITLE:
		vorbis_comment_add_tag(&vc,"Title",value);
		break;
	case ENC_ALBUM:
		vorbis_comment_add_tag(&vc,"Album",value);
		break;
	case ENC_YEAR:
		vorbis_comment_add_tag(&vc,"Year",value);
		break;
	case ENC_COMMENT:
		vorbis_comment_add_tag(&vc,"Comment",value);
		break;
	case ENC_TRACK:
		vorbis_comment_add_tag(&vc,"Track",value);
		break;
	default:
		break;

	}

}
