
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <fcntl.h>
#include <io.h>
#include <ctype.h>
#include <time.h>
#include "cygwin_inttypes.h"
#include "module.h" 
#include "com.h" 
#include "wrapperdll.h" 
DECLARE_HANDLE(HDRVR);
DECLARE_HANDLE(HKEY);
DECLARE_HANDLE(HGDIOBJ);
DECLARE_HANDLE(HPALETTE);

#include "registry.h" 

#pragma warning (disable:4018)
/* registry errors */
#define REGDB_E_READREGDB               0x80040150
#define REGDB_E_CLASSNOTREG             0x80040154

#define DECLSPEC_IMPORT
#define WINBASEAPI DECLSPEC_IMPORT

extern "C" HANDLE      __stdcall GetProcessHeap(void);
extern "C" WIN_BOOL    __stdcall IsBadReadPtr(LPCVOID,UINT);
extern "C" LPVOID      __stdcall VirtualAlloc(LPVOID,DWORD,DWORD,DWORD);
extern "C" WIN_BOOL    __stdcall HeapFree(HANDLE,DWORD,LPVOID);
extern "C" WINBASEAPI	 BOOL	 __stdcall	VirtualFree(    LPVOID lpAddress,    DWORD dwSize,    DWORD dwFreeType    );
extern "C" unsigned long   __stdcall htonl (unsigned long hostlong);
extern "C" unsigned long   __stdcall ntohl (unsigned long netlong);
extern "C" WINBASEAPI PVOID __stdcall InterlockedCompareExchange (    PVOID *Destination,    PVOID Exchange,    PVOID Comperand    );
extern "C" void __stdcall  Sleep(DWORD dwMilliseconds);
extern "C" LONG __stdcall  InterlockedExchange(LPLONG Target,LONG Value);
extern "C" void*  __stdcall  CreateThread(void* pSecAttr, long dwStackSize,void* lpStartAddress, void* lpParameter,long dwFlags, long* dwThreadId);
extern "C" void*  __stdcall  CreateEventA(void* pSecAttr, char bManualReset,char bInitialState, const char* name);
extern "C" void*  __stdcall  SetEvent(void* event);
extern "C" void*  __stdcall  ResetEvent(void* event);
extern "C" void*  __stdcall  WaitForSingleObject(void* object, int duration);
extern "C" LONG __stdcall  __stdcall GetVersion(void);
extern "C" HANDLE  __stdcall   HeapCreate(long flags, long init_size, long max_size);
extern "C" long  __stdcall   HeapDestroy(void* heap);
extern "C" long  __stdcall   HeapSize(int heap, int flags, void* pointer);
extern "C" void*  __stdcall   HeapReAlloc(HANDLE heap,int flags,void *lpMem,int size);
extern "C" void       __stdcall   InitializeCriticalSection(CRITICAL_SECTION *lpCrit);
extern "C" void  __stdcall   EnterCriticalSection(CRITICAL_SECTION* c);
extern "C" static void  __stdcall   LeaveCriticalSection(CRITICAL_SECTION* c);
extern "C" void       __stdcall   DeleteCriticalSection(CRITICAL_SECTION *lpCrit);
extern void* XboxCoTaskMemAlloc(unsigned long cb);
extern void XboxCoTaskMemFree(void* cb);
long  __stdcall  expCoCreateInstance(GUID* rclsid, struct IUnknown* pUnkOuter,long dwClsContext, const GUID* riid, void** ppv);
extern void* LookupExternal(const char* library, int ordinal);
extern void* LookupExternalByName(const char* library, const char* name);
extern "C" void mp_msg( int x, int lev,const char *format, ... );
extern WINE_MODREF *MODULE32_LookupHMODULE(HMODULE m);


#define ERROR_CALL_NOT_IMPLEMENTED                         120 
typedef long ( *DRIVERPROC)(long,HDRVR,unsigned int,long,long);
typedef struct {
    UINT             uDriverSignature;
    HINSTANCE        hDriverModule;
    DRIVERPROC       DriverProc;
    DWORD            dwDriverID;
} DRVR;

#define RVA(x) ((void *)((char *)load_addr+(unsigned int)(x)))
#define AdjustPtr(ptr,delta) ((char *)(ptr) + (delta))

DWORD  pos=0;
static DWORD extcode[1000];// place for 200 unresolved exports


static int mapping_size=0;
struct file_mapping_s;
typedef struct file_mapping_s
{
    int mapping_size;
    char* name;
    LPVOID handle;
    struct file_mapping_s* next;
    struct file_mapping_s* prev;
}
file_mapping;
static file_mapping* fm=0;


#define max(a,b)   (((a) > (b)) ? (a) : (b))
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HINSTANCE);
typedef struct {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	CHAR szCSDVersion[128];
} OSVERSIONINFOA;

/* The RECT structure */
typedef struct tagRECT
{
    short  left;
    short  top;
    short  right;
    short  bottom;
} RECT, *PRECT, *LPRECT;


typedef struct {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

typedef struct _TIME_ZONE_INFORMATION {
 long Bias;
 char StandardName[32];
 SYSTEMTIME StandardDate;
 long StandardBias;
 char DaylightName[32];
 SYSTEMTIME DaylightDate;
 long DaylightBias;
} TIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;
#define TIME_ZONE_ID_STANDARD   1


#define VER_PLATFORM_WIN32s             0
#define VER_PLATFORM_WIN32_WINDOWS      1
#define VER_PLATFORM_WIN32_NT           2

static short envs[]={'p', 'a', 't', 'h', ' ', 'C', ':', '\\', 0, 0};

static const char ch_envs[]=
														"__MSVCRT_HEAP_SELECT=__GLOBAL_HEAP_SELECTED,1\r\n"
														"PATH=Q:\\dll\r\n";
#define FILE_ATTRIBUTE_SYSTEM           0x00000004L
#define FILE_ATTRIBUTE_NORMAL           0x00000080L
static int cursor[100];
#define PSEUDO_SCREEN_WIDTH	800
#define PSEUDO_SCREEN_HEIGHT	600

typedef struct tagPALETTEENTRY
{
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
}
PALETTEENTRY;
/* reversed the first 2 entries */
typedef struct tagLOGPALETTE
{
    WORD         palNumEntries;
    WORD         palVersion;
    PALETTEENTRY palPalEntry[1];
} LOGPALETTE;
#define ERROR_SUCCESS                                      0
typedef struct
{
    UINT	wPeriodMin;
    UINT	wPeriodMax;
}
TIMECAPS, *LPTIMECAPS;

struct COM_OBJECT_INFO
{
    GUID clsid;
    long (*GetClassObject) (GUID* clsid, const GUID* iid, void** ppv);
};
#define	MODULE_HANDLE_kernel32	((HMODULE)0x120)
#define	MODULE_HANDLE_user32	((HMODULE)0x121)

#define	MODULE_HANDLE_wininet	((HMODULE)0x122)
#define	MODULE_HANDLE_ddraw	((HMODULE)0x123)
#define	MODULE_HANDLE_advapi32	((HMODULE)0x124)


typedef struct 
{
    char hay[0x40];
    unsigned long cbFormat;		//0x40
    char*	pbFormat;		//0x44
}
MY_MEDIA_TYPE;

typedef struct tagMONITORINFO
{
    DWORD  cbSize;
    RECT   rcMonitor;
    RECT   rcWork;
    DWORD  dwFlags;
}
MONITORINFO, *LPMONITORINFO;

typedef struct tagMONITORINFOEX
{
    DWORD  cbSize;
    RECT   rcMonitor;
    RECT   rcWork;
    DWORD  dwFlags;
    CHAR  szDevice[8];
}
MONITORINFOEX, *LPMONITORINFOEX;

typedef struct tagPOINT
{
    LONG x;
    LONG y;
}
POINT, *PPOINT;

#define LPWNDCLASS void *
#define RVA_PTR(module,field) ((LPBYTE)(module) + PE_HEADER(module)->field)
static int g_sl=0;

struct virt_alloc_s;
typedef struct virt_alloc_s
{
    int mapping_size;
    char* address;
    struct virt_alloc_s* next;
    struct virt_alloc_s* prev;
    int state;
}
virt_alloc;
static virt_alloc* vm=0;
struct  exports
{
    char name[64];
    int id;
    void* func;
};
struct libs
{
    char name[64];
    int length;
    struct   exports*  exps;
};
extern int unk_exp1;
char export_names[600][32]={
                               "name1",
                               //"name2",
                               //"name3"
                           };

HANDLE __stdcall FindResourceA( HMODULE hModule, LPCSTR name, LPCSTR type );
HANDLE __stdcall FindResourceW( HMODULE hModule, LPCWSTR name, LPCWSTR type );
BOOL __stdcall FreeResource( HGLOBAL handle );
DWORD __stdcall SizeofResource( HINSTANCE hModule, HRSRC hRsrc );
LPVOID __stdcall LockResource( HGLOBAL handle );

//***********************************************************************************************
void __stdcall expDummyStub()
{
	mp_msg(0,0,"expDummyStub()\n");
}
//***********************************************************************************************
void __stdcall expRtlUnwind(void* TargetFrame,void* TargetIp,void* ExceptionRecord,void* ReturnValue)
{
	
}

#define SM_CXSCREEN		0
#define SM_CYSCREEN		1
#define SM_XVIRTUALSCREEN	76
#define SM_YVIRTUALSCREEN	77
#define SM_CXVIRTUALSCREEN 	78
#define SM_CYVIRTUALSCREEN	79
#define SM_CMONITORS		80
//***********************************************************************************************
static int __stdcall expGetSystemMetrics(int index)
{
    switch(index)
    {
    case SM_XVIRTUALSCREEN:
    case SM_YVIRTUALSCREEN:
        return 0;
    case SM_CXSCREEN:
    case SM_CXVIRTUALSCREEN:
        return PSEUDO_SCREEN_WIDTH;
    case SM_CYSCREEN:
    case SM_CYVIRTUALSCREEN:
        return PSEUDO_SCREEN_HEIGHT;
    case SM_CMONITORS:
        return 1;
    }
		return 1;

}
//***********************************************************************************************
static int __stdcall expGetSysColor(int index)
{
    return 1;
}
//***********************************************************************************************
static int __stdcall expGetSysColorBrush(int index)
{
    return 1;
}

//***********************************************************************************************
static int expRegisterClassA(const void/*WNDCLASSA*/ *wc)
{
    return time(NULL); /* be precise ! */
}

//***********************************************************************************************
static int  expUnregisterClassA(const char *className, HINSTANCE hInstance)
{
    return 0;
}

//***********************************************************************************************
static int __stdcall expDrawTextA(int hDC, char* lpString, int nCount,LPRECT lpRect, unsigned int uFormat)
{
    mp_msg(0,0,"expDrawTextA(%p,...) => 8\n", hDC);
    return 8;
}

//***********************************************************************************************
static void* __stdcall expGetWindowDC(int hdc)
{
    return 0;
}

//***********************************************************************************************
static int __stdcall expRegisterWindowMessageA(char *message)
{
    //dbgprintf("RegisterWindowMessageA(%s)\n", message);
    return 1;
}
//***********************************************************************************************
static int __stdcall expShowCursor(int show)
{
		static int show_cursor = 0;

    //mp_msg(0,0,"ShowCursor(%d) => %d\n", show, show);
    if (show)
        show_cursor++;
    else
        show_cursor--;
    return show_cursor;
}



//***********************************************************************************************
static WIN_BOOL __stdcall expGetProcessAffinityMask(HANDLE hProcess,LPDWORD lpProcessAffinityMask,LPDWORD lpSystemAffinityMask)
{
    //mp_msg(0,0,"GetProcessAffinityMask(0x%x, 0x%x, 0x%x) => 1\n",hProcess, lpProcessAffinityMask, lpSystemAffinityMask);
    if(lpProcessAffinityMask)
        *lpProcessAffinityMask=1;
    if(lpSystemAffinityMask)
        *lpSystemAffinityMask=1;
    return 1;
}


//***********************************************************************************************
static UINT __stdcall expSetErrorMode(UINT i)
{
	//mp_msg(0,0,"expSetErrorMode");
	return 0;
}
//***********************************************************************************************
//<<TODO>>
static HANDLE __stdcall expFindFirstFileA(LPCSTR s, /*LPWIN32_FIND_DATA*/void* lpfd)
{
	mp_msg(0,0,"expFindFirstFileA");
	return (HANDLE)-1;
}

//***********************************************************************************************
//<<TODO>>
static WIN_BOOL __stdcall expFindNextFileA(HANDLE h,/*LPWIN32_FIND_DATA*/void* lpfd)
{
	mp_msg(0,0,"expFindNextFileA\n");
	return FALSE;
}
//***********************************************************************************************
//<<TODO>>
static WIN_BOOL __stdcall expFindClose(HANDLE h)
{
    mp_msg(0,0,"FindClose(0x%x) => 0\n", h);
    return TRUE;
}
//***********************************************************************************************
static WIN_BOOL  __stdcall expFileTimeToLocalFileTime(const FILETIME* cpf, LPFILETIME pf)
{
    mp_msg(0,0,"FileTimeToLocalFileTime(0x%x, 0x%x) => 0\n", cpf, pf);
    return 0;
}
//***********************************************************************************************
static WIN_BOOL  __stdcall expDeleteFileA(char* s)
{
    //mp_msg(0,0,"DeleteFileA(0x%x='%s') => 0\n", s, s);
    return 0;
}
#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2


//***********************************************************************************************
//<<TODO>>
static UINT __stdcall expGetTempFileNameA(LPCSTR cs1,LPCSTR cs2,UINT i,LPSTR ps)
{
	mp_msg(0,0,"expGetTempFileNameA\n");
	sprintf(ps,"AP_18274");
	return strlen(ps);
}

//***********************************************************************************************
static UINT __stdcall expGetSystemDirectoryA(char* lpBuffer, UINT uSize)
{
    //mp_msg(0,0,"GetSystemDirectoryA()\n");
    if(!lpBuffer)
        strcpy(lpBuffer,".");
    return 1;
}
//***********************************************************************************************
static UINT __stdcall expGetWindowsDirectoryA(LPSTR s,UINT c)
{
    char windir[]="C:\\windows";
    int result;
    strncpy(s, windir, c);
    result=1+((c<strlen(windir))?c:strlen(windir));
    //mp_msg(0,0,"GetWindowsDirectoryA(0x%x, %d) => %d\n", s, c, result);
    return result;
}
//***********************************************************************************************
static UINT __stdcall expGetCurrentDirectoryA(UINT c, LPSTR s)
{
    char curdir[]="C:\\";
    int result;
    strncpy(s, curdir, c);
    result=1+((c<strlen(curdir))?c:strlen(curdir));
    //mp_msg(0,0,"GetCurrentDirectoryA(0x%x, %d) => %d\n", s, c, result);
    return result;
}
//***********************************************************************************************
// <TODO>
static int __stdcall expSetCurrentDirectoryA(const char *pathname)
{
	//mp_msg(0,0,"expSetCurrentDirectoryA\n");
	return 1;
}
//***********************************************************************************************
// <TODO>
static int __stdcall expCreateDirectoryA(const char *pathname, void *sa)
{
	mp_msg(0,0,"expCreateDirectoryA %s\n",pathname);
	return 1;
}
//***********************************************************************************************
static unsigned int __stdcall expGetTempPathA(unsigned int len, char* path)
{
    //mp_msg(0,0,"GetTempPathA(%d, 0x%x)", len, path);
    if(len<6)
    {
        return 0;
    }
    strcpy(path, "Q:\\dll");
    return 6;
}
//***********************************************************************************************
static DWORD __stdcall expGetShortPathNameA(    LPCSTR longpath,LPSTR shortpath,DWORD shortlen)
{
    mp_msg(0,0,"GetShortPathNameA('%s',%p,%d)\n",longpath,shortpath,shortlen);
    if(!longpath)
        return 0;
    strcpy(shortpath,longpath);
    return strlen(shortpath);
}
//***********************************************************************************************
static DWORD __stdcall expGetFullPathNameA(    const char* lpFileName,    DWORD nBufferLength,    char* lpBuffer,    char* lpFilePart)
{
	mp_msg(0,0,"expGetFullPathNameA\n");
   if(!lpFileName)
        return 0;
	if (strrchr(lpFileName, '\\'))
			lpFilePart = strrchr(lpFileName, '\\');
   else
        lpFilePart = (char*)lpFileName;

    strcpy(lpBuffer, lpFileName);
    //    strncpy(lpBuffer, lpFileName, rindex(lpFileName, '\\')-lpFileName);
    return strlen(lpBuffer);
}
//***********************************************************************************************
static int __stdcall expGetProcessVersion(int pid)
{
    //mp_msg(0,0,"GetProcessVersion(%d)\n", pid);
    return 1;
}
//***********************************************************************************************
static int __stdcall expGetOEMCP(void)
{
    //mp_msg(0,0,"GetOEMCP()\n");
    return 1;
}
//***********************************************************************************************
static int __stdcall expGetCPInfo(int cp,void *info)
{
    //mp_msg(0,0,"GetCPInfo()\n");
    return 0;
}
//***********************************************************************************************
static int __stdcall expGetEnvironmentVariableA(const char* name, char* field, int size)
{
    //    mp_msg(0,0,"%s %x %x\n", name, field, size);
    if(field)
        field[0]=0;
    /*
     p = getenv(name);
     if (p) strncpy(field,p,size);
     */
    if (strcmp(name,"__MSVCRT_HEAP_SELECT")==0)
        strcpy(field,"__GLOBAL_HEAP_SELECTED,1");
    //mp_msg(0,0,"GetEnvironmentVariableA(0x%x='%s', 0x%x, %d) => %d\n", name, name, field, size, strlen(field));
    return strlen(field);
}
//***********************************************************************************************
static int __stdcall expDuplicateHandle(HANDLE hSourceProcessHandle,  // handle to source process
                                     HANDLE hSourceHandle,         // handle to duplicate
                                     HANDLE hTargetProcessHandle,  // handle to target process
                                     HANDLE* lpTargetHandle,      // duplicate handle
                                     DWORD dwDesiredAccess,        // requested access
                                     int bInheritHandle,          // handle inheritance option
                                     DWORD dwOptions               // optional actions
                                    )
{
    //mp_msg(0,0,"DuplicateHandle(%p, %p, %p, %p, 0x%x, %d, %d) called\n",hSourceProcessHandle, hSourceHandle, hTargetProcessHandle,lpTargetHandle, dwDesiredAccess, bInheritHandle, dwOptions);
    *lpTargetHandle = hSourceHandle;
    return 1;
}

//***********************************************************************************************
static int __stdcall expSetEnvironmentVariableA(const char *name, const char *value)
{
    //mp_msg(0,0,"SetEnvironmentVariableA(%s, %s)\n", name, value);
    return 0;
}
//***********************************************************************************************
static int __stdcall expGetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation)
{
    const short name[]=
        {'C', 'e', 'n', 't', 'r', 'a', 'l', ' ', 'S', 't', 'a',
         'n', 'd', 'a', 'r', 'd', ' ', 'T', 'i', 'm', 'e', 0
        };
    const short pname[]=
        {'C', 'e', 'n', 't', 'r', 'a', 'l', ' ', 'D', 'a', 'y',
         'l', 'i', 'g', 'h', 't', ' ', 'T', 'i', 'm', 'e', 0
        };
    //mp_msg(0,0,"GetTimeZoneInformation(0x%x) => TIME_ZONE_ID_STANDARD\n");
    memset(lpTimeZoneInformation, 0, sizeof(TIME_ZONE_INFORMATION));
    lpTimeZoneInformation->Bias=360;//GMT-6
    memcpy(lpTimeZoneInformation->StandardName, name, sizeof(name));
    lpTimeZoneInformation->StandardDate.wMonth=10;
    lpTimeZoneInformation->StandardDate.wDay=5;
    lpTimeZoneInformation->StandardDate.wHour=2;
    lpTimeZoneInformation->StandardBias=0;
    memcpy(lpTimeZoneInformation->DaylightName, pname, sizeof(pname));
    lpTimeZoneInformation->DaylightDate.wMonth=4;
    lpTimeZoneInformation->DaylightDate.wDay=1;
    lpTimeZoneInformation->DaylightDate.wHour=2;
    lpTimeZoneInformation->DaylightBias=-60;
    return TIME_ZONE_ID_STANDARD;
}


//***********************************************************************************************
//<<TODO>>
static long __stdcall expCreateFileMappingA(int hFile, void* lpAttr,long flProtect, long dwMaxHigh,long dwMaxLow, const char* name)
{
//	long result=CreateFileMappingA(hFile, lpAttr, flProtect, dwMaxHigh, dwMaxLow, name);
	//return result;
	return -1;
}

//***********************************************************************************************
static int __stdcall expGetPrivateProfileStringA(const char* appname,const char* keyname,const char* def_val,char* dest, unsigned int len,const char* filename)
{
    int result;
    int size;
    char* fullname;
  //  dbgprintf("GetPrivateProfileStringA('%s', '%s', def_val '%s', 0x%x, 0x%x, '%s')", appname, keyname, def_val, dest, len, filename );
    if(!(appname && keyname && filename) )
        return 0;
    fullname=(char*)malloc(50+strlen(appname)+strlen(keyname)+strlen(filename));
    strcpy(fullname, "Software\\IniFileMapping\\");
    strcat(fullname, appname);
    strcat(fullname, "\\");
    strcat(fullname, keyname);
    strcat(fullname, "\\");
    strcat(fullname, filename);
    size=len;
    result=RegQueryValueExA((long)HKEY_LOCAL_MACHINE, fullname, NULL, NULL, (int*)dest, &size);
    free(fullname);
    if(result)
    {
        strncpy(dest, def_val, size);
        if (strlen(def_val)< size)
            size = strlen(def_val);
    }
//    dbgprintf(" => %d ( '%s' )\n", size, dest);
    return size;

}
#define REG_SZ			1	/* string type (ASCII) */

//***********************************************************************************************
static int __stdcall expWritePrivateProfileStringA(const char* appname,const char* keyname,const char* string,const char* filename)
{
	mp_msg(0,0,"expWritePrivateProfileStringA\n");
    int size=256;
    char* fullname;
//    dbgprintf("WritePrivateProfileStringA('%s', '%s', '%s', '%s')", appname, keyname, string, filename );
    if(!(appname && keyname && filename) )
    {
//        dbgprintf(" => -1\n");
        return -1;
    }
    fullname=(char*)malloc(50+strlen(appname)+strlen(keyname)+strlen(filename));
    strcpy(fullname, "Software\\IniFileMapping\\");
    strcat(fullname, appname);
    strcat(fullname, "\\");
    strcat(fullname, keyname);
    strcat(fullname, "\\");
    strcat(fullname, filename);
    RegSetValueExA((long)HKEY_LOCAL_MACHINE, fullname, 0, REG_SZ, (int*)string, strlen(string));
    //    printf("RegSetValueExA(%s,%d)\n", string, strlen(string));
    //    printf("WritePrivateProfileStringA(%s, %s, %s, %s)\n", appname, keyname, string, filename );
    free(fullname);
    return 0;
}

//***********************************************************************************************
//<<TODO>>
static long __stdcall expOpenFileMappingA(long hFile, long hz, const char* name)
{
	mp_msg(0,0,"expOpenFileMappingA\n");
//	long result=OpenFileMappingA(hFile, hz, name);
//	return result;
	return -1;
}

//***********************************************************************************************
static void* __stdcall expMapViewOfFile(HANDLE file, DWORD mode, DWORD offHigh,DWORD offLow, DWORD size)
{
	mp_msg(0,0,"expMapViewOfFile\n");
	return (char*)file+offLow;
}

//***********************************************************************************************
static int __stdcall expGetPrivateProfileIntA(const char* appname,const char* keyname,int default_value,const char* filename)
{
    int size=255;
    char buffer[256];
    char* fullname;
    int result;

    buffer[255]=0;
    if(!(appname && keyname && filename) )
    {
        //dbgprintf("GetPrivateProfileIntA('%s', '%s', %d, '%s') => %d\n", appname, keyname, default_value, filename, default_value );
        return default_value;
    }
    fullname=(char*)malloc(50+strlen(appname)+strlen(keyname)+strlen(filename));
    strcpy(fullname, "Software\\IniFileMapping\\");
    strcat(fullname, appname);
    strcat(fullname, "\\");
    strcat(fullname, keyname);
    strcat(fullname, "\\");
    strcat(fullname, filename);
    result=RegQueryValueExA((long)HKEY_LOCAL_MACHINE, fullname, NULL, NULL, (int*)buffer, &size);
    if((size>=0)&&(size<256))
        buffer[size]=0;
    //    printf("GetPrivateProfileIntA(%s, %s, %s) -> %s\n", appname, keyname, filename, buffer);
    free(fullname);
    if(result)
        result=default_value;
    else
        result=atoi(buffer);
    //dbgprintf("GetPrivateProfileIntA('%s', '%s', %d, '%s') => %d\n", appname, keyname, default_value, filename, result);
    return result;
}
//***********************************************************************************************
WIN_BOOL __stdcall expUnmapViewOfFile(LPVOID handle)
{
	mp_msg(0,0,"UnmapViewOfFile\n");
	return 0;
}
//***********************************************************************************************
static int __stdcall expSetHandleCount(int count)
{
    //mp_msg(0,0,"SetHandleCount(0x%x) => 1\n", count);
    return 1;
}
//***********************************************************************************************
static int __stdcall expGetACP(void)
{
    //mp_msg(0,0,"GetACP() => 0\n");
    return 0;
}
//***********************************************************************************************
static const char* __stdcall expGetCommandLineA()
{
    //mp_msg(0,0,"expGetCommandLineA()");
    return "c:\\aviplay.exe";
}

static const WCHAR* __stdcall expGetCommandLineW()
{
    return L"c:\\aviplay.exe";
}


//***********************************************************************************************
static LPWSTR __stdcall expGetEnvironmentStringsW()
{
    //mp_msg(0,0,"GetEnvironmentStringsW() \n");
    return 0;
}
//***********************************************************************************************
static LPCSTR __stdcall expGetEnvironmentStrings()
{
    //mp_msg(0,0,"GetEnvironmentStrings() \n");
    return (LPCSTR)ch_envs;
}

//***********************************************************************************************
static int __stdcall expGetFileType(int handle)
{
    //mp_msg(0,0,"GetFileType() => 0x3 = pipe\n", handle);
    return 0x3;
}


//***********************************************************************************************
static int __stdcall expGetFileAttributesA(char *filename)
{
    //mp_msg(0,0,"GetFileAttributesA(%s) => FILE_ATTR_NORMAL\n", filename);
    if (strstr(filename, "QuickTime.qts"))
        return FILE_ATTRIBUTE_SYSTEM;
    return FILE_ATTRIBUTE_NORMAL;
}

//***********************************************************************************************
static int __stdcall expGetStartupInfoA(STARTUPINFOA *s)
{
    //mp_msg(0,0,"GetStartupInfoA()\n");
    memset(s, 0, sizeof(*s));
    s->cb=sizeof(*s);
    // s->lpReserved="Reserved";
    // s->lpDesktop="Desktop";
    // s->lpTitle="Title";
    // s->dwX=s->dwY=0;
    // s->dwXSize=s->dwYSize=200;
    s->dwFlags=s->wShowWindow=1;
    // s->hStdInput=s->hStdOutput=s->hStdError=0x1234;
    return 1;
}

//***********************************************************************************************
static int __stdcall expGetStdHandle(int z)
{
    //mp_msg(0,0,"GetStdHandle()\n");
    return z+0x1234;
}

//***********************************************************************************************
static int __stdcall expFreeEnvironmentStringsW(short* strings)
{
    //mp_msg(0,0,"FreeEnvironmentStringsW()\n");
    return 1;
}
//***********************************************************************************************
static BOOL __stdcall expFreeEnvironmentStringsA(char* strings)
{
    //mp_msg(0,0,"FreeEnvironmentStringsA() \n");
    return 1;
}

extern HRSRC __stdcall LoadResource(HMODULE, HRSRC);
//***********************************************************************************************
static HGLOBAL __stdcall expLoadResource(HMODULE module, HRSRC res)
{
	HGLOBAL result=LoadResource(module, res);
  return result;
}
//***********************************************************************************************
static HRSRC __stdcall expFindResourceA(HMODULE module, char* name, char* type)
{
	HRSRC result;

  result=FindResourceA(module, name, type);
  return result;
}

//***********************************************************************************************
static void* __stdcall expLockResource(long res)
{
		void* result=LockResource((HGLOBAL)res);
    return result;
}
//***********************************************************************************************
static int __stdcall expFreeResource(long res)
{
		mp_msg(0,0,"expFreeResource()");
		int result=FreeResource((HGLOBAL)res);
		return result;
}

//***********************************************************************************************
static int __stdcall expSizeofResource(int v1, int v2)
{
		mp_msg(0,0,"expSizeofResource()");
	int result=SizeofResource((HINSTANCE)v1, (HRSRC)v2);
	return result;
}
//***********************************************************************************************
WIN_BOOL    __stdcall expTerminateProcess(HANDLE,unsigned int)
{
	return TRUE;
}

//***********************************************************************************************
static LONG __stdcall expInterlockedExchange(long *dest, long l)
{
    long retval = *dest;
    *dest = l;
    return retval;
}
//***********************************************************************************************
static long __stdcall expInterlockedCompareExchange( unsigned long* dest, unsigned long exchange, unsigned long comperand)
{
    unsigned long retval = *dest;
    if(*dest == comperand)
        *dest = exchange;
    return retval;
}

//***********************************************************************************************
static unsigned int __stdcall expGetProfileIntA(const char* appname,const char* keyname,int default_value)
{
    mp_msg(0,0,"GetProfileIntA \n");
    return default_value;
}
//***********************************************************************************************
static int  __stdcall  expReleaseDC(HANDLE hwnd, HANDLE hdc)
{
    //mp_msg(0,0,"ReleaseDC(0x%x, 0x%x) => 1\n", hwnd, hdc);
    return 1;
}
//***********************************************************************************************
static HANDLE  __stdcall  expGetDesktopWindow()
{
    //mp_msg(0,0,"GetDesktopWindow() => 0\n");
    return 0;
}
//***********************************************************************************************
static HANDLE  __stdcall  expGetDC(HANDLE hwnd)
{
    //mp_msg(0,0,"GetDC(0x%x) => 1\n", hwnd);
    return (HANDLE)1;
}

//***********************************************************************************************
static BOOL  __stdcall  expIsRectEmpty(const RECT *lprc)
{
    BOOL r = FALSE;
    int w,h;
    //trapbug();
    if (lprc)
    {
        w = lprc->right - lprc->left;
        h = lprc->bottom - lprc->top;
        if (w <= 0 || h <= 0)
            r = TRUE;
    }
    else
        r = TRUE;

    //mp_msg(0,0,"IsRectEmpty(%p) => %s\n", lprc, (r) ? "TRUE" : "FALSE");
    //mp_msg(0,0,"Rect: left: %d, top: %d, right: %d, bottom: %d\n", lprc->left, lprc->top, lprc->right, lprc->bottom);
    //    return 0;	// wmv9?
    return r; // TM20
}
//***********************************************************************************************
static LONG  __stdcall  expGetWindowLongA(HWND win, int index)
{
    //mp_msg(0,0,"GetWindowLongA(0x%x, %d) => 0\n", win, index);
    return 1;
}
//***********************************************************************************************
static long  __stdcall  expSetWindowLongA(HWND win, int index, long dwNextLong)
{
    //mp_msg(0,0,"SetWindowLongA(0x%x, %d) => 0\n", win, index);
    return 1;
}
//***********************************************************************************************
static BOOL  __stdcall  expDestroyWindow(HWND win)
{
	return TRUE;
}
//***********************************************************************************************
static BOOL  __stdcall  expShowWindow(HWND win,int nCmdShow)
{
	return TRUE;
}
//***********************************************************************************************
static BOOL __stdcall expInvalidateRect(HWND hWnd,const RECT* lpRect,BOOL bErase)
{
	return TRUE;
}
//***********************************************************************************************
static void __stdcall expCoFreeUnusedLibraries(void)
{
}
//***********************************************************************************************
static void __stdcall expCoUninitialize(void)
{
}


//***********************************************************************************************
static BOOL  __stdcall  expGetWindowRect(HWND win, RECT *r)
{
    //mp_msg(0,0,"GetWindowRect(0x%x, 0x%x) => 1\n", win, r);
    /* (win == 0) => desktop */
    r->right = PSEUDO_SCREEN_WIDTH;
    r->left = 0;
    r->bottom = PSEUDO_SCREEN_HEIGHT;
    r->top = 0;
    return 1;
}

//***********************************************************************************************
static HANDLE  __stdcall  expLoadCursorA(int handle,LPCSTR name)
{
    //mp_msg(0,0,"LoadCursorA(%d, 0x%x='%s') => 0x%x\n", handle, name, (int)&cursor[0]);
    return (HANDLE)&cursor[0];
}
//***********************************************************************************************
static BOOL  __stdcall  expSetCursor(int x, int y)
{
    //mp_msg(0,0,"SetCursor(0x%x) => 0x%x\n", cursor, cursor);
    return TRUE;
}
//***********************************************************************************************
static BOOL __stdcall  expGetCursorPos(void *cursor)
{
    //mp_msg (0,0,"GetCursorPos(0x%x) => 0x%x\n", cursor, cursor);
    return 1;
}
//***********************************************************************************************
static DWORD  __stdcall  expGetModuleFileNameA(HANDLE module, char* s, DWORD len)
{
    WINE_MODREF *mr;
    int result;
    mp_msg(0,0,"File name of module %x (%s) requested\n", module, s);

    if (module == 0 && len >= 12)
    {
        /* return caller program name */
        strcpy(s, "aviplay.dll");
        result=1;
    }
    else if(s==0)
        result=0;
    else
        if(len<35)
            result=0;
        else
        {
            result=1;
            strcpy(s, "c:\\windows\\system\\");
            mr=MODULE32_LookupHMODULE((void*)module);
            if(mr==0)//oops
                strcat(s, "aviplay.dll");
            else
                if(strrchr(mr->filename, '/')==NULL)
                    strcat(s, mr->filename);
                else
                    strcat(s, strrchr(mr->filename, '/')+1);
        }
    if(!s)
        mp_msg(0,0,"GetModuleFileNameA(0x%x, 0x%x, %d) => %d\n", module, s, len, result);
    else
        mp_msg(0,0,"GetModuleFileNameA(0x%x, 0x%x, %d) => %d ( '%s' )\n",        module, s, len, result, s);
    return result;
}

//***********************************************************************************************
static LONG  __stdcall  explstrcmpiA(const char* str1, const char* str2)
{
    LONG result=stricmp(str1, str2);
    //mp_msg(0,0,"strcmpi(0x%x='%s', 0x%x='%s') => %d\n", str1, str1, str2, str2, result);
    return result;
}
//***********************************************************************************************
static LONG  __stdcall  explstrlenA(const char* str1)
{
    LONG result=strlen(str1);
    //mp_msg(0,0,"strlen(0x%x='%.50s') => %d\n", str1, str1, result);
    return result;
}

//***********************************************************************************************
static LONG  __stdcall  explstrcpyA(char* str1, const char* str2)
{
    int result= (int) strcpy(str1, str2);
    //mp_msg(0,0,"strcpy(0x%.50x, 0x%.50x='%.50s') => %d\n", str1, str2, str2, result);
    return result;
}
//***********************************************************************************************
LONG  __stdcall  explstrcpynA(char* str1, const char* str2,int len)
{
    int result;
    if (strlen(str2)>len)
        result = (int) strncpy(str1, str2,len);
    else
        result = (int) strcpy(str1,str2);
    //mp_msg(0,0,"strncpy(0x%x, 0x%x='%s' len %d strlen %d) => %x\n", str1, str2, str2,len, strlen(str2),result);
    return result;
}
//***********************************************************************************************
static LONG  __stdcall explstrcatA(char* str1, const char* str2)
{
    int result= (int) strcat(str1, str2);
    //mp_msg(0,0,"strcat(0x%x, 0x%x='%s') => %d\n", str1, str2, str2, result);
    return result;
}

//***********************************************************************************************
static void *  __stdcall  expRtlZeroMemory(void *p, size_t len)
{
    void* result=memset(p,0,len);
    //mp_msg(0,0,"RtlZeroMemory(0x%x, len %d) => 0x%x\n",p,len,result);
    return result;
}
//***********************************************************************************************
static void *  __stdcall  expRtlMoveMemory(void *dst, void *src, size_t len)
{
    void* result=memmove(dst,src,len);
    //mp_msg(0,0,"RtlMoveMemory (dest 0x%x, src 0x%x, len %d) => 0x%x\n",dst,src,len,result);
    return result;
}

//***********************************************************************************************
static void *  __stdcall  expRtlFillMemory(void *p, int ch, size_t len)
{
    void* result=memset(p,ch,len);
    //mp_msg(0,0,"RtlFillMemory(0x%x, char 0x%x, len %d) => 0x%x\n",p,ch,len,result);
    return result;
}

//***********************************************************************************************
static int  __stdcall  expSetThreadIdealProcessor(HANDLE thread, int proc)
{
    //mp_msg(0,0,"SetThreadIdealProcessor(0x%x, %x) => 0\n", thread, proc);
    return 0;
}

//***********************************************************************************************
static int  __stdcall  expSetUnhandledExceptionFilter(void* filter)
{
    //mp_msg(0,0,"SetUnhandledExceptionFilter(0x%x) => 1\n", filter);
    return 1;//unsupported and probably won't ever be supported
}

//***********************************************************************************************
static void  __stdcall  expOutputDebugStringA( const char* string )
{
    //mp_msg(0,0,"DEBUG: %s\n", string);
}
#define VER_PLATFORM_WIN32_WINDOWS      1
//***********************************************************************************************
static long  __stdcall  expGetVersionExA(OSVERSIONINFOA* c)
{
    //mp_msg(0,0,"GetVersionExA(0x%x) => 1\n");
    c->dwOSVersionInfoSize=sizeof(*c);
    c->dwMajorVersion=4;
    c->dwMinorVersion=0;
    c->dwBuildNumber=0x4000457;
    c->dwPlatformId=VER_PLATFORM_WIN32_WINDOWS;
    strcpy(c->szCSDVersion, " B");
    return 1;
}

//***********************************************************************************************
static int  __stdcall  expCreateFontA(void)
{
    //mp_msg(0,0,"CreateFontA() => 0x0\n");
    return 1;
}

//***********************************************************************************************
static WIN_BOOL  __stdcall  expDeleteDC(int hdc)
{
    //mp_msg(0,0,"DeleteDC(0x%x) => 0\n", hdc);
    if (hdc == 0x81)
        return 1;
    return 0;
}
//***********************************************************************************************
static int  __stdcall  expGetObjectA(HGDIOBJ hobj, int objsize, LPVOID obj)
{
    //mp_msg(0,0,"GetObjectA(0x%x, %d, 0x%x) => %d\n", hobj, objsize, obj, objsize);
    return objsize;
}
//***********************************************************************************************
static int  __stdcall  expCreateRectRgn(int x, int y, int width, int height)
{
    //mp_msg(0,0,"CreateRectRgn(%d, %d, %d, %d) => 0\n", x, y, width, height);
    return 0;
}
//***********************************************************************************************
static WIN_BOOL  __stdcall  expDeleteObject(int hdc)
{
    //mp_msg(0,0,"DeleteObject(0x%x) => 1\n", hdc);
    /* FIXME - implement code here */
    return 1;
}

//***********************************************************************************************
static HPALETTE  __stdcall  expCreatePalette( LOGPALETTE *lpgpl)
{
    HPALETTE test;
    int i;

    //mp_msg(0,0,"CreatePalette(%x) => NULL\n", lpgpl);

    i = sizeof(LOGPALETTE)+((lpgpl->palNumEntries-1)*sizeof(PALETTEENTRY));
    test = (HPALETTE)malloc(i);
    memcpy((void *)test, lpgpl, i);

    return test;
}

//***********************************************************************************************
static int  __stdcall  expGetSystemPaletteEntries(int hdc, int iStartIndex, int nEntries, void* lppe)
{
    mp_msg(0,0,"GetSystemPaletteEntries(0x%x, 0x%x, 0x%x, 0x%x) => 0\n", hdc, iStartIndex, nEntries, lppe);
    return 0;
}
//***********************************************************************************************
static int  __stdcall  expGetDeviceCaps(int hdc, int unk)
{
    //mp_msg(0,0,"GetDeviceCaps(0x%x, %d) => 0\n", hdc, unk);

#define BITSPIXEL 12
#define PLANES    14
    if (unk == BITSPIXEL)
        return 24;
    if (unk == PLANES)
        return 1;

    return 1;
}

//***********************************************************************************************
// <TODO>
void  exp_EH_prolog(void)
{
	mp_msg(0,0,"exp_EH_prolog(...)\n");
}
//***********************************************************************************************
static int __stdcall  expDirectDrawCreate(void)
{
    mp_msg(0,0,"DirectDrawCreate(...) => NULL\n");
    return 0;
}


//***********************************************************************************************
static HWND  __stdcall  expCreateUpDownControl (DWORD style, int x, int y, int cx, int cy,HWND parent, int id, HINSTANCE inst,HWND buddy, int maxVal, int minVal, int curVal)
{
    mp_msg(0,0,"CreateUpDownControl(...)\n");
    return 0;
}
//***********************************************************************************************
static void  __stdcall  expInitCommonControls(void)
{
    //mp_msg(0,0,"InitCommonControls called!\n");
    return;
}
//***********************************************************************************************
static int exp_initterm(int v1, int v2)
{
    //mp_msg(0,0,"_initterm(0x%x, 0x%x) => 0\n", v1, v2);
    return 0;
}
//***********************************************************************************************
static void* exp__dllonexit()
{
    // FIXME extract from WINE
    return NULL;
}

//***********************************************************************************************
static long __stdcall expRegOpenKeyExA(long key, const char* subkey, long reserved, long access, int* newkey)
{
    long result=RegOpenKeyExA(key, subkey, reserved, access, newkey);
    //mp_msg(0,0,"RegOpenKeyExA(key 0x%x, subkey %s, reserved %d, access 0x%x, pnewkey 0x%x) => %d\n",key, subkey, reserved, access, newkey, result);
//    if(newkey)
  //      mp_msg(0,0,"  New key: 0x%x\n", *newkey);
    return result;
}
//***********************************************************************************************
static long __stdcall expRegCloseKey(long key)
{
    long result=RegCloseKey(key);
    //mp_msg(0,0,"RegCloseKey(0x%x) => %d\n", key, result);
    return result;
}
//***********************************************************************************************
static long __stdcall expRegQueryValueExA(long key, const char* value, int* reserved, int* type, int* data, int* count)
{
    long result=RegQueryValueExA(key, value, reserved, type, data, count);
    return result;
}
//***********************************************************************************************
static long __stdcall expRegCreateKeyExA(long key, const char* name, long reserved,
                                      void* classs, long options, long security,
                                      void* sec_attr, int* newkey, int* status)
{
    long result=RegCreateKeyExA(key, name, reserved, classs, options, security, sec_attr, newkey, status);
    return result;
}
//***********************************************************************************************
static long __stdcall expRegSetValueExA(long key, const char* name, long v1, long v2, void* data, long size)
{
    long result=RegSetValueExA(key, name, v1, v2, data, size);
    return result;
}

//***********************************************************************************************
static long __stdcall expRegOpenKeyA (long hKey, LPCSTR lpSubKey, int* phkResult)
{
    long result=RegOpenKeyExA(hKey, lpSubKey, 0, 0, phkResult);
    return result;
}

//***********************************************************************************************
static DWORD __stdcall expRegEnumValueA(HKEY hkey, DWORD index, LPSTR value, LPDWORD val_count,LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD count)
{
    return RegEnumValueA(hkey, index, value, val_count,
                         reserved, type, data, count);
}

//***********************************************************************************************
static DWORD __stdcall expRegEnumKeyExA(HKEY hKey, DWORD dwIndex, LPSTR lpName, LPDWORD lpcbName,LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcbClass,LPFILETIME lpftLastWriteTime)
{
    return RegEnumKeyExA(hKey, dwIndex, lpName, lpcbName, lpReserved, lpClass,
                         lpcbClass, lpftLastWriteTime);
}

//***********************************************************************************************
static int   expdelete(void* memory)
{
    //mp_msg(0,0,"delete(%p)\n", memory);
    free(memory);
    return 0;
}
//***********************************************************************************************
static void*  expnew(size_t size)
{
    void* result;
    
    result=malloc(size);
    //mp_msg(0,0,"new(%d) => %p\n", size, result);
    if (result==0)
		{
      mp_msg(0,0,"WARNING: new() failed %i\n",size);
			return NULL;
		}
    return result;
}
//***********************************************************************************************
static void *exprealloc(void *ptr, size_t size)
{
    //mp_msg(0,0,"realloc(0x%x, %x)\n", ptr, size);
    return realloc(ptr, size);
}

//***********************************************************************************************
static void *expcalloc(int num, int size)
{
    void* result=malloc(num*size);
    //mp_msg(0,0,"calloc(%d,%d) => %p\n", num,size,result);
    if(result==0)
        mp_msg(0,0,"WARNING: calloc() failed\n");
    return result;
}
static int _adjust_fdiv=0; //what's this? - used to adjust division


//***********************************************************************************************
static void* expfopen(const char* path, const char* mode)
{
    mp_msg(0,0,"fopen: \"%s\"  mode:%s\n", path, mode);
    //return fopen(path, mode);
    return fdopen(0, mode); // everything on screen
}


//***********************************************************************************************
// <TODO>
static char* expgetenv(const char* varname)
{
		mp_msg(0,0,"GETENV(%s)\n", varname);
    return NULL;
}

//***********************************************************************************************
static void* expwcscpy(WCHAR* dst, const WCHAR* src)
{
    WCHAR* p = dst;
    while ((*p++ = *src++))
        ;
    return dst;
}

//***********************************************************************************************
static char* expstrrchr(char* string, int value)
{
    char* result=strrchr(string, value);
    //if(result)
      //  mp_msg(0,0,"strrchr(0x%x='%s', %d) => 0x%x='%s'", string, string, value, result, result);
    //else
      //  mp_msg(0,0,"strrchr(0x%x='%s', %d) => 0", string, string, value);
    return result;
}

//***********************************************************************************************
static char* expstrchr(char* string, int value)
{
    char* result=strchr(string, value);
    //if(result)
      //  mp_msg(0,0,"strchr(0x%x='%s', %d) => 0x%x='%s'", string, string, value, result, result);
    //else
      //  mp_msg(0,0,"strchr(0x%x='%s', %d) => 0", string, string, value);
    return result;
}
//***********************************************************************************************
static int expstrlen(char* str)
{
    int result=strlen(str);
    //mp_msg(0,0,"strlen(0x%x='%s') => %d\n", str, str, result);
    return result;
}
//***********************************************************************************************
static char*   expstrcpy(char* str1, const char* str2)
{
    char* result= strcpy(str1, str2);
    //mp_msg(0,0,"strcpy(0x%x, 0x%x='%s') => %p\n", str1, str2, str2, result);
    return result;
}

static char* expstrncpy(char* str1, const char* str2, size_t count)
{
    char* result= strncpy(str1, str2, count);
    return result;
}

//***********************************************************************************************
static int   expstrcmp(const char* str1, const char* str2)
{
    int result=strcmp(str1, str2);
    //mp_msg(0,0,"strcmp(0x%x='%s', 0x%x='%s') => %d\n", str1, str1, str2, str2, result);
    return result;
}
//***********************************************************************************************
static int   expstrncmp(const char* str1, const char* str2,int x)
{
    int result=strncmp(str1, str2,x);
    //mp_msg(0,0,"strcmp(0x%x='%s', 0x%x='%s') => %d\n", str1, str1, str2, str2, result);
    return result;
}
//***********************************************************************************************
static char* expstrcat(char* str1, const char* str2)
{
    char* result = strcat(str1, str2);
    //mp_msg(0,0,"strcat(0x%x='%s', 0x%x='%s') => %p\n", str1, str1, str2, str2, result);
    return result;
}
//***********************************************************************************************
static char*   exp_strdup(const char* str1)
{
    int l = strlen(str1);
    char* result = (char*) malloc(l + 1);
    if (result)
        strcpy(result, str1);
    //mp_msg(0,0,"_strdup(0x%x='%s') => %p\n", str1, str1, result);
    return result;
}
//***********************************************************************************************
static int expisalnum(int c)
{
    int result= (int) isalnum(c);
    //mp_msg(0,0,"isalnum(0x%x='%c' => %d\n", c, c, result);
    return result;
}
//***********************************************************************************************
static int expisspace(int c)
{
    int result= (int) isspace(c);
    //mp_msg(0,0,"isspace(0x%x='%c' => %d\n", c, c, result);
    return result;
}
//***********************************************************************************************
static int   expisalpha(int c)
{
    int result= (int) isalpha(c);
    //mp_msg(0,0,"isalpha(0x%x='%c' => %d\n", c, c, result);
    return result;
}
//***********************************************************************************************
static int   expisdigit(int c)
{
    int result= (int) isdigit(c);
    //mp_msg(0,0,"isdigit(0x%x='%c' => %d\n", c, c, result);
    return result;
}
//***********************************************************************************************
static void*   expmemmove(void* dest, void* src, int n)
{
    void* result = memmove(dest, src, n);
    //mp_msg(0,0,"memmove(0x%x, 0x%x, %d) => %p\n", dest, src, n, result);
    return result;
}
//***********************************************************************************************
static int   expmemcmp(void* dest, void* src, int n)
{
    int result = memcmp(dest, src, n);
    //mp_msg(0,0,"memcmp(0x%x, 0x%x, %d) => %d\n", dest, src, n, result);
    return result;
}
//***********************************************************************************************
static void*   expmemcpy(void* dest, void* src, int n)
{
    void *result = memcpy(dest, src, n);
    //mp_msg(0,0,"memcpy(0x%x, 0x%x, %d) => %p\n", dest, src, n, result);
    return result;
}
//***********************************************************************************************
static void*   expmemset(void* dest, int c, size_t n)
{
    void *result = memset(dest, c, n);
    //mp_msg(0,0,"memset(0x%x, %d, %d) => %p\n", dest, c, n, result);
    return result;
}
//***********************************************************************************************
static time_t exptime(time_t* t)
{
    time_t result = time(t);
    //mp_msg(0,0,"time(0x%x) => %d\n", t, result);
    return result;
}

//***********************************************************************************************
static int exprand(void)
{
    return rand();
}

//***********************************************************************************************
static void expsrand(int seed)
{
    srand(seed);
}

//***********************************************************************************************
static double explog10(double x)
{ 
    return log10(x);
}

//***********************************************************************************************
static double expcos(double x)
{ 
    return cos(x);
}

//***********************************************************************************************
static long   exp_ftol_wrong(double x)
{
    return (long) x;
}


//***********************************************************************************************
static void exp_ftol(void)
{
    mp_msg(0,0,"FTOL\n");
}
static char* __stdcall expCharNextA(const char* lpsz)
{
	if (*lpsz)
		return (char*)(lpsz+1);
	else return (char*)lpsz;
}
//***********************************************************************************************
static double exp_CIpow(double x, double y)
{
    /*("Pow %f  %f    0x%Lx  0x%Lx  => %f\n", x, y, *((int64_t*)&x), *((int64_t*)&y), pow(x, y));*/
    return pow(x, y);
}

//***********************************************************************************************
static double exppow(double x, double y)
{
    /*("Pow %f  %f    0x%Lx  0x%Lx  => %f\n", x, y, *((int64_t*)&x), *((int64_t*)&y), pow(x, y));*/
    return pow(x, y);
}

//***********************************************************************************************
static double expldexp(double x, int  expo)
{
    /*("Cos %f => %f  0x%Lx\n", x, cos(x), *((int64_t*)&x));*/
    return ldexp(x,  expo);
}

//***********************************************************************************************
static double expfrexp(double x, int*  expo)
{
    /*("Cos %f => %f  0x%Lx\n", x, cos(x), *((int64_t*)&x));*/
    return frexp(x,  expo);
}



//***********************************************************************************************
static int exp_stricmp(const char* s1, const char* s2)
{
    return stricmp(s1, s2);
}

//***********************************************************************************************
static int exp_setjmp3(void* jmpbuf, int x)
{
		mp_msg(0,0,"setjmp3\n");

    return 0;
}


//***********************************************************************************************
static int __stdcall  expSysStringByteLen(void *str)
{
    //mp_msg(0,0,"SysStringByteLen(%p) => %d\n", str, strlen((char*)str));
    return strlen((char*)str);
}

//***********************************************************************************************
static HDRVR __stdcall  expOpenDriverA(LPCSTR szDriverName, LPCSTR szSectionName, LPARAM lParam2)
{
    mp_msg(0,0,"OpenDriverA(0x%x='%s', 0x%x='%s', 0x%x) => -1\n", szDriverName,  szDriverName, szSectionName, szSectionName, lParam2);
    return (HDRVR)-1;
}
//***********************************************************************************************
static HDRVR __stdcall  expOpenDriver(LPCSTR szDriverName, LPCSTR szSectionName,  LPARAM lParam2)
{
    mp_msg(0,0,"OpenDriver(0x%x='%s', 0x%x='%s', 0x%x) => -1\n", szDriverName, szDriverName, szSectionName, szSectionName, lParam2);
    return (HDRVR)-1;
}


//***********************************************************************************************
void*__stdcall  expCoTaskMemAlloc(unsigned long cb)
{
    return XboxCoTaskMemAlloc(cb);
}
//***********************************************************************************************
void __stdcall  expCoTaskMemFree(void* cb)
{
    XboxCoTaskMemFree(cb);
}

//***********************************************************************************************
static HRESULT  __stdcall  expCoInitialize(LPVOID lpReserved	)
{
    /*
     * Just delegate to the newer method.
     */
    return 0; //CoInitializeEx(lpReserved, COINIT_APARTMENTTHREADED);
}
//***********************************************************************************************
static MMRESULT  __stdcall  exptimeGetDevCaps(LPTIMECAPS lpCaps, UINT wSize)
{
    //mp_msg(0,0,"timeGetDevCaps(%p, %u) !\n", lpCaps, wSize);

    lpCaps->wPeriodMin = 1;
    lpCaps->wPeriodMax = 65535;
    return 0;
}

//***********************************************************************************************
 HMODULE  __stdcall  expGetDriverModuleHandle(DRVR* pdrv)
{
    HMODULE result;
    if (pdrv==NULL)
        result=0;
    else
        result=pdrv->hDriverModule;
   //printf ("GetDriverModuleHandle(%p) => %p\n", pdrv, result);
    return result;
}
//***********************************************************************************************
static MMRESULT  __stdcall  exptimeBeginPeriod(UINT wPeriod)
{
   //mp_msg(0,0,"timeBeginPeriod(%u) !\n", wPeriod);

    if (wPeriod < 1 || wPeriod > 65535)
        return 96+1; //TIMERR_NOCANDO;
    return 0;
}
//***********************************************************************************************
static int  __stdcall  expwaveOutGetNumDevs(void)
{
    //mp_msg(0,0,"waveOutGetNumDevs() => 0\n");
    return 0;
}


//***********************************************************************************************
static MMRESULT  __stdcall  exptimeEndPeriod(UINT wPeriod)
{
   //printf ("timeEndPeriod(%u) !\n", wPeriod);

    if (wPeriod < 1 || wPeriod > 65535)
        return 96+1; //TIMERR_NOCANDO;
    return 0;
}

//***********************************************************************************************
/* <TODO> not in mplayer???*/
HMODULE  __stdcall expGetModuleHandleW(LPCWSTR name)
{   
		WINE_MODREF* wm;
    HMODULE result;
    if(!name)
        result=(HMODULE )1;

    else
    {
        wm=MODULE_FindModuleW(name);
        if(wm==0)
            result=0;
        else
            result=(HMODULE)(wm->module);
    }
    if(!result)
    {
        if(name && (wcscmp(name, L"kernel32")==0 || !wcscmp(name, L"kernel32.dll")))
            result=MODULE_HANDLE_kernel32;

        if(name && wcscmp(name, L"user32")==0)
            result=MODULE_HANDLE_user32;

    }
    mp_msg(0,0,"GetModuleHandleA('%s') => 0x%x\n", name, result);
    return result;
}
//***********************************************************************************************
 HMODULE  __stdcall  expGetModuleHandleA(const char* name)
{
    WINE_MODREF* wm;
    HMODULE result;
    if(!name)
        result=(HMODULE )1;

    else
    {
        wm=MODULE_FindModule(name);
        if(wm==0)
            result=0;
        else
            result=(HMODULE)(wm->module);
    }
    if(!result)
    {
			if(name && (strcmp(name, "kernel32")==0 || !strcmp(name, "kernel32.dll") ||
						      strcmp(name, "KERNEL32")==0 || !strcmp(name, "KERNEL32.DLL") 
								 ) )
            result=MODULE_HANDLE_kernel32;

        if(name && (strcmp(name, "user32")==0 || strcmp(name, "USER32")==0) )
            result=MODULE_HANDLE_user32;

    }
    mp_msg(0,0,"GetModuleHandleA('%s') => 0x%x\n", name, result);
    return result;
}

//***********************************************************************************************
 int  __stdcall  expStringFromGUID2(GUID* guid, char* str, int cbMax)
{
    int result=_snprintf(str, cbMax, "%.8x-%.4x-%.4x-%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x",
                        guid->f1, guid->f2, guid->f3,
                        (unsigned char)guid->f4[0], (unsigned char)guid->f4[1],
                        (unsigned char)guid->f4[2], (unsigned char)guid->f4[3],
                        (unsigned char)guid->f4[4], (unsigned char)guid->f4[5],
                        (unsigned char)guid->f4[6], (unsigned char)guid->f4[7]);
    //mp_msg(0,0,"StringFromGUID2(0x%x, 0x%x='%s', %d) => %d\n", guid, str, str, cbMax, result);
    return result;
}

//***********************************************************************************************
static HRESULT  __stdcall  expMoCopyMediaType(MY_MEDIA_TYPE* dest, const MY_MEDIA_TYPE* src)
{
    if (!dest || !src)
        return E_POINTER;
    memcpy(dest, src, sizeof(MY_MEDIA_TYPE));
    if (dest->cbFormat)
    {
        dest->pbFormat = (char*) malloc(dest->cbFormat);
        if (!dest->pbFormat)
            return E_OUTOFMEMORY;
        memcpy(dest->pbFormat, src->pbFormat, dest->cbFormat);
    }
    return S_OK;
}
//***********************************************************************************************
static HRESULT  __stdcall  expMoInitMediaType(MY_MEDIA_TYPE* dest, DWORD cbFormat)
{
    if (!dest)
        return E_POINTER;
    memset(dest, 0, sizeof(MY_MEDIA_TYPE));
    if (cbFormat)
    {
        dest->pbFormat = (char*) malloc(cbFormat);
        if (!dest->pbFormat)
            return E_OUTOFMEMORY;
    }
    return S_OK;
}
//***********************************************************************************************
static HRESULT  __stdcall  expMoCreateMediaType(MY_MEDIA_TYPE** dest, DWORD cbFormat)
{
    if (!dest)
        return E_POINTER;
    *dest = (MY_MEDIA_TYPE*)malloc(sizeof(MY_MEDIA_TYPE));
    return  expMoInitMediaType(*dest, cbFormat);
}
//***********************************************************************************************
static HRESULT  __stdcall  expMoDuplicateMediaType(MY_MEDIA_TYPE** dest, const void* src)
{
    if (!dest)
        return E_POINTER;
    *dest = (MY_MEDIA_TYPE*)malloc(sizeof(MY_MEDIA_TYPE));
    return  expMoCopyMediaType(*dest, (MY_MEDIA_TYPE*)src);
}
//***********************************************************************************************
static HRESULT  __stdcall  expMoFreeMediaType(MY_MEDIA_TYPE* dest)
{
    if (!dest)
        return E_POINTER;
    if (dest->pbFormat)
    {
        free(dest->pbFormat);
        dest->pbFormat = 0;
        dest->cbFormat = 0;
    }
    return S_OK;
}
//***********************************************************************************************
static HRESULT  __stdcall  expMoDeleteMediaType(MY_MEDIA_TYPE* dest)
{
    if (!dest)
        return E_POINTER;
     expMoFreeMediaType(dest);
    free(dest);
    return S_OK;
}


//***********************************************************************************************
static int __stdcall expMonitorFromWindow(HWND win, int flags)
{
    mp_msg(0,0,"MonitorFromWindow(0x%x, 0x%x) => 0\n", win, flags);
    return 0;
}

//***********************************************************************************************
static int __stdcall expMonitorFromRect(RECT *r, int flags)
{
    mp_msg(0,0,"MonitorFromRect(0x%x, 0x%x) => 0\n", r, flags);
    return 0;
}

//***********************************************************************************************
static int __stdcall expMonitorFromPoint(void *p, int flags)
{
    mp_msg(0,0,"MonitorFromPoint(0x%x, 0x%x) => 0\n", p, flags);
    return 0;
}

typedef BOOL (CALLBACK* MONITORENUMPROC)(HANDLE, HANDLE, RECT*, void*);
//***********************************************************************************************
static int __stdcall expEnumDisplayMonitors(void *dc, RECT *r, MONITORENUMPROC callback_proc, void* callback_param)
{
    mp_msg(0,0,"EnumDisplayMonitors(0x%x, 0x%x, 0x%x, 0x%x) => ?\n",dc, r, callback_proc, callback_param);
    return callback_proc(0, dc, r, callback_param);
}

//***********************************************************************************************
static int __stdcall expGetMonitorInfoA(void *mon, LPMONITORINFO lpmi)
{
    //mp_msg(0,0,"GetMonitorInfoA(0x%x, 0x%x) => 1\n", mon, lpmi);

    lpmi->rcMonitor.right = lpmi->rcWork.right = PSEUDO_SCREEN_WIDTH;
    lpmi->rcMonitor.left = lpmi->rcWork.left = 0;
    lpmi->rcMonitor.bottom = lpmi->rcWork.bottom = PSEUDO_SCREEN_HEIGHT;
    lpmi->rcMonitor.top = lpmi->rcWork.top = 0;

    lpmi->dwFlags = 1; /* primary monitor */

    if (lpmi->cbSize == sizeof(MONITORINFOEX))
    {
        LPMONITORINFOEX lpmiex = (LPMONITORINFOEX)lpmi;
        //mp_msg(0,0,"MONITORINFOEX!\n");
        strncpy(lpmiex->szDevice, "Monitor1", 8);
    }

    return 1;
}

//***********************************************************************************************
static int __stdcall expEnumDisplayDevicesA(const char *device, int devnum,void *dispdev, int flags)
{
    mp_msg(0,0,"EnumDisplayDevicesA(0x%x = %s, %d, 0x%x, %x) => 1\n",device, device, devnum, dispdev, flags);
    return 1;
}
//***********************************************************************************************
static int __stdcall expGetClientRect(HWND win, RECT *r)
{
    //mp_msg(0,0,"GetClientRect(0x%x, 0x%x) => 1\n", win, r);
    r->right = PSEUDO_SCREEN_WIDTH;
    r->left = 0;
    r->bottom = PSEUDO_SCREEN_HEIGHT;
    r->top = 0;
    return 1;
}
//***********************************************************************************************
static int __stdcall expIsWindowVisible(HWND win)
{
    //mp_msg(0,0,"IsWindowVisible(0x%x) => 1\n", win);
    return 1;
}

//***********************************************************************************************
static int __stdcall expClientToScreen(HWND win, POINT *p)
{
    //mp_msg(0,0,"ClientToScreen(0x%x, 0x%x = %d,%d) => 1\n", win, p, p->x, p->y);
    p->x = 0;
    p->y = 0;
    return 1;
}
//***********************************************************************************************
static int __stdcall expMessageBeep(int type)
{
    //mp_msg(0,0,"MessageBeep(%d) => 1\n", type);
    return 1;
}


//***********************************************************************************************
static HWND __stdcall expGetActiveWindow(void)
{
    //mp_msg(0,0,"GetActiveWindow() => 0\n");
    return (HWND)0;
}

//***********************************************************************************************
static int __stdcall expGetClassNameA(HWND win, char* classname, int maxcount)
{
    strncat(classname, "QuickTime", maxcount);
    //mp_msg(0,0,"GetClassNameA(0x%x, 0x%x, %d) => %d\n",  win, classname, maxcount, strlen(classname));
    return strlen(classname);
}

//***********************************************************************************************
static int __stdcall expGetClassInfoA(HINSTANCE inst, LPCSTR classname, LPWNDCLASS wndclass)
{
    //mp_msg(0,0,"GetClassInfoA(0x%x, 0x%x = %s, 0x%x) => 1\n", inst,classname, classname, wndclass);
    return 1;
}
typedef BOOL (__stdcall WNDENUMPROC)(HANDLE, void*);
//***********************************************************************************************
static int __stdcall expEnumWindows(WNDENUMPROC callback_func, void *callback_param)
{
		int i, i2;
    mp_msg(0,0,"EnumWindows(0x%x, 0x%x) => 1\n", callback_func, callback_param);
    i = callback_func((HANDLE)0, callback_param);
    i2 = callback_func((HANDLE)1, callback_param);
    return i && i2;
}

//***********************************************************************************************
static int __stdcall expGetWindowThreadProcessId(HWND win, int *pid_data)
{
		int tid=123;
    mp_msg(0,0,"GetWindowThreadProcessId() ");
    if (pid_data)
        (int)*pid_data = tid;
    return tid;

}
//***********************************************************************************************
static HWND __stdcall expCreateWindowExA(int exstyle, const char *classname,const char *winname, int style, int x, int y, int w, int h,HWND parent, HANDLE menu, HINSTANCE inst, LPVOID param)
{
    mp_msg(0,0,"CreateWindowEx() called\n");
    return (HWND)1;
}


#define IDOK                1
#define IDIGNORE            5
#define MB_ICONHAND		0x00000010
//***********************************************************************************************
static int __stdcall expMessageBoxA(HWND hWnd, LPCSTR text, LPCSTR title, UINT type)
{
    mp_msg(0,0,"MSGBOX '%s' '%s' (%d)\n",text,title,type);
    if (type == MB_ICONHAND && !strlen(text) && !strlen(title))
        return IDIGNORE;
    return IDOK;
}
//***********************************************************************************************
static int __stdcall expDialogBoxParamA(void *inst, const char *name, HWND parent, void *dialog_func, void *init_param)
{
    //mp_msg(0,0,"DialogBoxParamA(0x%x, 0x%x = %s, 0x%x, 0x%x, 0x%x) => 0x42424242\n",  inst, name, name, parent, dialog_func, init_param);
    return 0x42424242;
}


//***********************************************************************************************
int slwait (int *sl) 
{
    return 0;
}
//***********************************************************************************************
int slrelease (int *sl) 
{
    return 0;
}



//***********************************************************************************************
long getregionsize (void) 
{
    return 65536;
}

//***********************************************************************************************
void *mmap (void *ptr, long size, long prot, long type, long handle, long arg) 
{
    static long g_pagesize;
    static long g_regionsize;
    /* Wait for spin lock */
    slwait (&g_sl);
    /* First time initialization */
    if (! g_pagesize) 
        g_pagesize = getpagesize ();
    if (! g_regionsize) 
        g_regionsize = getregionsize ();
    /* Allocate this */
			ptr = VirtualAlloc (ptr, size,MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (! ptr) 
		{
				mp_msg(0,0,"VirtualAlloc() failed\n");
        ptr = (LPVOID)-1;
				errno=ENOEXEC;
    }
		else if (handle)
		{
			int fd=(int)handle;
			long lpos=_tell(fd);
			long len=lseek(fd, 0, SEEK_END);
			
      lseek(fd, arg, SEEK_SET);
			read(fd,ptr,size);
			lseek(fd, lpos, SEEK_SET);
		}
    /* Release spin lock */
    slrelease (&g_sl);
    return ptr;
}

//***********************************************************************************************
long munmap (void *ptr, long size) 
{
    static long g_pagesize;
    static long g_regionsize;
    int rc = 0;//MUNMAP_FAILURE;
    /* Wait for spin lock */
    slwait (&g_sl);
    /* First time initialization */
    if (! g_pagesize) 
        g_pagesize = getpagesize ();
    if (! g_regionsize) 
        g_regionsize = getregionsize ();
    /* Free this */
    if (! VirtualFree (ptr, 0, 
                       MEM_RELEASE))
        goto munmap_exit;
    rc = 0;
munmap_exit:
    /* Release spin lock */
    slrelease (&g_sl);
    return rc;
}


/***********************************************************************
 *           FILE_munmap
 */
int FILE_munmap( LPVOID start, DWORD size_high, DWORD size_low )
{
    //if (size_high)
    //    mp_msg(0,0,"offsets larger than 4Gb not supported\n");
    return munmap( start, size_low );
}

LPVOID FILE_dommap( int unix_handle, LPVOID start,
                    DWORD size_high, DWORD size_low,
                    DWORD offset_high, DWORD offset_low,
                    int prot, int flags )
{
    int fd = -1;
    int pos;
    LPVOID ret;

//    if (size_high || offset_high)
  //     printf ("offsets larger than 4Gb not supported\n");

    if (unix_handle == -1)
    {
			mp_msg(0,0,"huh?\n");
    }
    else
        fd = unix_handle;
    if ((ret = mmap( start, size_low, prot,MAP_PRIVATE | MAP_FIXED, fd, offset_low )) != (LPVOID)-1)
    {
        return ret;
    }

    if (unix_handle == -1)
        return ret;
    if ((errno != ENOEXEC) && (errno != EINVAL))
        return ret;
    if (prot & PROT_WRITE)
    {
        /* We cannot fake shared write mappings */
#ifdef MAP_SHARED
        if (flags & MAP_SHARED)
            return ret;
#endif
#ifdef MAP_PRIVATE
        if (!(flags & MAP_PRIVATE))
            return ret;
#endif
    }
    /*    ( "FILE_mmap: mmap failed (%d), faking it\n", errno );*/
    /* Reserve the memory with an anonymous mmap */
    ret = FILE_dommap( -1, start, size_high, size_low, 0, 0,PROT_READ | PROT_WRITE, flags );
    if (ret == (LPVOID)-1)
        return ret;
    /* Now read in the file */
    if ((pos = lseek( fd, offset_low, SEEK_SET )) == -1)
    {
        FILE_munmap( ret, size_high, size_low );
        return (LPVOID)-1;
    }
    read( fd, ret, size_low );
    lseek( fd, pos, SEEK_SET );  /* Restore the file pointer */
    return ret;
}

//****************************************************************************************
HANDLE  __stdcall CreateFileMappingA(HANDLE handle, LPSECURITY_ATTRIBUTES lpAttr,
                                 DWORD flProtect,
                                 DWORD dwMaxHigh, DWORD dwMaxLow,
                                 LPCSTR name)
{
    int hFile = (int)handle;
    unsigned int len;
    LPVOID answer;
    int anon=0;
    int mmap_access=0;
    if(hFile<0)
    {
        anon=1;
    }
    if(!anon)
    {
        len=lseek(hFile, 0, SEEK_END);
        lseek(hFile, 0, SEEK_SET);
    }
    else
        len=dwMaxLow;

    if(flProtect & PAGE_READONLY)
        mmap_access |=PROT_READ;
    else
        mmap_access |=PROT_READ|PROT_WRITE;

    answer=mmap(NULL, len, mmap_access, MAP_PRIVATE, hFile, 0);
    if(anon)
        close(hFile);
    if(answer!=(LPVOID)-1)
    {
        if(fm==0)
        {
            fm = (file_mapping*) malloc(sizeof(file_mapping));
            fm->prev=NULL;
        }
        else
        {
            fm->next = (file_mapping*) malloc(sizeof(file_mapping));
            fm->next->prev=fm;
            fm=fm->next;
        }
        fm->next=NULL;
        fm->handle=answer;
        if(name)
        {
            fm->name = (char*) malloc(strlen(name)+1);
            strcpy(fm->name, name);
        }
        else
            fm->name=NULL;
        fm->mapping_size=len;

        if(anon)
            close(hFile);
        return (HANDLE)answer;
    }
    return (HANDLE)0;
}

//***********************************************************************************************
WIN_BOOL __stdcall UnmapViewOfFile(LPVOID handle)
{
    file_mapping* p;
    int result;
    if(fm==0)
		{
        return 0;
		}
    for(p=fm; p; p=p->next)
    {
        if(p->handle==handle)
        {
            result=munmap((void*)handle, p->mapping_size);
            if(p->next)
                p->next->prev=p->prev;
            if(p->prev)
                p->prev->next=p->next;
            if(p->name)
                free(p->name);
            if(p==fm)
                fm=p->prev;
            free(p);
            return result;
        }
    }
    return 0;
}


//***********************************************************************************************
LPVOID  __stdcall LinVirtualAlloc(LPVOID address, DWORD size, DWORD type,  DWORD protection)
{
    void* answer;
    int fd=0;
    size=(size+0xffff)&(~0xffff);
    if(address!=0)
    {
        //check whether we can allow to allocate this
        virt_alloc* str=vm;
        while(str)
        {
            if((unsigned)address>=(unsigned)str->address+str->mapping_size)
            {
                str=str->prev;
                continue;
            }
            if((unsigned)address+size<(unsigned)str->address)
            {
                str=str->prev;
                continue;
            }
            if(str->state==0)
            {
                if(((unsigned)address+size<(unsigned)str->address+str->mapping_size) && (type & MEM_COMMIT))
                {
                    //close(fd);
                    return address; //returning previously reserved memory
                }
                return NULL;
            }
            //close(fd);
            return NULL;
        }
        answer=mmap(address, size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_FIXED | MAP_PRIVATE, fd, 0);
    }
    else
        answer=mmap(address, size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE, fd, 0);
    //    answer=FILE_dommap(-1, address, 0, size, 0, 0,
    //	PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE);
    //close(fd);
    if(answer==(void*)-1)
    {
        mp_msg(0,0,"Error no %d\n", errno);
        mp_msg(0,0,"VirtualAlloc(0x%p, %ld) failed\n", address, size);
        return NULL;
    }
    else
    {
        virt_alloc *new_vm = (virt_alloc*) malloc(sizeof(virt_alloc));
        new_vm->mapping_size=size;
        new_vm->address=(char*)answer;
        new_vm->prev=vm;
        if(type == MEM_RESERVE)
            new_vm->state=0;
        else
            new_vm->state=1;
        if(vm)
            vm->next=new_vm;
        vm=new_vm;
        vm->next=0;
        //if(va_size!=0)
        //   printf ("Multiple VirtualAlloc!\n");
        mp_msg(0,0,"answer=0x%08x\n", answer);
        return answer;
    }
}
//***********************************************************************************************
WIN_BOOL  __stdcall LinVirtualFree(LPVOID  address, SIZE_T dwSize, DWORD dwFreeType)//not sure
{
    virt_alloc* str=vm;
    int answer;
    while(str)
    {
        if(address!=str->address)
        {
            str=str->prev;
            continue;
        }
        mp_msg(0,0,"VirtualFree(0x%08X, %d - %d)\n", str->address, dwSize, str->mapping_size);
        answer=munmap(str->address, str->mapping_size);
        if(str->next)
            str->next->prev=str->prev;
        if(str->prev)
            str->prev->next=str->next;
        if(vm==str)
            vm=str->prev;
        free(str);
        return 0;
    }
    return -1;
}



//***********************************************************************************************
static int  __stdcall ext_unknown()
{
   mp_msg(0,0,"Unknown func called\n");
    return 0;
}



//***********************************************************************************************
static int  __stdcall  expLoadLibraryA(char* name)
{
    int result = 0;
    char* lastbc;
//    int i;
    if (!name)
        return -1;
    // we skip to the last backslash
    // this is effectively eliminating weird characters in
    // the text output windows

    lastbc = strrchr(name, '\\');
    if (lastbc)
    {
        int i;
        lastbc++;
        for (i = 0; 1 ;i++)
        {
            name[i] = *lastbc++;
            if (!name[i])
                break;
        }
    }
    if(strncmp(name, "c:\\windows\\", 11)==0)
        name += 11;
    if(strncmp(name, ".\\", 2)==0)
        name += 2;

    mp_msg(0,0,"Entering expLoadLibraryA(%s)\n", name);

    // PIMJ and VIVO audio are loading  kernel32.dll
    if (strcmp(name, "kernel32.dll") == 0 || strcmp(name, "kernel32") == 0 ||
			  strcmp(name, "KERNEL32.DLL") == 0 || strcmp(name, "KERNEL32") == 0 
			 )
        return (int)MODULE_HANDLE_kernel32;
    //	return ERROR_SUCCESS; /* yeah, we have also the kernel32 calls */
    /*__stdcall  exported -> do not return failed! */

    if (strcmp(name, "user32.dll") == 0 || strcmp(name, "user32") == 0 ||
			  strcmp(name, "USER32.DLL") == 0 || strcmp(name, "USER32") == 0)
        //	return MODULE_HANDLE_kernel32;
        return (int)MODULE_HANDLE_user32;

    if (strcmp(name, "wininet.dll") == 0 || strcmp(name, "wininet") == 0 ||
			  strcmp(name, "WININET.DLL") == 0 || strcmp(name, "WININET") == 0)
        return (int)MODULE_HANDLE_wininet;

    if (strcmp(name, "ddraw.dll") == 0 || strcmp(name, "ddraw") == 0 ||
			  strcmp(name, "DDRAW.DLL") == 0 || strcmp(name, "DDRAW") == 0)
        return (int)MODULE_HANDLE_ddraw;

    if (strcmp(name, "advapi32.dll") == 0 || strcmp(name, "advapi32") == 0 ||
			  strcmp(name, "ADVAPI32.DLL") == 0 || strcmp(name, "ADVAPI32") == 0)
        return (int)MODULE_HANDLE_advapi32;

    result=(int)LoadLibraryA(name);
    
    return result;
}

//***********************************************************************************************
static int  __stdcall  expFreeLibrary(int module)
{
    mp_msg(0,0,"FreeLibrary()\n");
		int result=FreeLibrary((void*)module);
    return result;
}

//***********************************************************************************************
static void*  __stdcall  expGetProcAddress(HMODULE mod, char* name)
{
    void* result;
    switch((int)mod)
    {
    case MODULE_HANDLE_kernel32:
        result=LookupExternalByName("kernel32.dll", name);
        break;
    case MODULE_HANDLE_user32:
        result=LookupExternalByName("user32.dll", name);
        break;
    case MODULE_HANDLE_wininet:
        result=LookupExternalByName("wininet.dll", name);
        break;
    case MODULE_HANDLE_ddraw:
        result=LookupExternalByName("ddraw.dll", name);
        break;
    case MODULE_HANDLE_advapi32:
        result=LookupExternalByName("advapi32.dll", name);
        break;
    default:
        result=GetProcAddress(mod, name);
    }
    mp_msg(0,0,"GetProcAddress(0x%x, '%s') => 0x%x\n", mod, name, result);
    return result;
}

//**********************************************************************************************
//***** WRAPPER DEFS****
#define FF(X,Y) {#X, Y, (void*)exp##X},
#define FF(X,Y) {#X, Y, (void*)exp##X},

struct exports exp_kernel32[]=
    {
        FF(IsBadWritePtr, 357)
        FF(IsBadReadPtr, 354)
				FF(RtlUnwind,-1)
        FF(IsBadStringPtrW, -1)
        FF(IsBadStringPtrA, -1)
        FF(DisableThreadLibraryCalls, -1)
        FF(CreateThread, -1)
        FF(CreateEventA, -1)
        FF(SetEvent, -1)
        FF(ResetEvent, -1)
        FF(WaitForSingleObject, -1)
        FF(WaitForMultipleObjects, -1)
        FF(ExitThread, -1)
        FF(CreateMutexA,-1)
        FF(ReleaseMutex,-1)
        FF(GetSystemInfo, -1)
        FF(GetVersion, 332)
        FF(HeapCreate, 461)
        FF(HeapAlloc, -1)
        FF(HeapDestroy, -1)
        FF(HeapFree, -1)
        FF(HeapSize, -1)
        FF(HeapReAlloc,-1)
        FF(GetProcessHeap, -1)
        FF(VirtualAlloc, -1)
        FF(VirtualFree, -1)
        FF(InitializeCriticalSection, -1)
        FF(EnterCriticalSection, -1)
        FF(LeaveCriticalSection, -1)
        FF(DeleteCriticalSection, -1)
        FF(TlsAlloc, -1)
        FF(TlsFree, -1)
        FF(TlsGetValue, -1)
        FF(TlsSetValue, -1)
        FF(GetCurrentThreadId, -1)
        FF(GetCurrentProcess, -1)
        FF(LocalAlloc, -1)
        FF(LocalReAlloc,-1)
        FF(LocalLock, -1)
        FF(GlobalAlloc, -1)
        FF(GlobalReAlloc, -1)
        FF(GlobalLock, -1)
        FF(GlobalSize, -1)
        FF(MultiByteToWideChar, 427)
        FF(WideCharToMultiByte, -1)
        FF(GetVersionExA, -1)
        FF(CreateSemaphoreA, -1)
        FF(QueryPerformanceCounter, -1)
        FF(QueryPerformanceFrequency, -1)
        FF(LocalHandle, -1)
        FF(LocalUnlock, -1)
        FF(LocalFree, -1)
        FF(GlobalHandle, -1)
        FF(GlobalUnlock, -1)
        FF(GlobalFree, -1)
        FF(LoadResource, -1)
        FF(ReleaseSemaphore, -1)
        FF(FindResourceA, -1)
        FF(LockResource, -1)
        FF(FreeResource, -1)
        FF(SizeofResource, -1)
        FF(CloseHandle, -1)
        FF(GetCommandLineA, -1)
        FF(GetCommandLineW, -1)
        FF(GetEnvironmentStringsW, -1)
        FF(FreeEnvironmentStringsW, -1)
        FF(FreeEnvironmentStringsA, -1)
        FF(GetEnvironmentStrings, -1)
        FF(GetStartupInfoA, -1)
        FF(GetStdHandle, -1)
        FF(GetFileType, -1)
        FF(GetFileAttributesA, -1)
        FF(SetHandleCount, -1)
        FF(GetACP, -1)
        FF(GetModuleFileNameA, -1)
        FF(SetUnhandledExceptionFilter, -1)
        FF(LoadLibraryA, -1)
        FF(GetProcAddress, -1)
        FF(FreeLibrary, -1)
        FF(CreateFileMappingA, -1)
        FF(OpenFileMappingA, -1)
        FF(MapViewOfFile, -1)
        FF(UnmapViewOfFile, -1)
        FF(Sleep, -1)
        FF(GetModuleHandleA, -1)
        FF(GetProfileIntA, -1)
        FF(GetPrivateProfileIntA, -1)
        FF(GetPrivateProfileStringA, -1)
        FF(WritePrivateProfileStringA, -1)
        FF(GetLastError, -1)
        FF(SetLastError, -1)
        FF(InterlockedIncrement, -1)
        FF(InterlockedDecrement, -1)
        FF(GetTimeZoneInformation, -1)
        FF(OutputDebugStringA, -1)
        FF(GetLocalTime, -1)
        FF(GetSystemTime, -1)
        FF(GetSystemTimeAsFileTime, -1)
        FF(GetEnvironmentVariableA, -1)
        FF(SetEnvironmentVariableA, -1)
        FF(RtlZeroMemory,-1)
        FF(RtlMoveMemory,-1)
        FF(RtlFillMemory,-1)
        FF(GetTempPathA,-1)
        FF(FindFirstFileA,-1)
        FF(FindNextFileA,-1)
        FF(FindClose,-1)
        FF(FileTimeToLocalFileTime,-1)
        FF(DeleteFileA,-1)
        FF(ReadFile,-1)
        FF(WriteFile,-1)
        FF(SetFilePointer,-1)
        FF(GetTempFileNameA,-1)
        FF(CreateFileA,-1)
        FF(GetSystemDirectoryA,-1)
        FF(GetWindowsDirectoryA,-1)
        FF(GetCurrentDirectoryA,-1)
        FF(SetCurrentDirectoryA,-1)
        FF(CreateDirectoryA,-1)
        FF(GetShortPathNameA,-1)
        FF(GetFullPathNameA,-1)
        FF(SetErrorMode, -1)
        FF(IsProcessorFeaturePresent, -1)
        FF(GetProcessAffinityMask, -1)
        FF(InterlockedExchange, -1)
        FF(InterlockedCompareExchange, -1)
        FF(MulDiv, -1)
        FF(lstrcmpiA, -1)
        FF(lstrlenA, -1)
        FF(lstrcpyA, -1)
        FF(lstrcatA, -1)
        FF(lstrcpynA,-1)
        FF(GetProcessVersion,-1)
        FF(GetCurrentThread,-1)
        FF(GetOEMCP,-1)
        FF(GetCPInfo,-1)
        FF(DuplicateHandle,-1)
        FF(GetTickCount, -1)
        FF(SetThreadAffinityMask,-1)
        FF(GetCurrentProcessId,-1)
        FF(GlobalMemoryStatus,-1)
        FF(SetThreadPriority,-1)
        FF(ExitProcess,-1)
        {"LoadLibraryExA", -1, (void*)&LoadLibraryExA
        },
        FF(SetThreadIdealProcessor,-1)
    };

long double __cdecl expceil(long double _X)
    {return (ceil(_X)); }

struct exports exp_msvcrt[]=
    {
        FF(malloc, -1)
        FF(_initterm, -1)
        FF(__dllonexit, -1)
        FF(free, -1)
        {"??3@YAXPAX@Z", -1, expdelete
        },
        {"??2@YAPAXI@Z", -1, expnew},
        {"_adjust_fdiv", -1, (void*)&_adjust_fdiv},
        FF(strrchr, -1)
        FF(strchr, -1)
        FF(strlen, -1)
        FF(strcpy, -1)
        FF(wcscpy, -1)
        FF(strcmp, -1)
        FF(strncmp, -1)
        FF(strcat, -1)
        FF(_stricmp,-1)
        FF(_strdup,-1)
        FF(_setjmp3,-1)
        FF(isalnum, -1)
        FF(isspace, -1)
        FF(isalpha, -1)
        FF(isdigit, -1)
        FF(memmove, -1)
        FF(memcmp, -1)
        FF(memset, -1)
        FF(memcpy, -1)
        FF(time, -1)
        FF(rand, -1)
        FF(srand, -1)
        FF(log10, -1)
        FF(pow, -1)
        FF(cos, -1)
        FF(_ftol,-1)
        FF(_CIpow,-1)
        FF(ldexp,-1)
        FF(frexp,-1)
        FF(sprintf,-1)
        FF(sscanf,-1)
        FF(fopen,-1)
        FF(fprintf,-1)
        FF(printf,-1)
        FF(getenv,-1)
        FF(_EH_prolog,-1)
        FF(calloc,-1)
        {"ceil",-1,&expceil},
        /* needed by imagepower mjpeg2k */
        {"clock",-1,(void*)&clock},
        {"memchr",-1,(void*)&memchr},
        {"vfprintf",-1,(void*)&vfprintf},
        //    {"realloc",-1,(void*)&realloc},
        FF(realloc,-1)
        {"puts",-1,(void*)&puts
        }
    };
struct exports exp_winmm[]=
    {
        FF(GetDriverModuleHandle, -1)
        FF(timeGetTime, -1)
        FF(DefDriverProc, -1)
        FF(OpenDriverA, -1)
        FF(OpenDriver, -1)
        FF(timeGetDevCaps, -1)
        FF(timeBeginPeriod, -1)
        FF(timeEndPeriod, -1)
        FF(waveOutGetNumDevs, -1)
    };
struct exports exp_user32[]=
    {
        FF(LoadStringA, -1)
        FF(wsprintfA, -1)
        FF(GetDC, -1)
        FF(GetDesktopWindow, -1)
        FF(ReleaseDC, -1)
        FF(IsRectEmpty, -1)
        FF(LoadCursorA,-1)
        FF(SetCursor,-1)
        FF(GetCursorPos,-1)
        FF(ShowCursor,-1)
        FF(RegisterWindowMessageA,-1)
        FF(GetSystemMetrics,-1)
        FF(GetSysColor,-1)
        FF(GetSysColorBrush,-1)
        FF(GetWindowDC, -1)
        FF(DrawTextA, -1)
        FF(MessageBoxA, -1)
        FF(RegisterClassA, -1)
        FF(UnregisterClassA, -1)
        FF(GetWindowRect, -1)
        FF(MonitorFromWindow, -1)
        FF(MonitorFromRect, -1)
        FF(MonitorFromPoint, -1)
        FF(EnumDisplayMonitors, -1)
        FF(GetMonitorInfoA, -1)
        FF(EnumDisplayDevicesA, -1)
        FF(GetClientRect, -1)
        FF(ClientToScreen, -1)
        FF(IsWindowVisible, -1)
        FF(GetActiveWindow, -1)
        FF(GetClassNameA, -1)
        FF(GetClassInfoA, -1)
        FF(GetWindowLongA, -1)
        FF(EnumWindows, -1)
        FF(GetWindowThreadProcessId, -1)
        FF(CreateWindowExA, -1)
        FF(MessageBeep, -1)
        FF(DialogBoxParamA, -1)
				FF(CharNextA,-1)
    };
struct exports exp_advapi32[]=
    {
        FF(RegCloseKey, -1)
        FF(RegCreateKeyExA, -1)
        FF(RegEnumKeyExA, -1)
        FF(RegEnumValueA, -1)
        FF(RegOpenKeyA, -1)
        FF(RegOpenKeyExA, -1)
        FF(RegQueryValueExA, -1)
        FF(RegSetValueExA, -1)
    };
struct exports exp_gdi32[]=
    {
        FF(CreateCompatibleDC, -1)
        FF(CreateFontA, -1)
        FF(DeleteDC, -1)
        FF(DeleteObject, -1)
        FF(GetDeviceCaps, -1)
        FF(GetSystemPaletteEntries, -1)
        FF(CreatePalette, -1)
        FF(GetObjectA, -1)
        FF(CreateRectRgn, -1)
    };
struct exports exp_version[]=
    {
        FF(GetFileVersionInfoSizeA, -1)
    };
struct exports exp_ole32[]=
    {
        FF(CoCreateFreeThreadedMarshaler,-1)
        FF(CoCreateInstance, -1)
        FF(CoInitialize, -1)
        FF(CoTaskMemAlloc, -1)
        FF(CoTaskMemFree, -1)
        FF(StringFromGUID2, -1)
    };
// do we really need crtdll ???
// msvcrt is the correct place probably...
struct exports exp_crtdll[]=
    {
        FF(memcpy, -1)
        FF(wcscpy, -1)
    };
struct exports exp_comctl32[]=
    {
        FF(StringFromGUID2, -1)
        FF(InitCommonControls, 17)
        FF(CreateUpDownControl, 16)
    };
struct exports exp_wsock32[]=
    {
        FF(htonl,8)
        FF(ntohl,14)
    };
struct exports exp_msdmo[]=
    {
        FF(memcpy, -1) // just test
        FF(MoCopyMediaType, -1)
        FF(MoCreateMediaType, -1)
        FF(MoDeleteMediaType, -1)
        FF(MoDuplicateMediaType, -1)
        FF(MoFreeMediaType, -1)
        FF(MoInitMediaType, -1)
    };
struct exports exp_oleaut32[]=
    {
        FF(VariantInit, 8)
        FF(SysStringByteLen, 149)
    };

/*  realplayer8:
	DLL Name: PNCRT.dll
	vma:  Hint/Ord Member-Name
	22ff4	  615  free
	2302e	  250  _ftol
	22fea	  666  malloc
	2303e	  609  fprintf
	2305e	  167  _adjust_fdiv
	23052	  280  _initterm
 
	22ffc	  176  _beginthreadex
	23036	  284  _iob
	2300e	   85  __CxxFrameHandler
	23022	  411  _purecall
*/
/*  realplayer8:
	DLL Name: PNCRT.dll
	vma:  Hint/Ord Member-Name
	22ff4	  615  free
	2302e	  250  _ftol
	22fea	  666  malloc
	2303e	  609  fprintf
	2305e	  167  _adjust_fdiv
	23052	  280  _initterm
 
	22ffc	  176  _beginthreadex
	23036	  284  _iob
	2300e	   85  __CxxFrameHandler
	23022	  411  _purecall
*/
struct exports exp_pncrt[]=
    {
        FF(malloc, -1) // just test
        FF(free, -1) // just test
        FF(fprintf, -1) // just test
        {"_adjust_fdiv", -1, (void*)&_adjust_fdiv
        },
        FF(_ftol,-1)
        FF(_initterm, -1)
        {"??3@YAXPAX@Z", -1, expdelete
        },
        {"??2@YAPAXI@Z", -1, expnew},
        FF(__dllonexit, -1)
        FF(strncpy, -1)
        FF(_CIpow,-1)
        FF(calloc,-1)
        FF(memmove, -1)
    };


struct exports exp_ddraw[]=
    {
        FF(DirectDrawCreate, -1)
    };

#define LL(X) \
{#X".dll", sizeof(exp_##X)/sizeof(struct exports), exp_##X},

struct libs libraries[]=
    {
        LL(kernel32)
        LL(msvcrt)
        LL(winmm)
        LL(user32)
        LL(advapi32)
        LL(gdi32)
        LL(version)
        LL(ole32)
        LL(oleaut32)
        LL(crtdll)
        LL(comctl32)
        LL(wsock32)
        LL(msdmo)
        LL(pncrt)
        LL(ddraw)
    };


//***********************************************************************************************
static void ext_stubs(void)
{
    // expects:
    //  ax  position index
    //  cx  address of printf function

}

//***********************************************************************************************
static void* add_stub(void)
{
		extcode[pos]=0xcccccccc;//int 3
		void* answ=&extcode[pos];
		pos++;
    return (void*)answ;
}

//***********************************************************************************************
void* LookupExternal(const char* library, int ordinal)
{
    int i,j;
    if(library==0)
    {
        ("ERROR: library=0\n");
        return (void*)ext_unknown;
    }
    //    mp_msg(0,0,"%x %x\n", &unk_exp1, &unk_exp2);

    //("External func %s:%d\n", library, ordinal);

    for(i=0; i<sizeof(libraries)/sizeof(struct libs); i++)
    {
        if(strcmp(library, libraries[i].name))
            continue;
        for(j=0; j<libraries[i].length; j++)
        {
            if(ordinal!=libraries[i].exps[j].id)
                continue;
            mp_msg(0,0,"Hit: 0x%p\n", libraries[i].exps[j].func);
            return libraries[i].exps[j].func;
        }
    }

		sprintf(export_names[pos], "%s:%d", library, ordinal);
		mp_msg(0,0,"external()%s.%d not found=%x\n",library, ordinal,&extcode[pos]);
    return add_stub();
}

//***********************************************************************************************
void* LookupExternalByName(const char* library, const char* name)
{
//    char* answ;
    int i,j;
    //   return (void*)ext_unknown;
    if(library==0)
    {
        mp_msg(0,0,"ERROR: library=0\n");
        return (void*)ext_unknown;
    }
    if(name==0)
    {
        mp_msg(0,0,"ERROR: name=0\n");
        return (void*)ext_unknown;
    }
		char szLib[128];
		strcpy(szLib,library);
		for (i=0; i < strlen(szLib);i++) szLib[i]=tolower((unsigned char)szLib[i]);
    //mp_msg(0,0,"External func %s:%s\n", szLib, name);
		
    for(i=0; i<sizeof(libraries)/sizeof(struct libs); i++)
    {
        if(strcmp(szLib, libraries[i].name))
            continue;
        for(j=0; j<libraries[i].length; j++)
        {
            if(strcmp(name, libraries[i].exps[j].name))
                continue;
            //	    ("Hit: 0x%08X\n", libraries[i].exps[j].func);
            return libraries[i].exps[j].func;
        }
    }

		mp_msg(0,0,"external()%s.%s not found=%x\n",library, name,&extcode[pos]);
		strcpy(export_names[pos], name);
		return add_stub();
}

//***********************************************************************************************
static void dump_exports( HMODULE hModule )
{
    char		*Module;
    unsigned int i, j;
    unsigned short	*ordinal;
    unsigned long	*function,*functions;
    unsigned char	**name;
    unsigned int load_addr = (unsigned int)hModule;


    IMAGE_NT_HEADERS *tmp=PE_HEADER(hModule);
    DWORD rva_start = PE_HEADER(hModule)->OptionalHeader
                      .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    DWORD rva_end = rva_start + PE_HEADER(hModule)->OptionalHeader
                    .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
    IMAGE_EXPORT_DIRECTORY *pe_exports = (IMAGE_EXPORT_DIRECTORY*)RVA(rva_start);

    Module = (char*)RVA(pe_exports->Name);
		
			//mp_msg(0,0,"*******EXPORT DATA*******\n");
			//mp_msg(0,0,"Module name is %s, %ld functions, %ld names\n",
				//		Module, pe_exports->NumberOfFunctions, pe_exports->NumberOfNames);
		

    ordinal=(unsigned short*) RVA(pe_exports->AddressOfNameOrdinals);
    functions=function=(unsigned long*) RVA(pe_exports->AddressOfFunctions);
    name=(unsigned char**) RVA(pe_exports->AddressOfNames);

    //mp_msg(0,0," Ord    RVA     Addr   Name\n" );
    for (i=0;i<pe_exports->NumberOfFunctions;i++, function++)
    {
        if (!*function)
            continue;
        if (1)
        {
      //      mp_msg(0,0, "%4ld %08lx %p", i + pe_exports->Base, *function, RVA(*function) );

            for (j = 0; j < pe_exports->NumberOfNames; j++)
                if (ordinal[j] == i)
                {
                    //mp_msg(0,0, "  %s", (char*)RVA(name[j]) );
                    break;
                }
            //if ((*function >= rva_start) && (*function <= rva_end))
              //  (" (forwarded -> %s)", (char *)RVA(*function));
            //mp_msg(0,0,"\n");
        }
    }
}

//***********************************************************************************************
FARPROC PE_FindExportedFunction( WINE_MODREF *wm,LPCSTR funcName,WIN_BOOL snoop )
{
    unsigned short				* ordinals;
    unsigned long				* function;
    unsigned char				** name;
    const char *ename = NULL;
    int				i, ordinal;
    PE_MODREF			*pem = &(wm->binfmt.pe);
    IMAGE_EXPORT_DIRECTORY 		*exports = pem->pe_export;
    unsigned int			load_addr = (unsigned int)wm->module;
    unsigned long				rva_start, rva_end, addr;
    char				* forward;

    //if (HIWORD(funcName))
      //  mp_msg(0,0,"(%s)\n",funcName);
    //else
      // printf ("(%d)\n",(int)funcName);
    if (!exports)
    {
        /* Not a fatal problem, some apps do
         * GetProcAddress(0,"RegisterPenApp") which triggers this
         * case.
         */
        mp_msg(0,0,"Module %08x(%s)/MODREF %p doesn't have a__stdcall  exports table.\n",wm->module,wm->modname,pem);
        return NULL;
    }
    ordinals= (unsigned short*)  RVA(exports->AddressOfNameOrdinals);
    function= (unsigned long*)   RVA(exports->AddressOfFunctions);
    name	= (unsigned char **) RVA(exports->AddressOfNames);
    forward = NULL;
    rva_start = PE_HEADER(wm->module)->OptionalHeader
                .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    rva_end = rva_start + PE_HEADER(wm->module)->OptionalHeader
              .DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    if (HIWORD(funcName))
    {

        int min = 0, max =  exports->NumberOfNames - 1;
        while (min <= max)
        {
            int res, pos = (min + max) / 2;
            ename = (const char*) RVA(name[pos]);
            if (!(res = strcmp( ename, funcName )))
            {
                ordinal = ordinals[pos];
                goto found;
            }
            if (res > 0)
                max = pos - 1;
            else
                min = pos + 1;
        }

        for (i = 0; i <  exports->NumberOfNames; i++)
        {
            ename = (const char*) RVA(name[i]);
            if (!strcmp( ename, funcName ))
            {
               //printf ( "%s.%s required a linear search\n", wm->modname, funcName );
                ordinal = ordinals[i];
                goto found;
            }
        }
        return NULL;
    }
    else
    {
        ordinal = LOWORD(funcName) -  exports->Base;
        if (snoop && name)
        {
            for (i = 0; i <  exports->NumberOfNames; i++)
                if (ordinals[i] == ordinal)
                {
                    ename = (const char*)RVA(name[i]);
                    break;
                }
        }
    }

found:
    if (ordinal >=  exports->NumberOfFunctions)
    {
        mp_msg(0,0,"	ordinal %ld out of range!\n", ordinal +  exports->Base );
        return NULL;
    }
    addr = function[ordinal];
    if (!addr)
        return NULL;
    if ((addr < rva_start) || (addr >= rva_end))
    {
        FARPROC proc = (FARPROC )RVA(addr);
        if (snoop)
        {
            if (!ename)
                ename = "@";
            //                proc = SNOOP_GetProcAddress(wm->module,ename,ordinal,proc);
            mp_msg(0,0,"SNOOP_GetProcAddress n/a\n");

        }
        return proc;
    }
    else
    {
        WINE_MODREF *wm;
        char *forward = (char*)RVA(addr);
        char module[256];
        char *end = strchr(forward, '.');

        if (!end)
            return NULL;
        if (end - forward >= sizeof(module))
            return NULL;
        memcpy( module, forward, end - forward );
        module[end-forward] = 0;
        if (!(wm = MODULE_FindModule( module )))
        {
           mp_msg(0,0,"module not found for forward '%s'\n", forward );
            return NULL;
        }
        return MODULE_GetProcAddress( wm->module, end + 1, snoop );
    }
}

//***********************************************************************************************
static DWORD fixup_imports( WINE_MODREF *wm )
{
    IMAGE_IMPORT_DESCRIPTOR	*pe_imp;
    PE_MODREF			*pem;
    unsigned int load_addr	= (unsigned int )wm->module;
    int				i,characteristics_detection=1;
    char			*modname;

//    assert(wm->type==MODULE32_PE);
    pem = &(wm->binfmt.pe);
    if (pem->pe_export)
        modname = (char*) RVA(pem->pe_export->Name);
    else
        modname = "<unknown>";


    mp_msg(0,0,"Dumping imports list\n");


    pe_imp = pem->pe_import;
    if (!pe_imp)
        return 0;

    /* We assume that we have at least one import with !0 characteristics and
     * detect broken imports with all characteristsics 0 (notably Borland) and
     * switch the detection off for them.
     */
    for (i = 0; pe_imp->Name ; pe_imp++)
    {
        if (!i && !pe_imp->u.Characteristics)
            characteristics_detection = 0;
        if (characteristics_detection && !pe_imp->u.Characteristics)
            break;
        i++;
    }
    if (!i)
        return 0;


    wm->nDeps = i;
		wm->deps  = (struct _wine_modref **)expHeapAlloc( GetProcessHeap(), 0, i*sizeof(WINE_MODREF *) );
    /* load the imported modules. They are automatically
     * added to the modref list of the process.
     */

    for (i = 0, pe_imp = pem->pe_import; pe_imp->Name ; pe_imp++)
    {
//        WINE_MODREF		*wmImp;
        IMAGE_IMPORT_BY_NAME	*pe_name;
        PIMAGE_THUNK_DATA	import_list,thunk_list;
        char			*name = (char *) RVA(pe_imp->Name);

        if (characteristics_detection && !pe_imp->u.Characteristics)
            break;

        //#warning : here we should fill imports
        mp_msg(0,0,"Loading imports for %s.dll\n", name);


        if (pe_imp->u.OriginalFirstThunk != 0)
        {
            mp_msg(0,0,"Microsoft style imports used\n");
            import_list =(PIMAGE_THUNK_DATA) RVA(pe_imp->u.OriginalFirstThunk);
            thunk_list = (PIMAGE_THUNK_DATA) RVA(pe_imp->FirstThunk);

            while (import_list->u1.Ordinal)
            {
                if (IMAGE_SNAP_BY_ORDINAL(import_list->u1.Ordinal))
                {
                    int ordinal = IMAGE_ORDINAL(import_list->u1.Ordinal);

                    //		    mp_msg(0,0,"--- Ordinal %s,%d\n", name, ordinal);

                    thunk_list->u1.Function=(unsigned long*)LookupExternal(name, ordinal);
										if (thunk_list->u1.Function==(unsigned long*)ext_stubs)
										{
											mp_msg(0,0,"%x = %s.%i\n",&thunk_list->u1.Function,name, ordinal);
										}
                }
                else
                {
                    pe_name = (PIMAGE_IMPORT_BY_NAME)RVA(import_list->u1.AddressOfData);
                    //		 printf   ("--- %s %s.%d\n", pe_name->Name, name, pe_name->Hint);
                    thunk_list->u1.Function=(unsigned long*)LookupExternalByName((char*)name, (char*)pe_name->Name);
										if (thunk_list->u1.Function==(unsigned long*)ext_stubs)
										{
											mp_msg(0,0,"%x = %s.%s\n",&thunk_list->u1.Function,name, (char*)pe_name->Name);
										}
                }
                import_list++;
                thunk_list++;
            }
        }
        else
        {
           //printf ("Borland style imports used\n");
            thunk_list = (PIMAGE_THUNK_DATA) RVA(pe_imp->FirstThunk);
            while (thunk_list->u1.Ordinal)
            {
                if (IMAGE_SNAP_BY_ORDINAL(thunk_list->u1.Ordinal))
                {

                    int ordinal = IMAGE_ORDINAL(thunk_list->u1.Ordinal);

                   //printf ("--- Ordinal %s.%d\n",name,ordinal);
                    thunk_list->u1.Function=(unsigned long*)LookupExternal( name, ordinal);
                }
                else
                {
                    pe_name=(PIMAGE_IMPORT_BY_NAME) RVA(thunk_list->u1.AddressOfData);
                   //printf ("--- %s %s.%d\n",
                     //     pe_name->Name,name,pe_name->Hint);
                    thunk_list->u1.Function=(unsigned long*)LookupExternalByName((char*)  name, (char*)pe_name->Name);
                }
                thunk_list++;
            }
        }
    }
    return 0;
}

//***********************************************************************************************
static int calc_vma_size( HMODULE hModule )
{
    int i,vma_size = 0;
    IMAGE_SECTION_HEADER *pe_seg = PE_SECTIONS(hModule);

   //printf ("Dump of segment table\n");
    mp_msg(0,0,"   Name    VSz  Vaddr     SzRaw   Fileadr  *Reloc *Lineum #Reloc #Linum Char\n");
    for (i = 0; i< PE_HEADER(hModule)->FileHeader.NumberOfSections; i++)
    {
      /*printf  ("%8s: %4.4lx %8.8lx %8.8lx %8.8lx %8.8lx %8.8lx %4.4x %4.4x %8.8lx\n",
              pe_seg->Name,
              pe_seg->Misc.VirtualSize,
              pe_seg->VirtualAddress,
              pe_seg->SizeOfRawData,
              pe_seg->PointerToRawData,
              pe_seg->PointerToRelocations,
              pe_seg->PointerToLinenumbers,
              pe_seg->NumberOfRelocations,
              pe_seg->NumberOfLinenumbers,
              pe_seg->Characteristics);*/
        vma_size=max(vma_size, pe_seg->VirtualAddress+pe_seg->SizeOfRawData);
        vma_size=max(vma_size, pe_seg->VirtualAddress+pe_seg->Misc.VirtualSize);
        pe_seg++;
    }
    return vma_size;
}

//***********************************************************************************************
static void do_relocations( unsigned int load_addr, IMAGE_BASE_RELOCATION *r )
{
    int delta = load_addr - PE_HEADER(load_addr)->OptionalHeader.ImageBase;
    int	hdelta = (delta >> 16) & 0xFFFF;
    int	ldelta = delta & 0xFFFF;

    if(delta == 0)

        return;
    while(r->VirtualAddress)
    {
        char *page = (char*) RVA(r->VirtualAddress);
        int count = (r->SizeOfBlock - 8)/2;
        int i;
        //mp_msg(0,0,"%x relocations for page %lx\n", count, r->VirtualAddress);

        for(i=0;i<count;i++)
        {
            int offset = r->TypeOffset[i] & 0xFFF;
            int type = r->TypeOffset[i] >> 12;
            //			mp_msg(0,0,"patching %x type %x\n", offset, type);
            switch(type)
            {
            case IMAGE_REL_BASED_ABSOLUTE:
                break;
            case IMAGE_REL_BASED_HIGH:
                *(short*)(page+offset) += hdelta;
                break;
            case IMAGE_REL_BASED_LOW:
                *(short*)(page+offset) += ldelta;
                break;
            case IMAGE_REL_BASED_HIGHLOW:
                *(int*)(page+offset) += delta;

                break;
            case IMAGE_REL_BASED_HIGHADJ:
               mp_msg(0,0,"Don't know what to do with IMAGE_REL_BASED_HIGHADJ\n");
                break;
            case IMAGE_REL_BASED_MIPS_JMPADDR:
                mp_msg(0,0,"Is this a MIPS machine ???\n");
                break;
            default:
               mp_msg(0,0,"Unknown fixup type\n");
                break;
            }
        }
        r = (IMAGE_BASE_RELOCATION*)((char*)r + r->SizeOfBlock);
    }
}

//***********************************************************************************************
HMODULE PE_LoadImage( int handle, LPCSTR filename, WORD *version )
{
    HMODULE	hModule;
    HANDLE	mapping;

    IMAGE_NT_HEADERS *nt;
    IMAGE_SECTION_HEADER *pe_sec;
    IMAGE_DATA_DIRECTORY *dir;
//    BY_HANDLE_FILE_INFORMATION bhfi;
    int	i, rawsize, lowest_va, vma_size, file_size = 0;
    DWORD load_addr = 0, aoep, reloc = 0;
    //    struct get_read_fd_request *req = get_req_buffer();
    int unix_handle = handle;
    int page_size = getpagesize();


    //    if ( GetFileInformationByHandle( hFile, &bhfi ) )
    //    	file_size = bhfi.nFileSizeLow;
    file_size=lseek(handle, 0, SEEK_END);
    lseek(handle, 0, SEEK_SET);

    //#warning fix CreateFileMappingA
    mapping = (void*)CreateFileMappingA( (HANDLE)handle, NULL, PAGE_READONLY | SEC_COMMIT, 0, 0, NULL );
    if (!mapping)
    {
        mp_msg(0,0,"CreateFileMapping error \n" );
        return 0;
    }
    //    hModule = (HMODULE)MapViewOfFile( mapping, FILE_MAP_READ, 0, 0, 0 );
    hModule=(HMODULE)mapping;
    //    CloseHandle( mapping );
    if (!hModule)
    {
        mp_msg(0,0,"MapViewOfFile error n" );
        return 0;
    }
    if ( *(WORD*)hModule !=IMAGE_DOS_SIGNATURE)
    {
        mp_msg(0,0,"%s image doesn't have DOS signature, but 0x%04x\n", filename,*(WORD*)hModule);
        goto error;
    }
 
    nt = PE_HEADER( hModule );

    if ( nt->Signature != IMAGE_NT_SIGNATURE )
    {
        mp_msg(0,0,"%s image doesn't have PE signature, but 0x%08lx\n", filename, nt->Signature );
        goto error;
    }


    if ( nt->FileHeader.Machine != IMAGE_FILE_MACHINE_I386 )
    {
        mp_msg(0,0,"Trying to load PE image for unsupported architecture (");
        switch (nt->FileHeader.Machine)
        {
        case IMAGE_FILE_MACHINE_UNKNOWN:
            mp_msg(0,0,"Unknown");
            break;
        case IMAGE_FILE_MACHINE_R3000:
            mp_msg(0,0,"R3000");
            break;
        case IMAGE_FILE_MACHINE_R4000:
            mp_msg(0,0,"R4000");
            break;
        case IMAGE_FILE_MACHINE_R10000:
            mp_msg(0,0,"R10000");
            break;
        case IMAGE_FILE_MACHINE_ALPHA:
            mp_msg(0,0,"Alpha");
            break;
        case IMAGE_FILE_MACHINE_POWERPC:
            mp_msg(0,0,"PowerPC");
            break;
        default:
            mp_msg(0,0,"Unknown-%04x", nt->FileHeader.Machine);
            break;
        }
        mp_msg(0,0,")\n");
        goto error;
    }


    pe_sec = PE_SECTIONS( hModule );
    rawsize = 0;
    lowest_va = 0x10000;
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++)
    {
        if (lowest_va > pe_sec[i].VirtualAddress)
            lowest_va = pe_sec[i].VirtualAddress;
        if (pe_sec[i].Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
            continue;
        if (pe_sec[i].PointerToRawData+pe_sec[i].SizeOfRawData > rawsize)
            rawsize = pe_sec[i].PointerToRawData+pe_sec[i].SizeOfRawData;
    }


    if ( file_size && file_size < rawsize )
    {
        mp_msg(0,0,"PE module is too small (header: %d, filesize: %d), ""probably truncated download?\n",rawsize, file_size );
        goto error;
    }


    aoep = nt->OptionalHeader.AddressOfEntryPoint;
    /*if (aoep && (aoep < lowest_va))
        mp_msg(0,0,"VIRUS ING: '%s' has an invalid entrypoint (0x%08lx) "
              "below the first virtual address (0x%08x) "
              "(possibly infected by Tchernobyl/SpaceFiller virus)!\n",
              filename, aoep, lowest_va );
*/

    /* :  Hack!  While we don't really support shared sections yet,
     *         this checks for those special cases where the whole DLL
     *         consists only of shared sections and is mapped into the
     *         shared address space > 2GB.  In this case, we assume that
     *         the module got mapped at its base address. Thus we simply
     *         check whether the module has actually been mapped there
     *         and use it, if so.  This is needed to get Win95 USER32.DLL
     *         to work (until we support shared sections properly).
     */

    if ( nt->OptionalHeader.ImageBase & 0x80000000 )
    {
        HMODULE sharedMod = (HMODULE)nt->OptionalHeader.ImageBase;
        IMAGE_NT_HEADERS *sharedNt = (PIMAGE_NT_HEADERS)
                                     ( (LPBYTE)sharedMod + ((LPBYTE)nt - (LPBYTE)hModule) );

        /* Well, this check is not really comprehensive,
           but should be good enough for now ... */
        if (    !IsBadReadPtr( (LPBYTE)sharedMod, sizeof(IMAGE_DOS_HEADER) )
                && memcmp( (LPBYTE)sharedMod, (LPBYTE)hModule, sizeof(IMAGE_DOS_HEADER) ) == 0
                && !IsBadReadPtr( sharedNt, sizeof(IMAGE_NT_HEADERS) )
                && memcmp( sharedNt, nt, sizeof(IMAGE_NT_HEADERS) ) == 0 )
        {
            UnmapViewOfFile( (LPVOID)hModule );
            return sharedMod;
        }
    }



    load_addr = nt->OptionalHeader.ImageBase;
    vma_size = calc_vma_size( hModule );

    /*load_addr = (DWORD)LinVirtualAlloc( (void*)load_addr, vma_size,
                                     MEM_RESERVE | MEM_COMMIT,
                                     PAGE_EXECUTE_READWRITE );
		*/
		load_addr =0;
    if (load_addr == 0)
    {

        mp_msg(0,0,"We need to perform base relocations for %s\n", filename);
				
        dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_BASERELOC;
        if (dir->Size)
            reloc = dir->VirtualAddress;
        else
        {
           /*printf ( "FATAL: Need to relocate %s, but no relocation records present (%s). Try to run that file directly !\n",
                   filename,
                   (nt->FileHeader.Characteristics&IMAGE_FILE_RELOCS_STRIPPED)?
                   "stripped during link" : "unknown reason" );*/
            goto error;
        }

        /* : If we need to relocate a system DLL (base > 2GB) we should
         *        really make sure that the *new* base address is also > 2GB.
         *        Some DLLs really check the MSB of the module handle :-/
         */
//        if ( nt->OptionalHeader.ImageBase & 0x80000000 )
  //          mp_msg(0,0 "Forced to relocate system DLL (base > 2GB). This is not good.\n" );

        load_addr = (DWORD)LinVirtualAlloc( NULL, vma_size,
                                         MEM_RESERVE | MEM_COMMIT,
                                         PAGE_EXECUTE_READWRITE );
        if (!load_addr)
        {
            mp_msg(0,0,"FATAL: Couldn't load module %s (out of memory, %d needed)!\n", filename, vma_size);
            goto error;
        }
    }

    mp_msg(0,0,"Load addr is %lx (base %lx), range %x\n",load_addr, nt->OptionalHeader.ImageBase, vma_size );
    mp_msg(0,0,"Loading %s at %lx, range %x\n",filename, load_addr, vma_size );

    if ((void*)FILE_dommap( handle, (void *)load_addr, 0, nt->OptionalHeader.SizeOfHeaders,
                            0, 0, PROT_EXEC | PROT_WRITE | PROT_READ,
                            MAP_PRIVATE | MAP_FIXED ) != (void*)load_addr)
    {
       mp_msg(0,0, "Critical Error: failed to map PE header to necessary address.\n");
        goto error;
    }
		//memcpy((BYTE*)load_addr,hModule,nt->OptionalHeader.SizeOfHeaders);


    pe_sec = PE_SECTIONS( hModule );
    for (i = 0; i < nt->FileHeader.NumberOfSections; i++, pe_sec++)
    {
        if (!pe_sec->SizeOfRawData || !pe_sec->PointerToRawData)
            continue;
        if ((void*)FILE_dommap( unix_handle, (void*)RVA(pe_sec->VirtualAddress),
                                0, pe_sec->SizeOfRawData, 0, pe_sec->PointerToRawData,
                                PROT_EXEC | PROT_WRITE | PROT_READ,
                                MAP_PRIVATE | MAP_FIXED ) != (void*)RVA(pe_sec->VirtualAddress))
        {

           mp_msg(0,0, "Critical Error: failed to map PE section to necessary address.\n");
            goto error;
        }
        if ((pe_sec->SizeOfRawData < pe_sec->Misc.VirtualSize) &&
                (pe_sec->SizeOfRawData & (page_size-1)))
        {
            DWORD end = (pe_sec->SizeOfRawData & ~(page_size-1)) + page_size;
            if (end > pe_sec->Misc.VirtualSize)
                end = pe_sec->Misc.VirtualSize;
            //mp_msg(0,0,"clearing %p - %p\n",
              //    RVA(pe_sec->VirtualAddress) + pe_sec->SizeOfRawData,
                //  RVA(pe_sec->VirtualAddress) + end );
            memset( (char*)RVA(pe_sec->VirtualAddress) + pe_sec->SizeOfRawData, 0,end - pe_sec->SizeOfRawData );
        }
    }


    if ( reloc )
		{
        do_relocations( load_addr, (IMAGE_BASE_RELOCATION *)RVA(reloc) );

		}

    *version =   ( (nt->OptionalHeader.MajorSubsystemVersion & 0xff) << 8 )|   (nt->OptionalHeader.MinorSubsystemVersion & 0xff);


    UnmapViewOfFile( (LPVOID)hModule );
    return (HMODULE)load_addr;

error:
    if (unix_handle != -1)
        close( unix_handle );
    if (load_addr)
        LinVirtualFree( (LPVOID)load_addr, 0, MEM_RELEASE );
    UnmapViewOfFile( (LPVOID)hModule );
    return 0;
}

//***********************************************************************************************
WINE_MODREF *PE_CreateModule( HMODULE hModule,LPCSTR filename, DWORD flags, WIN_BOOL builtin )
{
    DWORD load_addr = (DWORD)hModule;
    IMAGE_NT_HEADERS *nt = PE_HEADER(hModule);
    IMAGE_DATA_DIRECTORY *dir;
    IMAGE_IMPORT_DESCRIPTOR *pe_import = NULL;
    IMAGE_EXPORT_DIRECTORY *pe_export = NULL;
    IMAGE_RESOURCE_DIRECTORY *pe_resource = NULL;
    WINE_MODREF *wm;
//    int	result;




    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXPORT;
    if (dir->Size)
        pe_export = (PIMAGE_EXPORT_DIRECTORY)RVA(dir->VirtualAddress);

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_IMPORT;
    if (dir->Size)
        pe_import = (PIMAGE_IMPORT_DESCRIPTOR)RVA(dir->VirtualAddress);

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_RESOURCE;
    if (dir->Size)
        pe_resource = (PIMAGE_RESOURCE_DIRECTORY)RVA(dir->VirtualAddress);

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_EXCEPTION;
//    if (dir->Size)
  //     printf ("Exception directory ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_SECURITY;
    //if (dir->Size)
      //  mp_msg(0,0,"Security directory ignored\n" );




    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_DEBUG;
//    if (dir->Size)
  //      mp_msg(0,0,"Debug directory ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_COPYRIGHT;
    //if (dir->Size)
      //  mp_msg(0,0,"Copyright string ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_GLOBALPTR;
    //if (dir->Size)
      //  mp_msg(0,0,"Global Pointer (MIPS) ignored\n" );



    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG;
    //if (dir->Size)
      //  mp_msg(0,0,"Load Configuration directory ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT;
    //if (dir->Size)
      //  mp_msg(0,0,"Bound Import directory ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_IAT;
    //if (dir->Size)
      //  mp_msg(0,0,"Import Address Table directory ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT;
    if (dir->Size)
    {
        //("Delayed import, stub calls LoadLibrary\n" );
        /*
         * Nothing to do here.
         */

#ifdef ImgDelayDescr
        /*
         * This code is useful to observe what the heck is going on.
         */
        {
            ImgDelayDescr *pe_delay = NULL;
            pe_delay = (PImgDelayDescr)RVA(dir->VirtualAddress);
            /*mp_msg(0,0,"pe_delay->grAttrs = %08x\n", pe_delay->grAttrs);
            mp_msg(0,0,"pe_delay->szName = %s\n", pe_delay->szName);
            mp_msg(0,0,"pe_delay->phmod = %08x\n", pe_delay->phmod);
            mp_msg(0,0,"pe_delay->pIAT = %08x\n", pe_delay->pIAT);
            mp_msg(0,0,"pe_delay->pINT = %08x\n", pe_delay->pINT);
            mp_msg(0,0,"pe_delay->pBoundIAT = %08x\n", pe_delay->pBoundIAT);
            mp_msg(0,0,"pe_delay->pUnloadIAT = %08x\n", pe_delay->pUnloadIAT);
            mp_msg(0,0,"pe_delay->dwTimeStamp = %08x\n", pe_delay->dwTimeStamp);*/
        }
#endif
    }

    dir = nt->OptionalHeader.DataDirectory+IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR;
    //if (dir->Size)
      //  mp_msg(0,0,"Unknown directory 14 ignored\n" );

    dir = nt->OptionalHeader.DataDirectory+15;
//    if (dir->Size)
  //     printf ("Unknown directory 15 ignored\n" );



    wm = (WINE_MODREF *)expHeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*wm) );
    wm->module = hModule;

    if ( builtin )
        wm->flags |= WINE_MODREF_INTERNAL;
    if ( flags & DONT_RESOLVE_DLL_REFERENCES )
        wm->flags |= WINE_MODREF_DONT_RESOLVE_REFS;
    if ( flags & LOAD_LIBRARY_AS_DATAFILE )
        wm->flags |= WINE_MODREF_LOAD_AS_DATAFILE;

    wm->type = MODULE32_PE;
    wm->binfmt.pe.pe_export = pe_export;
    wm->binfmt.pe.pe_import = pe_import;
    wm->binfmt.pe.pe_resource = pe_resource;
    wm->binfmt.pe.tlsindex = -1;

    wm->filename = (char*)malloc(strlen(filename)+1);
    strcpy(wm->filename, filename );
    wm->modname = strrchr( wm->filename, '\\' );
    if (!wm->modname)
        wm->modname = wm->filename;
    else
        wm->modname++;

    if ( pe_export )
        dump_exports( hModule );

    /* Fixup Imports */

    if (    pe_import
            && !( wm->flags & WINE_MODREF_LOAD_AS_DATAFILE )
            && !( wm->flags & WINE_MODREF_DONT_RESOLVE_REFS )
            && fixup_imports( wm ) )
    {
        /* remove entry from modref chain */
        return NULL;
    }

    return wm;

    return wm;
}

//***********************************************************************************************
WINE_MODREF *PE_LoadLibraryExA (LPCSTR name, DWORD flags)
{
    HMODULE		hModule32;
    WINE_MODREF	*wm;
    char        	filename[256];
    int hFile;
    WORD		version = 0;


    strncpy(filename, name, sizeof(filename));
    hFile=open(filename, O_RDONLY|O_BINARY);
    if(hFile==-1)
        return NULL;


    hModule32 = PE_LoadImage( hFile, filename, &version );
    if (!hModule32)
    {
        //SetLastError( OR_OUTOFMEMORY );
        return NULL;
    }

    if ( !(wm = PE_CreateModule( hModule32, filename, flags, FALSE )) )
    {
       mp_msg(0,0, "can't load %s\n", filename );
        //SetLastError( OR_OUTOFMEMORY );
        return NULL;
    }
    close(hFile);
    //("^^^^^^^^^^^^^^^^Alloc VM1  %p\n", wm);
    return wm;
}


//***********************************************************************************************
void PE_UnloadLibrary(WINE_MODREF *wm)
{
    mp_msg(0,0," unloading %s\n", wm->filename);

    if (wm->filename)
        free(wm->filename);
    if (wm->short_filename)
        free(wm->short_filename);
    HeapFree( GetProcessHeap(), 0, wm->deps );
    LinVirtualFree( (LPVOID)wm->module, 0, MEM_RELEASE );
    HeapFree( GetProcessHeap(), 0, wm );
    //("^^^^^^^^^^^^^^^^Free VM1  %p\n", wm);
}


//***********************************************************************************************
static void extend_stack_for_dll_alloca(void)
{
		mp_msg(0,0,"extend stack\n");
    //void* mem=alloca(0x20000);

    //*(int*)mem=0x1234;
		mp_msg(0,0,"extend stack done\n");
}

//***********************************************************************************************
WIN_BOOL PE_InitDLL( WINE_MODREF *wm, DWORD type, LPVOID lpReserved )
{
  WIN_BOOL retv = TRUE;
  if ((PE_HEADER(wm->module)->FileHeader.Characteristics & IMAGE_FILE_DLL) &&
      (PE_HEADER(wm->module)->OptionalHeader.AddressOfEntryPoint))
  {
      DLLENTRYPROC entry;
      entry = (DLLENTRYPROC)PE_FindExportedFunction(wm, "DllMain", 0);
      if(entry ==NULL)
			{
         entry = (DLLENTRYPROC)RVA_PTR( wm->module,OptionalHeader.AddressOfEntryPoint );    
			}

      mp_msg(0,0,"Entering DllMain(");
      switch(type)
      {
				case DLL_PROCESS_DETACH:
					 //printf ("DLL_PROCESS_DETACH) ");
						break;
				case DLL_PROCESS_ATTACH:
						mp_msg(0,0,"DLL_PROCESS_ATTACH) ");
						break;
				case DLL_THREAD_DETACH:
						mp_msg(0,0,"DLL_THREAD_DETACH) ");
						break;
				case DLL_THREAD_ATTACH:
         //printf ("DLL_THREAD_ATTACH) ");
          break;
      }
      extend_stack_for_dll_alloca();
#ifdef _XBOX
			void* pmem=_alloca(0x8000);
			*(int*)pmem=0x1234;
			try
			{
			retv = entry ( wm->module, type, lpReserved ); // TESTJE
			}
			catch(...)
			{
			}	
#endif
  }

  return retv;
}
//***********************************************************************************************
static LPVOID _fixup_address(PIMAGE_OPTIONAL_HEADER opt,int delta,LPVOID addr)
{
  if (	((DWORD)addr>opt->ImageBase) && ((DWORD)addr<opt->ImageBase+opt->SizeOfImage) )
      return (LPVOID)(((DWORD)addr)+delta);
  else
      return addr;
}
