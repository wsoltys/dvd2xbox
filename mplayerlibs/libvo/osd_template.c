// Generic alpha renderers for all YUV modes and RGB depths.
// Optimized by Nick and Michael
// Code from Michael Niedermayer (michaelni@gmx.at) is under GPL

#undef PREFETCH
#undef EMMS
#undef PREFETCHW
#undef PAVGB
#pragma warning (disable : 4244)
#pragma warning (disable : 4305)
#pragma warning (disable : 4018)

static  void vo_draw_alpha_yv12_C(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
		if (!UseMPlayerForRendering()) return;
#if defined(FAST_OSD) && !defined(HAVE_MMX)
    w=w>>1;
#endif
    //PROFILE_START();
    for(y=0;y<h;y++)
    {
        register int x;
for(x=0;x<w;x++)
        {
#ifdef FAST_OSD
            if(srca[2*x+0])
                dstbase[2*x+0]=src[2*x+0];
            if(srca[2*x+1])
                dstbase[2*x+1]=src[2*x+1];
#else
if(srca[x])
        dstbase[x]=((dstbase[x]*srca[x])>>8)+src[x];
#endif
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    //PROFILE_END("vo_draw_alpha_yv12");
    return;
}

static  void vo_draw_alpha_yuy2_C(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
		
		if (!UseMPlayerForRendering()) return;
#if defined(FAST_OSD) && !defined(HAVE_MMX)
    w=w>>1;
#endif
    //PROFILE_START();
    for(y=0;y<h;y++)
    {
        register int x;
for(x=0;x<w;x++)
        {
#ifdef FAST_OSD
            if(srca[2*x+0])
                dstbase[4*x+0]=src[2*x+0];
            if(srca[2*x+1])
                dstbase[4*x+2]=src[2*x+1];
#else
if(srca[x])
        {
            dstbase[2*x]=((dstbase[2*x]*srca[x])>>8)+src[x];
            dstbase[2*x+1]=((((signed)dstbase[2*x+1]-128)*srca[x])>>8)+128;
        }
#endif
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    //PROFILE_END("vo_draw_alpha_yuy2");
    return;
}

static  void vo_draw_alpha_rgb24_C(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
		
		if (!UseMPlayerForRendering()) return;
    for(y=0;y<h;y++)
    {
        register unsigned char *dst = dstbase;
        register int x;

for(x=0;x<w;x++)
        {
            if(srca[x])
            {
#ifdef FAST_OSD
                dst[0]=dst[1]=dst[2]=src[x];
#else
dst[0]=((dst[0]*srca[x])>>8)+src[x];
        dst[1]=((dst[1]*srca[x])>>8)+src[x];
        dst[2]=((dst[2]*srca[x])>>8)+src[x];
#endif
            }
            dst+=3; // 24bpp
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    return;
}

static  void vo_draw_alpha_rgb32_C(int w,int h, unsigned char* src, unsigned char *srca, int srcstride, unsigned char* dstbase,int dststride)
{
    int y;
    
		if (!UseMPlayerForRendering()) return;
    for(y=0;y<h;y++)
    {
        register int x;
for(x=0;x<w;x++)
        {
            if(srca[x])
            {
#ifdef FAST_OSD
                dstbase[4*x+0]=dstbase[4*x+1]=dstbase[4*x+2]=src[x];
#else
dstbase[4*x+0]=((dstbase[4*x+0]*srca[x])>>8)+src[x];
        dstbase[4*x+1]=((dstbase[4*x+1]*srca[x])>>8)+src[x];
        dstbase[4*x+2]=((dstbase[4*x+2]*srca[x])>>8)+src[x];
#endif
            }
        }
        src+=srcstride;
        srca+=srcstride;
        dstbase+=dststride;
    }
    //PROFILE_END("vo_draw_alpha_rgb32");
    return;
}
