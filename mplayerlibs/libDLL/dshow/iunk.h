#ifndef DS_IUNK_H
#define DS_IUNK_H

#include "guids.h"

#define INHERIT_IUNKNOWN() \
    long STDCALL (__stdcall  *QueryInterface )(IUnknown * This, const GUID* riid, void **ppvObject); \
    long STDCALL (__stdcall  *AddRef )(IUnknown * This); \
    long STDCALL (__stdcall  *Release )(IUnknown * This);

#define DECLARE_IUNKNOWN() \
    int refcount;

#define IMPLEMENT_IUNKNOWN(CLASSNAME) 		\
static long __stdcall CLASSNAME ## _QueryInterface(IUnknown * This, \
					  const GUID* riid, void **ppvObject) \
{ \
    CLASSNAME * me = (CLASSNAME *)This;		\
    GUID* r; unsigned int i = 0;		\
    if (!ppvObject) return E_POINTER; 		\
    for(r=me->interfaces; i<sizeof(me->interfaces)/sizeof(me->interfaces[0]); r++, i++) \
	if(!memcmp(r, riid, sizeof(*r)))	\
	{ 					\
	    me->vt->AddRef((IUnknown*)This); 	\
	    *ppvObject=This; 			\
	    return 0; 				\
	} 					\
    return E_NOINTERFACE;			\
} 						\
						\
static long __stdcall CLASSNAME ## _AddRef(IUnknown * This) \
{						\
    CLASSNAME * me=( CLASSNAME *)This;		\
    return ++(me->refcount); 			\
}     						\
						\
static long __stdcall CLASSNAME ## _Release(IUnknown * This) \
{ 						\
    CLASSNAME* me=( CLASSNAME *)This;	 	\
    if(--(me->refcount) == 0)			\
		CLASSNAME ## _Destroy(me); 	\
    return 0; 					\
}

#endif /* DS_IUNK_H */
