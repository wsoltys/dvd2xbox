/*
 * Module definitions
 *
 * Copyright 1995 Alexandre Julliard
 */

#ifndef __WINE_MODULE_H
#define __WINE_MODULE_H

#include "cygwin_inttypes.h"
#include "pe_image.h"

extern "C" HMODULE __stdcall LoadLibraryA(LPCSTR libname);
extern "C" FARPROC __stdcall GetProcAddress( HMODULE hModule, LPCSTR function );
extern "C" HMODULE				 LoadLibraryExA(LPCSTR libname, HANDLE hfile, DWORD flags);
extern "C" BOOL __stdcall FreeLibrary(HINSTANCE hLibModule);
void					 MODULE_DllProcessDetach( WINE_MODREF* wm, WIN_BOOL bForceDetach, LPVOID lpReserved );
WIN_BOOL			 MODULE_DllProcessAttach( WINE_MODREF *wm, LPVOID lpReserved );
WIN_BOOL			 MODULE_FreeLibrary( WINE_MODREF *wm );
void					 MODULE_RemoveFromList(WINE_MODREF *mod);
WINE_MODREF* MODULE_FindModuleW(LPCWSTR name);
WINE_MODREF *	 MODULE_LoadLibraryExA( LPCSTR libname, HFILE hfile, DWORD flags );
typedef struct {
    BYTE type;
    BYTE flags;
    BYTE segnum;
    WORD offs WINE_PACKED;
} ET_ENTRY;

typedef struct {
    WORD first; /* ordinal */
    WORD last; /* ordinal */
    WORD next; /* bundle */
} ET_BUNDLE;


  /* In-memory segment table */
typedef struct
{
    WORD      filepos;   /* Position in file, in sectors */
    WORD      size;      /* Segment size on disk */
    WORD      flags;     /* Segment flags */
    WORD      minsize;   /* Min. size of segment in memory */
    HANDLE16  hSeg;      /* Selector or handle (selector - 1) */
                         /* of segment in memory */
} SEGTABLEENTRY;


  /* Self-loading modules contain this structure in their first segment */
typedef LRESULT CALLBACK (*FARPROC16)();

#include "pshpack1.h"


typedef struct
{
    WORD      version;       /* Must be "A0" (0x3041) */
    WORD      reserved;
    FARPROC16 BootApp;       /* startup procedure */
    FARPROC16 LoadAppSeg;    /* procedure to load a segment */
    FARPROC16 reserved2;
    FARPROC16 MyAlloc;       /* memory allocation procedure, 
                              * wine must write this field */
    FARPROC16 EntryAddrProc;
    FARPROC16 ExitProc;      /* exit procedure */
    WORD      reserved3[4];
    FARPROC16 SetOwner;      /* Set Owner procedure, exported by wine */
} SELFLOADHEADER;

  /* Parameters for LoadModule() 
typedef struct
{
    HGLOBAL16 hEnvironment;         
    SEGPTR    cmdLine WINE_PACKED;  
    SEGPTR    showCmd WINE_PACKED;  
    SEGPTR    reserved WINE_PACKED;
} LOADPARAMS16;*/

typedef struct 
{
    LPSTR lpEnvAddress;
    LPSTR lpCmdLine;
    UINT16 *lpCmdShow;
    DWORD dwReserved;
} LOADPARAMS;

#include "poppack.h"

/* internal representation of 32bit modules. per process. 
typedef enum {
	MODULE32_PE = 1,
	MODULE32_ELF,
	MODULE32_ELFDLL
} MODULE32_TYPE;

typedef struct _wine_modref
{
	struct _wine_modref	*next;
	struct _wine_modref	*prev;
	MODULE32_TYPE		type;
	union {
		PE_MODREF	pe;
		ELF_MODREF	elf;
	} binfmt;

	HMODULE			module;

	int			nDeps;
	struct _wine_modref	**deps;

	int			flags;
	int			refCount;

	char			*filename;
	char			*modname;
	char			*short_filename;
	char			*short_modname;
} WINE_MODREF;*/

#define WINE_MODREF_INTERNAL              0x00000001
#define WINE_MODREF_NO_DLL_CALLS          0x00000002
#define WINE_MODREF_PROCESS_ATTACHED      0x00000004
#define WINE_MODREF_LOAD_AS_DATAFILE      0x00000010
#define WINE_MODREF_DONT_RESOLVE_REFS     0x00000020
#define WINE_MODREF_MARKER                0x80000000



/* Resource types */
typedef struct resource_typeinfo_s NE_TYPEINFO;
typedef struct resource_nameinfo_s NE_NAMEINFO;

#define NE_SEG_TABLE(pModule) \
    ((SEGTABLEENTRY *)((char *)(pModule) + (pModule)->seg_table))

#define NE_MODULE_TABLE(pModule) \
    ((WORD *)((char *)(pModule) + (pModule)->modref_table))

#define NE_MODULE_NAME(pModule) \
    (((OFSTRUCT *)((char*)(pModule) + (pModule)->fileinfo))->szPathName)

struct modref_list_t;

typedef struct modref_list_t
{
    WINE_MODREF* wm;
    struct modref_list_t *next;
    struct modref_list_t *prev;
} modref_list;


/* module.c */
extern FARPROC MODULE_GetProcAddress( HMODULE hModule, LPCSTR function, WIN_BOOL snoop );
extern WINE_MODREF *MODULE32_LookupHMODULE( HMODULE hModule );
extern WINE_MODREF *MODULE_FindModule( LPCSTR path );
#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct
{
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
} FILETIME, *LPFILETIME;
#endif /* _FILETIME_ */
typedef struct
{
  int dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  int dwVolumeSerialNumber;
  int nFileSizeHigh;
  int nFileSizeLow;
  int nNumberOfLinks;
  int nFileIndexHigh;
  int nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION ;
#define	IMAGE_DIRECTORY_ENTRY_EXPORT		0
#define	IMAGE_DIRECTORY_ENTRY_IMPORT		1
#define	IMAGE_DIRECTORY_ENTRY_RESOURCE		2
#define	IMAGE_DIRECTORY_ENTRY_EXCEPTION		3
#define	IMAGE_DIRECTORY_ENTRY_SECURITY		4
#define	IMAGE_DIRECTORY_ENTRY_BASERELOC		5
#define	IMAGE_DIRECTORY_ENTRY_DEBUG		6
#define	IMAGE_DIRECTORY_ENTRY_COPYRIGHT		7
#define	IMAGE_DIRECTORY_ENTRY_GLOBALPTR		8   /* (MIPS GP) */
#define	IMAGE_DIRECTORY_ENTRY_TLS		9
#define	IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG	10
#define	IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT	11
#define	IMAGE_DIRECTORY_ENTRY_IAT		12  /* Import Address Table */
#define	IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT	13
#define	IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR	14

#define MEM_COMMIT           0x1000     
#define MEM_RESERVE          0x2000     
#define MEM_DECOMMIT         0x4000     
#define MEM_RELEASE          0x8000     
#define MEM_FREE            0x10000     
#define MEM_PRIVATE         0x20000     
#define MEM_MAPPED          0x40000     
#define MEM_RESET           0x80000     
#define MEM_TOP_DOWN       0x100000     
#define MEM_4MB_PAGES    0x80000000   

#define HEAP_NO_SERIALIZE               0x00000001
#define HEAP_GROWABLE                   0x00000002
#define HEAP_GENERATE_EXCEPTIONS        0x00000004
#define HEAP_ZERO_MEMORY                0x00000008
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020
#define HEAP_FREE_CHECKING_ENABLED      0x00000040
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080
#define HEAP_CREATE_ALIGN_16            0x00010000
#define HEAP_CREATE_ENABLE_TRACING      0x00020000


#define SEC_FILE                0x00800000
#define SEC_IMAGE               0x01000000
#define SEC_RESERVE             0x04000000
#define SEC_COMMIT              0x08000000
#define SEC_NOCACHE             0x10000000
DECLARE_HANDLE(HFILE);
#endif  /* __WINE_MODULE_H */
