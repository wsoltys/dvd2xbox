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


#include "cdripxlib.h"
#include "..\cdrip\cdrip.h"
#include <time.h>
#include <algorithm>


// CCDRipperX
CCDRipperX::CCDRipperX()
{
	rip_in_progress = false;
}

CCDRipperX::~CCDRipperX()
{

}

void CCDRipperX::GetCDRipInfo(int& nSampleFreq,int& nChannels,int& nBitsPerChannel)
{
	// Initialize to default values
	nSampleFreq=44100;
	nChannels=2;
	nBitsPerChannel=16;
}

HRESULT CCDRipperX::OpenRipper(int ntrack)
{
	if ( CDEX_OK != CR_Init( "test.ini" ) ) {
		DPf("CR_Init Ogg failed");
		//return E_FAIL;
	}
	CR_ReadToc();
	if(CR_GetNumTocEntries()<1) {
		DPf("CR_GetNumEntries Ogg failed");
		return E_FAIL;
	}
	if (CR_OpenRipper(	&nBufferSize,
						CR_GetTocEntry(ntrack-1).dwStartSector,
						CR_GetTocEntry(ntrack).dwStartSector
						)!=CDEX_OK) {
							DPf("CR_OpenRipper Ogg failed");
                            return E_FAIL;
						}
	return S_OK;
}

int CCDRipperX::RipToOgg(	int				ntrack,
							char*			file,
							int&			nPercent,
							int&			nPeakValue,
							int&			nJitterErrors,
							int&			nJitterPos )
{
	if(rip_in_progress) {
		CDEX_ERR ripErr;
		LONG nNumBytesRead;
		// Initialize incoming paramters
		nPercent		= 0;
		nJitterErrors	= 0;
		nPeakValue		= 0;
		eos				= 0;
		BOOL			bAbort=false;

		// create the stream buffer
		std::auto_ptr<BYTE> pbtBufferStream(new BYTE[nBufferSize]);
		//pbtStream =new BYTE[nBufferSize];

		// Get a pointer to the buffer
		//BYTE* pbtStream=pbtBufferStream.get();
		BYTE* pbtStream=new BYTE[nBufferSize];

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
			for(i=0;i<(block/4);i++){
				buffer[0][i]=((pbtStream[a*4096+i*4+1]<<8)|
					(pbtStream[a*4096+i*4] & 0xff))/32768.0f;
				buffer[1][i]=((pbtStream[a*4096+i*4+3]<<8)|
					(pbtStream[a*4096+i*4+2] & 0xff))/32768.0f;
			}
			
			DPf("uninterleave samples complete");
			
		
			/* tell the library how much we actually submitted */
			vorbis_analysis_wrote(&vd,i);
	
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

			vorbis_analysis_wrote(&vd,0);

			/* clean up and exit.  vorbis_info_clear() must be called last */
			ogg_stream_clear(&os);
			vorbis_block_clear(&vb);
			vorbis_dsp_clear(&vd);
			vorbis_comment_clear(&vc);
			vorbis_info_clear(&vi);
  
			/* ogg_page and ogg_packet structs always point to storage in
			libvorbis.  They're never freed or manipulated directly */
			fclose(OGG);
			rip_in_progress=false;
			return CDRIPX_DONE;
		}

	} else {	

		if (S_OK != OpenRipper(ntrack) ) {
			DPf("Cannot openripper Ogg");
			return CDRIPX_ERR;
		}

		if((OGG = fopen(file,"wb+"))==NULL) {
			DPf("Cannot open file: %s",file);
			return CDRIPX_ERR;
		}

		//ogg
		vorbis_info_init(&vi);
		ret=vorbis_encode_init_vbr(&vi,2,44100,.5);
		if(ret){return CDRIPX_ERR;};

		/* add a comment */
		vorbis_comment_init(&vc);
		vorbis_comment_add_tag(&vc,"ENCODER","encoder_example.c");

		/* set up the analysis state and auxiliary encoding storage */
		vorbis_analysis_init(&vd,&vi);
		vorbis_block_init(&vd,&vb);
  
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
		DPf("Ogg header wrote");
		rip_in_progress = true;
	}

	return CDRIPX_OK;
}
/*
int CCDRipperX::RipToWav(	int				ntrack,
							char*			file,
							int&			nPercent,
							int&			nPeakValue,
							int&			nJitterErrors,
							int&			nJitterPos )
{
	if(rip_in_progress) {

		CDEX_ERR ripErr;
		LONG nNumBytesRead;
		// Initialize incoming paramters
		nPercent		= 0;
		nJitterErrors	= 0;
		nPeakValue		= 0;
		BOOL			bAbort=false;

		// create the stream buffer
		std::auto_ptr<BYTE> pbtBufferStream(new BYTE[nBufferSize]);

		// Get a pointer to the buffer
		BYTE* pbtStream=pbtBufferStream.get();

		if(( CDEX_RIPPING_DONE != (ripErr=CR_RipChunk(pbtStream,&nNumBytesRead, bAbort )))) {
			// Check for jitter errors
			if ( CDEX_JITTER_ERROR == ripErr )
			{
				DWORD dwStartSector,dwEndSector;

				// Get info where jitter error did occur
				CR_GetLastJitterErrorPosition(dwStartSector,dwEndSector);

				// Add the jitter error to the logging file
				//RipInfoDB.SetJitterError(dwStartSector,dwEndSector);

			}

			// Check if an error did occur
			if ( CDEX_ERROR == ripErr )
			{
				return CDRIPX_ERR;
			}

			// Write chunk to disk
			outWav.SaveDataChunk(pbtStream,nNumBytesRead);

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
			// Close the output file
			outWav.CloseStream();
			rip_in_progress=false;
			return CDRIPX_DONE;
		}

	} else {

		int				nSampleFreq;
		int				nChannels;
		int				nBitsPerChannel;


		// Get the information regarding the CD-Ripping settings
		GetCDRipInfo( nSampleFreq, nChannels, nBitsPerChannel);

		// Try to create the file name
		if (outWav.OpenForWrite(file, nSampleFreq,nBitsPerChannel,nChannels)!=CDEX_OK) {
			DPf(L"Failed to open file %s",file);
			return CDRIPX_ERR;
		}
		if (OpenRipper != CDRIPX_OK)
			return CDRIPX_ERR;
		rip_in_progress = true;
	}
	return CDRIPX_OK;

}
*/

