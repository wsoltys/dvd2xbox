/********************************************************
 
         DirectShow audio decoder
	 Copyright 2001 Eugene Kuznetsov  (divx@euro.ru)
 
*********************************************************/


#include "DS_Filter.h"


struct _DS_AudioDecoder
{
    WAVEFORMATEX in_fmt;
    AM_MEDIA_TYPE m_sOurType, m_sDestType;
    DS_Filter* m_pDS_Filter;
    char* m_sVhdr;
    char* m_sVhdr2;
};

#include "DS_AudioDecoder.h"
//#include "../ldt_keeper.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define __MODULE__ "DirectShow audio decoder"

typedef long STDCALL (__stdcall *GETCLASS) (GUID*, GUID*, void**);
extern "C" void Setup_FS_Segment(void);

extern "C" DS_AudioDecoder * DS_AudioDecoder_Open(char* dllname, GUID* guid, WAVEFORMATEX* wf)
//DS_AudioDecoder * DS_AudioDecoder_Create(const CodecInfo * info, const WAVEFORMATEX* wf)
{
    DS_AudioDecoder *pthis;
    int sz;
    WAVEFORMATEX* pWF;

    //Setup_LDT_Keeper();
    //Setup_FS_Segment();

    pthis = (DS_AudioDecoder*)malloc(sizeof(DS_AudioDecoder));

    sz = 18 + wf->cbSize;
    pthis->m_sVhdr = (char*)malloc(sz);
    memcpy(pthis->m_sVhdr, wf, sz);
    pthis->m_sVhdr2 = (char*)malloc(18);
    memcpy(pthis->m_sVhdr2, pthis->m_sVhdr, 18);

    pWF = (WAVEFORMATEX*)pthis->m_sVhdr2;
    pWF->wFormatTag = 1;
    pWF->wBitsPerSample = 16;
    pWF->nBlockAlign = pWF->nChannels * (pWF->wBitsPerSample + 7) / 8;
    pWF->cbSize = 0;
    pWF->nAvgBytesPerSec = pWF->nBlockAlign * pWF->nSamplesPerSec;

    memcpy(&pthis->in_fmt,wf,sizeof(WAVEFORMATEX));

    memset(&pthis->m_sOurType, 0, sizeof(pthis->m_sOurType));
    pthis->m_sOurType.majortype=MEDIATYPE_Audio;
    pthis->m_sOurType.subtype=MEDIASUBTYPE_PCM;
    pthis->m_sOurType.subtype.f1=wf->wFormatTag;
    pthis->m_sOurType.formattype=FORMAT_WaveFormatEx;
    pthis->m_sOurType.lSampleSize=wf->nBlockAlign;
    pthis->m_sOurType.bFixedSizeSamples=1;
    pthis->m_sOurType.bTemporalCompression=0;
    pthis->m_sOurType.pUnk=0;
    pthis->m_sOurType.cbFormat=sz;
    pthis->m_sOurType.pbFormat=pthis->m_sVhdr;

    memset(&pthis->m_sDestType, 0, sizeof(pthis->m_sDestType));
    pthis->m_sDestType.majortype=MEDIATYPE_Audio;
    pthis->m_sDestType.subtype=MEDIASUBTYPE_PCM;
    //    pthis->m_sDestType.subtype.f1=pWF->wFormatTag;
    pthis->m_sDestType.formattype=FORMAT_WaveFormatEx;
    pthis->m_sDestType.bFixedSizeSamples=1;
    pthis->m_sDestType.bTemporalCompression=0;
    pthis->m_sDestType.lSampleSize=pWF->nBlockAlign;
    if (wf->wFormatTag == 0x130)
        // ACEL hack to prevent memory corruption
        // obviosly we are missing something here
        pthis->m_sDestType.lSampleSize *= 288;
    pthis->m_sDestType.pUnk=0;
    pthis->m_sDestType.cbFormat=18; //pWF->cbSize;
    pthis->m_sDestType.pbFormat=pthis->m_sVhdr2;

    //print_wave_header(pthis->m_sVhdr);
    //print_wave_header(pthis->m_sVhdr2);

    /*try*/
    {
        ALLOCATOR_PROPERTIES props, props1;
        pthis->m_pDS_Filter = DS_FilterCreate(dllname, guid, &pthis->m_sOurType, &pthis->m_sDestType);
        if( !pthis->m_pDS_Filter )
        {
            free(pthis);
            return NULL;
        }

        pthis->m_pDS_Filter->Start(pthis->m_pDS_Filter);

        props.cBuffers=1;
        props.cbBuffer=pthis->m_sOurType.lSampleSize;
        props.cbAlign=1;
				props.cbPrefix=0;
        pthis->m_pDS_Filter->m_pAll->vt->SetProperties(pthis->m_pDS_Filter->m_pAll, &props, &props1);
        pthis->m_pDS_Filter->m_pAll->vt->Commit(pthis->m_pDS_Filter->m_pAll);
    }
    /*
    catch (FatalError& e)
{
    e.PrintAll();
    delete[] m_sVhdr;
    delete[] m_sVhdr2;
    delete m_pDS_Filter;
    throw;
}
    */
    return pthis;
}

extern "C" void DS_AudioDecoder_Destroy(DS_AudioDecoder *pthis)
{
    free(pthis->m_sVhdr);
    free(pthis->m_sVhdr2);
    DS_Filter_Destroy(pthis->m_pDS_Filter);
    free(pthis);
}

extern "C" int DS_AudioDecoder_Convert(DS_AudioDecoder *pthis, const void* in_data, unsigned int in_size,
                            void* out_data, unsigned int out_size,
                            unsigned int* size_read, unsigned int* size_written)
{
    unsigned int written = 0;
    unsigned int read = 0;

    if (!in_data || !out_data)
        return -1;

    //Setup_FS_Segment();

    in_size -= in_size%pthis->in_fmt.nBlockAlign;
    while (in_size>0)
    {
        unsigned int frame_size = 0;
        char* frame_pointer;
        IMediaSample* sample=0;
        char* ptr;
        int result;

        //	pthis->m_pOurOutput->SetFramePointer(out_data+written);
        pthis->m_pDS_Filter->m_pOurOutput->SetFramePointer(pthis->m_pDS_Filter->m_pOurOutput,&frame_pointer);
        pthis->m_pDS_Filter->m_pOurOutput->SetFrameSizePointer(pthis->m_pDS_Filter->m_pOurOutput,(long*)&frame_size);
        pthis->m_pDS_Filter->m_pAll->vt->GetBuffer(pthis->m_pDS_Filter->m_pAll, &sample, 0, 0, 0);
        if (!sample)
        {
            //printf("DS_AudioDecoder::Convert() Error: null sample\n");
            break;
        }
        sample->vt->SetActualDataLength(sample, pthis->in_fmt.nBlockAlign);
        sample->vt->GetPointer(sample, (BYTE **)&ptr);
        memcpy(ptr, (const uint8_t*)in_data + read, pthis->in_fmt.nBlockAlign);
        sample->vt->SetSyncPoint(sample, 1);
        sample->vt->SetPreroll(sample, 0);
        result = pthis->m_pDS_Filter->m_pImp->vt->Receive(pthis->m_pDS_Filter->m_pImp, sample);
        //if (result)
            //printf("DS_AudioDecoder::Convert() Error: putting data into input pin %x\n", result);
        if ((written + frame_size) > out_size)
        {
            sample->vt->Release((IUnknown*)sample);
            break;
        }
        memcpy((uint8_t*)out_data + written, frame_pointer, frame_size);
        sample->vt->Release((IUnknown*)sample);
        read+=pthis->in_fmt.nBlockAlign;
        written+=frame_size;
        break;
    }
    if (size_read)
        *size_read = read;
    if (size_written)
        *size_written = written;
    return 0;
}

extern "C" int DS_AudioDecoder_GetSrcSize(DS_AudioDecoder *pthis, int dest_size)
{
    double efficiency =(double) pthis->in_fmt.nAvgBytesPerSec
                       / (pthis->in_fmt.nSamplesPerSec*pthis->in_fmt.nBlockAlign);
    int frames = (int)(dest_size*efficiency);
    ;

    if (frames < 1)
        frames = 1;
    return frames * pthis->in_fmt.nBlockAlign;
}
