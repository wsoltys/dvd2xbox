/********************************************************
 
         DirectShow audio decoder
	 Copyright 2001 Eugene Kuznetsov  (divx@euro.ru)
 
*********************************************************/

#include "../dshow/guids.h"
#include "DMO_Filter.h"
#include "DMO_AudioDecoder.h"


#include "DMO_AudioDecoder.h"
//#include "../ldt_keeper.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern "C" void mp_msg( int x, int lev,const char *format, ... );

#define __MODULE__ "DirectShow audio decoder"

typedef long  (__stdcall *GETCLASS) (GUID*, GUID*, void**);
//extern void print_wave_header(WAVEFORMATEX *h);

extern "C" DMO_AudioDecoder * DMO_AudioDecoder_Open(char* dllname, GUID* guid, WAVEFORMATEX* wf,int out_channels)
//DMO_AudioDecoder * DMO_AudioDecoder_Create(const CodecInfo * info, const WAVEFORMATEX* wf)
{
    DMO_AudioDecoder *pThis;
    int sz;
    WAVEFORMATEX* pWF;

    //Setup_LDT_Keeper();
//    Setup_FS_Segment();

    pThis = (DMO_AudioDecoder*)malloc(sizeof(DMO_AudioDecoder));

    pThis->m_iFlushed=1;

    sz = 18 + wf->cbSize;
    pThis->m_sVhdr = (char*)malloc(sz);
    memcpy(pThis->m_sVhdr, wf, sz);
    pThis->m_sVhdr2 = (char*)malloc(18);
    memcpy(pThis->m_sVhdr2, pThis->m_sVhdr, 18);

    pWF = (WAVEFORMATEX*)pThis->m_sVhdr2;
    pWF->wFormatTag = 1;
    pWF->wBitsPerSample = 16;
    pWF->nChannels = out_channels;
    pWF->nBlockAlign = 2*pWF->nChannels; //pWF->nChannels * (pWF->wBitsPerSample + 7) / 8;
    pWF->nAvgBytesPerSec = pWF->nBlockAlign * pWF->nSamplesPerSec;
    pWF->cbSize = 0;

    memset(&pThis->m_sOurType, 0, sizeof(pThis->m_sOurType));
    pThis->m_sOurType.majortype=MEDIATYPE_Audio;
    pThis->m_sOurType.subtype=MEDIASUBTYPE_PCM;
    pThis->m_sOurType.subtype.f1=wf->wFormatTag;
    pThis->m_sOurType.formattype=FORMAT_WaveFormatEx;
    pThis->m_sOurType.lSampleSize=wf->nBlockAlign;
    pThis->m_sOurType.bFixedSizeSamples=1;
    pThis->m_sOurType.bTemporalCompression=0;
    pThis->m_sOurType.cbFormat=sz;
    pThis->m_sOurType.pbFormat=pThis->m_sVhdr;

    memset(&pThis->m_sDestType, 0, sizeof(pThis->m_sDestType));
    pThis->m_sDestType.majortype=MEDIATYPE_Audio;
    pThis->m_sDestType.subtype=MEDIASUBTYPE_PCM;
    pThis->m_sDestType.formattype=FORMAT_WaveFormatEx;
    pThis->m_sDestType.bFixedSizeSamples=1;
    pThis->m_sDestType.bTemporalCompression=0;
    pThis->m_sDestType.lSampleSize=pWF->nBlockAlign;
    pThis->m_sDestType.cbFormat=18; //pWF->cbSize;
    pThis->m_sDestType.pbFormat=pThis->m_sVhdr2;

//    print_wave_header((WAVEFORMATEX *)pThis->m_sVhdr);
  //  print_wave_header((WAVEFORMATEX *)pThis->m_sVhdr2);

    pThis->m_pDMO_Filter = DMO_FilterCreate(dllname, guid, &pThis->m_sOurType, &pThis->m_sDestType);
    if( !pThis->m_pDMO_Filter )
    {
				mp_msg(0,0,"failed to create DMO filter\n");
        free(pThis);
        return NULL;
    }

    return pThis;
}

extern "C" void DMO_AudioDecoder_Destroy(DMO_AudioDecoder *pThis)
{
    free(pThis->m_sVhdr);
    free(pThis->m_sVhdr2);
    DMO_Filter_Destroy(pThis->m_pDMO_Filter);
    free(pThis);
}

extern "C" int DMO_AudioDecoder_Convert(DMO_AudioDecoder *pThis, const void* in_data, unsigned int in_size,
                             void* out_data, unsigned int out_size,
                             unsigned int* size_read, unsigned int* size_written)
{
    DMO_OUTPUT_DATA_BUFFER db;
    CMediaBuffer* bufferin;
    unsigned long written = 0;
    unsigned long read = 0;
    int r = 0;

    if (!in_data || !out_data)
        return -1;

    //Setup_FS_Segment();

    //m_pDMO_Filter->m_pMedia->vt->Lock(m_pDMO_Filter->m_pMedia, 1);
    bufferin = CMediaBufferCreate(in_size, (void*)in_data, in_size, 1);
    r = pThis->m_pDMO_Filter->m_pMedia->vt->ProcessInput(pThis->m_pDMO_Filter->m_pMedia, 0,
            (IMediaBuffer*)bufferin,
            (pThis->m_iFlushed) ? DMO_INPUT_DATA_BUFFERF_SYNCPOINT : 0,
            0, 0);
    if (r == 0)
    {
        ((IMediaBuffer*)bufferin)->vt->GetBufferAndLength((IMediaBuffer*)bufferin, 0, &read);
        pThis->m_iFlushed = 0;
    }

    ((IMediaBuffer*)bufferin)->vt->Release((IUnknown*)bufferin);

    //printf("RESULTA: %d 0x%x %ld    %d   %d\n", r, r, read, m_iFlushed, out_size);
    if (r == 0 || (unsigned)r == DMO_E_NOTACCEPTING)
    {
        unsigned long status = 0;
        /* something for process */
        db.rtTimestamp = 0;
        db.rtTimelength = 0;
        db.dwStatus = 0;
        db.pBuffer = (IMediaBuffer*) CMediaBufferCreate(out_size, out_data, 0, 0);
        //printf("OUTSIZE  %d\n", out_size);
        r = pThis->m_pDMO_Filter->m_pMedia->vt->ProcessOutput(pThis->m_pDMO_Filter->m_pMedia,
                0, 1, &db, &status);

        ((IMediaBuffer*)db.pBuffer)->vt->GetBufferAndLength((IMediaBuffer*)db.pBuffer, 0, &written);
        ((IMediaBuffer*)db.pBuffer)->vt->Release((IUnknown*)db.pBuffer);

        //printf("RESULTB: %d 0x%x %ld\n", r, r, written);
        //printf("Converted  %d  -> %d\n", in_size, out_size);
    }
    else if (in_size > 0)
      mp_msg(0,0,"failed ProcessInputError  r:0x%x=%d\n", r, r);

    if (size_read)
        *size_read = read;
    if (size_written)
        *size_written = written;
    return r;
}

extern "C" int DMO_AudioDecoder_GetSrcSize(DMO_AudioDecoder *pThis, int dest_size)
{
    //    unsigned long inputs, outputs;
    //    Setup_FS_Segment();
    //    pThis->m_pDMO_Filter->m_pMedia->vt->GetOutputSizeInfo(pThis->m_pDMO_Filter->m_pMedia, 0, &inputs, &outputs);
    return ((WAVEFORMATEX*)pThis->m_sVhdr)->nBlockAlign*4;
}
