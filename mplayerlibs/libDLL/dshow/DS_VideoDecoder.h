#ifndef AVIFILE_DS_VIDEODECODER_H
#define AVIFILE_DS_VIDEODECODER_H
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "guids.h"
#include "interfaces.h"
#include "DS_Filter.h"


typedef struct _CodecInfo
{
    char* dll;
    GUID* guid;
}CodecInfo;

typedef struct _BitmapInfo
{
    long 	biSize;
    long  	biWidth;
    long  	biHeight;
    short 	biPlanes;
    short 	biBitCount;
    long 	biCompression;
    long 	biSizeImage;
    long  	biXPelsPerMeter;
    long  	biYPelsPerMeter;
    long 	biClrUsed;
    long 	biClrImportant;
    int 	colors[3];    
} BitmapInfo;

typedef struct _IVideoDecoder
{
    int VBUFSIZE;
    int QMARKHI;
    int QMARKLO;
    int DMARKHI;
    int DMARKLO;

    /*
    IVideoDecoder(CodecInfo& info, const BITMAPINFOHEADER& format) : record(info)
    {
        // implement init part
    }
    virtual ~IVideoDecoder();
    void Stop()
    {
    }
    void Start()
    {
    }
    */
    const CodecInfo record;
    int m_Mode;	// should we do precaching (or even change Quality on the fly)
    int m_State;
    int m_iDecpos;
    int m_iPlaypos;
    float m_fQuality;           // quality for the progress bar 0..1(best)
    int m_bCapable16b;

    BITMAPINFOHEADER* m_bh;	// format of input data (might be larger - e.g. huffyuv)
    BitmapInfo m_decoder;	// format of decoder output
    BitmapInfo m_obh;		// format of returned frames
}IVideoDecoder;


struct _DS_VideoDecoder
{
    IVideoDecoder iv;

    DS_Filter* m_pDS_Filter;
    AM_MEDIA_TYPE m_sOurType, m_sDestType;
    VIDEOINFOHEADER* m_sVhdr;
    VIDEOINFOHEADER* m_sVhdr2;
    int m_Caps;//CAPS m_Caps;                // capabilities of DirectShow decoder
    int m_iLastQuality;         // remember last quality as integer
    int m_iMinBuffers;
    int m_iMaxAuto;
    int m_bIsDivX;             // for speed
    int m_bIsDivX4;            // for speed
} ;

typedef struct _DS_VideoDecoder DS_VideoDecoder;

#ifdef __cplusplus
extern "C" {
#endif
int DS_VideoDecoder_GetCapabilities(DS_VideoDecoder *pthis);

DS_VideoDecoder * DS_VideoDecoder_Open(char* dllname, GUID* guid, BITMAPINFOHEADER * format, int flip, int maxauto);

void DS_VideoDecoder_Destroy(DS_VideoDecoder *pthis);

void DS_VideoDecoder_StartInternal(DS_VideoDecoder *pthis);

void DS_VideoDecoder_StopInternal(DS_VideoDecoder *pthis);

int DS_VideoDecoder_DecodeInternal(DS_VideoDecoder *pthis, const void* src, int size, int is_keyframe, char* pImage);

/*
 * bits == 0   - leave unchanged
 */
//int SetDestFmt(DS_VideoDecoder * pthis, int bits = 24, fourcc_t csp = 0);
int DS_VideoDecoder_SetDestFmt(DS_VideoDecoder *pthis, int bits, unsigned int csp);
int DS_VideoDecoder_SetDirection(DS_VideoDecoder *pthis, int d);
int DS_VideoDecoder_GetValue(DS_VideoDecoder *pthis, const char* name, int* value);
int DS_VideoDecoder_SetValue(DS_VideoDecoder *pthis, const char* name, int value);
int DS_SetAttr_DivX(char* attribute, int value);

#ifdef __cplusplus
}
#endif

#endif /* AVIFILE_DS_VIDEODECODER_H */
