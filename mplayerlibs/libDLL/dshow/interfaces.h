#ifndef DS_INTERFACES_H
#define DS_INTERFACES_H

/*
 * Definition of important DirectShow interfaces.
 * Created using freely-available DirectX 8.0 SDK
 * ( http://msdn.microsoft.com )
 */

#include "iunk.h"


/*    Sh*t. MSVC++ and g++ use different methods of storing vtables.    */

typedef struct _IReferenceClock IReferenceClock;
typedef struct _IFilterGraph IFilterGraph;

typedef struct _IBaseFilter IBaseFilter;

typedef enum
{
    PINDIR_INPUT = 0,
    PINDIR_OUTPUT
} PIN_DIRECTION;

typedef struct _PinInfo
{
    IBaseFilter* pFilter;
    PIN_DIRECTION dir;
    unsigned short achName[128];
} PIN_INFO;

typedef struct _AllocatorProperties
{
    long cBuffers;
    long cbBuffer;
    long cbAlign;
    long cbPrefix;
} ALLOCATOR_PROPERTIES;

typedef struct _IEnumMediaTypes IEnumMediaTypes;
typedef struct IEnumMediaTypes_vt
{
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);


    HRESULT (__stdcall  *Next )(IEnumMediaTypes* This,
			      /* [in] */ unsigned long cMediaTypes,
			      /* [size_is][out] */ AM_MEDIA_TYPE** ppMediaTypes,
			      /* [out] */ unsigned long* pcFetched);
    HRESULT (__stdcall  *Skip )(IEnumMediaTypes* This,
			      /* [in] */ unsigned long cMediaTypes);
    HRESULT (__stdcall  *Reset )(IEnumMediaTypes* This);
    HRESULT (__stdcall  *Clone )(IEnumMediaTypes* This,
			       /* [out] */ IEnumMediaTypes** ppEnum);
} IEnumMediaTypes_vt;
struct _IEnumMediaTypes { IEnumMediaTypes_vt* vt; };



typedef struct _IPin IPin;
typedef struct IPin_vt
{
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);


    HRESULT (__stdcall  *Connect )(IPin * This,
				 /* [in] */ IPin *pReceivePin,
				 /* [in] */ /*const*/ AM_MEDIA_TYPE *pmt);
    HRESULT (__stdcall  *ReceiveConnection )(IPin * This,
					   /* [in] */ IPin *pConnector,
					   /* [in] */ const AM_MEDIA_TYPE *pmt);
    HRESULT (__stdcall  *Disconnect )(IPin * This);
    HRESULT (__stdcall  *ConnectedTo )(IPin * This, /* [out] */ IPin **pPin);
    HRESULT (__stdcall  *ConnectionMediaType )(IPin * This,
					     /* [out] */ AM_MEDIA_TYPE *pmt);
    HRESULT (__stdcall  *QueryPinInfo )(IPin * This, /* [out] */ PIN_INFO *pInfo);
    HRESULT (__stdcall  *QueryDirection )(IPin * This,
					/* [out] */ PIN_DIRECTION *pPinDir);
    HRESULT (__stdcall  *QueryId )(IPin * This, /* [out] */ unsigned short* *Id);
    HRESULT (__stdcall  *QueryAccept )(IPin * This,
				     /* [in] */ const AM_MEDIA_TYPE *pmt);
    HRESULT (__stdcall  *EnumMediaTypes )(IPin * This,
					/* [out] */ IEnumMediaTypes **ppEnum);
    HRESULT (__stdcall  *QueryInternalConnections )(IPin * This,
						  /* [out] */ IPin **apPin,
						  /* [out][in] */ unsigned long *nPin);
    HRESULT (__stdcall  *EndOfStream )(IPin * This);
    HRESULT (__stdcall  *BeginFlush )(IPin * This);
    HRESULT (__stdcall  *EndFlush )(IPin * This);
    HRESULT (__stdcall  *NewSegment )(IPin * This,
				    /* [in] */ REFERENCE_TIME tStart,
				    /* [in] */ REFERENCE_TIME tStop,
				    /* [in] */ double dRate);
} IPin_vt;
struct _IPin { IPin_vt *vt; };


typedef struct _IEnumPins IEnumPins;
typedef struct IEnumPins_vt
{
 
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    // retrieves a specified number of pins in the enumeration sequence..
    HRESULT (__stdcall  *Next )(IEnumPins* This,
			      /* [in] */ unsigned long cPins,
			      /* [size_is][out] */ IPin** ppPins,
			      /* [out] */ unsigned long* pcFetched);
    // skips over a specified number of pins.
    HRESULT (__stdcall  *Skip )(IEnumPins* This,
			      /* [in] */ unsigned long cPins);
    // resets the enumeration sequence to the beginning.
    HRESULT (__stdcall  *Reset )(IEnumPins* This);
    // makes a copy of the enumerator with the same enumeration state.
    HRESULT (__stdcall  *Clone )(IEnumPins* This,
			       /* [out] */ IEnumPins** ppEnum);
} IEnumPins_vt;
struct _IEnumPins { struct IEnumPins_vt* vt; };


typedef struct _IMediaSample IMediaSample;
typedef struct IMediaSample_vt
{
    
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *GetPointer )(IMediaSample* This,
				    /* [out] */ unsigned char** ppBuffer);
    LONG    (__stdcall  *GetSize )(IMediaSample* This);
    HRESULT (__stdcall  *GetTime )(IMediaSample* This,
				 /* [out] */ REFERENCE_TIME* pTimeStart,
				 /* [out] */ REFERENCE_TIME* pTimeEnd);
    HRESULT (__stdcall  *SetTime )(IMediaSample* This,
				 /* [in] */ REFERENCE_TIME* pTimeStart,
				 /* [in] */ REFERENCE_TIME* pTimeEnd);

    // sync-point property. If true, then the beginning of this
    // sample is a sync-point. (note that if AM_MEDIA_TYPE.bTemporalCompression
    // is false then all samples are sync points). A filter can start
    // a stream at any sync point.  S_FALSE if not sync-point, S_OK if true.
    HRESULT (__stdcall  *IsSyncPoint )(IMediaSample* This);
    HRESULT (__stdcall  *SetSyncPoint )(IMediaSample* This,
				      long bIsSyncPoint);

    // preroll property.  If true, this sample is for preroll only and
    // shouldn't be displayed.
    HRESULT (__stdcall  *IsPreroll )(IMediaSample* This);
    HRESULT (__stdcall  *SetPreroll )(IMediaSample* This,
				    long bIsPreroll);

    LONG    (__stdcall  *GetActualDataLength )(IMediaSample* This);
    HRESULT (__stdcall  *SetActualDataLength )(IMediaSample* This,
					     long __MIDL_0010);

    // these allow for limited format changes in band - if no format change
    // has been made when you receive a sample GetMediaType will return S_FALSE
    HRESULT (__stdcall  *GetMediaType )(IMediaSample* This,
				      AM_MEDIA_TYPE** ppMediaType);
    HRESULT (__stdcall  *SetMediaType )(IMediaSample* This,
				      AM_MEDIA_TYPE* pMediaType);

    // returns S_OK if there is a discontinuity in the data (this frame is
    // not a continuation of the previous stream of data
    // - there has been a seek or some dropped samples).
    HRESULT (__stdcall  *IsDiscontinuity )(IMediaSample* This);
    HRESULT (__stdcall  *SetDiscontinuity )(IMediaSample* This,
					  long bDiscontinuity);

    // get the media times for this sample
    HRESULT (__stdcall  *GetMediaTime )(IMediaSample* This,
				      /* [out] */ INT64* pTimeStart,
				      /* [out] */ INT64* pTimeEnd);
    // Set the media times for this sample
    // pTimeStart==pTimeEnd==NULL will invalidate the media time stamps in
    // this sample
    HRESULT (__stdcall  *SetMediaTime )(IMediaSample* This,
				      /* [in] */ INT64* pTimeStart,
				      /* [in] */ INT64* pTimeEnd);
} IMediaSample_vt;
struct _IMediaSample { struct IMediaSample_vt* vt; };



//typedef struct _IBaseFilter IBaseFilter;
typedef struct IBaseFilter_vt
{
   
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *GetClassID )(IBaseFilter * This,
				    /* [out] */ CLSID *pClassID);
    HRESULT (__stdcall  *Stop )(IBaseFilter * This);
    HRESULT (__stdcall  *Pause )(IBaseFilter * This);
    HRESULT (__stdcall  *Run )(IBaseFilter * This,
			     REFERENCE_TIME tStart);
    HRESULT (__stdcall  *GetState )(IBaseFilter * This,
				  /* [in] */ unsigned long dwMilliSecsTimeout,
				  ///* [out] */ FILTER_STATE *State);
				  void* State);
    HRESULT (__stdcall  *SetSyncSource )(IBaseFilter* This,
				       /* [in] */ IReferenceClock *pClock);
    HRESULT (__stdcall  *GetSyncSource )(IBaseFilter* This,
				       /* [out] */ IReferenceClock **pClock);
    HRESULT (__stdcall  *EnumPins )(IBaseFilter* This,
				  /* [out] */ IEnumPins **ppEnum);
    HRESULT (__stdcall  *FindPin )(IBaseFilter* This,
				 /* [string][in] */ const unsigned short* Id,
				 /* [out] */ IPin** ppPin);
    HRESULT (__stdcall  *QueryFilterInfo )(IBaseFilter* This,
					 // /* [out] */ FILTER_INFO *pInfo);
					 void* pInfo);
    HRESULT (__stdcall  *JoinFilterGraph )(IBaseFilter* This,
					 /* [in] */ IFilterGraph* pGraph,
					 /* [string][in] */ const unsigned short* pName);
    HRESULT (__stdcall  *QueryVendorInfo )(IBaseFilter* This,
					 /* [string][out] */ unsigned short** pVendorInfo);
} IBaseFilter_vt;
struct _IBaseFilter { struct IBaseFilter_vt* vt; };



typedef struct _IMemAllocator IMemAllocator;
typedef struct IMemAllocator_vt
{
   
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    // specifies the number of buffers to allocate and the size of each buffer.
    HRESULT (__stdcall  *SetProperties )(IMemAllocator* This,
				       /* [in] */ ALLOCATOR_PROPERTIES *pRequest,
				       /* [out] */ ALLOCATOR_PROPERTIES *pActual);
    // retrieves the number of buffers that the allocator will create, and the buffer properties.
    HRESULT (__stdcall  *GetProperties )(IMemAllocator* This,
				       /* [out] */ ALLOCATOR_PROPERTIES *pProps);
    // allocates the buffer memory.
    HRESULT (__stdcall  *Commit )(IMemAllocator* This);
    // releases the memory for the buffers.
    HRESULT (__stdcall  *Decommit )(IMemAllocator* This);
    // retrieves a media sample that contains an empty buffer.
    HRESULT (__stdcall  *GetBuffer )(IMemAllocator* This,
				   /* [out] */ IMediaSample** ppBuffer,
				   /* [in] */ REFERENCE_TIME* pStartTime,
				   /* [in] */ REFERENCE_TIME* pEndTime,
				   /* [in] */ unsigned long dwFlags);
    // releases a media sample.
    HRESULT (__stdcall  *ReleaseBuffer )(IMemAllocator* This,
				       /* [in] */ IMediaSample* pBuffer);
} IMemAllocator_vt;
struct _IMemAllocator { IMemAllocator_vt* vt; };



typedef struct _IMemInputPin IMemInputPin;
typedef struct IMemInputPin_vt
{
   
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *GetAllocator )(IMemInputPin * This, /* [out] */ IMemAllocator **ppAllocator);
    HRESULT (__stdcall  *NotifyAllocator )(IMemInputPin * This,/* [in] */ IMemAllocator *pAllocator,/* [in] */ int bReadOnly);
    HRESULT (__stdcall  *GetAllocatorRequirements )(IMemInputPin * This,/* [out] */ ALLOCATOR_PROPERTIES *pProps);
    HRESULT (__stdcall  *Receive )(IMemInputPin * This,/* [in] */ IMediaSample *pSample);
    HRESULT (__stdcall  *ReceiveMultiple )(IMemInputPin * This,/* [size_is][in] */ IMediaSample **pSamples,/* [in] */ long nSamples,/* [out] */ long *nSamplesProcessed);
    HRESULT (__stdcall  *ReceiveCanBlock )(IMemInputPin * This);
} IMemInputPin_vt;
struct _IMemInputPin { IMemInputPin_vt* vt; };


typedef struct _IHidden IHidden;
typedef struct IHidden_vt
{
    
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *GetSmth )(IHidden* This, int* pv);
    HRESULT (__stdcall  *SetSmth )(IHidden* This, int v1, int v2);
    HRESULT (__stdcall  *GetSmth2 )(IHidden* This, int* pv);
    HRESULT (__stdcall  *SetSmth2 )(IHidden* This, int v1, int v2);
    HRESULT (__stdcall  *GetSmth3 )(IHidden* This, int* pv);
    HRESULT (__stdcall  *SetSmth3 )(IHidden* This, int v1, int v2);
    HRESULT (__stdcall  *GetSmth4 )(IHidden* This, int* pv);
    HRESULT (__stdcall  *SetSmth4 )(IHidden* This, int v1, int v2);
    HRESULT (__stdcall  *GetSmth5 )(IHidden* This, int* pv);
    HRESULT (__stdcall  *SetSmth5 )(IHidden* This, int v1, int v2);
    HRESULT (__stdcall  *GetSmth6 )(IHidden* This, int* pv);
} IHidden_vt;
struct _IHidden { struct IHidden_vt* vt; };


typedef struct _IHidden2 IHidden2;
typedef struct IHidden2_vt
{
   
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *unk1 )(void);
    HRESULT (__stdcall  *unk2 )(void);
    HRESULT (__stdcall  *unk3 )(void);
    HRESULT (__stdcall  *DecodeGet )(IHidden2* This, int* region);
    HRESULT (__stdcall  *unk5 )(void);
    HRESULT (__stdcall  *DecodeSet )(IHidden2* This, int* region);
    HRESULT (__stdcall  *unk7 )(void);
    HRESULT (__stdcall  *unk8 )(void);
} IHidden2_vt;
struct _IHidden2 { struct IHidden2_vt* vt; };


// fixme
typedef struct IDivxFilterInterface {
    struct IDivxFilterInterface_vt* vt;
} IDivxFilterInterface;

struct IDivxFilterInterface_vt
{
   
    long (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long (__stdcall  *AddRef )(IUnknown * This); 
    long (__stdcall  *Release )(IUnknown * This);

    HRESULT (__stdcall  *get_PPLevel )(IDivxFilterInterface* This, int* PPLevel); // current postprocessing level
    HRESULT (__stdcall  *put_PPLevel )(IDivxFilterInterface* This, int PPLevel); // new postprocessing level
    HRESULT (__stdcall  *put_DefaultPPLevel )(IDivxFilterInterface* This);
    HRESULT (__stdcall  *put_MaxDelayAllowed )(IDivxFilterInterface* This, int maxdelayallowed);
    HRESULT (__stdcall  *put_Brightness )(IDivxFilterInterface* This,  int brightness);
    HRESULT (__stdcall  *put_Contrast )(IDivxFilterInterface* This,  int contrast);
    HRESULT (__stdcall  *put_Saturation )(IDivxFilterInterface* This, int saturation);
    HRESULT (__stdcall  *get_MaxDelayAllowed )(IDivxFilterInterface* This,  int* maxdelayallowed);
    HRESULT (__stdcall  *get_Brightness)(IDivxFilterInterface* This, int* brightness);
    HRESULT (__stdcall  *get_Contrast)(IDivxFilterInterface* This, int* contrast);
    HRESULT (__stdcall  *get_Saturation )(IDivxFilterInterface* This, int* saturation);
    HRESULT (__stdcall  *put_AspectRatio )(IDivxFilterInterface* This, int x, IDivxFilterInterface* Thisit, int y);
    HRESULT (__stdcall  *get_AspectRatio )(IDivxFilterInterface* This, int* x, IDivxFilterInterface* Thisit, int* y);
};

#endif  /* DS_INTERFACES_H */
