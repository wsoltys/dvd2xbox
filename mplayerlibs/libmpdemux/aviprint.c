

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "config.h"
#include <stdlib.h>
#include <stdio.h>

#include "cygwin_inttypes.h"
#include "mp_msg.h"
#include "url.h"
#include "network.h"
#include "stream.h"
#include "demuxer.h"
#include "stheader.h"
#include "aviheader.h"

//#include "codec-cfg.h"
//#include "stheader.h"

void print_avih_flags(MainAVIHeader *h)
{
    mp_msg(0,0,"MainAVIHeader.dwFlags: (%ld)%s%s%s%s%s%s\n",h->dwFlags,
           (h->dwFlags&AVIF_HASINDEX)?" HAS_INDEX":"",
           (h->dwFlags&AVIF_MUSTUSEINDEX)?" MUST_USE_INDEX":"",
           (h->dwFlags&AVIF_ISINTERLEAVED)?" IS_INTERLEAVED":"",
           (h->dwFlags&AVIF_TRUSTCKTYPE)?" TRUST_CKTYPE":"",
           (h->dwFlags&AVIF_WASCAPTUREFILE)?" WAS_CAPTUREFILE":"",
           (h->dwFlags&AVIF_COPYRIGHTED)?" COPYRIGHTED":""
          );
}

void print_avih(MainAVIHeader *h)
{
    mp_msg(0,0,"======= AVI Header =======\n");
    mp_msg(0,0,"us/frame: %ld  (fps=%5.3f)\n",h->dwMicroSecPerFrame,1000000.0f/(float)h->dwMicroSecPerFrame);
    mp_msg(0,0,"max bytes/sec: %ld\n",h->dwMaxBytesPerSec);
    mp_msg(0,0,"padding: %ld\n",h->dwPaddingGranularity);
    print_avih_flags(h);
    mp_msg(0,0,"frames  total: %ld   initial: %ld\n",h->dwTotalFrames,h->dwInitialFrames);
    mp_msg(0,0,"streams: %ld\n",h->dwStreams);
    mp_msg(0,0,"Suggested BufferSize: %ld\n",h->dwSuggestedBufferSize);
    mp_msg(0,0,"Size:  %ld x %ld\n",h->dwWidth,h->dwHeight);
}

void print_strh(AVIStreamHeader *h)
{
    mp_msg(0,0,"======= STREAM Header =======\n");
    mp_msg(0,0,"Type: %.4s   FCC: %.4s (%X)\n",(char *)&h->fccType,(char *)&h->fccHandler,(unsigned int)h->fccHandler);
    mp_msg(0,0,"Flags: %ld\n",h->dwFlags);
    mp_msg(0,0,"Priority: %d   Language: %d\n",h->wPriority,h->wLanguage);
    mp_msg(0,0,"InitialFrames: %ld\n",h->dwInitialFrames);
    mp_msg(0,0,"Rate: %ld/%ld = %5.3f\n",h->dwRate,h->dwScale,(float)h->dwRate/(float)h->dwScale);
    mp_msg(0,0,"Start: %ld   Len: %ld\n",h->dwStart,h->dwLength);
    mp_msg(0,0,"Suggested BufferSize: %ld\n",h->dwSuggestedBufferSize);
    mp_msg(0,0,"Quality %ld\n",h->dwQuality);
    mp_msg(0,0,"Sample size: %ld\n",h->dwSampleSize);
}

void print_wave_header(WAVEFORMATEX *h)
{
    mp_msg(0,0,"======= WAVE Format =======\n");
    mp_msg(0,0,"Format Tag: %d (0x%X)\n",h->wFormatTag,h->wFormatTag);
    mp_msg(0,0,"Channels: %d\n",h->nChannels);
    mp_msg(0,0,"Samplerate: %ld\n",h->nSamplesPerSec);
    mp_msg(0,0,"avg byte/sec: %ld\n",h->nAvgBytesPerSec);
    mp_msg(0,0,"Block align: %d\n",h->nBlockAlign);
    mp_msg(0,0,"bits/sample: %d\n",h->wBitsPerSample);
    mp_msg(0,0,"cbSize: %d\n",h->cbSize);
    if(h->wFormatTag==0x55 && h->cbSize>=12)
    {
        /*MPEGLAYER3WAVEFORMAT* h2=(MPEGLAYER3WAVEFORMAT *)h;
        mp_msg(0,0,"mp3.wID=%d\n",h2->wID);
        mp_msg(0,0,"mp3.fdwFlags=0x%lX\n",h2->fdwFlags);
        mp_msg(0,0,"mp3.nBlockSize=%d\n",h2->nBlockSize);
        mp_msg(0,0,"mp3.nFramesPerBlock=%d\n",h2->nFramesPerBlock);
        mp_msg(0,0,"mp3.nCodecDelay=%d\n",h2->nCodecDelay);*/
    }
    else if (h->cbSize > 0)
    {
        int i;
        uint8_t* p = ((uint8_t*)h) + sizeof(WAVEFORMATEX);
        mp_msg(0,0,"Unknown extra header dump: ");
        for (i = 0; i < h->cbSize; i++)
            mp_msg(0,0,"[%x] ", *(p+i));
        mp_msg(0,0,"\n");
    }
}


void print_video_header(BITMAPINFOHEADER *h)
{
    mp_msg(0,0,"======= VIDEO Format ======\n");
    mp_msg(0,0,"  biSize %d\n", h->biSize);
    mp_msg(0,0,"  biWidth %d\n", h->biWidth);
    mp_msg(0,0,"  biHeight %d\n", h->biHeight);
    mp_msg(0,0,"  biPlanes %d\n", h->biPlanes);
    mp_msg(0,0,"  biBitCount %d\n", h->biBitCount);
    mp_msg(0,0,"  biCompression %d='%.4s'\n", h->biCompression, (char *)&h->biCompression);
    mp_msg(0,0,"  biSizeImage %d\n", h->biSizeImage);
    mp_msg(0,0,"===========================\n");
}


void print_index(AVIINDEXENTRY *idx,int idx_size)
{
    int i;
    unsigned int pos[256];
    unsigned int num[256];
    for(i=0;i<256;i++)
        num[i]=pos[i]=0;
    for(i=0;i<idx_size;i++)
    {
        int id=avi_stream_id(idx[i].ckid);
        if(id<0 || id>255)
            id=255;
        mp_msg(0,0,"%5d:  %.4s  %4X  %08X  len:%6ld  pos:%7d->%7.3f %7d->%7.3f\n",i,
               (char *)&idx[i].ckid,
               (unsigned int)idx[i].dwFlags,
               (unsigned int)idx[i].dwChunkOffset,
               //      idx[i].dwChunkOffset+demuxer->movi_start,
               idx[i].dwChunkLength,
               pos[id],(float)pos[id]/18747.0f,
               num[id],(float)num[id]/23.976f
              );
        pos[id]+=idx[i].dwChunkLength;
        ++num[id];
    }
}


