#ifndef AVIFILE_COM_H
#define AVIFILE_COM_H

#include "cygwin_inttypes.h"

/**
 * Internal functions and structures for COM emulation code.
 */


#ifdef __cplusplus
extern "C" {
#endif

void* CoTaskMemAlloc(unsigned long cb);
void CoTaskMemFree(void* cb);

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

extern const GUID IID_IUnknown;
extern const GUID IID_IClassFactory;

typedef long (*GETCLASSOBJECT) (GUID* clsid, const GUID* iid, void** ppv);
int RegisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);
int UnregisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);

#ifndef STDCALL
#define STDCALL 
#endif

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

long CoCreateInstance(GUID* rclsid, struct IUnknown* pUnkOuter, long dwClsContext, const GUID* riid, void** ppv);

#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /* AVIFILE_COM_H */
