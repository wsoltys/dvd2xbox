#ifndef AVIFILE_DMO_VIDEODECODER_H
#define AVIFILE_DMO_VIDEODECODER_H
#include "../dshow/ds_videodecoder.h"
#include "dmo_filter.h"
struct _DMO_VideoDecoder
{
    IVideoDecoder iv;

    DMO_Filter* m_pDMO_Filter;
    AM_MEDIA_TYPE m_sOurType, m_sDestType;
    VIDEOINFOHEADER* m_sVhdr;
    VIDEOINFOHEADER* m_sVhdr2;
    int m_Caps;//CAPS m_Caps;                // capabilities of DirectShow decoder
    int m_iLastQuality;         // remember last quality as integer
    int m_iMinBuffers;
    int m_iMaxAuto;
};
typedef struct _DMO_VideoDecoder DMO_VideoDecoder;

#ifdef __cplusplus
extern "C" {
#endif

int DMO_VideoDecoder_GetCapabilities(DMO_VideoDecoder *pThis);

DMO_VideoDecoder * DMO_VideoDecoder_Open(char* dllname, GUID* guid, BITMAPINFOHEADER * format, int flip, int maxauto);

void DMO_VideoDecoder_Destroy(DMO_VideoDecoder *pThis);

void DMO_VideoDecoder_StartInternal(DMO_VideoDecoder *pThis);

void DMO_VideoDecoder_StopInternal(DMO_VideoDecoder *pThis);

int DMO_VideoDecoder_DecodeInternal(DMO_VideoDecoder *pThis, const void* src, int size, int is_keyframe, char* pImage);

/*
 * bits == 0   - leave unchanged
 */
//int SetDestFmt(DMO_VideoDecoder * pThis, int bits = 24, fourcc_t csp = 0);
int DMO_VideoDecoder_SetDestFmt(DMO_VideoDecoder *pThis, int bits, unsigned int csp);
int DMO_VideoDecoder_SetDirection(DMO_VideoDecoder *pThis, int d);

#ifdef __cplusplus
}
#endif

#endif /* AVIFILE_DMO_VIDEODECODER_H */
