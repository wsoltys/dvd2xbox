 // Generic alpha renderers for all YUV modes and RGB depths.
// These are "reference implementations", should be optimized later (MMX, etc)
// Templating Code from Michael Niedermayer (michaelni@gmx.at) is under GPL

//#define FAST_OSD
//#define FAST_OSD_TABLE


#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "../cygwin_inttypes.h"
#include "../mp_msg.h"
#include "osd.h"

//#define ENABLE_PROFILE

extern int verbose; // defined in mplayer.c

#define COMPILE_C
#undef HAVE_MMX
#undef HAVE_MMX2
#undef HAVE_3DNOW
#undef ARCH_X86

#include "osd_template.c"


extern BOOL UseMPlayerForRendering();
void vo_draw_alpha_yv12(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride){

		if (!UseMPlayerForRendering()) return;
#ifdef RUNTIME_CPUDETECT
#ifdef CAN_COMPILE_X86_ASM
    // ordered per speed fasterst first
    if(gCpuCaps.hasMMX2)
        vo_draw_alpha_yv12_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.has3DNow)
        vo_draw_alpha_yv12_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.hasMMX)
        vo_draw_alpha_yv12_MMX(w, h, src, srca, srcstride, dstbase, dststride);
    else
        vo_draw_alpha_yv12_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
vo_draw_alpha_yv12_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#else //RUNTIME_CPUDETECT
#ifdef HAVE_MMX2
vo_draw_alpha_yv12_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_3DNOW)
vo_draw_alpha_yv12_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_MMX)
vo_draw_alpha_yv12_MMX(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (ARCH_X86)
vo_draw_alpha_yv12_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
vo_draw_alpha_yv12_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#endif //!RUNTIME_CPUDETECT
}

void vo_draw_alpha_yuy2(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
	
		if (!UseMPlayerForRendering()) return;
#ifdef RUNTIME_CPUDETECT
#ifdef CAN_COMPILE_X86_ASM
    // ordered per speed fasterst first
    if(gCpuCaps.hasMMX2)
        vo_draw_alpha_yuy2_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.has3DNow)
        vo_draw_alpha_yuy2_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.hasMMX)
        vo_draw_alpha_yuy2_MMX(w, h, src, srca, srcstride, dstbase, dststride);
    else
        vo_draw_alpha_yuy2_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
vo_draw_alpha_yuy2_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#else //RUNTIME_CPUDETECT
#ifdef HAVE_MMX2
    vo_draw_alpha_yuy2_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_3DNOW)
vo_draw_alpha_yuy2_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_MMX)
vo_draw_alpha_yuy2_MMX(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (ARCH_X86)
vo_draw_alpha_yuy2_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
    vo_draw_alpha_yuy2_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#endif //!RUNTIME_CPUDETECT
}

void vo_draw_alpha_rgb24(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
	
		if (!UseMPlayerForRendering()) return;
#ifdef RUNTIME_CPUDETECT
#ifdef CAN_COMPILE_X86_ASM
    // ordered per speed fasterst first
    if(gCpuCaps.hasMMX2)
        vo_draw_alpha_rgb24_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.has3DNow)
        vo_draw_alpha_rgb24_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
    else if(gCpuCaps.hasMMX)
        vo_draw_alpha_rgb24_MMX(w, h, src, srca, srcstride, dstbase, dststride);
    else
        vo_draw_alpha_rgb24_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
    vo_draw_alpha_rgb24_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#else //RUNTIME_CPUDETECT
#ifdef HAVE_MMX2
    vo_draw_alpha_rgb24_MMX2(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_3DNOW)
    vo_draw_alpha_rgb24_3DNow(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (HAVE_MMX)
vo_draw_alpha_rgb24_MMX(w, h, src, srca, srcstride, dstbase, dststride);
#elif defined (ARCH_X86)
vo_draw_alpha_rgb24_X86(w, h, src, srca, srcstride, dstbase, dststride);
#else
vo_draw_alpha_rgb24_C(w, h, src, srca, srcstride, dstbase, dststride);
#endif
#endif //!RUNTIME_CPUDETECT
}

void vo_draw_alpha_rgb32(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
	
		if (!UseMPlayerForRendering()) return;
	vo_draw_alpha_rgb32_C(w, h, src, srca, srcstride, dstbase, dststride);
}

#ifdef FAST_OSD_TABLE
static unsigned short fast_osd_15bpp_table[256];
static unsigned short fast_osd_16bpp_table[256];
#endif

void vo_draw_alpha_init()
{
#ifdef FAST_OSD_TABLE
    int i;
    for(i=0;i<256;i++)
    {
        fast_osd_15bpp_table[i]=((i>>3)<<10)|((i>>3)<<5)|(i>>3);
        fast_osd_16bpp_table[i]=((i>>3)<<11)|((i>>2)<<5)|(i>>3);
    }
#endif
    //FIXME the optimized stuff is a lie for 15/16bpp as they arent optimized yet
    if(verbose)
    {
#ifdef RUNTIME_CPUDETECT
#ifdef CAN_COMPILE_X86_ASM
        // ordered per speed fasterst first
        if(gCpuCaps.hasMMX2)
            mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX (with tiny bit MMX2) Optimized OnScreenDisplay\n");
        else if(gCpuCaps.has3DNow)
            mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX (with tiny bit 3DNow) Optimized OnScreenDisplay\n");
        else if(gCpuCaps.hasMMX)
            mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX Optimized OnScreenDisplay\n");
        else
            mp_msg(MSGT_OSD,MSGL_INFO,"Using X86 Optimized OnScreenDisplay\n");
#else
        mp_msg(MSGT_OSD,MSGL_INFO,"Using Unoptimized OnScreenDisplay\n");
#endif
#else //RUNTIME_CPUDETECT
#ifdef HAVE_MMX2
        mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX (with tiny bit MMX2) Optimized OnScreenDisplay\n");
#elif defined (HAVE_3DNOW)
mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX (with tiny bit 3DNow) Optimized OnScreenDisplay\n");
#elif defined (HAVE_MMX)
mp_msg(MSGT_OSD,MSGL_INFO,"Using MMX Optimized OnScreenDisplay\n");
#elif defined (ARCH_X86)
mp_msg(MSGT_OSD,MSGL_INFO,"Using X86 Optimized OnScreenDisplay\n");
#else
        mp_msg(MSGT_OSD,MSGL_INFO,"Using Unoptimized OnScreenDisplay\n");
#endif
#endif //!RUNTIME_CPUDETECT
    }
}

void vo_draw_alpha_rgb15(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
		
		if (!UseMPlayerForRendering()) return;
    for(y=0;y<h;y++)
    {
        register unsigned short *dst = (unsigned short*) dstbase;
        register int x;
        for(x=0;x<w;x++)
        {
            if(srca[x])
            {
#ifdef FAST_OSD
#ifdef FAST_OSD_TABLE
                dst[x]=fast_osd_15bpp_table[src[x]];
#else
register unsigned int a=src[x]>>3;
                dst[x]=(a<<10)|(a<<5)|a;
#endif
#else
                unsigned char r=dst[x]&0x1F;
                unsigned char g=(dst[x]>>5)&0x1F;
                unsigned char b=(dst[x]>>10)&0x1F;
                r=(((r*srca[x])>>5)+src[x])>>3;
                g=(((g*srca[x])>>5)+src[x])>>3;
                b=(((b*srca[x])>>5)+src[x])>>3;
                dst[x]=(b<<10)|(g<<5)|r;
#endif
            }
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    return;
}

void vo_draw_alpha_rgb16(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
		
		if (!UseMPlayerForRendering()) return;
    for(y=0;y<h;y++)
    {
        register unsigned short *dst = (unsigned short*) dstbase;
        register int x;
        for(x=0;x<w;x++)
        {
            if(srca[x])
            {
#ifdef FAST_OSD
#ifdef FAST_OSD_TABLE
                dst[x]=fast_osd_16bpp_table[src[x]];
#else
dst[x]=((src[x]>>3)<<11)|((src[x]>>2)<<5)|(src[x]>>3);
#endif
#else
unsigned char r=dst[x]&0x1F;
                unsigned char g=(dst[x]>>5)&0x3F;
                unsigned char b=(dst[x]>>11)&0x1F;
                r=(((r*srca[x])>>5)+src[x])>>3;
                g=(((g*srca[x])>>6)+src[x])>>2;
                b=(((b*srca[x])>>5)+src[x])>>3;
                dst[x]=(b<<11)|(g<<5)|r;
#endif
            }
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    return;
}

