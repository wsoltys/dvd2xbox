

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <sys/stat.h>
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "../cygwin_inttypes.h"
#include "../mp_msg.h"
#include "../libmpdemux/url.h"
#include "../libmpdemux/network.h"
#include "../libmpdemux/stream.h"
#include "../libmpdemux/demuxer.h"
#include "../libmpdemux/stheader.h"
#include "../libmpdemux/aviheader.h"
#include "../version.h"
#include "../libmpdemux/aviwrite.h"
#include "../help.orig\help_mp-en.h"
#include "../m_option.h"
#include "../m_struct.h"

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

static struct vf_priv_s
{
    unsigned int fmt;
}
vf_priv_dflt = {
                   IMGFMT_YUY2
               };

//===========================================================================//

static int query_format(struct vf_instance_s* vf, unsigned int fmt)
{
    if(fmt==vf->priv->fmt)
        return vf_next_query_format(vf,fmt);
    return 0;
}

static int open(vf_instance_t *vf, char* args)
{
    vf->query_format=query_format;
    vf->default_caps=0;
    if(!vf->priv)
    {
        vf->priv=malloc(sizeof(struct vf_priv_s));
        vf->priv->fmt=IMGFMT_YUY2;
    }
    if(args)
    {
        if(!strcmp(args,"444p"))
            vf->priv->fmt=IMGFMT_444P;
        else
            if(!strcmp(args,"422p"))
                vf->priv->fmt=IMGFMT_422P;
            else
                if(!strcmp(args,"411p"))
                    vf->priv->fmt=IMGFMT_411P;
                else
                    if(!strcmp(args,"yuy2"))
                        vf->priv->fmt=IMGFMT_YUY2;
                    else
                        if(!strcmp(args,"yv12"))
                            vf->priv->fmt=IMGFMT_YV12;
                        else
                            if(!strcmp(args,"i420"))
                                vf->priv->fmt=IMGFMT_I420;
                            else
                                if(!strcmp(args,"yvu9"))
                                    vf->priv->fmt=IMGFMT_YVU9;
                                else
                                    if(!strcmp(args,"if09"))
                                        vf->priv->fmt=IMGFMT_IF09;
                                    else
                                        if(!strcmp(args,"iyuv"))
                                            vf->priv->fmt=IMGFMT_IYUV;
                                        else
                                            if(!strcmp(args,"uyvy"))
                                                vf->priv->fmt=IMGFMT_UYVY;
                                            else
                                                if(!strcmp(args,"bgr24"))
                                                    vf->priv->fmt=IMGFMT_BGR24;
                                                else
                                                    if(!strcmp(args,"bgr32"))
                                                        vf->priv->fmt=IMGFMT_BGR32;
                                                    else
                                                        if(!strcmp(args,"bgr16"))
                                                            vf->priv->fmt=IMGFMT_BGR16;
                                                        else
                                                            if(!strcmp(args,"bgr15"))
                                                                vf->priv->fmt=IMGFMT_BGR15;
                                                            else
                                                                if(!strcmp(args,"bgr8"))
                                                                    vf->priv->fmt=IMGFMT_BGR8;
                                                                else
                                                                    if(!strcmp(args,"bgr4"))
                                                                        vf->priv->fmt=IMGFMT_BGR4;
                                                                    else
                                                                        if(!strcmp(args,"bgr1"))
                                                                            vf->priv->fmt=IMGFMT_BGR1;
                                                                        else
                                                                            if(!strcmp(args,"rgb24"))
                                                                                vf->priv->fmt=IMGFMT_RGB24;
                                                                            else
                                                                                if(!strcmp(args,"rgb32"))
                                                                                    vf->priv->fmt=IMGFMT_RGB32;
                                                                                else
                                                                                    if(!strcmp(args,"rgb16"))
                                                                                        vf->priv->fmt=IMGFMT_RGB16;
                                                                                    else
                                                                                        if(!strcmp(args,"rgb15"))
                                                                                            vf->priv->fmt=IMGFMT_RGB15;
                                                                                        else
                                                                                            if(!strcmp(args,"rgb8"))
                                                                                                vf->priv->fmt=IMGFMT_RGB8;
                                                                                            else
                                                                                                if(!strcmp(args,"rgb4"))
                                                                                                    vf->priv->fmt=IMGFMT_RGB4;
                                                                                                else
                                                                                                    if(!strcmp(args,"rgb1"))
                                                                                                        vf->priv->fmt=IMGFMT_RGB1;
                                                                                                    else
                                                                                                    {
                                                                                                        mp_msg(0,0,"Unknown format name: '%s'\n",args);
                                                                                                        return 0;
                                                                                                    }
    }


    return 1;
}

#define ST_OFF(f) M_ST_OFF(struct vf_priv_s,f)
static m_option_t vf_opts_fields[] = {
                                         {"fmt", ST_OFF(fmt), CONF_TYPE_IMGFMT, 0,0 ,0, NULL},
                                         { NULL, NULL, 0, 0, 0, 0,  NULL }
                                     };

static m_struct_t vf_opts = {
                                "format",
                                sizeof(struct vf_priv_s),
                                &vf_priv_dflt,
                                vf_opts_fields
                            };

vf_info_t vf_info_format = {
                               "force output format",
                               "format",
                               "A'rpi",
                               "FIXME! get_image()/put_image()",
                               open,
                               &vf_opts
                           };

//===========================================================================//
