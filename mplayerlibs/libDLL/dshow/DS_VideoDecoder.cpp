/********************************************************
 
	DirectShow Video decoder implementation
	Copyright 2000 Eugene Kuznetsov  (divx@euro.ru)
 
*********************************************************/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "guids.h"
#include "interfaces.h"
#include "DS_Filter.h"
#include "../pe_image.h"
#include "../module.h"
//#include "../ldt_keeper.h"
DECLARE_HANDLE(HKEY);

#include "../registry.h"

#define VFW_E_NOT_RUNNING               0x80040226

#include "DS_VideoDecoder.h"
extern "C" void mp_msg( int x, int lev,const char *format, ... );

extern "C" void Setup_FS_Segment(void);

#ifndef NOAVIFILE_HEADERS
#define VFW_E_NOT_RUNNING               0x80040226
//#include "fourcc.h"
//#include "except.h"
#endif

#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>  // labs

// strcmp((const char*)info.dll,...)  is used instead of  (... == ...)
// so Arpi could use char* pointer in his simplified DS_VideoDecoder class

#define __MODULE__ "DirectShow_VideoDecoder"

#define false 0
#define true 1
#pragma warning (disable:4065)
extern "C" int DS_VideoDecoder_GetCapabilities(DS_VideoDecoder *pthis)
{
    return pthis->m_Caps;
}

typedef struct _ct ct;

typedef uint32_t fourcc_t;
struct _ct
{
    unsigned int bits;
    fourcc_t fcc;
    const GUID *subtype;
    int cap;
};

// might be minimalized to contain just those which are needed by DS_VideoDecoder

#ifndef mmioFOURCC
#define mmioFOURCC( ch0, ch1, ch2, ch3 )				\
		( (long)(unsigned char)(ch0) | ( (long)(unsigned char)(ch1) << 8 ) |	\
		( (long)(unsigned char)(ch2) << 16 ) | ( (long)(unsigned char)(ch3) << 24 ) )
#endif /* mmioFOURCC */

/* OpenDivX */
#define fccMP4S	mmioFOURCC('M', 'P', '4', 'S')
#define fccmp4s	mmioFOURCC('m', 'p', '4', 's')
#define fccDIVX	mmioFOURCC('D', 'I', 'V', 'X')
#define fccdivx	mmioFOURCC('d', 'i', 'v', 'x')
#define fccDIV1	mmioFOURCC('D', 'I', 'V', '1')
#define fccdiv1	mmioFOURCC('d', 'i', 'v', '1')

/* DivX codecs */
#define fccDIV2 mmioFOURCC('D', 'I', 'V', '2')
#define fccdiv2 mmioFOURCC('d', 'i', 'v', '2')
#define fccDIV3 mmioFOURCC('D', 'I', 'V', '3')
#define fccdiv3 mmioFOURCC('d', 'i', 'v', '3')
#define fccDIV4 mmioFOURCC('D', 'I', 'V', '4')
#define fccdiv4 mmioFOURCC('d', 'i', 'v', '4')
#define fccDIV5 mmioFOURCC('D', 'I', 'V', '5')
#define fccdiv5 mmioFOURCC('d', 'i', 'v', '5')
#define fccDIV6 mmioFOURCC('D', 'I', 'V', '6')
#define fccdiv6 mmioFOURCC('d', 'i', 'v', '6')
#define fccMP41	mmioFOURCC('M', 'P', '4', '1')
#define fccmp41	mmioFOURCC('m', 'p', '4', '1')
#define fccMP43	mmioFOURCC('M', 'P', '4', '3')
#define fccmp43 mmioFOURCC('m', 'p', '4', '3')
/* old ms mpeg-4 codecs */
#define fccMP42	mmioFOURCC('M', 'P', '4', '2')
#define fccmp42	mmioFOURCC('m', 'p', '4', '2')
#define fccMPG4	mmioFOURCC('M', 'P', 'G', '4')
#define fccmpg4	mmioFOURCC('m', 'p', 'g', '4')
/* Windows media codecs */
#define fccWMV1 mmioFOURCC('W', 'M', 'V', '1')
#define fccwmv1 mmioFOURCC('w', 'm', 'v', '1')
#define fccWMV2 mmioFOURCC('W', 'M', 'V', '2')
#define fccwmv2 mmioFOURCC('w', 'm', 'v', '2')
#define fccMWV1 mmioFOURCC('M', 'W', 'V', '1')

/* Angel codecs */
#define fccAP41	mmioFOURCC('A', 'P', '4', '1')
#define fccap41	mmioFOURCC('a', 'p', '4', '1')
#define fccAP42	mmioFOURCC('A', 'P', '4', '2')
#define fccap42	mmioFOURCC('a', 'p', '4', '2')

/* other codecs	*/
#define fccIV31 mmioFOURCC('I', 'V', '3', '1')
#define fcciv31 mmioFOURCC('i', 'v', '3', '1')
#define fccIV32 mmioFOURCC('I', 'V', '3', '2')
#define fcciv32 mmioFOURCC('i', 'v', '3', '2')
#define fccIV41 mmioFOURCC('I', 'V', '4', '1')
#define fcciv41 mmioFOURCC('i', 'v', '4', '1')
#define fccIV50 mmioFOURCC('I', 'V', '5', '0')
#define fcciv50 mmioFOURCC('i', 'v', '5', '0')
#define fccI263 mmioFOURCC('I', '2', '6', '3')
#define fcci263 mmioFOURCC('i', '2', '6', '3')

#define fccMJPG mmioFOURCC('M', 'J', 'P', 'G')
#define fccmjpg mmioFOURCC('m', 'j', 'p', 'g')

#define fccHFYU mmioFOURCC('H', 'F', 'Y', 'U')

#define fcccvid mmioFOURCC('c', 'v', 'i', 'd')
#define fccdvsd mmioFOURCC('d', 'v', 's', 'd')

/* Ati codecs */
#define fccVCR2 mmioFOURCC('V', 'C', 'R', '2')
#define fccVCR1 mmioFOURCC('V', 'C', 'R', '1')
#define fccVYUY mmioFOURCC('V', 'Y', 'U', 'Y')
#define fccIYU9 mmioFOURCC('I', 'Y', 'U', '9') // it was defined as fccYVU9

/* Asus codecs */
#define fccASV1 mmioFOURCC('A', 'S', 'V', '1')
#define fccASV2 mmioFOURCC('A', 'S', 'V', '2')

/* Microsoft video */
#define fcccram mmioFOURCC('c', 'r', 'a', 'm')
#define fccCRAM mmioFOURCC('C', 'R', 'A', 'M')
#define fccMSVC mmioFOURCC('M', 'S', 'V', 'C')


#define fccMSZH mmioFOURCC('M', 'S', 'Z', 'H')

#define fccZLIB mmioFOURCC('Z', 'L', 'I', 'B')

#define fccTM20 mmioFOURCC('T', 'M', '2', '0')

#define fccYUV  mmioFOURCC('Y', 'U', 'V', ' ')
#define fccYUY2 mmioFOURCC('Y', 'U', 'Y', '2')
#define fccYV12 mmioFOURCC('Y', 'V', '1', '2')/* Planar mode: Y + V + U  (3 planes) */
#define fccI420 mmioFOURCC('I', '4', '2', '0')
#define fccIYUV mmioFOURCC('I', 'Y', 'U', 'V')/* Planar mode: Y + U + V  (3 planes) */
#define fccUYVY mmioFOURCC('U', 'Y', 'V', 'Y')/* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define fccYVYU mmioFOURCC('Y', 'V', 'Y', 'U')/* Packed mode: Y0+V0+Y1+U0 (1 plane) */
#define fccYVU9 mmioFOURCC('Y', 'V', 'U', '9')/* Planar 4:1:0 */
#define fccIF09 mmioFOURCC('I', 'F', '0', '9')/* Planar 4:1:0 + delta */
    enum CAPS
    {
	CAP_NONE = 0,
	CAP_YUY2 = 1,
	CAP_YV12 = 2,
	CAP_IYUV = 4,
	CAP_UYVY = 8,
	CAP_YVYU = 16,
	CAP_I420 = 32,
	CAP_YVU9 = 64,
	CAP_IF09 = 128,
    };
    enum DecodingMode
    {
	DIRECT = 0,
	REALTIME,
	REALTIME_QUALITY_AUTO,
    };
    enum DecodingState
    {
	STOP = 0,
	START,
    };

static ct check[] = {
                        {16, fccYUY2, &MEDIASUBTYPE_YUY2, CAP_YUY2},
                        {12, fccIYUV, &MEDIASUBTYPE_IYUV, CAP_IYUV},
                        {16, fccUYVY, &MEDIASUBTYPE_UYVY, CAP_UYVY},
                        {12, fccYV12, &MEDIASUBTYPE_YV12, CAP_YV12},
                        //{16, fccYV12, &MEDIASUBTYPE_YV12, CAP_YV12},
                        {16, fccYVYU, &MEDIASUBTYPE_YVYU, CAP_YVYU},
                        {12, fccI420, &MEDIASUBTYPE_I420, CAP_I420},
                        {9,  fccYVU9, &MEDIASUBTYPE_YVU9, CAP_YVU9},
                        {0, 0, 0, 0},
                    };


extern "C" DS_VideoDecoder * DS_VideoDecoder_Open(char* dllname, GUID* guid, BITMAPINFOHEADER * format, int flip, int maxauto)
{
    DS_VideoDecoder *pthis;
    HRESULT result;
    ct* c;

    pthis = (DS_VideoDecoder*)malloc(sizeof(DS_VideoDecoder));
    memset( pthis, 0, sizeof(DS_VideoDecoder));

    pthis->m_sVhdr2 = 0;
    pthis->m_iLastQuality = -1;
    pthis->m_iMaxAuto = maxauto;

    //Setup_LDT_Keeper();

    //memset(&m_obh, 0, sizeof(m_obh));
    //m_obh.biSize = sizeof(m_obh);
    /*try*/
    {
        unsigned int bihs;

        bihs = (format->biSize < (int) sizeof(BITMAPINFOHEADER)) ?
               sizeof(BITMAPINFOHEADER) : format->biSize;

        pthis->iv.m_bh = (BITMAPINFOHEADER*)malloc(bihs);
        memcpy(pthis->iv.m_bh, format, bihs);

        pthis->iv.m_State = STOP;
        //pthis->iv.m_pFrame = 0;
        pthis->iv.m_Mode = DIRECT;
        pthis->iv.m_iDecpos = 0;
        pthis->iv.m_iPlaypos = -1;
        pthis->iv.m_fQuality = 0.0f;
        pthis->iv.m_bCapable16b = true;

        bihs += sizeof(VIDEOINFOHEADER) - sizeof(BITMAPINFOHEADER);
        pthis->m_sVhdr = (VIDEOINFOHEADER*)malloc(bihs);
        memset(pthis->m_sVhdr, 0, bihs);
        memcpy(&pthis->m_sVhdr->bmiHeader, pthis->iv.m_bh, pthis->iv.m_bh->biSize);
        pthis->m_sVhdr->rcSource.left = pthis->m_sVhdr->rcSource.top = 0;
        pthis->m_sVhdr->rcSource.right = pthis->m_sVhdr->bmiHeader.biWidth;
        pthis->m_sVhdr->rcSource.bottom = pthis->m_sVhdr->bmiHeader.biHeight;
        //pthis->m_sVhdr->rcSource.right = 0;
        //pthis->m_sVhdr->rcSource.bottom = 0;
        pthis->m_sVhdr->rcTarget = pthis->m_sVhdr->rcSource;

        pthis->m_sOurType.majortype = MEDIATYPE_Video;
        pthis->m_sOurType.subtype = MEDIATYPE_Video;
        pthis->m_sOurType.subtype.f1 = pthis->m_sVhdr->bmiHeader.biCompression;
        pthis->m_sOurType.formattype = FORMAT_VideoInfo;
        pthis->m_sOurType.bFixedSizeSamples = false;
        pthis->m_sOurType.bTemporalCompression = true;
        pthis->m_sOurType.pUnk = 0;
        pthis->m_sOurType.cbFormat = bihs;
        pthis->m_sOurType.pbFormat = (char*)pthis->m_sVhdr;

        pthis->m_sVhdr2 = (VIDEOINFOHEADER*)(malloc(sizeof(VIDEOINFOHEADER)+12));
        memcpy(pthis->m_sVhdr2, pthis->m_sVhdr, sizeof(VIDEOINFOHEADER));
        memset((char*)pthis->m_sVhdr2 + sizeof(VIDEOINFOHEADER), 0, 12);
        pthis->m_sVhdr2->bmiHeader.biCompression = 0;
        pthis->m_sVhdr2->bmiHeader.biBitCount = 24;

        memset(&pthis->m_sDestType, 0, sizeof(pthis->m_sDestType));
        pthis->m_sDestType.majortype = MEDIATYPE_Video;
        pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;
        pthis->m_sDestType.formattype = FORMAT_VideoInfo;
        pthis->m_sDestType.bFixedSizeSamples = true;
        pthis->m_sDestType.bTemporalCompression = false;
        pthis->m_sDestType.lSampleSize = labs(pthis->m_sVhdr2->bmiHeader.biWidth*pthis->m_sVhdr2->bmiHeader.biHeight
                                             * ((pthis->m_sVhdr2->bmiHeader.biBitCount + 7) / 8));
        pthis->m_sVhdr2->bmiHeader.biSizeImage = pthis->m_sDestType.lSampleSize;
        pthis->m_sDestType.pUnk = 0;
        pthis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);
        pthis->m_sDestType.pbFormat = (char*)pthis->m_sVhdr2;

        memset(&pthis->iv.m_obh, 0, sizeof(pthis->iv.m_obh));
        memcpy(&pthis->iv.m_obh, pthis->iv.m_bh, sizeof(pthis->iv.m_obh) < (unsigned) pthis->iv.m_bh->biSize
               ? sizeof(pthis->iv.m_obh) : (unsigned) pthis->iv.m_bh->biSize);
        pthis->iv.m_obh.biBitCount=24;
        pthis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
        pthis->iv.m_obh.biCompression = 0;	//BI_RGB
        //pthis->iv.m_obh.biHeight = labs(pthis->iv.m_obh.biHeight);
        pthis->iv.m_obh.biSizeImage = labs(pthis->iv.m_obh.biWidth * pthis->iv.m_obh.biHeight)
                                     * ((pthis->iv.m_obh.biBitCount + 7) / 8);


        pthis->m_pDS_Filter = DS_FilterCreate(dllname, guid, &pthis->m_sOurType, &pthis->m_sDestType);

        if (!pthis->m_pDS_Filter)
        {
            mp_msg(0,0,"Failed to create DirectShow filter\n");
						free(pthis);
            return 0;
        }

        if (!flip)
        {
            pthis->iv.m_obh.biHeight *= -1;
            pthis->m_sVhdr2->bmiHeader.biHeight = pthis->iv.m_obh.biHeight;
            result = pthis->m_pDS_Filter->m_pOutputPin->vt->QueryAccept(pthis->m_pDS_Filter->m_pOutputPin, &pthis->m_sDestType);
            if (result)
            {
                mp_msg(0,0,"Decoder does not support upside-down RGB frames\n");
                pthis->iv.m_obh.biHeight *= -1;
                pthis->m_sVhdr2->bmiHeader.biHeight = pthis->iv.m_obh.biHeight;
            }
        }

        memcpy( &pthis->iv.m_decoder, &pthis->iv.m_obh, sizeof(pthis->iv.m_obh) );

        switch (pthis->iv.m_bh->biCompression)
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
            pthis->m_Caps = (CAP_YUY2 | CAP_UYVY);
            break;
#endif
        default:

            pthis->m_Caps = CAP_NONE;

            mp_msg(0,0,"Decoder supports the following YUV formats: ");
            for (c = check; c->bits; c++)
            {
                pthis->m_sVhdr2->bmiHeader.biBitCount = c->bits;
                pthis->m_sVhdr2->bmiHeader.biCompression = c->fcc;
                pthis->m_sDestType.subtype = *c->subtype;
                result = pthis->m_pDS_Filter->m_pOutputPin->vt->QueryAccept(pthis->m_pDS_Filter->m_pOutputPin, &pthis->m_sDestType);
                if (!result)
                {
                    pthis->m_Caps = (pthis->m_Caps | c->cap);
                    mp_msg(0,0,"%.4s ", (char *)&c->fcc);
                }
            }
            mp_msg(0,0,"\n");
        }

        if (pthis->m_Caps != CAP_NONE)
          mp_msg(0,0,"Decoder is capable of YUV output (flags 0x%x)\n", (int)pthis->m_Caps);

        pthis->m_sVhdr2->bmiHeader.biBitCount = 24;
        pthis->m_sVhdr2->bmiHeader.biCompression = 0;
        pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;

        pthis->m_iMinBuffers = pthis->iv.VBUFSIZE;
        pthis->m_bIsDivX = (strcmp(dllname, "divxcvki.ax") == 0
                           || strcmp(dllname, "divx_c32.ax") == 0
                           || strcmp(dllname, "wmvds32.ax") == 0
                           || strcmp(dllname, "wmv8ds32.ax") == 0);
        pthis->m_bIsDivX4 = (strcmp(dllname, "divxdec.ax") == 0);
        if (pthis->m_bIsDivX)
            pthis->iv.VBUFSIZE += 7;
        else if (pthis->m_bIsDivX4)
            pthis->iv.VBUFSIZE += 9;
    }
    /*catch (FatalError& error)
{
        delete[] m_sVhdr;
    delete[] m_sVhdr2;
        delete m_pDS_Filter;
    throw;
}*/
    return pthis;
}

extern "C" void DS_VideoDecoder_Destroy(DS_VideoDecoder *pthis)
{
    DS_VideoDecoder_StopInternal(pthis);
    pthis->iv.m_State = STOP;
    free(pthis->m_sVhdr);
    free(pthis->m_sVhdr2);
    DS_Filter_Destroy(pthis->m_pDS_Filter);
}

extern "C" void DS_VideoDecoder_StartInternal(DS_VideoDecoder *pthis)
{
    ALLOCATOR_PROPERTIES props, props1;
    //mp_msg(0,0,"DS_VideoDecoder_StartInternal\n");
    //cout << "DSSTART" << endl;
    pthis->m_pDS_Filter->Start(pthis->m_pDS_Filter);

    props.cBuffers = 1;
    props.cbBuffer = pthis->m_sDestType.lSampleSize;

    //don't know how to do pthis correctly
    props.cbAlign = 1;
		props.cbPrefix = 0;
			pthis->m_pDS_Filter->m_pAll->vt->SetProperties(pthis->m_pDS_Filter->m_pAll, &props, &props1);
			pthis->m_pDS_Filter->m_pAll->vt->Commit(pthis->m_pDS_Filter->m_pAll);

    pthis->iv.m_State = START;
}

extern "C" void DS_VideoDecoder_StopInternal(DS_VideoDecoder *pthis)
{
    pthis->m_pDS_Filter->Stop(pthis->m_pDS_Filter);
    //??? why was pthis here ??? m_pOurOutput->SetFramePointer(0);
}

extern "C" int DS_VideoDecoder_DecodeInternal(DS_VideoDecoder *pthis, const void* src, int size, int is_keyframe, char* pImage)
{
    IMediaSample* sample = 0;
    char* ptr;
    int result;

    //mp_msg(0,0,"DS_VideoDecoder_DecodeInternal(%p,%p,%d,%d,%p)\n",pthis,src,size,is_keyframe,pImage);

    pthis->m_pDS_Filter->m_pAll->vt->GetBuffer(pthis->m_pDS_Filter->m_pAll, &sample, 0, 0, 0);

    if (!sample)
    {
        mp_msg(0,0,"ERROR: null sample\n");
        return -1;
    }

    //cout << "DECODE " << (void*) pImage << "   d: " << (void*) pImage->Data() << endl;
    if (pImage)
    {
        pthis->m_pDS_Filter->m_pOurOutput->SetPointer2(pthis->m_pDS_Filter->m_pOurOutput,pImage);
    }


    sample->vt->SetActualDataLength(sample, size);
    sample->vt->GetPointer(sample, (BYTE **)&ptr);
    memcpy(ptr, src, size);
    sample->vt->SetSyncPoint(sample, is_keyframe);
    sample->vt->SetPreroll(sample, pImage ? 0 : 1);
    // sample->vt->SetMediaType(sample, &m_sOurType);

    // FIXME: - crashing with YV12 at pthis place decoder will crash
    //          while doing pthis call
    // %FS register was not setup for calling into win32 dll. Are all
    // crashes inside ...->Receive() fixed now?
    //
    // nope - but pthis is surely helpfull - I'll try some more experiments
    //Setup_FS_Segment();
#if 0
    if (!pthis->m_pDS_Filter || !pthis->m_pDS_Filter->m_pImp
            || !pthis->m_pDS_Filter->m_pImp->vt
            || !pthis->m_pDS_Filter->m_pImp->vt->Receive)
        printf("DecodeInternal ERROR???\n");
#endif
    result = pthis->m_pDS_Filter->m_pImp->vt->Receive(pthis->m_pDS_Filter->m_pImp, sample);
    if (result)
    {
        mp_msg(0,0,"DS_VideoDecoder::DecodeInternal() error putting data into input pin %x\n", result);
    }

    sample->vt->Release((IUnknown*)sample);

#if 0
    if (pthis->m_bIsDivX)
    {
        int q;
        IHidden* hidden=(IHidden*)((int)pthis->m_pDS_Filter->m_pFilter + 0xb8);
        // always check for actual value
        // pthis seems to be the only way to know the actual value
        hidden->vt->GetSmth2(hidden, &pthis->m_iLastQuality);
        if (pthis->m_iLastQuality > 9)
            pthis->m_iLastQuality -= 10;

        if (pthis->m_iLastQuality < 0)
            pthis->m_iLastQuality = 0;
        else if (pthis->m_iLastQuality > pthis->m_iMaxAuto)
            pthis->m_iLastQuality = pthis->m_iMaxAuto;

        //cout << " Qual: " << pthis->m_iLastQuality << endl;
        pthis->iv.m_fQuality = pthis->m_iLastQuality / 4.0;
    }
    else if (pthis->m_bIsDivX4)
    {

        // maybe access methods directly to safe some cpu cycles...
        DS_VideoDecoder_GetValue(pthis, "Postprocessing", pthis->m_iLastQuality);
        if (pthis->m_iLastQuality < 0)
            pthis->m_iLastQuality = 0;
        else if (pthis->m_iLastQuality > pthis->m_iMaxAuto)
            pthis->m_iLastQuality = pthis->m_iMaxAuto;

        //cout << " Qual: " << m_iLastQuality << endl;
        pthis->iv.m_fQuality = pthis->m_iLastQuality / 6.0;
    }

    if (pthis->iv.m_Mode == -1 ) // ???BUFFERED_QUALITY_AUTO)
    {
        // adjust Quality - depends on how many cached frames we have
        int buffered = pthis->iv.m_iDecpos - pthis->iv.m_iPlaypos;

        if (pthis->m_bIsDivX || pthis->m_bIsDivX4)
        {
            int to = buffered - pthis->m_iMinBuffers;
            if (to < 0)
                to = 0;
            if (to != pthis->m_iLastQuality)
            {
                if (to > pthis->m_iMaxAuto)
                    to = pthis->m_iMaxAuto;
                if (pthis->m_iLastQuality != to)
                {
                    if (pthis->m_bIsDivX)
                    {
                        IHidden* hidden=(IHidden*)((int)pthis->m_pDS_Filter->m_pFilter + 0xb8);
                        hidden->vt->SetSmth(hidden, to, 0);
                    }
                    else
                        DS_VideoDecoder_SetValue(pthis, "Postprocessing", to);
#ifndef QUIET
                    //mp_msg(0,0,"Switching quality %d -> %d  b:%d\n",m_iLastQuality, to, buffered);
#endif
                }
            }
        }
    }
#endif

    return 0;
}

/*
 * bits == 0   - leave unchanged
 */
//int SetDestFmt(DS_VideoDecoder * pthis, int bits = 24, fourcc_t csp = 0);
extern "C" int DS_VideoDecoder_SetDestFmt(DS_VideoDecoder *pthis, int bits, unsigned int csp)
{
    HRESULT result;
    int should_test=1;
    int stoped = 0;

    //mp_msg(0,0,"DS_VideoDecoder_SetDestFmt (%p, %d, %d)\n",pthis,bits,(int)csp);

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
            pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB555;
            break;
        case 16:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB565;
            break;
        case 24:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB24;
            break;
        case 32:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_RGB32;
            break;
        default:
            ok = false;
            break;
        }

        if (ok)
        {
						if (bits == 15)
								pthis->iv.m_obh.biBitCount=16;
						else
								pthis->iv.m_obh.biBitCount=bits;

            if( bits == 15 || bits == 16 )
            {
                pthis->iv.m_obh.biSize=sizeof(BITMAPINFOHEADER)+12;
                pthis->iv.m_obh.biCompression=3;//BI_BITFIELDS
                pthis->iv.m_obh.biSizeImage=abs((int)(2*pthis->iv.m_obh.biWidth*pthis->iv.m_obh.biHeight));
            }

            if( bits == 16 )
            {
                pthis->iv.m_obh.colors[0]=0xF800;
                pthis->iv.m_obh.colors[1]=0x07E0;
                pthis->iv.m_obh.colors[2]=0x001F;
            }
            else if ( bits == 15 )
            {
                pthis->iv.m_obh.colors[0]=0x7C00;
                pthis->iv.m_obh.colors[1]=0x03E0;
                pthis->iv.m_obh.colors[2]=0x001F;
            }
            else
            {
                pthis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
                pthis->iv.m_obh.biCompression = 0;	//BI_RGB
                //pthis->iv.m_obh.biHeight = labs(pthis->iv.m_obh.biHeight);
                pthis->iv.m_obh.biSizeImage = labs(pthis->iv.m_obh.biWidth * pthis->iv.m_obh.biHeight)
                                             * ((pthis->iv.m_obh.biBitCount + 7) / 8);
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
            pthis->m_sDestType.subtype = MEDIASUBTYPE_YUY2;
            break;
        case fccYV12:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_YV12;
            break;
        case fccIYUV:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_IYUV;
            break;
        case fccI420:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_I420;
            break;
        case fccUYVY:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_UYVY;
            break;
        case fccYVYU:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_YVYU;
            break;
        case fccYVU9:
            pthis->m_sDestType.subtype = MEDIASUBTYPE_YVU9;
        default:
            ok = false;
            break;
        }

        if (ok)
        {
            if (csp != 0 && csp != 3 && pthis->iv.m_obh.biHeight > 0)
                pthis->iv.m_obh.biHeight *= -1; // YUV formats uses should have height < 0
            pthis->iv.m_obh.biSize = sizeof(BITMAPINFOHEADER);
            pthis->iv.m_obh.biCompression=csp;
            pthis->iv.m_obh.biBitCount=bits;
            pthis->iv.m_obh.biSizeImage=labs(pthis->iv.m_obh.biBitCount*
                                            pthis->iv.m_obh.biWidth*pthis->iv.m_obh.biHeight)>>3;
        }
    }
    pthis->m_sDestType.lSampleSize = pthis->iv.m_obh.biSizeImage;
    memcpy(&(pthis->m_sVhdr2->bmiHeader), &pthis->iv.m_obh, sizeof(pthis->iv.m_obh));
    pthis->m_sVhdr2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    if (pthis->m_sVhdr2->bmiHeader.biCompression == 3)
        pthis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER) + 12;
    else
        pthis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);


    switch(csp)
    {
    case fccYUY2:
        if(!(pthis->m_Caps & CAP_YUY2))
            should_test=false;
        break;
    case fccYV12:
        if(!(pthis->m_Caps & CAP_YV12))
            should_test=false;
        break;
    case fccIYUV:
        if(!(pthis->m_Caps & CAP_IYUV))
            should_test=false;
        break;
    case fccI420:
        if(!(pthis->m_Caps & CAP_I420))
            should_test=false;
        break;
    case fccUYVY:
        if(!(pthis->m_Caps & CAP_UYVY))
            should_test=false;
        break;
    case fccYVYU:
        if(!(pthis->m_Caps & CAP_YVYU))
            should_test=false;
        break;
    case fccYVU9:
        if(!(pthis->m_Caps & CAP_YVU9))
            should_test=false;
        break;
    }
    if(should_test)
        result = pthis->m_pDS_Filter->m_pOutputPin->vt->QueryAccept(pthis->m_pDS_Filter->m_pOutputPin, &pthis->m_sDestType);
    else
        result = -1;

    if (result != 0)
    {
        if (csp)
            mp_msg(0,0,"Warning: unsupported color space\n");
        else
            mp_msg(0,0,"Warning: unsupported bit depth\n");

        pthis->m_sDestType.lSampleSize = pthis->iv.m_decoder.biSizeImage;
        memcpy(&(pthis->m_sVhdr2->bmiHeader), &pthis->iv.m_decoder, sizeof(pthis->iv.m_decoder));
        pthis->m_sVhdr2->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        if (pthis->m_sVhdr2->bmiHeader.biCompression == 3)
            pthis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER) + 12;
        else
            pthis->m_sDestType.cbFormat = sizeof(VIDEOINFOHEADER);

        return -1;
    }

    memcpy( &pthis->iv.m_decoder, &pthis->iv.m_obh, sizeof(pthis->iv.m_obh));

    //    m_obh=temp;
    //    if(csp)
    //	m_obh.biBitCount=BitmapInfo::BitCount(csp);
    pthis->iv.m_bh->biBitCount = bits;

    //DS_VideoDecoder_Restart(pthis);

    if (pthis->iv.m_State == START)
    {
        DS_VideoDecoder_StopInternal(pthis);
        pthis->iv.m_State = STOP;
        stoped = true;
    }

    pthis->m_pDS_Filter->m_pInputPin->vt->Disconnect(pthis->m_pDS_Filter->m_pInputPin);
    pthis->m_pDS_Filter->m_pOutputPin->vt->Disconnect(pthis->m_pDS_Filter->m_pOutputPin);
    pthis->m_pDS_Filter->m_pOurOutput->SetNewFormat(pthis->m_pDS_Filter->m_pOurOutput,&pthis->m_sDestType);
    result = pthis->m_pDS_Filter->m_pInputPin->vt->ReceiveConnection(pthis->m_pDS_Filter->m_pInputPin,
             pthis->m_pDS_Filter->m_pOurInput,
             &pthis->m_sOurType);
    if (result)
    {
        mp_msg(0,0,"Error reconnecting input pin 0x%x\n", (int)result);
        return -1;
    }
    result = pthis->m_pDS_Filter->m_pOutputPin->vt->ReceiveConnection(pthis->m_pDS_Filter->m_pOutputPin,
             (IPin *)pthis->m_pDS_Filter->m_pOurOutput,
             &pthis->m_sDestType);
    if (result)
    {
        mp_msg(0,0,"Error reconnecting output pin 0x%x\n", (int)result);
        return -1;
    }

    if (stoped)
    {
        DS_VideoDecoder_StartInternal(pthis);
        pthis->iv.m_State = START;
    }

    return 0;
}


extern "C" int DS_VideoDecoder_SetDirection(DS_VideoDecoder *pthis, int d)
{
    pthis->iv.m_obh.biHeight = (d) ? pthis->iv.m_bh->biHeight : -pthis->iv.m_bh->biHeight;
    pthis->m_sVhdr2->bmiHeader.biHeight = pthis->iv.m_obh.biHeight;
    return 0;
}

extern "C" int DS_VideoDecoder_GetValue(DS_VideoDecoder *pthis, const char* name, int* value)
{
    /*
        if (m_bIsDivX4)
        {
    	IDivxFilterInterface* pIDivx;
    	if (m_pDS_Filter->m_pFilter->vt->QueryInterface((IUnknown*)m_pDS_Filter->m_pFilter, &IID_IDivxFilterInterface, (void**)&pIDivx))
    	{
    	    printf("No such interface\n");
    	    return -1;
    	}
    	if (strcmp(name, "Postprocessing") == 0)
    	{
    	    pIDivx->vt->get_PPLevel(pIDivx, &value);
    	    value /= 10;
    	}
    	else if (strcmp(name, "Brightness") == 0)
    	    pIDivx->vt->get_Brightness(pIDivx, &value);
    	else if (strcmp(name, "Contrast") == 0)
    	    pIDivx->vt->get_Contrast(pIDivx, &value);
    	else if (strcmp(name, "Saturation") == 0)
    	    pIDivx->vt->get_Saturation(pIDivx, &value);
    	else if (strcmp(name, "MaxAuto") == 0)
    	    value = m_iMaxAuto;
    	pIDivx->vt->Release((IUnknown*)pIDivx);
    	return 0;
        }
        else if (m_bIsDivX)
        {
    	if (m_State != START)
    	    return VFW_E_NOT_RUNNING;
    // brightness 87
    // contrast 74
    // hue 23
    // saturation 20
    // post process mode 0
    // get1 0x01
    // get2 10
    // get3=set2 86
    // get4=set3 73
    // get5=set4 19
    // get6=set5 23
    	IHidden* hidden=(IHidden*)((int)m_pDS_Filter->m_pFilter+0xb8);
    	if (strcmp(name, "Quality") == 0)
    	{
    #warning NOT SURE
    	    int r = hidden->vt->GetSmth2(hidden, &value);
    	    if (value >= 10)
    		value -= 10;
    	    return 0;
    	}
    	if (strcmp(name, "Brightness") == 0)
    	    return hidden->vt->GetSmth3(hidden, &value);
    	if (strcmp(name, "Contrast") == 0)
    	    return hidden->vt->GetSmth4(hidden, &value);
    	if (strcmp(name, "Hue") == 0)
    	    return hidden->vt->GetSmth6(hidden, &value);
    	if (strcmp(name, "Saturation") == 0)
    	    return hidden->vt->GetSmth5(hidden, &value);
    	if (strcmp(name, "MaxAuto") == 0)
    	{
    	    value = m_iMaxAuto;
                return 0;
    	}
        }
        else if (strcmp((const char*)record.dll, "ir50_32.dll") == 0)
        {
    	IHidden2* hidden = 0;
    	if (m_pDS_Filter->m_pFilter->vt->QueryInterface((IUnknown*)m_pDS_Filter->m_pFilter, &IID_Iv50Hidden, (void**)&hidden))
    	{
    	    printf("No such interface\n");
    	    return -1;
    	}
    #warning FIXME
    	int recordpar[30];
    	recordpar[0]=0x7c;
    	recordpar[1]=fccIV50;
    	recordpar[2]=0x10005;
    	recordpar[3]=2;
    	recordpar[4]=1;
    	recordpar[5]=0x80000000;
     
    	if (strcmp(name, "Brightness") == 0)
    	    recordpar[5]|=0x20;
    	else if (strcmp(name, "Saturation") == 0)
    	    recordpar[5]|=0x40;
    	else if (strcmp(name, "Contrast") == 0)
    	    recordpar[5]|=0x80;
    	if (!recordpar[5])
    	{
    	    hidden->vt->Release((IUnknown*)hidden);
    	    return -1;
    	}
    	if (hidden->vt->DecodeSet(hidden, recordpar))
    	    return -1;
     
    	if (strcmp(name, "Brightness") == 0)
    	    value = recordpar[18];
    	else if (strcmp(name, "Saturation") == 0)
    	    value = recordpar[19];
    	else if (strcmp(name, "Contrast") == 0)
    	    value = recordpar[20];
     
    	hidden->vt->Release((IUnknown*)hidden);
        }
    */
    return 0;
}

extern "C" int DS_VideoDecoder_SetValue(DS_VideoDecoder *pthis, const char* name, int value)
{
    if (pthis->m_bIsDivX4)
    {
        IDivxFilterInterface* pIDivx=NULL;
        //	printf("DS_SetValue for DIVX4, name=%s  value=%d\n",name,value);
        if (pthis->m_pDS_Filter->m_pFilter->vt->QueryInterface((IUnknown*)pthis->m_pDS_Filter->m_pFilter, &IID_IDivxFilterInterface, (void**)&pIDivx))
        {
            mp_msg(0,0,"No such interface\n");
            return -1;
        }
        if (strcmp(name, "Postprocessing") == 0)
            pIDivx->vt->put_PPLevel(pIDivx, value * 10);
        else if (strcmp(name, "Brightness") == 0)
            pIDivx->vt->put_Brightness(pIDivx, value);
        else if (strcmp(name, "Contrast") == 0)
            pIDivx->vt->put_Contrast(pIDivx, value);
        else if (strcmp(name, "Saturation") == 0)
            pIDivx->vt->put_Saturation(pIDivx, value);
        else if (strcmp(name, "MaxAuto") == 0)
            pthis->m_iMaxAuto = value;
        pIDivx->vt->Release((IUnknown*)pIDivx);
        mp_msg(0,0,"Set %s  %d\n", name, value);
        return 0;
    }

    if (pthis->m_bIsDivX)
    {
        IHidden* hidden;
        if (pthis->iv.m_State != START)
            return VFW_E_NOT_RUNNING;

        //cout << "set value " << name << "  " << value << endl;
        // brightness 87
        // contrast 74
        // hue 23
        // saturation 20
        // post process mode 0
        // get1 0x01
        // get2 10
        // get3=set2 86
        // get4=set3 73
        // get5=set4 19
        // get6=set5 23
        hidden = (IHidden*)((int)pthis->m_pDS_Filter->m_pFilter + 0xb8);
        mp_msg(0,0,"DS_SetValue for DIVX, name=%s  value=%d\n",name,value);
        if (strcmp(name, "Quality") == 0)
        {
            pthis->m_iLastQuality = value;
            return hidden->vt->SetSmth(hidden, value, 0);
        }
        if (strcmp(name, "Brightness") == 0)
            return hidden->vt->SetSmth2(hidden, value, 0);
        if (strcmp(name, "Contrast") == 0)
            return hidden->vt->SetSmth3(hidden, value, 0);
        if (strcmp(name, "Saturation") == 0)
            return hidden->vt->SetSmth4(hidden, value, 0);
        if (strcmp(name, "Hue") == 0)
            return hidden->vt->SetSmth5(hidden, value, 0);
        if (strcmp(name, "MaxAuto") == 0)
        {
            pthis->m_iMaxAuto = value;
        }
        return 0;
    }
#if 0
    if (strcmp((const char*)record.dll, "ir50_32.dll") == 0)
    {
        IHidden2* hidden = 0;
        if (m_pDS_Filter->m_pFilter->vt->QueryInterface((IUnknown*)m_pDS_Filter->m_pFilter, &IID_Iv50Hidden, (void**)&hidden))
        {
            mp_msg(0,0,"No such interface\n");
            return -1;
        }
        int recordpar[30];
        recordpar[0]=0x7c;
        recordpar[1]=fccIV50;
        recordpar[2]=0x10005;
        recordpar[3]=2;
        recordpar[4]=1;
        recordpar[5]=0x80000000;
        if (strcmp(name, "Brightness") == 0)
        {
            recordpar[5]|=0x20;
            recordpar[18]=value;
        }
        else if (strcmp(name, "Saturation") == 0)
        {
            recordpar[5]|=0x40;
            recordpar[19]=value;
        }
        else if (strcmp(name, "Contrast") == 0)
        {
            recordpar[5]|=0x80;
            recordpar[20]=value;
        }
        if(!recordpar[5])
        {
            hidden->vt->Release((IUnknown*)hidden);
            return -1;
        }
        HRESULT result = hidden->vt->DecodeSet(hidden, recordpar);
        hidden->vt->Release((IUnknown*)hidden);

        return result;
    }
#endif
    //    printf("DS_SetValue for ????, name=%s  value=%d\n",name,value);
    return 0;
}
/*
vim: vi* sux.
*/

int DS_SetAttr_DivX(char* attribute, int value)
{
    int result, status;
		int newkey;
    if(strcmp(attribute, "Quality")==0)
    {
        char* keyname="SOFTWARE\\Microsoft\\Scrunch";
        result=RegCreateKeyExA((long)HKEY_CURRENT_USER, keyname, 0, 0, 0, 0, 0,	   		&newkey, &status);
        if(result!=0)
        {
            printf("VideoDecoder::SetExtAttr: registry failure\n");
            return -1;
        }
        result=RegSetValueExA((long)newkey, "Current Post Process Mode", 0, REG_DWORD, &value, 4);
        if(result!=0)
        {
            printf("VideoDecoder::SetExtAttr: error writing value\n");
            return -1;
        }
        value=-1;
        result=RegSetValueExA(newkey, "Force Post Process Mode", 0, REG_DWORD, &value, 4);
        if(result!=0)
        {
            printf("VideoDecoder::SetExtAttr: error writing value\n");
            return -1;
        }
        RegCloseKey(newkey);
        return 0;
    }

    if(
        (strcmp(attribute, "Saturation")==0) ||
        (strcmp(attribute, "Hue")==0) ||
        (strcmp(attribute, "Contrast")==0) ||
        (strcmp(attribute, "Brightness")==0)
    )
    {
        char* keyname="SOFTWARE\\Microsoft\\Scrunch\\Video";
        result=RegCreateKeyExA((long)HKEY_CURRENT_USER, keyname, 0, 0, 0, 0, 0,	   		&newkey, &status);
        if(result!=0)
        {
            printf("VideoDecoder::SetExtAttr: registry failure\n");
            return -1;
        }
        result=RegSetValueExA((long)newkey, attribute, 0, REG_DWORD, &value, 4);
        if(result!=0)
        {
            printf("VideoDecoder::SetExtAttr: error writing value\n");
            return -1;
        }
        RegCloseKey(newkey);
        return 0;
    }

    mp_msg(0,0,"Unknown attribute!\n");
    return -200;
}
