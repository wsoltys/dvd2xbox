#ifndef DSHOW_GUIDS_H
#define DSHOW_GUIDS_H

//#include "com.h"
//#include "wine/module.h"
//#include "wine/windef.h"

#include "../cygwin_inttypes.h"
#ifndef XTL_INCLUDED
#define E_FAIL						0x80004005
#define	E_INVALIDARG			0x80070057
#define E_POINTER					0x80004003
#define	E_OUTOFMEMORY			0x8007000E
#define E_NOINTERFACE			0x80004002
#define E_NOTIMPL					0x80004001
//#define Debug if(1)
#ifndef GUID_TYPE
#define GUID_TYPE
typedef struct
{
    uint32_t f1;
    uint16_t f2;
    uint16_t f3;
    uint8_t  f4[8];
} GUID;
#endif

typedef long (*GETCLASSOBJECT) (GUID* clsid, const GUID* iid, void** ppv);
int UnregisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);
int RegisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);
extern const GUID IID_IUnknown;
extern const GUID IID_IClassFactory;
#define STDCALL
struct IUnknown;
struct IClassFactory;
struct IUnknown_vt
{
    long STDCALL (__stdcall *QueryInterface)(struct IUnknown* _this, const GUID* iid, void** ppv);
    long STDCALL (__stdcall *AddRef)(struct IUnknown* _this) ;
    long STDCALL (__stdcall *Release)(struct IUnknown* _this) ;
} ;

typedef struct IUnknown
{
    struct IUnknown_vt* vt;
} IUnknown;

struct IClassFactory_vt
{
    long STDCALL (__stdcall *QueryInterface)(struct IUnknown* _this, const GUID* iid, void** ppv);
    long STDCALL (__stdcall *AddRef)(struct IUnknown* _this) ;
    long STDCALL (__stdcall *Release)(struct IUnknown* _this) ;
    long STDCALL (__stdcall *CreateInstance)(struct IClassFactory* _this, struct IUnknown* pUnkOuter, const GUID* riid, void** ppvObject);
};

struct IClassFactory
{
    struct IClassFactory_vt* vt;
};

long CoCreateInstance(GUID* rclsid, struct IUnknown* pUnkOuter,
		      long dwClsContext, const GUID* riid, void** ppv);



typedef __int64 REFERENCE_TIME;

typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef GUID CLSID;
typedef GUID IID;

extern const GUID IID_IBaseFilter;
extern const GUID IID_IEnumPins;
extern const GUID IID_IEnumMediaTypes;
extern const GUID IID_IMemInputPin;
extern const GUID IID_IMemAllocator;
extern const GUID IID_IMediaSample;
extern const GUID IID_DivxHidden;
extern const GUID IID_Iv50Hidden;
extern const GUID CLSID_DivxDecompressorCF;
extern const GUID IID_IDivxFilterInterface;
extern const GUID CLSID_IV50_Decoder;
extern const GUID CLSID_MemoryAllocator;
extern const GUID MEDIATYPE_Video;
extern const GUID GUID_NULL;
extern const GUID FORMAT_VideoInfo;
extern const GUID MEDIASUBTYPE_RGB1;
extern const GUID MEDIASUBTYPE_RGB4;
extern const GUID MEDIASUBTYPE_RGB8;
extern const GUID MEDIASUBTYPE_RGB565;
extern const GUID MEDIASUBTYPE_RGB555;
extern const GUID MEDIASUBTYPE_RGB24;
extern const GUID MEDIASUBTYPE_RGB32;
extern const GUID MEDIASUBTYPE_YUYV;
extern const GUID MEDIASUBTYPE_IYUV;
extern const GUID MEDIASUBTYPE_YVU9;
extern const GUID MEDIASUBTYPE_Y411;
extern const GUID MEDIASUBTYPE_Y41P;
extern const GUID MEDIASUBTYPE_YUY2;
extern const GUID MEDIASUBTYPE_YVYU;
extern const GUID MEDIASUBTYPE_UYVY;
extern const GUID MEDIASUBTYPE_Y211;
extern const GUID MEDIASUBTYPE_YV12;
extern const GUID MEDIASUBTYPE_I420;
extern const GUID MEDIASUBTYPE_IF09;

extern const GUID FORMAT_WaveFormatEx;
extern const GUID MEDIATYPE_Audio;
extern const GUID MEDIASUBTYPE_PCM;

extern void* XboxCoTaskMemAlloc(unsigned long cb);
extern void XboxCoTaskMemFree(void* cb);
#endif
typedef struct  _MediaType
{
    GUID	majortype;		//0x0
    GUID	subtype;		//0x10
    int		bFixedSizeSamples;	//0x20
    int		bTemporalCompression;	//0x24
    unsigned long lSampleSize;		//0x28
    GUID	formattype;		//0x2c
    IUnknown*	pUnk;			//0x3c
    unsigned long cbFormat;		//0x40
    char*	pbFormat;		//0x44
} AM_MEDIA_TYPE;
typedef AM_MEDIA_TYPE DMO_MEDIA_TYPE;
typedef struct  RECT32
{
    int left, top, right, bottom;
} RECT32;

typedef __int64 REFERENCE_TIME;
typedef struct  tagVIDEOINFOHEADER
{
    RECT32            rcSource;          // The bit we really want to use
    RECT32            rcTarget;          // Where the video should go
    unsigned long     dwBitRate;         // Approximate bit data rate
    unsigned long     dwBitErrorRate;    // Bit error rate for this stream
    REFERENCE_TIME    AvgTimePerFrame;   // Average time per frame (100ns units)
    BITMAPINFOHEADER  bmiHeader;
    //int               reserved[3];
} VIDEOINFOHEADER;

#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_

typedef struct  _WAVEFORMATEX {
  WORD   wFormatTag;
  WORD   nChannels;
  DWORD  nSamplesPerSec;
  DWORD  nAvgBytesPerSec;
  WORD   nBlockAlign;
  WORD   wBitsPerSample;
  WORD   cbSize;
} WAVEFORMATEX, *PWAVEFORMATEX, *NPWAVEFORMATEX, *LPWAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

#endif /* DS_GUIDS_H */
