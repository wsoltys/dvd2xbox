#ifndef WRAPPER_DLL_H
#define WRAPPER_DLL_H
DWORD  __stdcall exptimeGetTime();
BOOL  __stdcall expIsBadWritePtr(void* ptr, unsigned int count);
BOOL  __stdcall expIsBadReadPtr(void* ptr, unsigned int count);
BOOL  __stdcall expDisableThreadLibraryCalls(HANDLE module);
BOOL __stdcall expIsBadStringPtrW(const short* string, unsigned int nchars);
BOOL __stdcall expIsBadStringPtrA(const char* string, unsigned int nchars);

 HANDLE  __stdcall expCreateThread(void* pSecAttr, DWORD dwStackSize,
                                    void* lpStartAddress, void* lpParameter,
                                    DWORD dwFlags, DWORD* dwThreadId);
HANDLE  __stdcall expCreateEventA(void* pSecAttr, BOOL bManualReset,BOOL bInitialState, const char* name);
BOOL  __stdcall expSetEvent(HANDLE event);
BOOL  __stdcall expResetEvent(HANDLE event);
DWORD  __stdcall expHeapSize(HANDLE heap, DWORD flags, void* pointer);
HANDLE  __stdcall expGetProcessHeap(void);
void*  __stdcall expHeapAlloc(HANDLE heap, DWORD flags, DWORD size);
BOOL  __stdcall expHeapFree(HANDLE heap, DWORD dwFlags, LPVOID lpMem);
void*  __stdcall expHeapReAlloc(HANDLE heap,DWORD flags,void *lpMem,DWORD size);
BOOL  __stdcall expHeapDestroy(HANDLE* heap);
DWORD  __stdcall expWaitForSingleObject(HANDLE* object, DWORD duration);
void  __stdcall expGetSystemInfo(void* si);
DWORD  __stdcall expGetVersion(void);
HANDLE  __stdcall expHeapCreate(DWORD flags, DWORD init_size, DWORD max_size);
LPVOID __stdcall expVirtualAlloc(LPVOID addr,DWORD x,DWORD y ,DWORD z);
BOOL	 __stdcall expVirtualFree(    LPVOID lpAddress,    DWORD dwSize,    DWORD dwFreeType    );
void  __stdcall expInitializeCriticalSection(void* c);
void  __stdcall expEnterCriticalSection(void* c);
void  __stdcall expLeaveCriticalSection(void* c);
void  __stdcall expDeleteCriticalSection(void *c);
void* expmalloc(int size);
void expfree(void* mem);
int  __stdcall expLoadStringA(HANDLE instance, unsigned int  id, char* buf, int size);

HANDLE  __stdcall expCreateCompatibleDC(HANDLE hdc);
DWORD  __stdcall expGetFileVersionInfoSizeA(const char* name, DWORD* lpHandle);
HRESULT  __stdcall expCoCreateFreeThreadedMarshaler(void *pUnkOuter, void **ppUnkInner);
void* expmemcpy(void* dest, void* src, int n);
void* expwcscpy(WCHAR* dst, const WCHAR* src);
unsigned long __stdcall exphtonl(unsigned long hostlong);
unsigned long  __stdcall expntohl(unsigned long  netlong);
void  __stdcall expVariantInit(void* p);
int __stdcall expDefDriverProc(int _private, int id, int msg, int arg1, int arg2);
int expsscanf(const char* str, const char* format, ...);
int expsprintf(char* str, const char* format, ...);
int __stdcall expwsprintfA(char* string, char* format, ...);
int expfprintf(void* stream, const char* format, ...);
int expprintf(const char* format, ...); 
DWORD __stdcall expWaitForMultipleObjects(DWORD x,const HANDLE* h,BOOL b,DWORD y);

void __stdcall expExitThread(DWORD retcode);
HANDLE __stdcall expCreateMutexA(void *pSecAttr,  char bInitialOwner, const char *name);
BOOL __stdcall expReleaseMutex(HANDLE hMutex);
HANDLE WINAPI __stdcall expGetCurrentProcess();
DWORD WINAPI __stdcall expGetCurrentThreadId();

HANDLE __stdcall expLocalAlloc(unsigned int flags, unsigned int size);
HANDLE __stdcall expLocalReAlloc(HANDLE  handle,unsigned int size, unsigned int flags);
void* __stdcall expLocalLock(HANDLE* z);
HANDLE  __stdcall expGlobalAlloc(unsigned int flags, DWORD size);
void*  __stdcall expGlobalLock(HANDLE* z);
DWORD  __stdcall expGlobalSize(HANDLE* amem);
HANDLE  __stdcall expGlobalReAlloc(HANDLE v, DWORD size, unsigned int flags);
int __stdcall expMultiByteToWideChar(unsigned int v1, DWORD v2, char* s1, int siz1, short* s2, int siz2);
int __stdcall expWideCharToMultiByte(unsigned int v1, DWORD v2, short* s1, int siz1,char* s2, int siz2, char* c3, BOOL* siz3);
HANDLE  __stdcall expCreateSemaphoreA(void* v1, long init_count, long max_count, char* name);
BOOL  __stdcall expReleaseSemaphore(HANDLE hsem, long increment, long* prev_count);
BOOL __stdcall expQueryPerformanceFrequency(void* z);

HANDLE  __stdcall expLocalHandle(void* v);
BOOL  __stdcall expLocalUnlock(HANDLE v);
HANDLE  __stdcall expLocalFree(HANDLE v);
HANDLE  __stdcall expGlobalHandle(void* v);
BOOL  __stdcall expGlobalUnlock(HANDLE v);
HANDLE  __stdcall expGlobalFree(HANDLE v);
BOOL __stdcall expCloseHandle(HANDLE v1); 

DWORD  __stdcall expGetLastError();
void  __stdcall expSetLastError(DWORD error);
BOOL  __stdcall expSetThreadPriority(HANDLE hthread,int priority)   ;
void __stdcall expExitProcess( unsigned int status );
void  __stdcall expGlobalMemoryStatus(void* lpmem);
DWORD  __stdcall expGetCurrentProcessId(void);
DWORD  __stdcall expSetThreadAffinityMask(HANDLE hThread,DWORD dwThreadAffinityMask);
DWORD  __stdcall expGetTickCount(void);
int __stdcall expMulDiv(int nNumber, int nNumerator, int nDenominator);
void __stdcall expSleep(DWORD  time);

LONG __stdcall __stdcall expInterlockedIncrement(LONG *pl);
LONG __stdcall expInterlockedDecrement(LONG *pl);
VOID __stdcall expGetSystemTimeAsFileTime(void* lpSystemTimeAsFileTime );;
VOID __stdcall expGetLocalTime(void* pSystemTime);
HANDLE __stdcall expGetCurrentThread(VOID);
int __stdcall expGetThreadPriority(HANDLE hThread   );
BOOL  __stdcall  expQueryPerformanceCounter(void* z);
DWORD  __stdcall  expTlsAlloc();
int  __stdcall  expTlsSetValue(DWORD index, void* value);
void*  __stdcall  expTlsGetValue(DWORD index);
BOOL  __stdcall  expTlsFree(DWORD idx);
BOOL __stdcall expCloseHandle(HANDLE v1);
int __stdcall expGetSystemTime(void* systime);
int __cdecl myprintf(const char *, ...);
int expwsprintfA(char* string, char* format, ...);
BOOL __stdcall expIsProcessorFeaturePresent(DWORD v);
BOOL __stdcall expWriteFile(HANDLE h,LPCVOID pv,DWORD size,LPDWORD wr,/*LPOVERLAPPED*/void* unused);
BOOL __stdcall expReadFile(HANDLE h,LPVOID pv,DWORD size,LPDWORD rd,/*LPOVERLAPPED*/void* unused);
DWORD  __stdcall expSetFilePointer(HANDLE h, LONG val, LPLONG ext, DWORD whence);
void* __stdcall expCreateFileA(LPCSTR cs1,DWORD i1,DWORD i2,struct _SECURITY_ATTRIBUTES* p1, DWORD i3,DWORD i4,void* i5);

#endif
