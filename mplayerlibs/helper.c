/*
 * XBoxMediaPlayer
 * Copyright (c) 2002 Frodo
 * Portions Copyright (c) by the authors of ffmpeg and xvid
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#define __WINDOWS32__
#undef USE_DVDREAD
#include <xtl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
	#include "cygwin_inttypes.h"

	#include "mp_msg.h"
	#include "libmpdemux/url.h"
	#include "libmpdemux/network.h"
	#include "libmpdemux/stream.h"
	#include "libmpdemux/demuxer.h"
	#include "libmpdemux/stheader.h"
	#include "libao2/audio_out.h"
	#include "libvo/video_out.h"
	#include "help.orig/help_mp-en.h"
	#include "cpudetect.h"
	#include "libao2/audio_plugin.h"
	#include "libmpcodecs/mp_image.h"
	#include "libmpcodecs/vf.h"
	#include "libmpcodecs/dec_video.h"
	#include "codec-cfg.h"
	#include "libao2/audio_out.h"
vo_functions_t video_out_md5;
vo_functions_t video_out_null;
vo_functions_t video_out_pgm;
vo_functions_t video_out_yuv4mpeg;
vo_functions_t video_out_mpegpes;
double	video_time_usage=0;
int			verbose=0;
int			subcc_enabled=0;
double vout_time_usage=0;


void * fast_memcpy(void * to, const void * from, size_t len)
{
		memcpy(to, from, len); // prior to mmx we use the standart memcpy
	return to;
}


void eprintf( const char *format, ... )
{
}


void mp_msg_c( int x, const char *format, ... )
{
}

void af_msg( int x, const char *format, ... )
{

    va_list va;
    char tmp[2048];
    
    va_start(va, format);
    _vsnprintf(tmp, 2048, format, va);
    va_end(va);
    tmp[2048-1] = 0;

		OutputDebugString(tmp);
}

void mp_msgff( int x, int lev,const char *format, ... )
{
}
void mp_msg( int x, int lev,const char *format, ... )
{
  va_list va;
  static char tmp[2048];
#ifndef _DEBUG
//	if (lev >= MSGL_HINT) return;
#endif
    
    va_start(va, format);
    _vsnprintf(tmp, 2048, format, va);
    va_end(va);
    tmp[2048-1] = 0;

	OutputDebugString(tmp);

}

void mp_dbg( int x, int lev,const char *format, ... )
{
//#if 0
#ifdef _DEBUG
  va_list va;
  static char tmp[2048];
    
  va_start(va, format);
  _vsnprintf(tmp, 2048, format, va);
  va_end(va);
  tmp[2048-1] = 0;

	OutputDebugString(tmp);
//#endif
#endif
}


void globfree(void *x)
{
	exit(0);
}
int	glob(const char *x, int xa, int xb)
{
	
	exit(0);
	return 00;
}

char *strsep(char **stringp, const char *delim) {
  char *begin, *end;

  begin = *stringp;
  if(begin == NULL)
    return NULL;

  if(delim[0] == '\0' || delim[1] == '\0') {
    char ch = delim[0];

    if(ch == '\0')
      end = NULL;
    else {
      if(*begin == ch)
        end = begin;
      else if(*begin == '\0')
        end = NULL;
      else
        end = strchr(begin + 1, ch);
    }
  }
  else
    end = strpbrk(begin, delim);

  if(end) {
    *end++ = '\0';
    *stringp = end;
  }
  else
    *stringp = NULL;
 
  return begin;
}

static unsigned long	RelativeTime=0;
FLOAT									m_fuSecsPerTick;
FLOAT									fLastTime = 0.0f;
LARGE_INTEGER         m_lStartTime;


// Returns current time in microseconds
unsigned long GetTimer()
{
	LARGE_INTEGER qwTime ;
	FLOAT					fTime;
	UINT64				uiQuadPart;
		__asm emms;

	QueryPerformanceCounter( &qwTime );
	qwTime.QuadPart -= m_lStartTime.QuadPart;
	uiQuadPart  =(UINT64)qwTime.QuadPart;
	uiQuadPart /= ((UINT64)10);  // prevent overflow after 4294.1 secs, now overflows after 42941 secs
	fTime = ((FLOAT)(uiQuadPart)) / m_fuSecsPerTick ;
	return (unsigned long)fTime;	
}  


// Returns current time in microseconds
float GetRelativeTime()
{
	unsigned long t,r;
		__asm emms;

  t=GetTimer();
  r=t-RelativeTime;
  RelativeTime=t;
  return (float)r * 0.000001F;
}

void InitTimer()
{
/*	int i;
	LARGE_INTEGER test;
	*/
  LARGE_INTEGER qwTicksPerSec;
	FLOAT t;
	__asm emms;
  QueryPerformanceFrequency( &qwTicksPerSec );   // ticks/sec
  m_fuSecsPerTick = (FLOAT)(((FLOAT)(qwTicksPerSec.QuadPart))  /1000.0);			 // tics/msec
	m_fuSecsPerTick = (FLOAT)(m_fuSecsPerTick/1000.0);			 // tics/usec
	m_fuSecsPerTick/=10.0;
	QueryPerformanceCounter( &m_lStartTime );
	/*
	Sleep(1000);
	QueryPerformanceCounter( &test );
	test.QuadPart -= m_lStartTime.QuadPart;
	for (i=0; i < 4280; i++) m_lStartTime.QuadPart -= test.QuadPart;
	*/
		t=GetRelativeTime();
}


int S_ISDIR(m)
{
	return (((m) & S_IFMT) == S_IFDIR);
}

void *av_malloc(int size)
{
    void *ptr;
//		size+=100; // allocate 100 bytes extra for memory overwrites
    ptr = malloc(size*2);
    if (!ptr)
        return NULL;
//fprintf(stderr, "%X %d\n", (int)ptr, size);
    /* NOTE: this memset should not be present */
    memset(ptr, 0, size);
    return ptr;
}

/* NOTE: ptr = NULL is explicetly allowed */
void av_free(void *ptr)
{
    /* XXX: this test should not be needed on most libcs */
    if (ptr)
        free(ptr);
}

void* av_realloc(void *ptr, size_t iLength)
{
	void* pPtr;
	pPtr=realloc(ptr,iLength);
	return pPtr;
}

//***********************************************************************************************
//***********************************************************************************************
//***********************************************************************************************
#define ENOEXEC         8

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
    //slwait (&g_sl);
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
    //slrelease (&g_sl);
    return ptr;
}



//***********************************************************************************************
long munmap (void *ptr, long size) 
{
    static long g_pagesize;
    static long g_regionsize;
    int rc = 0;//MUNMAP_FAILURE;
    /* Wait for spin lock */
    //slwait (&g_sl);
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
    //slrelease (&g_sl);
    return rc;
}