#ifndef __WINE_PE_IMAGE_H
#define __WINE_PE_IMAGE_H

#include "cygwin_inttypes.h"
#include "pe.h"
#pragma pack(1)
#define PE_HEADER(module) \
    ((IMAGE_NT_HEADERS*)((LPBYTE)(module)  + /*-1*/ +\
                         (((IMAGE_DOS_HEADER*)(module))->e_lfanew)))

#define PE_SECTIONS(module) \
    ((IMAGE_SECTION_HEADER*)((LPBYTE)&PE_HEADER(module)->OptionalHeader + \
                           PE_HEADER(module)->FileHeader.SizeOfOptionalHeader))

#define RVA_PTR(module,field) ((LPBYTE)(module) + PE_HEADER(module)->field)

/* modreference used for attached processes
 * all section are calculated here, relocations etc.
 
typedef struct {
	PIMAGE_IMPORT_DESCRIPTOR	pe_import;
	PIMAGE_EXPORT_DIRECTORY	pe_export;
	PIMAGE_RESOURCE_DIRECTORY	pe_resource;
	int				tlsindex;
} PE_MODREF;
*/

/* The security attributes structure */
typedef struct _SECURITY_ATTRIBUTES
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    WIN_BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct {
        DWORD cb;		/* 00: size of struct */
        LPSTR lpReserved;	/* 04: */
        LPSTR lpDesktop;	/* 08: */
        LPSTR lpTitle;		/* 0c: */
        DWORD dwX;		/* 10: */
        DWORD dwY;		/* 14: */
        DWORD dwXSize;		/* 18: */
        DWORD dwYSize;		/* 1c: */
        DWORD dwXCountChars;	/* 20: */
        DWORD dwYCountChars;	/* 24: */
        DWORD dwFillAttribute;	/* 28: */
        DWORD dwFlags;		/* 2c: */
        WORD wShowWindow;	/* 30: */
        WORD cbReserved2;	/* 32: */
        BYTE *lpReserved2;	/* 34: */
        HANDLE hStdInput;	/* 38: */
        HANDLE hStdOutput;	/* 3c: */
        HANDLE hStdError;	/* 40: */
} STARTUPINFOA, *LPSTARTUPINFOA;
typedef struct {
	HANDLE	hProcess;
	HANDLE	hThread;
	DWORD		dwProcessId;
	DWORD		dwThreadId;
} PROCESS_INFORMATION,*LPPROCESS_INFORMATION;

#define DECLARE_HANDLE(name) typedef HANDLE name
DECLARE_HANDLE(HFILE);
typedef WIN_BOOL CALLBACK (*ENUMRESTYPEPROCA)(HMODULE,LPSTR,LONG);
typedef WIN_BOOL CALLBACK (*ENUMRESTYPEPROCW)(HMODULE,LPWSTR,LONG);
typedef WIN_BOOL CALLBACK (*ENUMRESNAMEPROCA)(HMODULE,LPCSTR,LPSTR,LONG);
typedef WIN_BOOL CALLBACK (*ENUMRESNAMEPROCW)(HMODULE,LPCWSTR,LPWSTR,LONG);
typedef WIN_BOOL CALLBACK (*ENUMRESLANGPROCA)(HMODULE,LPCSTR,LPCSTR,WORD,LONG);
typedef WIN_BOOL CALLBACK (*ENUMRESLANGPROCW)(HMODULE,LPCWSTR,LPCWSTR,WORD,LONG);
DECLARE_HANDLE(HRSRC);
typedef HANDLE HGLOBAL;
typedef UINT16          HANDLE16;
struct _wine_modref;
extern int PE_unloadImage(HMODULE hModule);
extern FARPROC PE_FindExportedFunction(struct _wine_modref *wm, LPCSTR funcName, WIN_BOOL snoop);
extern WIN_BOOL PE_EnumResourceTypesA(HMODULE,ENUMRESTYPEPROCA,LONG);
extern WIN_BOOL PE_EnumResourceTypesW(HMODULE,ENUMRESTYPEPROCW,LONG);
extern WIN_BOOL PE_EnumResourceNamesA(HMODULE,LPCSTR,ENUMRESNAMEPROCA,LONG);
extern WIN_BOOL PE_EnumResourceNamesW(HMODULE,LPCWSTR,ENUMRESNAMEPROCW,LONG);
extern WIN_BOOL PE_EnumResourceLanguagesA(HMODULE,LPCSTR,LPCSTR,ENUMRESLANGPROCA,LONG);
extern WIN_BOOL PE_EnumResourceLanguagesW(HMODULE,LPCWSTR,LPCWSTR,ENUMRESLANGPROCW,LONG);
extern HRSRC PE_FindResourceExW(struct _wine_modref*,LPCWSTR,LPCWSTR,WORD);
extern DWORD PE_SizeofResource(HMODULE,HRSRC);
extern struct _wine_modref *PE_LoadLibraryExA(LPCSTR, DWORD);
extern void PE_UnloadLibrary(struct _wine_modref *);
extern HGLOBAL PE_LoadResource(struct _wine_modref *wm,HRSRC);
extern HMODULE PE_LoadImage( int hFile, LPCSTR filename, WORD *version );
extern struct _wine_modref *PE_CreateModule( HMODULE hModule, LPCSTR filename,
                                             DWORD flags, WIN_BOOL builtin );
extern WIN_BOOL PE_CreateProcess( HANDLE hFile, LPCSTR filename, LPCSTR cmd_line, LPCSTR env, 
                              LPSECURITY_ATTRIBUTES psa, LPSECURITY_ATTRIBUTES tsa,
                              WIN_BOOL inherit, DWORD flags, LPSTARTUPINFOA startup,
                              LPPROCESS_INFORMATION info );

extern void PE_InitTls(void);
extern WIN_BOOL PE_InitDLL(struct _wine_modref *wm, DWORD type, LPVOID lpReserved);

extern PIMAGE_RESOURCE_DIRECTORY GetResDirEntryA(PIMAGE_RESOURCE_DIRECTORY,LPCSTR,DWORD,WIN_BOOL);
extern PIMAGE_RESOURCE_DIRECTORY GetResDirEntryW(PIMAGE_RESOURCE_DIRECTORY,LPCWSTR,DWORD,WIN_BOOL);

typedef DWORD CALLBACK (*DLLENTRYPROC)(HMODULE,DWORD,LPVOID);


extern struct _wine_modref *ELF_LoadLibraryExA( LPCSTR libname, DWORD flags);
extern void ELF_UnloadLibrary(struct _wine_modref *);
extern FARPROC ELF_FindExportedFunction(struct _wine_modref *wm, LPCSTR funcName);

#pragma pack()
/* Could this type be considered opaque? */
typedef struct {
	LPVOID	DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	HANDLE OwningThread;
	HANDLE LockSemaphore;
	DWORD Reserved;
}CRITICAL_SECTION;

#define E_NOTIMPL			0x80004001
#define E_NOINTERFACE			0x80004002
#define E_POINTER			0x80004003
#define E_ABORT				0x80004004
#define E_FAIL				0x80004005

#define E_ACCESSDENIED			      0x80070005
#define E_HANDLE            			0x80070006
#define	E_OUTOFMEMORY			        0x8007000E
#define	E_INVALIDARG			        0x80070057
#define NOERROR                                0L
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)

#endif /* __WINE_PE_IMAGE_H */
