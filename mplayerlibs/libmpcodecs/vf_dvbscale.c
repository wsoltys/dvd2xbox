

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <sys/stat.h>
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>

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

#include "img_format.h"
#include "mp_image.h"
#include "vf.h"

struct vf_priv_s
{
    int aspect;
};

//===========================================================================//

static int config(struct vf_instance_s* vf,
                  int width, int height, int d_width, int d_height,
                  unsigned int flags, unsigned int outfmt)
{

    int scaled_y=vf->priv->aspect*d_height/d_width;

    d_width=width; // do X-scaling by hardware
    d_height=scaled_y;

    return vf_next_config(vf,width,height,d_width,d_height,flags,outfmt);
}

static int open(vf_instance_t *vf, char* args)
{
    vf->config=config;
    vf->default_caps=0;
    vf->priv=malloc(sizeof(struct vf_priv_s));
    vf->priv->aspect=768;
    if(args)
        vf->priv->aspect=atoi(args);
    return 1;
}

vf_info_t vf_info_dvbscale = {
                                 "calc Y scaling for DVB card",
                                 "dvbscale",
                                 "A'rpi",
                                 "",
                                 open,
                                 NULL
                             };

//===========================================================================//
