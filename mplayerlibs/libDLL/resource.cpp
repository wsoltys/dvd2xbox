/*
 * Resources
 *
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Alexandre Julliard
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "module.h"
#define CP_ACP					0

DECLARE_HANDLE(HRSRC);
#define HFILE_ERROR     ((HFILE)-1)

extern HANDLE  __stdcall expGetProcessHeap(void);
extern BOOL  __stdcall expHeapFree(HANDLE heap, DWORD dwFlags, LPVOID lpMem);
extern void*  __stdcall expHeapAlloc(HANDLE heap, DWORD flags, DWORD size);
extern HRSRC __stdcall LoadResource(HMODULE, HRSRC);
extern LONG  __stdcall  explstrcpynA(char* str1, const char* str2,int len);

#define __max(a,b) (((a) > (b)) ? (a) : (b))
#define __min(a,b) (((a) < (b)) ? (a) : (b))
#ifndef max
#define max(a,b)   (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#endif
#define MAKEINTRESOURCEA(i) (LPSTR)((DWORD)((WORD)(i)))
#define MAKEINTRESOURCEW(i) (LPWSTR)((DWORD)((WORD)(i)))

#define RT_STRINGW         MAKEINTRESOURCEW(6)
#define RT_MESSAGELISTW    MAKEINTRESOURCEW(11)

#define IMAGE_SEPARATE_DEBUG_SIGNATURE 0x4944


typedef struct tagMESSAGE_RESOURCE_ENTRY {
	WORD	Length;
	WORD	Flags;
	BYTE	Text[1];
} MESSAGE_RESOURCE_ENTRY,*PMESSAGE_RESOURCE_ENTRY;
#define	MESSAGE_RESOURCE_UNICODE	0x0001

typedef struct tagMESSAGE_RESOURCE_BLOCK {
	DWORD	LowId;
	DWORD	HighId;
	DWORD	OffsetToEntries;
} MESSAGE_RESOURCE_BLOCK,*PMESSAGE_RESOURCE_BLOCK;

typedef struct tagMESSAGE_RESOURCE_DATA {
	DWORD			NumberOfBlocks;
	MESSAGE_RESOURCE_BLOCK	Blocks[ 1 ];
} MESSAGE_RESOURCE_DATA,*PMESSAGE_RESOURCE_DATA;


WORD WINE_LanguageId=0x409;//english

#define HRSRC_MAP_BLOCKSIZE 16

typedef struct _HRSRC_ELEM
{
    HANDLE hRsrc;
    WORD     type;
}
HRSRC_ELEM;

typedef struct _HRSRC_MAP
{
    int nAlloc;
    int nUsed;
    HRSRC_ELEM *elem;
}
HRSRC_MAP;

int __stdcall WideCharToMultiByte(unsigned int codepage, DWORD flags, LPCWSTR src,
                               int srclen,LPSTR dest, int destlen, LPCSTR defch, BOOL* used_defch)
{
    int i;
    if(src==0)
        return 0;
    if ((srclen==-1)&&(dest==0))
        return 0;
    if(srclen==-1)
    {
        srclen=0;
        while(src[srclen++])
            ;
    }
    //    for(i=0; i<srclen; i++)
    //	printf("%c", src[i]);
    //    printf("\n");
    if(dest==0)
    {
        for(i=0; i<srclen; i++)
        {
            src++;
            if(*src==0)
                return i+1;
        }
        return srclen+1;
    }
    if(used_defch)
        *used_defch=0;
    for(i=0; i<min(srclen, destlen); i++)
    {
        *dest=(char)*src;
        dest++;
        src++;
        if(*src==0)
            return i+1;
    }
    return min(srclen, destlen);
}


LPSTR HEAP_strdupA(HANDLE heap, DWORD flags, LPCSTR string)
{
    //    return strdup(string);
    char* answ = (char*) malloc(strlen(string) + 1);
    strcpy(answ, string);
    return answ;
}
LPWSTR HEAP_strdupAtoW(HANDLE heap, DWORD flags, LPCSTR string)
{
    int size, i;
    WCHAR* answer;
    if(string==0)
        return 0;
    size=strlen(string);
    answer = (WCHAR*) malloc(sizeof(WCHAR) * (size + 1));
    for(i=0; i<=size; i++)
        answer[i]=(short)string[i];
    return answer;
}
LPSTR HEAP_strdupWtoA(HANDLE heap, DWORD flags, LPCWSTR string)
{
    int size, i;
    char* answer;
    if(string==0)
        return 0;
    size=0;
    while(string[size])
        size++;
    answer = (char*) malloc(size + 2);
    for(i=0; i<=size; i++)
        answer[i]=(char)string[i];
    return answer;
}


static HRSRC RES_FindResource2( HMODULE hModule, LPCSTR type,
                                LPCSTR name, WORD lang, int unicode)
{
    HRSRC hRsrc = 0;
    LPWSTR typeStr, nameStr;
    WINE_MODREF *wm = MODULE32_LookupHMODULE( hModule );

    if(!wm)
        return 0;
    /* 32-bit PE module */


    if ( HIWORD( type ) && (!unicode))
        typeStr = HEAP_strdupAtoW( expGetProcessHeap(), 0, type );
    else
        typeStr = (LPWSTR)type;
    if ( HIWORD( name ) && (!unicode))
        nameStr = HEAP_strdupAtoW( expGetProcessHeap(), 0, name );
    else
        nameStr = (LPWSTR)name;

    hRsrc = PE_FindResourceExW( wm, nameStr, typeStr, lang );

    if ( HIWORD( type ) && (!unicode))
        expHeapFree( expGetProcessHeap(), 0, typeStr );
    if ( HIWORD( name ) && (!unicode))
        expHeapFree( expGetProcessHeap(), 0, nameStr );

    return hRsrc;
}

/**********************************************************************
 *          RES_FindResource
 */

static HRSRC RES_FindResource( HMODULE hModule, LPCSTR type,
                               LPCSTR name, WORD lang, int unicode )
{
    HRSRC hRsrc;
    //    __TRY
    //    {
    hRsrc = RES_FindResource2(hModule, type, name, lang, unicode);
    //    }
    //    __EXCEPT(page_fault)
    //    {
    //	WARN("page fault\n");
    //	SetLastError(ERROR_INVALID_PARAMETER);
    //	return 0;
    //    }
    //    __ENDTRY
    return hRsrc;
}

/**********************************************************************
 *          RES_SizeofResource
 */
static DWORD RES_SizeofResource( HMODULE hModule, HRSRC hRsrc)
{
    DWORD size = 0;

    //    HMODULE16 hMod16   = MapHModuleLS( hModule );
    //    NE_MODULE *pModule = NE_GetPtr( hMod16 );
    //    WINE_MODREF *wm    = pModule && pModule->module32?
    //                         MODULE32_LookupHMODULE( pModule->module32 ) : NULL;
    WINE_MODREF *wm = MODULE32_LookupHMODULE( hModule );

    if ( !hModule || !hRsrc )
        return 0;

    /* 32-bit PE module */
    /* If we got a 16-bit hRsrc, convert it */
    //    hRsrc32  = HIWORD(hRsrc)? hRsrc : MapHRsrc16To32( pModule, hRsrc );
    if(!HIWORD(hRsrc))
    {
        printf("16-bit hRsrcs not supported\n");
        return 0;
    }
    size = PE_SizeofResource( hModule, hRsrc );
    return size;
}

/**********************************************************************
 *          RES_AccessResource
 */
static HFILE RES_AccessResource( HMODULE hModule, HRSRC hRsrc )
{
    HFILE hFile = HFILE_ERROR;

    WINE_MODREF *wm = MODULE32_LookupHMODULE( hModule );

    if ( !hModule || !hRsrc )
        return HFILE_ERROR;

    /* 32-bit PE module */
    //FIXME("32-bit modules not yet supported.\n" );
    hFile = HFILE_ERROR;

    return hFile;
}

/**********************************************************************
 *          RES_LoadResource
 */
static HGLOBAL RES_LoadResource( HMODULE hModule, HRSRC hRsrc)
{
    HGLOBAL hMem = 0;
//    HRSRC hRsrc32;
    WINE_MODREF *wm = MODULE32_LookupHMODULE( hModule );


    if ( !hModule || !hRsrc )
        return 0;

    /* 32-bit PE module */

    /* If we got a 16-bit hRsrc, convert it */
    //    hRsrc32 = HIWORD(hRsrc)? hRsrc : MapHRsrc16To32( pModule, hRsrc );
    if(!HIWORD(hRsrc))
    {
        printf("16-bit hRsrcs not supported\n");
        return 0;
    }
    hMem = PE_LoadResource( wm, hRsrc );

    return hMem;
}

/**********************************************************************
 *          RES_LockResource
 */
static LPVOID RES_LockResource( HGLOBAL handle )
{
    LPVOID bits = NULL;

//    TRACE("(%08x, %s)\n", handle, "PE" );

    bits = (LPVOID)handle;

    return bits;
}

/**********************************************************************
 *          RES_FreeResource
 */
static BOOL RES_FreeResource( HGLOBAL handle )
{
    HGLOBAL retv = handle;
    return (BOOL)retv;
}

/**********************************************************************
 *	    FindResourceA    (KERNEL32.128)
 */
HANDLE __stdcall FindResourceA( HMODULE hModule, LPCSTR name, LPCSTR type )
{
    return RES_FindResource( hModule, type, name,
                             WINE_LanguageId, 0);
}
HANDLE __stdcall FindResourceW( HMODULE hModule, LPCWSTR name, LPCWSTR type )
{
    return RES_FindResource( hModule, (LPCSTR)type, (LPCSTR)name,
                             WINE_LanguageId, 1);
}

/**********************************************************************
 *	    FindResourceExA  (KERNEL32.129)
 */
HANDLE __stdcall FindResourceExA( HMODULE hModule,
                               LPCSTR type, LPCSTR name, WORD lang )
{
    return RES_FindResource( hModule, type, name,
                             lang, 0 );
}

HANDLE __stdcall FindResourceExW( HMODULE hModule,
                               LPCWSTR type, LPCWSTR name, WORD lang )
{
    return RES_FindResource( hModule, (LPCSTR)type, (LPCSTR)name,
                             lang, 1 );
}



/**********************************************************************
 *	    LockResource     (KERNEL32.384)
 */
LPVOID __stdcall LockResource( HGLOBAL handle )
{
    return RES_LockResource( handle );
}


/**********************************************************************
 *	    FreeResource     (KERNEL32.145)
 */
BOOL __stdcall FreeResource( HGLOBAL handle )
{
    return RES_FreeResource( handle );
}


/**********************************************************************
 *	    AccessResource   (KERNEL32.64)
 */
int __stdcall AccessResource( HMODULE hModule, HRSRC hRsrc )
{
    return (int)RES_AccessResource( hModule, hRsrc );
}
/**********************************************************************
 *	    SizeofResource   (KERNEL32.522)
 */
DWORD __stdcall SizeofResource( HINSTANCE hModule, HRSRC hRsrc )
{
    return RES_SizeofResource( hModule, hRsrc );
}


int __stdcall LoadStringW( HINSTANCE instance, unsigned int resource_id,
                        LPWSTR buffer, int buflen );

/**********************************************************************
 *	LoadStringA	(USER32.375)
 */
int __stdcall LoadStringA( HINSTANCE instance, unsigned int resource_id,
                        LPSTR buffer, int buflen )
{
    int   retval;
    int   wbuflen;
    int   abuflen;
    LPWSTR wbuf = NULL;
    LPSTR  abuf = NULL;

    if ( buffer != NULL && buflen > 0 )
        *buffer = 0;

    wbuflen = LoadStringW(instance,resource_id,NULL,0);
    if ( !wbuflen )
        return 0;
    wbuflen ++;

    retval = 0;
    wbuf = (LPWSTR) expHeapAlloc( expGetProcessHeap(), 0, wbuflen * sizeof(WCHAR) );
    wbuflen = LoadStringW(instance,resource_id,wbuf,wbuflen);
    if ( wbuflen > 0 )
    {
        abuflen = WideCharToMultiByte(CP_ACP,0,wbuf,wbuflen,NULL,0,NULL,NULL);
        if ( abuflen > 0 )
        {
            if ( buffer == NULL || buflen == 0 )
                retval = abuflen;
            else
            {
                abuf = (LPSTR) expHeapAlloc( expGetProcessHeap(), 0, abuflen * sizeof(CHAR) );
                abuflen = WideCharToMultiByte(CP_ACP,0,wbuf,wbuflen,abuf,abuflen,NULL,NULL);
                if ( abuflen > 0 )
                {
                    abuflen = min(abuflen,buflen - 1);
                    memcpy( buffer, abuf, abuflen );
                    buffer[abuflen] = 0;
                    retval = abuflen;
                }
                expHeapFree( expGetProcessHeap(), 0, abuf );
            }
        }
    }
    expHeapFree( expGetProcessHeap(), 0, wbuf );

    return retval;
}

/**********************************************************************
 *	LoadStringW		(USER32.376)
 */
int __stdcall LoadStringW( HINSTANCE instance, unsigned int resource_id,
                        LPWSTR buffer, int buflen )
{
    HGLOBAL hmem;
    HRSRC hrsrc;
    WCHAR *p;
    int string_num;
    int i;

    if (HIWORD(resource_id)==0xFFFF) /* netscape 3 passes this */
        resource_id = (unsigned int )(-((int )resource_id));
    //TRACE("instance = %04x, id = %04x, buffer = %08x, "
      //    "length = %d\n", instance, (int)resource_id, (int) buffer, buflen);

    /* Use bits 4 - 19 (incremented by 1) as resourceid, mask out
     * 20 - 31. */
    hrsrc = FindResourceW( instance, (LPCWSTR)(((resource_id>>4)&0xffff)+1),
                           RT_STRINGW );
    if (!hrsrc)
        return 0;
    hmem = LoadResource( instance, hrsrc );
    if (!hmem)
        return 0;

    p = (WCHAR*) LockResource(hmem);
    string_num = resource_id & 0x000f;
    for (i = 0; i < string_num; i++)
        p += *p + 1;

//    TRACE("strlen = %d\n", (int)*p );

    if (buffer == NULL)
        return *p;
    i = min(buflen - 1, *p);
    if (i > 0)
    {
        memcpy(buffer, p + 1, i * sizeof (WCHAR));
        buffer[i] = (WCHAR) 0;
    }
    else
    {
        if (buflen > 1)
        {
            buffer[0] = (WCHAR) 0;
            return 0;
        }
#if 0
        WARN("Dont know why caller give buflen=%d *p=%d trying to obtain string '%s'\n", buflen, *p, p + 1);
#endif
    }

//    TRACE("String loaded !\n");
    return i;
}

/* Messages...used by FormatMessage32* (KERNEL32.something)
 * 
 * They can be specified either directly or using a message ID and
 * loading them from the resource.
 * 
 * The resourcedata has following format:
 * start:
 * 0: DWORD nrofentries
 * nrofentries * subentry:
 *	0: DWORD firstentry
 *	4: DWORD lastentry
 *      8: DWORD offset from start to the stringentries
 *
 * (lastentry-firstentry) * stringentry:
 * 0: WORD len (0 marks end)
 * 2: WORD flags
 * 4: CHAR[len-4]
 * 	(stringentry i of a subentry refers to the ID 'firstentry+i')
 *
 * Yes, ANSI strings in win32 resources. Go figure.
 */

/**********************************************************************
 *	LoadMessageA		(internal)
 */
int __stdcall LoadMessageA( HMODULE instance, unsigned int id, WORD lang,
                         LPSTR buffer, int buflen )
{
    HGLOBAL	hmem;
    HRSRC	hrsrc;
    PMESSAGE_RESOURCE_DATA	mrd;
    PMESSAGE_RESOURCE_BLOCK	mrb;
    PMESSAGE_RESOURCE_ENTRY	mre;
    int		i,slen;

//    TRACE("instance = %08lx, id = %08lx, buffer = %p, length = %ld\n", (DWORD)instance, (DWORD)id, buffer, (DWORD)buflen);

    /*FIXME: I am not sure about the '1' ... But I've only seen those entries*/
    hrsrc = FindResourceExW(instance,RT_MESSAGELISTW,(LPWSTR)1,lang);
    if (!hrsrc)
        return 0;
    hmem = LoadResource( instance, hrsrc );
    if (!hmem)
        return 0;

    mrd = (PMESSAGE_RESOURCE_DATA)LockResource(hmem);
    mre = NULL;
    mrb = &(mrd->Blocks[0]);
    for (i=mrd->NumberOfBlocks;i--;)
    {
        if ((id>=mrb->LowId) && (id<=mrb->HighId))
        {
            mre = (PMESSAGE_RESOURCE_ENTRY)(((char*)mrd)+mrb->OffsetToEntries);
            id	-= mrb->LowId;
            break;
        }
        mrb++;
    }
    if (!mre)
        return 0;
    for (i=id;i--;)
    {
        if (!mre->Length)
            return 0;
        mre = (PMESSAGE_RESOURCE_ENTRY)(((char*)mre)+(mre->Length));
    }
    slen=mre->Length;
//    TRACE("	- strlen=%d\n",slen);
    i = min(buflen - 1, slen);
    if (buffer == NULL)
        return slen;
    if (i>0)
    {
        explstrcpynA(buffer,(char*)mre->Text,i);
        buffer[i]=0;
    }
    else
    {
        if (buflen>1)
        {
            buffer[0]=0;
            return 0;
        }
    }
//    if (buffer)
  //      TRACE("'%s' copied !\n", buffer);
    return i;
}



/**********************************************************************
 *	EnumResourceTypesA	(KERNEL32.90)
 */
BOOL __stdcall EnumResourceTypesA( HMODULE hmodule,ENUMRESTYPEPROCA lpfun,
                                    LONG lParam)
{
    /* FIXME: move WINE_MODREF stuff here */
    return PE_EnumResourceTypesA(hmodule,lpfun,lParam);
}

/**********************************************************************
 *	EnumResourceNamesA	(KERNEL32.88)
 */
BOOL __stdcall EnumResourceNamesA( HMODULE hmodule, LPCSTR type,
                                    ENUMRESNAMEPROCA lpfun, LONG lParam )
{
    /* FIXME: move WINE_MODREF stuff here */
    return PE_EnumResourceNamesA(hmodule,type,lpfun,lParam);
}
/**********************************************************************
 *	EnumResourceLanguagesA	(KERNEL32.86)
 */
BOOL __stdcall EnumResourceLanguagesA( HMODULE hmodule, LPCSTR type,
                                        LPCSTR name, ENUMRESLANGPROCA lpfun,
                                        LONG lParam)
{
    /* FIXME: move WINE_MODREF stuff here */
    return PE_EnumResourceLanguagesA(hmodule,type,name,lpfun,lParam);
}
/**********************************************************************
 *	    LoadResource     (KERNEL32.370)
 */
HGLOBAL __stdcall LoadResource( HINSTANCE hModule, HRSRC hRsrc )
{
    return RES_LoadResource( hModule, hRsrc);
}
