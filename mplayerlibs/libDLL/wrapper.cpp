#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "wrapperdll.h"
#include "../cpudetect.h"
#ifdef _XBOX
typedef struct tagSYSTEM_INFO
{
    union {
			DWORD	dwOemId; /* Obsolete field - do not use */
			struct {
				WORD wProcessorArchitecture;
				WORD wReserved;
			} s;
    } u;
    DWORD	dwPageSize;
    LPVOID	lpMinimumApplicationAddress;
    LPVOID	lpMaximumApplicationAddress;
    DWORD	dwActiveProcessorMask;
    DWORD	dwNumberOfProcessors;
    DWORD	dwProcessorType;
    DWORD	dwAllocationGranularity;
    WORD	wProcessorLevel;
    WORD	wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;
#endif

static int pf_set = 0;
static BYTE PF[64] = {0,};
#define PF_AMD3D_INSTRUCTIONS_AVAILABLE		7

extern "C" void mp_msg( int x, int lev,const char *format, ... );
//**********************************************************************************************
DWORD  __stdcall exptimeGetTime()
{
	return timeGetTime();
}

//**********************************************************************************************
BOOL  __stdcall expIsBadWritePtr(void* ptr, unsigned int count)
{
    return IsBadWritePtr(ptr, count);
}
//**********************************************************************************************
BOOL  __stdcall expIsBadReadPtr(void* ptr, unsigned int count)
{
    return IsBadReadPtr(ptr,count);
}
//**********************************************************************************************
BOOL  __stdcall expDisableThreadLibraryCalls(HANDLE module)
{
    //mp_msg(0,0,"DisableThreadLibraryCalls(0x%x) => 0\n", module);
    return FALSE;
}


//**********************************************************************************************
 BOOL __stdcall expIsBadStringPtrW(const short* string, unsigned int nchars)
{
    return IsBadStringPtrW((LPCWSTR)string, nchars);
}
//**********************************************************************************************
BOOL __stdcall expIsBadStringPtrA(const char* string, unsigned int nchars)
{
    return  IsBadStringPtrA(string, nchars);
}

//**********************************************************************************************
 HANDLE  __stdcall expCreateThread(void* pSecAttr, DWORD dwStackSize,
                                    void* lpStartAddress, void* lpParameter,
                                    DWORD dwFlags, DWORD* dwThreadId)
{
	//mp_msg(0,0,"expCreateThread\n");
	return CreateThread((struct _SECURITY_ATTRIBUTES *)pSecAttr,dwStackSize,(LPTHREAD_START_ROUTINE )lpStartAddress,lpParameter,dwFlags,(LPDWORD)dwThreadId);
}

//**********************************************************************************************
HANDLE  __stdcall expCreateEventA(void* pSecAttr, BOOL bManualReset,BOOL bInitialState, const char* name)
{
	//mp_msg(0,0,"expCreateEvent\n");
	return CreateEventA((struct _SECURITY_ATTRIBUTES *)pSecAttr, bManualReset,bInitialState, name);
}

//**********************************************************************************************
BOOL  __stdcall expSetEvent(HANDLE event)
{
	//mp_msg(0,0,"expSetEvent\n");
	return SetEvent(event);
}
//**********************************************************************************************
BOOL  __stdcall expResetEvent(HANDLE event)
{
	//mp_msg(0,0,"expResetEvent\n");
	return ResetEvent(event);
}

//**********************************************************************************************
DWORD  __stdcall expHeapSize(HANDLE heap, DWORD flags, void* pointer)
{
	//mp_msg(0,0,"expHeapSize\n");
	return HeapSize((HANDLE)heap, flags, pointer);
}

//**********************************************************************************************
 HANDLE  __stdcall expGetProcessHeap(void)
{
	//mp_msg(0,0,"expGetProcessHeap\n");
	return GetProcessHeap();
}

//**********************************************************************************************
void*  __stdcall expHeapReAlloc(HANDLE heap,DWORD flags,void *lpMem,DWORD size)
{
	//mp_msg(0,0,"expHeapReAlloc\n");
	return HeapReAlloc(heap,flags,lpMem,size);
}

//**********************************************************************************************
BOOL  __stdcall expHeapFree(HANDLE heap, DWORD dwFlags, LPVOID lpMem)
{
	//mp_msg(0,0,"expHeapFree\n");
	return HeapFree(heap, dwFlags, lpMem);
}

//**********************************************************************************************
void*  __stdcall expHeapAlloc(HANDLE heap, DWORD flags, DWORD size)
{
	//mp_msg(0,0,"expHeapAlloc\n");
	return HeapAlloc(heap, flags, size);
}

//**********************************************************************************************
BOOL  __stdcall expHeapDestroy(HANDLE* heap)
{
	//mp_msg(0,0,"expHeapDestroy\n");
	return HeapDestroy(heap);
}

//**********************************************************************************************
DWORD  __stdcall expWaitForSingleObject(HANDLE* object, DWORD duration)
{
		//mp_msg(0,0,"expWaitForSingleObject\n");
		return WaitForSingleObject(object, duration);
}

//**********************************************************************************************
void  __stdcall expGetSystemInfo(void* si)
{
    /* FIXME: better values for the two entries below... */
    static int cache = 0;
    static SYSTEM_INFO cachedsi;
    
    if (cache) 
		{
			memcpy(si,&cachedsi,sizeof(SYSTEM_INFO));
			return;
    }
    memset(PF,0,sizeof(PF));
    pf_set = 1;

    cachedsi.u.s.wProcessorArchitecture     = PROCESSOR_ARCHITECTURE_INTEL;
    cachedsi.dwPageSize 			= 4096;

    /* FIXME: better values for the two entries below... */
    cachedsi.lpMinimumApplicationAddress	= (void *)0x00000000;
    cachedsi.lpMaximumApplicationAddress	= (void *)0x7FFFFFFF;
    cachedsi.dwActiveProcessorMask		= 1;
    cachedsi.dwNumberOfProcessors		= 1;
    cachedsi.dwProcessorType		= PROCESSOR_INTEL_386;
    cachedsi.dwAllocationGranularity	= 0x10000;
    cachedsi.wProcessorLevel		= 5; /* pentium */
    cachedsi.wProcessorRevision		= 0x0101;

    /* mplayer's way to detect PF's */
    {


	if (gCpuCaps.hasMMX)
	    PF[PF_MMX_INSTRUCTIONS_AVAILABLE] = TRUE;
	if (gCpuCaps.hasSSE)
	    PF[PF_XMMI_INSTRUCTIONS_AVAILABLE] = TRUE;
	if (gCpuCaps.has3DNow)
	    PF[PF_AMD3D_INSTRUCTIONS_AVAILABLE] = TRUE;

	    switch(gCpuCaps.cpuType)
	    {
				case CPUTYPE_I686:
				case CPUTYPE_I586:
						cachedsi.dwProcessorType = PROCESSOR_INTEL_PENTIUM;
						cachedsi.wProcessorLevel = 5;
						break;
				case CPUTYPE_I486:
						cachedsi.dwProcessorType = PROCESSOR_INTEL_486;
						cachedsi.wProcessorLevel = 4;
						break;
				case CPUTYPE_I386:
				default:
						cachedsi.dwProcessorType = PROCESSOR_INTEL_386;
						cachedsi.wProcessorLevel = 3;
						break;
	    }
	    cachedsi.wProcessorRevision = gCpuCaps.cpuStepping;
    	cachedsi.dwNumberOfProcessors = 1;	/* hardcoded */

    }
    cache = 1;
    memcpy(si,&cachedsi,sizeof(SYSTEM_INFO));
}

//**********************************************************************************************
DWORD  __stdcall expGetVersion(void)
{
	//mp_msg(0,0,"expGetVersion\n");
    //return 0xa280105;//Windows XP
		return 0xC0000004;//Windows 95

}

//**********************************************************************************************
HANDLE  __stdcall expHeapCreate(DWORD flags, DWORD init_size, DWORD max_size)
{
	//mp_msg(0,0,"expHeapCreate\n");
	return HeapCreate(flags, init_size, max_size);
}

//**********************************************************************************************
LPVOID __stdcall expVirtualAlloc(LPVOID addr,DWORD x,DWORD y ,DWORD z)
{
	//mp_msg(0,0,"expVirtualAlloc\n");
	return VirtualAlloc(addr,x,y ,z);
}
//**********************************************************************************************
 BOOL	 __stdcall expVirtualFree(    LPVOID lpAddress,    DWORD dwSize,    DWORD dwFreeType    )
{
	//mp_msg(0,0,"expVirtualFree\n");
	return VirtualFree(lpAddress,dwSize,dwFreeType);
}

//**********************************************************************************************
 void  __stdcall expInitializeCriticalSection(void* c)
{
	////mp_msg(0,0,"expInitializeCriticalSection\n");
	InitializeCriticalSection((CRITICAL_SECTION *)c);
}

//**********************************************************************************************
 void  __stdcall expEnterCriticalSection(void* c)
{
	////mp_msg(0,0,"expEnterCriticalSection\n");
	EnterCriticalSection((CRITICAL_SECTION *)c);
}
//**********************************************************************************************
 void  __stdcall expLeaveCriticalSection(void* c)
{
	////mp_msg(0,0,"expLeaveCriticalSection\n");
	LeaveCriticalSection((CRITICAL_SECTION *)c);
}

//**********************************************************************************************
 void  __stdcall expDeleteCriticalSection(void *c)
{
	////mp_msg(0,0,"expDeleteCriticalSection\n");
	DeleteCriticalSection((CRITICAL_SECTION *)c);
}

//**********************************************************************************************
 void*  expmalloc(int size)
{
    ////mp_msg(0,0,"malloc");
    //    return malloc(size);
    void* result=malloc(size);
    if(result==0)
        mp_msg(0,0,"WARNING: malloc() failed\n");
    return result;
}
//**********************************************************************************************
 void  expfree(void* mem)
{
    //    return free(mem);
    ////mp_msg(0,0,"free(%p)\n", mem);
    free(mem);
}
 
//**********************************************************************************************
 int  __stdcall expLoadStringA(HANDLE instance, unsigned int  id, char* buf, int size)
{
	int result=0;
	 //mp_msg(0,0,"LoadStringA(instance 0x%x, id 0x%x, buffer 0x%x, size %d) => %d ( %s )\n",instance, id, buf, size, result, buf);
	 return result;  
}

//**********************************************************************************************
 long  __stdcall expRegCloseKey(HANDLE key)
{
    //mp_msg(0,0,"RegCloseKey(0x%x)\n", key);
    return 0;
}

//**********************************************************************************************
HANDLE  __stdcall expCreateCompatibleDC(HANDLE hdc)
{
    int dc = 0;//0x81;
    //mp_msg(0,0,"CreateCompatibleDC(%d) => %d\n", hdc, dc);
    return (HANDLE)dc;
}

//**********************************************************************************************
DWORD  __stdcall expGetFileVersionInfoSizeA(const char* name, DWORD* lpHandle)
{
    //mp_msg(0,0,"GetFileVersionInfoSizeA(0x%x='%s', 0x%X) => 0\n", name, name, lpHandle);
    return 0;
}

//**********************************************************************************************
 HRESULT  __stdcall expCoCreateFreeThreadedMarshaler(void *pUnkOuter, void **ppUnkInner)
{
    //mp_msg(0,0,"CoCreateFreeThreadedMarshaler(%p, %p) called!\n",pUnkOuter, ppUnkInner);
    //    return 0;
    return ERROR_CALL_NOT_IMPLEMENTED;
}

//**********************************************************************************************
 void* expmemcpy(void* dest, void* src, int n)
{
    void *result = memcpy(dest, src, n);
    //mp_msg(0,0,"memcpy(0x%x, 0x%x, %d) => %p\n", dest, src, n, result);
    return result;
}

//**********************************************************************************************
 void* expwcscpy(WCHAR* dst, const WCHAR* src)
{
    WCHAR* p = dst;
    while ((*p++ = *src++))
        ;
    return dst;
}

//**********************************************************************************************
  unsigned long __stdcall exphtonl(unsigned long hostlong)
{
    //    //mp_msg(0,0,"htonl(%x) => %x\n", hostlong, htonl(hostlong));
    return htonl(hostlong);
}

//**********************************************************************************************
  unsigned long  __stdcall expntohl(unsigned long  netlong)
{
    //    //mp_msg(0,0,"ntohl(%x) => %x\n", netlong, ntohl(netlong));
    return ntohl(netlong);
}

//**********************************************************************************************
void  __stdcall expVariantInit(void* p)
{
    //mp_msg(0,0,"InitCommonControls called!\n");
    return;
}

//**********************************************************************************************
int __stdcall expDefDriverProc(int _private, int id, int msg, int arg1, int arg2)
{
    //mp_msg(0,0,"DefDriverProc(0x%x, 0x%x, 0x%x, 0x%x, 0x%x) => 0\n", _private, id, msg, arg1, arg2);
    return 0;
}


//**********************************************************************************************
 int expsprintf(char* str, const char* format, ...)
{
    va_list args;
    int r;
    //mp_msg(0,0,"s//mp_msg(0,0,%s, %s)\n", str, format);
    va_start(args, format);
    r = vsprintf(str, format, args);
    va_end(args);
    return r;
}
//**********************************************************************************************
 int expsscanf(const char* str, const char* format, ...)
{
    va_list args;
    int r;
    //mp_msg(0,0,"sscanf(%s, %s)\n", str, format);
    va_start(args, format);
    r = sscanf(str, format, args);
    va_end(args);
    return r;
}

//**********************************************************************************************
int expfprintf(void* stream, const char* format, ...)
{
    va_list args;
    int r = 0;
    //mp_msg(0,0,"f//mp_msg(0,0,%p, %s, ...)\n", stream, format);
    va_start(args, format);
    r = vfprintf((FILE*) stream, format, args);
    va_end(args);
    return r;
}
//**********************************************************************************************
 int expprintf(const char* format, ...)
{
    va_list args;
    int r;
    //mp_msg(0,0,"//mp_msg(0,0,%s, ...)\n", format);
    va_start(args, format);
    r = vprintf(format, args);
    va_end(args);
    return r;
}
//**********************************************************************************************
DWORD __stdcall expWaitForMultipleObjects(DWORD x,const HANDLE* h,BOOL b,DWORD y)
{
	return WaitForMultipleObjects(x,h,b,y);
}

//**********************************************************************************************
void __stdcall expExitThread(DWORD retcode)
{
	ExitThread(retcode);
}

//**********************************************************************************************
HANDLE __stdcall expCreateMutexA(void *pSecAttr,  char bInitialOwner, const char *name)
{
	return CreateMutexA((struct _SECURITY_ATTRIBUTES *)pSecAttr,  bInitialOwner, name);
}


//**********************************************************************************************
BOOL __stdcall expReleaseMutex(HANDLE hMutex)
{
	return ReleaseMutex(hMutex);
}

//**********************************************************************************************
DWORD __stdcall expGetCurrentThreadId()
{
    //mp_msg(0,0,"GetCurrentThreadId()\n");
    return GetCurrentThreadId();
}
//**********************************************************************************************
HANDLE __stdcall expGetCurrentProcess()
{
    //mp_msg(0,0,"GetCurrentProcess() \n");
    return GetCurrentProcess();
}
//**********************************************************************************************
HANDLE __stdcall expLocalAlloc(unsigned int flags, unsigned int size)
{
	HANDLE hMem=LocalAlloc(flags, size);
  if(!hMem) 
		mp_msg(0,0,"expLocalAlloc() failed\n");
  return hMem;
}

//**********************************************************************************************
HANDLE __stdcall expLocalReAlloc(HANDLE  handle,unsigned int size, unsigned int flags)
{

    //mp_msg(0,0,"expLocalReAlloc()\n");
    return LocalReAlloc(handle,flags, size);
}

//**********************************************************************************************
void* __stdcall expLocalLock(HANDLE* z)
{
    //mp_msg(0,0,"LocalLock(0x%x) => 0x%x\n", z, z);
    return LocalLock(z);
}

//**********************************************************************************************
HANDLE  __stdcall expGlobalAlloc(unsigned int flags, DWORD size)
{
    //mp_msg(0,0,"GlobalAlloc(\n");
    return GlobalAlloc( flags,size);
}
//**********************************************************************************************
 void*  __stdcall expGlobalLock(HANDLE* z)
{
    //mp_msg(0,0,"GlobalLock(0x%x)\n ", z);
    return GlobalLock( z);
}
//**********************************************************************************************
DWORD  __stdcall expGlobalSize(HANDLE* amem)
{
    //mp_msg(0,0,"expGlobalSize(0x%x)\n ", amem);
    return GlobalSize( amem);
}
//**********************************************************************************************
HANDLE  __stdcall expGlobalReAlloc(HANDLE v, DWORD size, unsigned int flags)
{
    //mp_msg(0,0,"GlobalReAlloc\n ");
    return GlobalReAlloc(v, size, flags);
}

//**********************************************************************************************
int __stdcall expMultiByteToWideChar(unsigned int v1, DWORD v2, char* s1, int siz1, short* s2, int siz2)
{
    //mp_msg(0,0,"expMultiByteToWideChar\n ");
	return MultiByteToWideChar(v1, v2, s1, siz1, (unsigned short*)s2, siz2);
}


//**********************************************************************************************
 HANDLE  __stdcall expCreateSemaphoreA(void* v1, long init_count, long max_count, char* name)
{
    //mp_msg(0,0,"expCreateSemaphoreA\n ");
	return CreateSemaphoreA( (struct _SECURITY_ATTRIBUTES *)v1,  init_count,  max_count,  name);
}

//**********************************************************************************************
BOOL  __stdcall expReleaseSemaphore(HANDLE hsem, long increment, long* prev_count)
{
    //mp_msg(0,0,"expReleaseSemaphore\n ");
    return ReleaseSemaphore((void*)hsem, increment, prev_count);
}


//**********************************************************************************************
int __stdcall expWideCharToMultiByte(unsigned int v1, DWORD v2, short* s1, int siz1,char* s2, int siz2, char* c3, BOOL* siz3)
{

    //mp_msg(0,0,"expWideCharToMultiByte\n ");
    return WideCharToMultiByte(v1, v2, (const unsigned short *)s1, siz1,s2, siz2, c3, siz3);
}
//**********************************************************************************************
BOOL __stdcall expQueryPerformanceFrequency(void* z)
{
    //mp_msg(0,0,"expQueryPerformanceFrequency\n ");
	return QueryPerformanceFrequency((LARGE_INTEGER*)z);
}

//**********************************************************************************************
HANDLE  __stdcall expLocalHandle(void* v)
{
    //mp_msg(0,0,"expLocalHandle\n ");
	return LocalHandle(v);
}
//**********************************************************************************************
BOOL  __stdcall expLocalUnlock(HANDLE v)
{
    //mp_msg(0,0,"expLocalUnlock\n ");
	return LocalUnlock(v);
}

//**********************************************************************************************
HANDLE  __stdcall expLocalFree(HANDLE v)
{
    //mp_msg(0,0,"expLocalFree\n ");
	return LocalFree(v);
}

//**********************************************************************************************
HANDLE  __stdcall expGlobalHandle(void* v)
{
    //mp_msg(0,0,"expGlobalHandle\n ");
	return GlobalHandle(v);
}
//**********************************************************************************************
BOOL  __stdcall expGlobalUnlock(HANDLE v)
{
    //mp_msg(0,0,"expGlobalUnlock\n ");
	return GlobalUnlock(v);
}

//**********************************************************************************************
HANDLE  __stdcall expGlobalFree(HANDLE v)
{
    //mp_msg(0,0,"expGlobalFree\n ");
	return GlobalFree(v);
}
//**********************************************************************************************
BOOL __stdcall expCloseHandle(HANDLE v1)
{
    //mp_msg(0,0,"expCloseHandle\n ");
	return CloseHandle((void*)v1);
}
//**********************************************************************************************
DWORD  __stdcall expGetLastError()
{
    //mp_msg(0,0,"expGetLastError\n ");
    return GetLastError();
}

//**********************************************************************************************
 void  __stdcall expSetLastError(DWORD error)
{
    //mp_msg(0,0,"expSetLastError\n ");
    SetLastError(error);
}
//**********************************************************************************************
void  __stdcall expGlobalMemoryStatus(void* lpmem)
{
    //mp_msg(0,0,"expGlobalMemoryStatus\n ");
    GlobalMemoryStatus((LPMEMORYSTATUS)lpmem);
}

//**********************************************************************************************
BOOL  __stdcall expSetThreadPriority(HANDLE hthread,INT priority)   
{
    //mp_msg(0,0,"expSetThreadPriority\n ");
    return SetThreadPriority(hthread,priority);
}

//**********************************************************************************************
 void __stdcall expExitProcess( unsigned int status )
{
    //mp_msg(0,0,"EXIT - code %ld\n",status);
    
}
//**********************************************************************************************
 DWORD  __stdcall expGetCurrentProcessId(void)
{
    //mp_msg(0,0,"expGetCurrentProcessId\n ");
    return 1;
}
//**********************************************************************************************
DWORD  __stdcall expSetThreadAffinityMask(HANDLE hThread,DWORD dwThreadAffinityMask)
{
    //mp_msg(0,0,"expSetThreadAffinityMask\n ");
    return 0;
}
//**********************************************************************************************
DWORD  __stdcall expGetTickCount(void)
{
	return GetTickCount();
}
//**********************************************************************************************
int __stdcall expMulDiv(int nNumber, int nNumerator, int nDenominator)
{
	//mp_msg(0,0,"expMulDiv()\n");
	return MulDiv(nNumber, nNumerator, nDenominator);
}
//**********************************************************************************************
void __stdcall expSleep(DWORD time)
{
	//mp_msg(0,0,"Sleep()\n");
	Sleep(time);
}

//**********************************************************************************************
LONG  __stdcall expInterlockedIncrement(PLONG pl)
{
	//mp_msg(0,0,"expInterlockedIncrement\n");
	return InterlockedIncrement(pl);
}

//**********************************************************************************************
LONG   __stdcall expInterlockedDecrement(PLONG pl)
{
	//mp_msg(0,0,"expInterlockedDecrement\n");
	return InterlockedDecrement(pl);
}

//**********************************************************************************************
VOID __stdcall expGetSystemTimeAsFileTime(void* lpSystemTimeAsFileTime )
{
	LPFILETIME lpTime=(LPFILETIME )lpSystemTimeAsFileTime ;
	GetSystemTimeAsFileTime(lpTime);
}

//**********************************************************************************************
VOID __stdcall expGetLocalTime(void* pSystemTime)
{
  LPSYSTEMTIME lpSystemTime =(LPSYSTEMTIME )pSystemTime;  // system time
	GetLocalTime(lpSystemTime );
}

//**********************************************************************************************
HANDLE __stdcall expGetCurrentThread(VOID)
{
	return GetCurrentThread();
}
//**********************************************************************************************
int __stdcall expGetThreadPriority(HANDLE hThread   )
{
	return GetThreadPriority(hThread);
}
//**********************************************************************************************
BOOL __stdcall  expQueryPerformanceCounter(void* z)
{
    return QueryPerformanceCounter((LARGE_INTEGER*)z);
}


//**********************************************************************************************
DWORD  __stdcall  expTlsAlloc()
{
    return TlsAlloc();
}

//**********************************************************************************************
int  __stdcall  expTlsSetValue(DWORD index, void* value)
{
    return TlsSetValue(index,value);
}

//**********************************************************************************************
void*  __stdcall  expTlsGetValue(DWORD index)
{
		return TlsGetValue(index);
}

//**********************************************************************************************
BOOL  __stdcall  expTlsFree(DWORD idx)
{
    return TlsFree(idx);
}

//**********************************************************************************************
int __stdcall expGetSystemTime(void* systime)
{
	GetSystemTime((SYSTEMTIME*) systime);
	return 1;
}

//**********************************************************************************************
int __cdecl myprintf(const char *format, ...)
{

    va_list va;
    char tmp[2048];
    
    va_start(va, format);
    _vsnprintf(tmp, 2048, format, va);
    va_end(va);
    tmp[2048-1] = 0;

		mp_msg(0,0,tmp);mp_msg(0,0,"\n");
		return 1;
}
//**********************************************************************************************
int expwsprintfA(char* string, char* format, ...)
{
    va_list va;
    int result;
    va_start(va, format);
    result = vsprintf(string, format, va);
    mp_msg(0,0,"wsprintfA(0x%x, '%s', ...) => %d\n", string, format, result);
    va_end(va);
    return result;
}


//**********************************************************************************************
BOOL __stdcall expIsProcessorFeaturePresent(DWORD v)
{
    BOOL result = 0;
    if (!pf_set)
    {
			SYSTEM_INFO si;
			expGetSystemInfo(&si);
    }
    else result=PF[v];
    return result;

}


//***********************************************************************************************
DWORD  __stdcall expSetFilePointer(HANDLE h, LONG val, LPLONG ext, DWORD whence)
{    
	return SetFilePointer(h, val, ext,whence);
}
//***********************************************************************************************
BOOL __stdcall expReadFile(HANDLE h,LPVOID pv,DWORD size,LPDWORD rd,/*LPOVERLAPPED*/void* unused)
{
    return ReadFile(h,pv,size,rd,(LPOVERLAPPED)unused);
}
//***********************************************************************************************
BOOL __stdcall expWriteFile(HANDLE h,LPCVOID pv,DWORD size,LPDWORD wr,/*LPOVERLAPPED*/void* unused)
{
	return WriteFile(h,pv,size,wr,(LPOVERLAPPED)unused);
}

//***********************************************************************************************
void* __stdcall expCreateFileA(LPCSTR cs1,DWORD i1,DWORD i2,struct _SECURITY_ATTRIBUTES* p1, DWORD i3,DWORD i4,void* i5)
{
	return CreateFileA(cs1,i1,i2,p1, i3,i4,i5);
}