#ifndef AVIFILE_DMO_AUDIODECODER_H
#define AVIFILE_DMO_AUDIODECODER_H
#include "../dshow/guids.h"
#include "dmo_filter.h"

struct _DMO_AudioDecoder
{
    DMO_MEDIA_TYPE m_sOurType, m_sDestType;
    DMO_Filter* m_pDMO_Filter;
    char* m_sVhdr;
    char* m_sVhdr2;
    int m_iFlushed;
};

typedef struct _DMO_AudioDecoder DMO_AudioDecoder;

#ifdef __cplusplus
extern "C" {
#endif

DMO_AudioDecoder * DMO_AudioDecoder_Open(char* dllname, GUID* guid, WAVEFORMATEX* wf,int out_channels);

void DMO_AudioDecoder_Destroy(DMO_AudioDecoder *pThis);

int DMO_AudioDecoder_Convert(DMO_AudioDecoder *pThis, const void* in_data, unsigned int in_size,
			     void* out_data, unsigned int out_size,
			     unsigned int* size_read, unsigned int* size_written);

int DMO_AudioDecoder_GetSrcSize(DMO_AudioDecoder *pThis, int dest_size);

#ifdef __cplusplus
}
#endif

#endif // AVIFILE_DMO_AUDIODECODER_H
