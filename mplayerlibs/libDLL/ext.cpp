#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
/***********************************************************************
 *           FILE_dommap
 */

//#define MAP_PRIVATE
//#define MAP_SHARED
#undef MAP_ANON
LPVOID FILE_dommap( int unix_handle, LPVOID start,
                    DWORD size_high, DWORD size_low,
                    DWORD offset_high, DWORD offset_low,
                    int prot, int flags )
{
    int fd = -1;
    int pos;
    LPVOID ret;

    if (size_high || offset_high)
        printf("offsets larger than 4Gb not supported\n");

    if (unix_handle == -1)
    {
#ifdef MAP_ANON
        //	printf("Anonymous\n");
        flags |= MAP_ANON;
#else
        static int fdzero = -1;

        if (fdzero == -1)
        {
            if ((fdzero = open( "/dev/zero", O_RDONLY )) == -1)
            {
                perror( "Cannot open /dev/zero for READ. Check permissions! error: " );
                exit(1);
            }
        }
        fd = fdzero;
#endif  /* MAP_ANON */
        /* Linux EINVAL's on us if we don't pass MAP_PRIVATE to an anon mmap */
#ifdef MAP_SHARED
        flags &= ~MAP_SHARED;
#endif
#ifdef MAP_PRIVATE
        flags |= MAP_PRIVATE;
#endif
    }
    else
        fd = unix_handle;
    //    printf("fd %x, start %x, size %x, pos %x, prot %x\n",fd,start,size_low, offset_low, prot);
    //    if ((ret = mmap( start, size_low, prot,
    //                     flags, fd, offset_low )) != (LPVOID)-1)
    if ((ret = mmap( start, size_low, prot,
                     MAP_PRIVATE | MAP_FIXED, fd, offset_low )) != (LPVOID)-1)
    {
        //	    printf("address %08x\n", *(int*)ret);
        //	printf("%x\n", ret);
        return ret;
    }

    //    printf("mmap %d\n", errno);

    /* mmap() failed; if this is because the file offset is not    */
    /* page-aligned (EINVAL), or because the underlying filesystem */
    /* does not support mmap() (ENOEXEC), we do it by hand.        */

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
    /*    printf( "FILE_mmap: mmap failed (%d), faking it\n", errno );*/
    /* Reserve the memory with an anonymous mmap */
    ret = FILE_dommap( -1, start, size_high, size_low, 0, 0,
                       PROT_READ | PROT_WRITE, flags );
    if (ret == (LPVOID)-1)
        //    {
        //	perror(
        return ret;
    /* Now read in the file */
    if ((pos = lseek( fd, offset_low, SEEK_SET )) == -1)
    {
        FILE_munmap( ret, size_high, size_low );
        //	printf("lseek\n");
        return (LPVOID)-1;
    }
    read( fd, ret, size_low );
    lseek( fd, pos, SEEK_SET );  /* Restore the file pointer */
    mprotect( ret, size_low, prot );  /* Set the right protection */
    //    printf("address %08x\n", *(int*)ret);
    return ret;
}


/***********************************************************************
 *           FILE_munmap
 */
int FILE_munmap( LPVOID start, DWORD size_high, DWORD size_low )
{
    if (size_high)
        printf("offsets larger than 4Gb not supported\n");
    return munmap( start, size_low );
}
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



#define	PAGE_NOACCESS		0x01
#define	PAGE_READONLY		0x02
#define	PAGE_READWRITE		0x04
#define	PAGE_WRITECOPY		0x08
#define	PAGE_EXECUTE		0x10
#define	PAGE_EXECUTE_READ	0x20
#define	PAGE_EXECUTE_READWRITE	0x40
#define	PAGE_EXECUTE_WRITECOPY	0x80
#define	PAGE_GUARD		0x100
#define	PAGE_NOCACHE		0x200

HANDLE WINAPI CreateFileMappingA(HANDLE handle, LPSECURITY_ATTRIBUTES lpAttr,
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
        hFile=open("/dev/zero", O_RDWR);
        if(hFile<0)
        {
            perror( "Cannot open /dev/zero for READ+WRITE. Check permissions! error: " );
            return 0;
        }
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
WIN_BOOL WINAPI UnmapViewOfFile(LPVOID handle)
{
    file_mapping* p;
    int result;
    if(fm==0)
        return 0;
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