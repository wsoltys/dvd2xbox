#ifndef DMO_INTERFACE_H
#define DMO_INTERFACE_H

#include "dmo.h"

/*
 * IMediaBuffer interface
 */
typedef struct _IMediaBuffer IMediaBuffer;
typedef struct IMediaBuffer_vt
{
    long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *SetLength )(IMediaBuffer* This,
				   unsigned long cbLength);
    HRESULT  (__stdcall *GetMaxLength )(IMediaBuffer* This,
				      /* [out] */ unsigned long *pcbMaxLength);
    HRESULT  (__stdcall *GetBufferAndLength )(IMediaBuffer* This,
					    /* [out] */ char** ppBuffer,
					    /* [out] */ unsigned long* pcbLength);
} IMediaBuffer_vt;
struct _IMediaBuffer { IMediaBuffer_vt* vt; };


typedef struct _DMO_OUTPUT_DATA_BUFFER
{
    IMediaBuffer *pBuffer;
    unsigned long dwStatus;
    REFERENCE_TIME rtTimestamp;
    REFERENCE_TIME rtTimelength;
} DMO_OUTPUT_DATA_BUFFER;


/*
 * IMediaObject interface
 */
typedef struct _IMediaObject IMediaObject;
typedef struct IMediaObject_vt
{
    long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *GetStreamCount )(IMediaObject * This,
					/* [out] */ unsigned long *pcInputStreams,
					/* [out] */ unsigned long *pcOutputStreams);
    HRESULT  (__stdcall *GetInputStreamInfo )(IMediaObject * This,
					    unsigned long dwInputStreamIndex,
					    /* [out] */ unsigned long *pdwFlags);
    HRESULT  (__stdcall *GetOutputStreamInfo )(IMediaObject * This,
					     unsigned long dwOutputStreamIndex,
					     /* [out] */ unsigned long *pdwFlags);
    HRESULT  (__stdcall *GetInputType )(IMediaObject * This,
				      unsigned long dwInputStreamIndex,
				      unsigned long dwTypeIndex,
				      /* [out] */ DMO_MEDIA_TYPE *pmt);
    HRESULT  (__stdcall *GetOutputType )(IMediaObject * This,
				       unsigned long dwOutputStreamIndex,
				       unsigned long dwTypeIndex,
				       /* [out] */ DMO_MEDIA_TYPE *pmt);
    HRESULT  (__stdcall *SetInputType )(IMediaObject * This,
				      unsigned long dwInputStreamIndex,
				      /* [in] */ const DMO_MEDIA_TYPE *pmt,
				      unsigned long dwFlags);
    HRESULT  (__stdcall *SetOutputType )(IMediaObject * This,
				       unsigned long dwOutputStreamIndex,
				       /* [in] */ const DMO_MEDIA_TYPE *pmt,
				       unsigned long dwFlags);
    HRESULT  (__stdcall *GetInputCurrentType )(IMediaObject * This,
					     unsigned long dwInputStreamIndex,
					     /* [out] */ DMO_MEDIA_TYPE *pmt);
    HRESULT  (__stdcall *GetOutputCurrentType )(IMediaObject * This,
					      unsigned long dwOutputStreamIndex,
					      /* [out] */ DMO_MEDIA_TYPE *pmt);
    HRESULT  (__stdcall *GetInputSizeInfo )(IMediaObject * This,
					  unsigned long dwInputStreamIndex,
					  /* [out] */ unsigned long *pcbSize,
					  /* [out] */ unsigned long *pcbMaxLookahead,
					  /* [out] */ unsigned long *pcbAlignment);
    HRESULT  (__stdcall *GetOutputSizeInfo )(IMediaObject * This,
					   unsigned long dwOutputStreamIndex,
					   /* [out] */ unsigned long *pcbSize,
					   /* [out] */ unsigned long *pcbAlignment);
    HRESULT  (__stdcall *GetInputMaxLatency )(IMediaObject * This,
					    unsigned long dwInputStreamIndex,
					    /* [out] */ REFERENCE_TIME *prtMaxLatency);
    HRESULT  (__stdcall *SetInputMaxLatency )(IMediaObject * This,
					    unsigned long dwInputStreamIndex,
					    REFERENCE_TIME rtMaxLatency);
    HRESULT  (__stdcall *Flush )(IMediaObject * This);
    HRESULT  (__stdcall *Discontinuity )(IMediaObject * This,
				       unsigned long dwInputStreamIndex);
    HRESULT  (__stdcall *AllocateStreamingResources )(IMediaObject * This);
    HRESULT  (__stdcall *FreeStreamingResources )(IMediaObject * This);
    HRESULT  (__stdcall *GetInputStatus )(IMediaObject * This,
					unsigned long dwInputStreamIndex,
					/* [out] */ unsigned long *dwFlags);
    HRESULT  (__stdcall *ProcessInput )(IMediaObject * This,
				      unsigned long dwInputStreamIndex,
				      IMediaBuffer *pBuffer,
				      unsigned long dwFlags,
				      REFERENCE_TIME rtTimestamp,
				      REFERENCE_TIME rtTimelength);
    HRESULT  (__stdcall *ProcessOutput )(IMediaObject * This,
				       unsigned long dwFlags,
				       unsigned long cOutputBufferCount,
				       /* [size_is][out][in] */ DMO_OUTPUT_DATA_BUFFER *pOutputBuffers,
				       /* [out] */ unsigned long *pdwStatus);
    HRESULT  (__stdcall *Lock )(IMediaObject * This, long bLock);
} IMediaObject_vt;
struct _IMediaObject { IMediaObject_vt* vt; };

/*
 * IEnumDMO interface
 */
typedef struct _IEnumDMO IEnumDMO;
typedef struct IEnumDMO_vt
{
    long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *Next )(IEnumDMO * This,
			      unsigned long cItemsToFetch,
			      /* [length_is][size_is][out] */ CLSID *pCLSID,
			      /* [string][length_is][size_is][out] */ WCHAR **Names,
			      /* [out] */ unsigned long *pcItemsFetched);
    HRESULT  (__stdcall *Skip )(IEnumDMO * This,
			      unsigned long cItemsToSkip);
    HRESULT  (__stdcall *Reset )(IEnumDMO * This);
    HRESULT  (__stdcall *Clone )(IEnumDMO * This,
			       /* [out] */ IEnumDMO **ppEnum);
} IEnumDMO_vt;
struct _IEnumDMO { IEnumDMO_vt* vt; };

/*
 * IMediaObjectInPlace interface
 */
typedef struct _IMediaObjectInPlace IMediaObjectInPlace;
typedef struct IMediaObjectInPlace_vt
{
    long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *Process )(IMediaObjectInPlace * This,
				 /* [in] */ unsigned long ulSize,
				 /* [size_is][out][in] */ BYTE *pData,
				 /* [in] */ REFERENCE_TIME refTimeStart,
				 /* [in] */ unsigned long dwFlags);
    HRESULT  (__stdcall *Clone )(IMediaObjectInPlace * This,
			       /* [out] */ IMediaObjectInPlace **ppMediaObject);
    HRESULT  (__stdcall *GetLatency )(IMediaObjectInPlace * This,
				    /* [out] */ REFERENCE_TIME *pLatencyTime);

} IMediaObjectInPlace_vt;
struct _IMediaObjectInPlace { IMediaObjectInPlace_vt* vt; };


/*
 * IDMOQualityControl interface
 */
typedef struct _IDMOQualityControl IDMOQualityControl;
typedef struct IDMOQualityControl_vt
{
        long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *SetNow )(IDMOQualityControl * This,
				/* [in] */ REFERENCE_TIME rtNow);
    HRESULT  (__stdcall *SetStatus )(IDMOQualityControl * This,
				   /* [in] */ unsigned long dwFlags);
    HRESULT  (__stdcall *GetStatus )(IDMOQualityControl * This,
				   /* [out] */ unsigned long *pdwFlags);
} IDMOQualityControl_vt;
struct _IDMOQualityControl { IDMOQualityControl_vt* vt; };

/*
 * IDMOVideoOutputOptimizations  interface
 */
typedef struct _IDMOVideoOutputOptimizations  IDMOVideoOutputOptimizations;
typedef struct IDMOVideoOutputOptimizations_vt
{
       long  (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); 
    long  (__stdcall  *AddRef )(IUnknown * This); 
    long  (__stdcall  *Release )(IUnknown * This);



    HRESULT  (__stdcall *QueryOperationModePreferences )(IDMOVideoOutputOptimizations * This,
						       unsigned long ulOutputStreamIndex,
						       unsigned long *pdwRequestedCapabilities);
    HRESULT  (__stdcall *SetOperationMode )(IDMOVideoOutputOptimizations * This,
					  unsigned long ulOutputStreamIndex,
					  unsigned long dwEnabledFeatures);
    HRESULT  (__stdcall *GetCurrentOperationMode )(IDMOVideoOutputOptimizations * This,
						 unsigned long ulOutputStreamIndex,
						 unsigned long *pdwEnabledFeatures);
    HRESULT  (__stdcall *GetCurrentSampleRequirements )(IDMOVideoOutputOptimizations * This,
						      unsigned long ulOutputStreamIndex,
						      unsigned long *pdwRequestedFeatures);
} IDMOVideoOutputOptimizations_vt;
struct _IDMOVideoOutputOptimizations { IDMOVideoOutputOptimizations_vt* vt; };

#endif /* DMO_INTERFACE_H */
