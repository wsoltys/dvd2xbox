#include "guids.h"
#include "cmediasample.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * currently hack to make some extra room for DS Acel codec which
 * seems to overwrite allocated memory - FIXME better later
 * check the buffer allocation
 */
static const int SAFETY_ACEL = 1024;
extern "C" void mp_msg( int x, int lev,const char *format, ... );

static long __stdcall  CMediaSample_QueryInterface(IUnknown* This,
        /* [in] */ const GUID* iid,
        /* [iid_is][out] */ void **ppv)
{
    //mp_msg(0,0,"CMediaSample_QueryInterface(%p) called\n", This);
    if (!ppv)
		{
				mp_msg(0,0,"CMediaSample_QueryInterface failed");
        return E_INVALIDARG;
		}
    if (memcmp(iid, &IID_IUnknown, sizeof(*iid)) == 0)
    {
        *ppv = (void*)This;
        ((IMediaSample*) This)->vt->AddRef(This);
        return 0;
    }
    if (memcmp(iid, &IID_IMediaSample, sizeof(*iid)) == 0)
    {
        *ppv = (void*)This;
        ((IMediaSample*) This)->vt->AddRef(This);
        return 0;
    }
		mp_msg(0,0,"CMediaSample_QueryInterface failed");
    return E_NOINTERFACE;
}

static long __stdcall  CMediaSample_AddRef(IUnknown* This)
{
    //mp_msg(0,0,"CMediaSample_AddRef(%p) called\n", This);
    ((CMediaSample*)This)->refcount++;
    return 0;
}

void CMediaSample_Destroy(CMediaSample* This)
{

    //mp_msg(0,0,"CMediaSample_Destroy(%p) called (ref:%d)\n", This, This->refcount);
    free(This->vt);
    free(This->own_block);
    if (This->media_type.pbFormat)
        free(This->media_type.pbFormat);
    free(This);
}

static long __stdcall  CMediaSample_Release(IUnknown* This)
{
    CMediaSample* parent = (CMediaSample*)This;
    //mp_msg(0,0,"CMediaSample_Release(%p) called  (new ref:%d)\n",This, ((CMediaSample*)This)->refcount-1);

    if (--((CMediaSample*) This)->refcount == 0)
    {
        parent->all->vt->ReleaseBuffer((IMemAllocator*)(parent->all),
                                       (IMediaSample*)This);
    }
    return 0;
}

static HRESULT __stdcall  CMediaSample_GetPointer(IMediaSample* This,/* [out] */ BYTE** ppBuffer)
{
    //mp_msg(0,0,"CMediaSample_GetPointer(%p) called -> %p, size: %d  %d\n", This, ((CMediaSample*) This)->block, ((CMediaSample*)This)->actual_size, ((CMediaSample*)This)->size);
    if (!ppBuffer)
		{
				mp_msg(0,0,"CMediaSample_GetPointer failed");
        return E_INVALIDARG;
		}
    *ppBuffer = (BYTE*) ((CMediaSample*) This)->block;
    return 0;
}

static long __stdcall  CMediaSample_GetSize(IMediaSample * This)
{
    //mp_msg(0,0,"CMediaSample_GetSize(%p) called -> %d\n", This, ((CMediaSample*) This)->size);
    return ((CMediaSample*) This)->size;
}

static HRESULT __stdcall  CMediaSample_GetTime(IMediaSample * This,
        /* [out] */ REFERENCE_TIME *pTimeStart,
        /* [out] */ REFERENCE_TIME *pTimeEnd)
{
    //mp_msg(0,0,"CMediaSample_GetTime(%p) called (UNIMPLEMENTED)\n", This);
    return E_NOTIMPL;
}

static HRESULT __stdcall  CMediaSample_SetTime(IMediaSample * This,
        /* [in] */ REFERENCE_TIME *pTimeStart,
        /* [in] */ REFERENCE_TIME *pTimeEnd)
{
    //mp_msg(0,0,"CMediaSample_SetTime(%p) called (UNIMPLEMENTED)\n", This);
    return E_NOTIMPL;
}

static HRESULT __stdcall  CMediaSample_IsSyncPoint(IMediaSample * This)
{
    //mp_msg(0,0,"CMediaSample_IsSyncPoint(%p) called\n", This);
    if (((CMediaSample*)This)->isSyncPoint)
        return 0;
    return 1;
}

static HRESULT __stdcall  CMediaSample_SetSyncPoint(IMediaSample * This,
        long bIsSyncPoint)
{
    //mp_msg(0,0,"CMediaSample_SetSyncPoint(%p) called\n", This);
    ((CMediaSample*)This)->isSyncPoint = bIsSyncPoint;
    return 0;
}

static HRESULT __stdcall  CMediaSample_IsPreroll(IMediaSample * This)
{
    //mp_msg(0,0,"CMediaSample_IsPreroll(%p) called\n", This);

    if (((CMediaSample*)This)->isPreroll)
        return 0;//S_OK

    return 1;//S_FALSE
}

static HRESULT __stdcall  CMediaSample_SetPreroll(IMediaSample * This,
        long bIsPreroll)
{
    //mp_msg(0,0,"CMediaSample_SetPreroll(%p) called\n", This);
    ((CMediaSample*)This)->isPreroll=bIsPreroll;
    return 0;
}

static long __stdcall  CMediaSample_GetActualDataLength(IMediaSample* This)
{
    //mp_msg(0,0,"CMediaSample_GetActualDataLength(%p) called -> %d\n", This, ((CMediaSample*)This)->actual_size);
    return ((CMediaSample*)This)->actual_size;
}

static HRESULT __stdcall  CMediaSample_SetActualDataLength(IMediaSample* This,
        long __MIDL_0010)
{
    CMediaSample* cms = (CMediaSample*)This;
    //mp_msg(0,0,"CMediaSample_SetActualDataLength(%p, %ld) called\n", This, __MIDL_0010);

    if (__MIDL_0010 > cms->size)
    {
        char* c = cms->own_block;
        //mp_msg(0,0,"CMediaSample - buffer overflow   %ld %d   %p %p\n",__MIDL_0010, ((CMediaSample*)This)->size, cms->own_block, cms->block);
        cms->own_block = (char*) realloc(cms->own_block, (size_t) __MIDL_0010 + SAFETY_ACEL);
        if (c == cms->block)
            cms->block = cms->own_block;
        cms->size = __MIDL_0010;
    }
    cms->actual_size = __MIDL_0010;
    return 0;
}

static HRESULT __stdcall  CMediaSample_GetMediaType(IMediaSample* This,
        AM_MEDIA_TYPE** ppMediaType)
{
    AM_MEDIA_TYPE* t;
    //mp_msg(0,0,"CMediaSample_GetMediaType(%p) called\n", This);
    if(!ppMediaType)
        return E_INVALIDARG;
    if(!((CMediaSample*)This)->type_valid)
    {
        *ppMediaType=0;
        return 1;
    }

    t = &((CMediaSample*)This)->media_type;
    //    if(t.pbFormat)free(t.pbFormat);
    (*ppMediaType) = (AM_MEDIA_TYPE*)malloc(sizeof(AM_MEDIA_TYPE));
    **ppMediaType = *t;
    (*ppMediaType)->pbFormat = (char*)malloc(t->cbFormat);
    memcpy((*ppMediaType)->pbFormat, t->pbFormat, t->cbFormat);
    //    *ppMediaType=0; //media type was not changed
    return 0;
}

static HRESULT __stdcall  CMediaSample_SetMediaType(IMediaSample * This,
        AM_MEDIA_TYPE *pMediaType)
{
    AM_MEDIA_TYPE* t;
    //mp_msg(0,0,"CMediaSample_SetMediaType(%p) called\n", This);
    if (!pMediaType)
		{
				mp_msg(0,0,"CMediaSample_SetMediaType failed");
        return E_INVALIDARG;
		}
    t = &((CMediaSample*)This)->media_type;
    if (t->pbFormat)
        free(t->pbFormat);
    t = pMediaType;
    if (t->cbFormat)
    {
        t->pbFormat = (char*)malloc(t->cbFormat);
        memcpy(t->pbFormat, pMediaType->pbFormat, t->cbFormat);
    }
    else
        t->pbFormat = 0;
    ((CMediaSample*) This)->type_valid=1;

    return 0;
}

static HRESULT __stdcall  CMediaSample_IsDiscontinuity(IMediaSample * This)
{
    //mp_msg(0,0,"CMediaSample_IsDiscontinuity(%p) called\n", This);
    return ((CMediaSample*) This)->isDiscontinuity;
}

static HRESULT __stdcall  CMediaSample_SetDiscontinuity(IMediaSample * This,
        long bDiscontinuity)
{
    //mp_msg(0,0,"CMediaSample_SetDiscontinuity(%p) called (%ld)\n", This, bDiscontinuity);
    ((CMediaSample*) This)->isDiscontinuity = bDiscontinuity;
    return 0;
}

static HRESULT __stdcall  CMediaSample_GetMediaTime(IMediaSample * This,
        /* [out] */ INT64 *pTimeStart,
        /* [out] */ INT64 *pTimeEnd)
{
    //mp_msg(0,0,"CMediaSample_GetMediaTime(%p) called\n", This);
    if (pTimeStart)
        *pTimeStart = ((CMediaSample*) This)->time_start;
    if (pTimeEnd)
        *pTimeEnd = ((CMediaSample*) This)->time_end;
    return 0;
}

static HRESULT __stdcall  CMediaSample_SetMediaTime(IMediaSample * This,
        /* [in] */ INT64 *pTimeStart,
        /* [in] */ INT64 *pTimeEnd)
{
    //mp_msg(0,0,"CMediaSample_SetMediaTime(%p) called\n", This);
    if (pTimeStart)
        ((CMediaSample*) This)->time_start = *pTimeStart;
    if (pTimeEnd)
        ((CMediaSample*) This)->time_end = *pTimeEnd;
    return 0;
}

// extension for direct memory write or decompressed data
static void CMediaSample_SetPointer(CMediaSample* This, char* pointer)
{
    //mp_msg(0,0,"CMediaSample_SetPointer(%p) called  -> %p\n", This, pointer);
    if (pointer)
        This->block = pointer;
    else
        This->block = This->own_block;
}

static void CMediaSample_ResetPointer(CMediaSample* This)
{
    //mp_msg(0,0,"CMediaSample_ResetPointer(%p) called\n", This);
    This->block = This->own_block;
}

CMediaSample* CMediaSampleCreate(IMemAllocator* allocator, int _size)
{
    CMediaSample* This = (CMediaSample*) malloc(sizeof(CMediaSample));
    if (!This)
        return NULL;

    // some hack here!
    // it looks like Acelp decoder is actually accessing
    // the allocated memory before it sets the new size for it ???
    // -- maybe it's being initialized with wrong parameters
    // anyway this is fixes the problem somehow with some reserves
    //
    // using different trick for now - in DS_Audio modify sample size
    //if (_size < 0x1000)
    //    _size = (_size + 0xfff) & ~0xfff;

    This->vt = (IMediaSample_vt*) malloc(sizeof(IMediaSample_vt));
    This->own_block = (char*) malloc((size_t)_size + SAFETY_ACEL);
    This->media_type.pbFormat = 0;

    if (!This->vt || !This->own_block)
    {
        CMediaSample_Destroy(This);
        return NULL;
    }

    This->vt->QueryInterface = CMediaSample_QueryInterface;
    This->vt->AddRef = CMediaSample_AddRef;
    This->vt->Release = CMediaSample_Release;
    This->vt->GetPointer = CMediaSample_GetPointer;
    This->vt->GetSize = CMediaSample_GetSize;
    This->vt->GetTime = CMediaSample_GetTime;
    This->vt->SetTime = CMediaSample_SetTime;
    This->vt->IsSyncPoint = CMediaSample_IsSyncPoint;
    This->vt->SetSyncPoint = CMediaSample_SetSyncPoint;
    This->vt->IsPreroll = CMediaSample_IsPreroll;
    This->vt->SetPreroll = CMediaSample_SetPreroll;
    This->vt->GetActualDataLength = CMediaSample_GetActualDataLength;
    This->vt->SetActualDataLength = CMediaSample_SetActualDataLength;
    This->vt->GetMediaType = CMediaSample_GetMediaType;
    This->vt->SetMediaType = CMediaSample_SetMediaType;
    This->vt->IsDiscontinuity = CMediaSample_IsDiscontinuity;
    This->vt->SetDiscontinuity = CMediaSample_SetDiscontinuity;
    This->vt->GetMediaTime = CMediaSample_GetMediaTime;
    This->vt->SetMediaTime = CMediaSample_SetMediaTime;

    This->all = allocator;
    This->size = _size;
    This->refcount = 0; // increased by MemAllocator
    This->actual_size = 0;
    This->isPreroll = 0;
    This->isDiscontinuity = 1;
    This->time_start = 0;
    This->time_end = 0;
    This->type_valid = 0;
    This->block = This->own_block;

    This->SetPointer = CMediaSample_SetPointer;
    This->ResetPointer = CMediaSample_ResetPointer;

    //mp_msg(0,0,"CMediaSample_Create(%p) called - sample size %d, buffer %p\n",This, This->size, This->block);

    return This;
}
