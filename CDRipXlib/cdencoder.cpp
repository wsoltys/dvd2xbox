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

#include <xtl.h>
#include <time.h>
#include <stdio.h>
#include "cdencoder.h"
#include "cdripxlib.h"
#include <vorbis\vorbisenc.h>

CCDEnc::CCDEnc()
{

}

CCDEnc::~CCDEnc()
{

}

int CCDEnc::InitOgg(char* file,float quality,vorbis_comment vc)
{
	if((OGG = fopen(file,"wb+"))==NULL) {
		DPf("Cannot open file: %s",file);
		return CDRIPX_ERR;
	}

	DPf("Entering InitOgg2");
	//ogg
	vorbis_info_init(&vi);
	DPf("Vorbis init ok");
	if(quality<0.1)
		quality=0.1;
	if(quality>1.0)
		quality=1.0;
	ret=vorbis_encode_init_vbr(&vi,2,44100,quality);
	
	if(ret){return CDRIPX_ERR;};
	DPf("vorbis encode init ok");
	/* add a comment */
	//vorbis_comment_init(&vc);
	//vorbis_comment_add_tag(&vc,"TRACK",file);

	/* set up the analysis state and auxiliary encoding storage */
	vorbis_analysis_init(&vd,&vi);
	DPf("vorbis analysis init ok");
	vorbis_block_init(&vd,&vb);
	DPf("vorbis block init ok");
  
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
	tBuf = new BYTE[4096];
	DPf("InitOgg2 ok");
	return CDRIPX_OK;
}

int CCDEnc::OggEnc(int nNumBytesRead,BYTE* pbtStream)
{
			eos				= 0;
			/* data to encode */
			LONG nBlocks = (int)(nNumBytesRead/4096);
			LONG nBytesleft = nNumBytesRead - nBlocks*4096;
			LONG block = 4096;

			DPf("Ripped %d bytes; %d Blocks, %d left",nNumBytesRead,nBlocks,nBytesleft);
			for(int a=0;a<=nBlocks;a++) {
			
			if(a==nBlocks) {
				DPf("Enter left Bytes");
				block = nBytesleft;
			}
	
			
		
			 /* expose the buffer to submit data */
			float **buffer=vorbis_analysis_buffer(&vd,1024);
      
			
			/* uninterleave samples */
			memcpy(tBuf,pbtStream,block);
			pbtStream+=4096;
			LONG realsamples = block/(2*2);
			signed char* b = (signed char*) tBuf;
			for (i=0; i<realsamples; i++) {
				int j = i << 2;
				buffer[0][i]=(((long)b[j+1]<<8)|(0x00ff&(int)b[j]))/32768.0f;
				buffer[1][i]=(((long)b[j+3]<<8)|(0x00ff&(int)b[j+2]))/32768.0f;
			}
			
			DPf("uninterleave samples complete");
			
		
			/* tell the library how much we actually submitted */
			vorbis_analysis_wrote(&vd,realsamples);
	
			DPf("Analysis wrote");
			/* vorbis does some data preanalysis, then divvies up blocks for
			more involved (potentially parallel) processing.  Get a single
			block for encoding now */
			while(vorbis_analysis_blockout(&vd,&vb)==1){

				DPf("analysis_blockout");
				/* analysis, assume we want to use bitrate management */
				vorbis_analysis(&vb,NULL);
				vorbis_bitrate_addblock(&vb);

				DPf("bitrate addblock");

				while(vorbis_bitrate_flushpacket(&vd,&op)){
	
					/* weld the packet into the bitstream */
					ogg_stream_packetin(&os,&op);
	
					DPf("stream packetin");
					/* write out pages (if any) */
					while(!eos){
						int result=ogg_stream_pageout(&os,&og);
						DPf("stream pageout");
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
	return 0;
}

int CCDEnc::OggClose()
{
	vorbis_analysis_wrote(&vd,0);

	while(vorbis_analysis_blockout(&vd,&vb)==1){

				DPf("analysis_blockout");
				/* analysis, assume we want to use bitrate management */
				vorbis_analysis(&vb,NULL);
				vorbis_bitrate_addblock(&vb);

				DPf("bitrate addblock");

				while(vorbis_bitrate_flushpacket(&vd,&op)){
	
					/* weld the packet into the bitstream */
					ogg_stream_packetin(&os,&op);
	
					DPf("stream packetin");
					/* write out pages (if any) */
					while(!eos){
						int result=ogg_stream_pageout(&os,&og);
						DPf("stream pageout");
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

	delete[] tBuf;
	tBuf=NULL;
	return 0;
}
