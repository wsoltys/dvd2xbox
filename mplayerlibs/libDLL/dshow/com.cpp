#include "guids.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
extern "C" void mp_msg( int x, int lev,const char *format, ... );

#define REGDB_E_READREGDB               0x80040150
#define REGDB_E_CLASSNOTREG             0x80040154
void* XboxCoTaskMemAlloc(unsigned long cb)
{
    return malloc(cb);
}
void XboxCoTaskMemFree(void* cb)
{
    free(cb);
}

struct COM_OBJECT_INFO
{
    GUID clsid;
    long (*GetClassObject) (GUID* clsid, const GUID* iid, void** ppv);
};



int RegisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);
int UnregisterComClass(const GUID* clsid, GETCLASSOBJECT gcs);





static struct COM_OBJECT_INFO* com_object_table=0;
static int com_object_size=0;
int RegisterComClass(const GUID* clsid, GETCLASSOBJECT gcs)
{
    if(!clsid || !gcs)
		{
				mp_msg(0,0,"RegisterComClass() failed\n");
        return -1;
		}
    com_object_table=(struct COM_OBJECT_INFO*)realloc(com_object_table, sizeof(struct COM_OBJECT_INFO)*(++com_object_size));
    com_object_table[com_object_size-1].clsid=*clsid;
    com_object_table[com_object_size-1].GetClassObject=gcs;
    return 0;
}

int UnregisterComClass(const GUID* clsid, GETCLASSOBJECT gcs)
{
    int found = 0;
    int i = 0;
    if(!clsid || !gcs)
		{
			mp_msg(0,0,"UnRegisterComClass() failed\n");
        return -1;
		}
    if (com_object_table == 0)
      mp_msg(0,0,"Warning: UnregisterComClass() called without any registered class\n");
    while (i < com_object_size)
    {
        if (found && i > 0)
        {
            memcpy(&com_object_table[i - 1].clsid,
                   &com_object_table[i].clsid, sizeof(GUID));
            com_object_table[i - 1].GetClassObject =
                com_object_table[i].GetClassObject;
        }
        else if (memcmp(&com_object_table[i].clsid, clsid, sizeof(GUID)) == 0
                 && com_object_table[i].GetClassObject == gcs)
        {
            found++;
        }
        i++;
    }
    if (found)
    {
        if (--com_object_size == 0)
        {
            free(com_object_table);
            com_object_table = 0;
        }
    }
    return 0;
}

 long  __stdcall  expCoCreateInstance(GUID* rclsid, struct IUnknown* pUnkOuter,
                                       long dwClsContext, const GUID* riid, void** ppv)
{
    int i;
    struct COM_OBJECT_INFO* ci=0;
    for(i=0; i<com_object_size; i++)
        if(!memcmp(rclsid, &com_object_table[i].clsid, sizeof(GUID)))
            ci=&com_object_table[i];
    if(!ci)
		{
			mp_msg(0,0,"expCoCreateInstance ClassNotRegistered\n");
        return REGDB_E_CLASSNOTREG;
		}
    // in 'real' world we should mess with IClassFactory here
    i=ci->GetClassObject(rclsid, riid, ppv);
    return i;
}