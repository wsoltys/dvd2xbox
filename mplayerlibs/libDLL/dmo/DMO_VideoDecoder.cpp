/********************************************************
 
	DirectShow Video decoder implementation
	Copyright 2000 Eugene Kuznetsov  (divx@euro.ru)
 
*********************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "../dshow/guids.h"
#include "../dshow/interfaces.h"
#include "../dshow/libwin32.h"
//#include "../ldt_keeper.h"
#include "DMO_Filter.h"

#include "DMO_VideoDecoder.h"
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)

//#include "DMO_VideoDecoder.h"

 
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>  // labs

#pragma warning (disable:4065)
// strcmp((const char*)info.dll,...)  is used instead of  (... == ...)
// so Arpi could use char* pointer in his simplified DMO_VideoDecoder class

#define __MODULE__ "DirectShow_VideoDecoder"

#define false 0
#define true 1
extern "C" void mp_msg( int x, int lev,const char *format, ... );
extern "C" void Setup_FS_Segment(void);
//int DMO_VideoDecoder_GetCapabilities(DMO_VideoDecoder *pThis){return pThis->m_Caps;}

typedef uint32_t fourcc_t;

struct _ct
{
    fourcc_t fcc;
    unsigned int bits;
    const GUID* subtype;
    int cap;
};
typedef struct _ct ct;

static ct check[] = {
                        { fccI420, 12, &MEDIASUBTYPE_I420, CAP_I420 },
                        { fccYV12, 12, &MEDIASUBTYPE_YV12, CAP_YV12 },
                        { fccYUY2, 16, &MEDIASUBTYPE_YUY2, CAP_YUY2 },
                        { fccUYVY, 16, &MEDIASUBTYPE_UYVY, CAP_UYVY },
                        { fccYVYU, 16, &MEDIASUBTYPE_YVYU, CAP_YVYU },
                        { fccIYUV, 24, &MEDIASUBTYPE_IYUV, CAP_IYUV },

                        {      8,  8, &MEDIASUBTYPE_RGB8, CAP_NONE },
                        {     15, 16, &MEDIASUBTYPE_RGB555, CAP_NONE },
                        {     16, 16, &MEDIASUBTYPE_RGB565, CAP_NONE },
                        {     24, 24, &MEDIASUBTYPE_RGB24, CAP_NONE },
                        {     32, 32, &MEDIASUBTYPE_RGB32, CAP_NONE },

                        {0},
                    };

extern "C" DMO_VideoDecoder * DMO_VideoDecoder_Open(char* dllname, GUID* guid, BITMAPINFOHEADER * format, int flip, int maxauto)
{
    DMO_VideoDecoder *pThis;
    HRESULT result;
    ct* c;

    pThis = (DMO_VideoDecoder*)malloc(sizeof(DMO_VideoDecoder));
    memset( pThis, 0, sizeof(DMO_VideoDecoder));

    pThis->m_sVhdr2 = 0;
    pThis->m_iLastQuality = -1;
    pThis->m_iMaxAuto = maxauto;

    //Setup_LDT_Keeper();


    //memset(&m_obh, 0, sizeof(m_obh));
    //m_obh.biSize = sizeof(m_obh);
    /*try*/
    {
        unsigned int bihs;

        bihs = (format->biSize < (int) sizeof(BITMAPINFOHEADER)) ?
               sizeof(BITMAPINFOHEADER) : format->biSize;

        pThis->iv.m_bh = (BITMAPINFOHEADER*)malloc(bihs);
        memcpy(pThis->iv.m_bh, format, bihs);

        pThis->iv.m_State = STOP;
        //pThis->iv.m_pFrame = 0;
        pThis->iv.m_Mode = DIRECT;
        pThis->iv.m_iDecpos = 0;
        pThis->iv.m_iPlaypos = -1;
        pThis->iv.m_fQuality = 0.0f;
        pThis->iv.m_bCapable16b = true;

        bihs += sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER);
        pThis->m_sVhdr = (VIDEOINFOHEADER*)malloc(bihs);
        memset(pThis->m_sVhdr, 0, bihs);
        memcpy(&pThis->m_sVhdr->bmiHeader, pThis->iv.m_bh, pThis->iv.m_bh->biSize);
        pThis->m_sVhdr->rcSource.left = pThis->m_sVhdr->rcSource.top = 0;
        pThis->m_sVhdr->rcSource.right = pThis->m_sVhdr->bmiHeader.biWidth;
        pThis->m_sVhdr->rcSource.bottom = pThis->m_sVhdr->bmiHeader.biHeight;
        //pThis->m_sVhdr->rcSource.right = 0;
        //pThis->m_sVhdr->rcSource.bottom = 0;
        pThis->m_sVhdr->rcTarget = pThis->m_sVhdr->rcSource;

        pThis->m_sOurType.majortype = MEDIATYPE_Video;
        pThis->m_sOurType.subtype = MEDIATYPE_Video;
        pThis->m_sOurType.subtype.f1 = pThis->m_sVhdr->bmiHeader.biCompression;
        pThis->m_sOurType.formattype = FORMAT_VideoInfo;
        pThis->m_sOurType.bFixedSizeSamples = false;
        pThis->m_sOurType.bTemporalCompression = true;
        pThis->m_sOurType.pUnk = 0;
        pThis->m_sOurType.cbFormat = bihs;
        pThis->m_sOurType.pbFormat = (char*)pThis->m_sVhdr;

        pThis->m_sVhdr2 = (VIDEOINFOHEADER*)(malloc(sizeof(VIDEOINFOHEADER)+12));
        memcpy(pThis->m_sVhdr2, pThis->m_sVhdr, sizeof(VIDEOINFOHEADER));
        memset((char*)pThis->m_sVhdr2 + sizeof(VIDEOINFOHEADER), 0, 12);
        pThis->m_sVhdr2->bmiHeader.biCompression = 0;
        pThis->m_sVhdr2->bmiHeader.biBitCount = 24;

        //	memset((char*)pThis->m_sVhdr2, 0, sizeof(VIDEOINFOHEADER)+12);
        pThis->m_sVhdr2->rcTarget = pThis->m_sVhdr->rcTarget;
        //	pThis->m_sVhdr2->rcSource = pThis->m_sVhdr->rcSource;

        memset(&pThis->m_sDestType, 0, sizeof(pThis->m_sDestType));
        pThis->m_sDestType.majortype = MEDIATYPE_Video;
        pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;
        pThis->m_sDestType.formattype = FORMAT_VideoInfo;
        pThis->m_sDestType.bFixedSizeSamples = true;
        pThis->m_sDestType.bTemporalCompression = false;
        pThis->m_sDestType.lSampleSize = labs(pThis->m_sVhdr2->bmiHeader.biWidth*pThis->m_sVhdr2->bmiHeader.biHeight
                                             * ((pThis->m_sVhdr2->bmiHeader.biBitCount + 7) / 8));
        pThis->m_sVhdr2->bmiHeader.biSizeImage = pThis->m_sDestType.lSampleSize;
        pThis->m_sDestType.pUnk = 0;
        pThis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);
        pThis->m_sDestType.pbFormat = (char*)pThis->m_sVhdr2;

        memset(&pThis->iv.m_obh, 0, sizeof(pThis->iv.m_obh));
        memcpy(&pThis->iv.m_obh, pThis->iv.m_bh, sizeof(pThis->iv.m_obh) < (unsigned) pThis->iv.m_bh->biSize
               ? sizeof(pThis->iv.m_obh) : (unsigned) pThis->iv.m_bh->biSize);
        pThis->iv.m_obh.biBitCount=24;
        pThis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
        pThis->iv.m_obh.biCompression = 0;	//BI_RGB
        //pThis->iv.m_obh.biHeight = labs(pThis->iv.m_obh.biHeight);
        pThis->iv.m_obh.biSizeImage = labs(pThis->iv.m_obh.biWidth * pThis->iv.m_obh.biHeight)
                                     * ((pThis->iv.m_obh.biBitCount + 7) / 8);


        pThis->m_pDMO_Filter = DMO_FilterCreate(dllname, guid, &pThis->m_sOurType, &pThis->m_sDestType);

        if (!pThis->m_pDMO_Filter)
        {
            //printf("Failed to create DMO filter\n");
            return 0;
        }

        if (!flip)
        {
            pThis->iv.m_obh.biHeight *= -1;
            pThis->m_sVhdr2->bmiHeader.biHeight = pThis->iv.m_obh.biHeight;
            //	    result = pThis->m_pDMO_Filter->m_pOutputPin->vt->QueryAccept(pThis->m_pDMO_Filter->m_pOutputPin, &pThis->m_sDestType);
            result = pThis->m_pDMO_Filter->m_pMedia->vt->SetOutputType(pThis->m_pDMO_Filter->m_pMedia, 0, &pThis->m_sDestType, DMO_SET_TYPEF_TEST_ONLY);
            if (result)
            {
                //printf("Decoder does not support upside-down RGB frames\n");
                pThis->iv.m_obh.biHeight *= -1;
                pThis->m_sVhdr2->bmiHeader.biHeight = pThis->iv.m_obh.biHeight;
            }
        }

        memcpy( &pThis->iv.m_decoder, &pThis->iv.m_obh, sizeof(pThis->iv.m_obh) );

        switch (pThis->iv.m_bh->biCompression)
        {
#if 0
        case fccDIV3:
        case fccDIV4:
        case fccDIV5:
        case fccDIV6:
        case fccMP42:
        case fccWMV2:
            //YV12 seems to be broken for DivX :-) codec
            //	case fccIV50:
            //produces incorrect picture
            //m_Caps = (CAPS) (m_Caps & ~CAP_YV12);
            //m_Caps = CAP_UYVY;//CAP_YUY2; // | CAP_I420;
            //m_Caps = CAP_I420;
            pThis->m_Caps = (CAP_YUY2 | CAP_UYVY);
            break;
#endif
        default:

            pThis->m_Caps = CAP_NONE;

            //printf("Decoder supports the following YUV formats: ");
            for (c = check; c->bits; c++)
            {
                pThis->m_sVhdr2->bmiHeader.biBitCount = c->bits;
                pThis->m_sVhdr2->bmiHeader.biCompression = c->fcc;
                pThis->m_sDestType.subtype = *c->subtype;
                //result = pThis->m_pDMO_Filter->m_pOutputPin->vt->QueryAccept(pThis->m_pDMO_Filter->m_pOutputPin, &pThis->m_sDestType);
                result = pThis->m_pDMO_Filter->m_pMedia->vt->SetOutputType(pThis->m_pDMO_Filter->m_pMedia, 0, &pThis->m_sDestType, DMO_SET_TYPEF_TEST_ONLY);
                if (!result)
                {
                    pThis->m_Caps = (pThis->m_Caps | c->cap);
                    //printf("%.4s ", (char*) &c->fcc);
                }
            }
            //printf("\n");
        }

        //if (pThis->m_Caps != CAP_NONE)
            //printf("Decoder is capable of YUV output (flags 0x%x)\n", (int)pThis->m_Caps);

        pThis->m_sVhdr2->bmiHeader.biBitCount = 24;
        pThis->m_sVhdr2->bmiHeader.biCompression = 0;
        pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;

        pThis->m_iMinBuffers = pThis->iv.VBUFSIZE;
    }
    /*catch (FatalError& error)
{
        delete[] m_sVhdr;
    delete[] m_sVhdr2;
        delete m_pDMO_Filter;
    throw;
}*/
    return pThis;
}

extern "C" void DMO_VideoDecoder_Destroy(DMO_VideoDecoder *pThis)
{
    DMO_VideoDecoder_StopInternal(pThis);
    pThis->iv.m_State = STOP;
    free(pThis->m_sVhdr);
    free(pThis->m_sVhdr2);
    DMO_Filter_Destroy(pThis->m_pDMO_Filter);
}

extern "C" void DMO_VideoDecoder_StartInternal(DMO_VideoDecoder *pThis)
{
#if 0
    ALLOCATOR_PROPERTIES props, props1;
    //printf("DMO_VideoDecoder_StartInternal\n");
    //cout << "DSSTART" << endl;
    pThis->m_pDMO_Filter->Start(pThis->m_pDMO_Filter);

    props.cBuffers = 1;
    props.cbBuffer = pThis->m_sDestType.lSampleSize;

    props.cbAlign = 1;
		props.cbPrefix = 0;
    pThis->m_pDMO_Filter->m_pAll->vt->SetProperties(pThis->m_pDMO_Filter->m_pAll, &props, &props1);
    pThis->m_pDMO_Filter->m_pAll->vt->Commit(pThis->m_pDMO_Filter->m_pAll);
#endif
    pThis->iv.m_State = START;
}

extern "C" void DMO_VideoDecoder_StopInternal(DMO_VideoDecoder *pThis)
{
    // pThis->m_pDMO_Filter->Stop(pThis->m_pDMO_Filter);
    //??? why was pThis here ??? m_pOurOutput->SetFramePointer(0);
}

extern "C" int DMO_VideoDecoder_DecodeInternal(DMO_VideoDecoder *pThis, const void* src, int size, int is_keyframe, char* imdata)
{
    //    IMediaSample* sample = 0;
//    char* ptr;
    int result;
    unsigned long status; // to be ignored by M$ specs
    DMO_OUTPUT_DATA_BUFFER db;
    CMediaBuffer* bufferin;
    //+    uint8_t* imdata = dest ? dest->Data() : 0;

    //printf("DMO_VideoDecoder_DecodeInternal(%p,%p,%d,%d,%p)\n",pThis,src,size,is_keyframe,imdata);

    //    pThis->m_pDMO_Filter->m_pAll->vt->GetBuffer(pThis->m_pDMO_Filter->m_pAll, &sample, 0, 0, 0);
    //    if (!sample)
    //    {
    //	printf("ERROR: null sample\n");
    //	return -1;
    //    }

    //Setup_FS_Segment();

    bufferin = CMediaBufferCreate(size, (void*)src, size, 0);
    result = pThis->m_pDMO_Filter->m_pMedia->vt->ProcessInput(pThis->m_pDMO_Filter->m_pMedia, 0,
             (IMediaBuffer*)bufferin,
             (is_keyframe) ? DMO_INPUT_DATA_BUFFERF_SYNCPOINT : 0,
             0, 0);
    ((IMediaBuffer*)bufferin)->vt->Release((IUnknown*)bufferin);

    if (result != S_OK)
    {
        /* something for process */
        if (result != S_FALSE)
            mp_msg(0,0,"ProcessInputError  r:0x%x=%d (keyframe: %d)\n", result, result, is_keyframe);
        else
            mp_msg(0,0,"ProcessInputError  FALSE ?? (keyframe: %d)\n", is_keyframe);
        return size;
    }

    db.rtTimestamp = 0;
    db.rtTimelength = 0;
    db.dwStatus = 0;
    db.pBuffer = (IMediaBuffer*) CMediaBufferCreate(pThis->m_sDestType.lSampleSize,
                 imdata, 0, 0);
    result = pThis->m_pDMO_Filter->m_pMedia->vt->ProcessOutput(pThis->m_pDMO_Filter->m_pMedia,
             (imdata) ? 0 : DMO_PROCESS_OUTPUT_DISCARD_WHEN_NO_BUFFER,
             1, &db, &status);
    //m_pDMO_Filter->m_pMedia->vt->Lock(m_pDMO_Filter->m_pMedia, 0);
    //if ((unsigned)result == DMO_E_NOTACCEPTING)
      //  printf("ProcessOutputError: Not accepting\n");
    //else if (result)
      //  printf("ProcessOutputError: r:0x%x=%d  %ld  stat:%ld\n", result, result, status, db.dwStatus);

    ((IMediaBuffer*)db.pBuffer)->vt->Release((IUnknown*)db.pBuffer);

    //int r = m_pDMO_Filter->m_pMedia->vt->Flush(m_pDMO_Filter->m_pMedia);
    //printf("FLUSH %d\n", r);

    return 0;
}

/*
 * bits == 0   - leave unchanged
 */
//int SetDestFmt(DMO_VideoDecoder * pThis, int bits = 24, fourcc_t csp = 0);
extern "C" int DMO_VideoDecoder_SetDestFmt(DMO_VideoDecoder *pThis, int bits, unsigned int csp)
{
    HRESULT result;
    int should_test=1;
    int stoped = 0;

    //printf("DMO_VideoDecoder_SetDestFmt (%p, %d, %d)\n",pThis,bits,(int)csp);

    /* if (!CImage::Supported(csp, bits))
    return -1;
    */
    // BitmapInfo temp = m_obh;

    if (!csp)	// RGB
    {
        int ok = true;

        switch (bits)
        {
        case 15:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB555;
            break;
        case 16:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB565;
            break;
        case 24:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;
            break;
        case 32:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_RGB32;
            break;
        default:
            ok = false;
            break;
        }

        if (ok)
        {
            pThis->iv.m_obh.biBitCount=bits;
            if( bits == 15 || bits == 16 )
            {
                pThis->iv.m_obh.biSize=sizeof(BITMAPINFOHEADER)+12;
                pThis->iv.m_obh.biCompression=3;//BI_BITFIELDS
                pThis->iv.m_obh.biSizeImage=abs((int)(2*pThis->iv.m_obh.biWidth*pThis->iv.m_obh.biHeight));
            }

            if( bits == 16 )
            {
                pThis->iv.m_obh.colors[0]=0xF800;
                pThis->iv.m_obh.colors[1]=0x07E0;
                pThis->iv.m_obh.colors[2]=0x001F;
            }
            else if ( bits == 15 )
            {
                pThis->iv.m_obh.colors[0]=0x7C00;
                pThis->iv.m_obh.colors[1]=0x03E0;
                pThis->iv.m_obh.colors[2]=0x001F;
            }
            else
            {
                pThis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
                pThis->iv.m_obh.biCompression = 0;	//BI_RGB
                //pThis->iv.m_obh.biHeight = labs(pThis->iv.m_obh.biHeight);
                pThis->iv.m_obh.biSizeImage = labs(pThis->iv.m_obh.biWidth * pThis->iv.m_obh.biHeight)
                                             * ((pThis->iv.m_obh.biBitCount + 7) / 8);
            }
        }
        //.biSizeImage=abs(temp.biWidth*temp.biHeight*((temp.biBitCount+7)/8));
    }
    else
    {	// YUV
        int ok = true;
        switch (csp)
        {
        case fccYUY2:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_YUY2;
            break;
        case fccYV12:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_YV12;
            break;
        case fccIYUV:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_IYUV;
            break;
        case fccI420:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_I420;
            break;
        case fccUYVY:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_UYVY;
            break;
        case fccYVYU:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_YVYU;
            break;
        case fccYVU9:
            pThis->m_sDestType.subtype = MEDIASUBTYPE_YVU9;
        default:
            ok = false;
            break;
        }

        if (ok)
        {
            if (csp != 0 && csp != 3 && pThis->iv.m_obh.biHeight > 0)
                pThis->iv.m_obh.biHeight *= -1; // YUV formats uses should have height < 0
            pThis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
            pThis->iv.m_obh.biCompression=csp;
            pThis->iv.m_obh.biBitCount=bits;
            pThis->iv.m_obh.biSizeImage=labs(pThis->iv.m_obh.biBitCount*
                                            pThis->iv.m_obh.biWidth*pThis->iv.m_obh.biHeight)>>3;
        }
    }
    pThis->m_sDestType.lSampleSize = pThis->iv.m_obh.biSizeImage;
    memcpy(&(pThis->m_sVhdr2->bmiHeader), &pThis->iv.m_obh, sizeof(pThis->iv.m_obh));
    pThis->m_sVhdr2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    if (pThis->m_sVhdr2->bmiHeader.biCompression == 3)
        pThis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER) + 12;
    else
        pThis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);


    switch(csp)
    {
    case fccYUY2:
        if(!(pThis->m_Caps & CAP_YUY2))
            should_test=false;
        break;
    case fccYV12:
        if(!(pThis->m_Caps & CAP_YV12))
            should_test=false;
        break;
    case fccIYUV:
        if(!(pThis->m_Caps & CAP_IYUV))
            should_test=false;
        break;
    case fccI420:
        if(!(pThis->m_Caps & CAP_I420))
            should_test=false;
        break;
    case fccUYVY:
        if(!(pThis->m_Caps & CAP_UYVY))
            should_test=false;
        break;
    case fccYVYU:
        if(!(pThis->m_Caps & CAP_YVYU))
            should_test=false;
        break;
    case fccYVU9:
        if(!(pThis->m_Caps & CAP_YVU9))
            should_test=false;
        break;
    }

    //Setup_FS_Segment();

    //    if(should_test)
    //	result = pThis->m_pDMO_Filter->m_pOutputPin->vt->QueryAccept(pThis->m_pDMO_Filter->m_pOutputPin, &pThis->m_sDestType);
    //    else
    //	result = -1;

    // test accept
    if(!pThis->m_pDMO_Filter)
        return 0;
    result = pThis->m_pDMO_Filter->m_pMedia->vt->SetOutputType(pThis->m_pDMO_Filter->m_pMedia, 0, &pThis->m_sDestType, DMO_SET_TYPEF_TEST_ONLY);

    if (result != 0)
    {
        if (csp)
          mp_msg(0,0,"Warning: unsupported color space\n");
        else
          mp_msg(0,0,"Warning: unsupported bit depth\n");

        pThis->m_sDestType.lSampleSize = pThis->iv.m_decoder.biSizeImage;
        memcpy(&(pThis->m_sVhdr2->bmiHeader), &pThis->iv.m_decoder, sizeof(pThis->iv.m_decoder));
        pThis->m_sVhdr2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        if (pThis->m_sVhdr2->bmiHeader.biCompression == 3)
            pThis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER) + 12;
        else
            pThis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);

        return -1;
    }

    memcpy( &pThis->iv.m_decoder, &pThis->iv.m_obh, sizeof(pThis->iv.m_obh));

    //    m_obh=temp;
    //    if(csp)
    //	m_obh.biBitCount=BitmapInfo::BitCount(csp);
    pThis->iv.m_bh->biBitCount = bits;

    //DMO_VideoDecoder_Restart(pThis);

    pThis->m_pDMO_Filter->m_pMedia->vt->SetOutputType(pThis->m_pDMO_Filter->m_pMedia, 0, &pThis->m_sDestType, 0);

    return 0;
}


extern "C" int DMO_VideoDecoder_SetDirection(DMO_VideoDecoder *pThis, int d)
{
    pThis->iv.m_obh.biHeight = (d) ? pThis->iv.m_bh->biHeight : -pThis->iv.m_bh->biHeight;
    pThis->m_sVhdr2->bmiHeader.biHeight = pThis->iv.m_obh.biHeight;
    return 0;
}

