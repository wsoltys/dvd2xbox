#ifndef AVIFILE_DS_AUDIODECODER_H
#define AVIFILE_DS_AUDIODECODER_H

typedef struct _DS_AudioDecoder DS_AudioDecoder;
#ifdef __cplusplus
extern "C" {
#endif

//DS_AudioDecoder * DS_AudioDecoder_Create(const CodecInfo * info, const WAVEFORMATEX* wf);
DS_AudioDecoder * DS_AudioDecoder_Open(char* dllname, GUID* guid, WAVEFORMATEX* wf);

void DS_AudioDecoder_Destroy(DS_AudioDecoder *pthis);

int DS_AudioDecoder_Convert(DS_AudioDecoder *pthis, const void* in_data, unsigned int in_size,
			     void* out_data, unsigned int out_size,
			     unsigned int* size_read, unsigned int* size_written);

int DS_AudioDecoder_GetSrcSize(DS_AudioDecoder *pthis, int dest_size);
#ifdef __cplusplus
}
#endif

#endif // AVIFILE_DS_AUDIODECODER_H
