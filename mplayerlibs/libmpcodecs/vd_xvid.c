
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <sys/stat.h>
#include "../config.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "../cygwin_inttypes.h"
#include "../help.orig\help_mp-en.h"

#include "../mp_msg.h"
#include "../bswap.h"

#ifdef HAVE_XVID

#include "vd_internal.h"
#include "../cfgparser.h"
#include "../../lib/xvid.h"

static vd_info_t info = 
{
	"xvid decoder",
	"xvid",
	"Elcabesa",
	"Elcabesa",
	""
};

LIBVD_EXTERN(xvid)

typedef struct {
  int cs;
  unsigned char img_type;
  void* hdl;
  mp_image_t* mpi;
} priv_t;

static int do_dr2 = 0;

struct config xvid_dec_opts[] = {
  { "dr2", &do_dr2, CONF_TYPE_FLAG, 0, 0, 1, NULL},
  { "nodr2", &do_dr2, CONF_TYPE_FLAG, 0, 1, 0, NULL},
  {NULL, NULL, 0, 0, 0, 0, NULL}
};

// to set/get/query special features/parameters
static int control(sh_video_t *sh,int cmd,void* arg,...){
  return CONTROL_UNKNOWN;
}

// init driver
static int init(sh_video_t *sh){
  xvid_gbl_init_t xvid_ini;
  xvid_dec_create_t dec_p;
  priv_t* p;
  int cs;


  memset(&xvid_ini,0,sizeof(xvid_gbl_init_t));
  xvid_ini.version = XVID_VERSION;
  memset(&dec_p,0,sizeof(xvid_dec_create_t));
  dec_p.version = XVID_VERSION;

  if(!mpcodecs_config_vo(sh,sh->disp_w,sh->disp_h,IMGFMT_YV12))
    return 0;

  switch(sh->codec->outfmt[sh->outfmtidx]){
  case IMGFMT_YV12:
    cs= XVID_CSP_YV12;
    break;
  case IMGFMT_YUY2:
    cs=XVID_CSP_YUY2;
    break;
  case IMGFMT_UYVY:
    cs=XVID_CSP_UYVY;
    break;
  case IMGFMT_I420: 
  case IMGFMT_IYUV:
    cs=XVID_CSP_I420;
    break;
  case IMGFMT_BGR15: 
    cs=XVID_CSP_RGB555;
    break;
  case IMGFMT_BGR16: 
    cs=XVID_CSP_RGB565;
    break;
  case IMGFMT_BGR32:
    cs=XVID_CSP_BGR;
    break;
  case IMGFMT_YVYU:
    cs=XVID_CSP_YVYU;
    break;
  default:
    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"Unsupported out_fmt: 0x%X\n",sh->codec->outfmt[sh->outfmtidx]);
    return 0;
  }
  

  if(xvid_global(NULL, XVID_GBL_INIT,&xvid_ini, NULL))
    return 0;

  dec_p.width = sh->disp_w;
  dec_p.height =  sh->disp_h;

  if(xvid_decore(0, XVID_DEC_CREATE, &dec_p, NULL)<0) {
    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"xvid init failed\n");
    return 0;
  }

  p = (priv_t*)malloc(sizeof(priv_t));
  p->cs = cs;
  p->hdl = dec_p.handle;
  sh->context = p;

  
  
    p->img_type = MP_IMGTYPE_TEMP;
  
  

  return 1;
}

// uninit driver
static void uninit(sh_video_t *sh){
  priv_t* p = sh->context;
  if(!p)
    return;
  xvid_decore(p->hdl,XVID_DEC_DESTROY, NULL, NULL);
  free(p);
}

// decode a frame
static mp_image_t* decode(sh_video_t *sh,void* data,int len,int flags){
  xvid_dec_frame_t dec;
  priv_t* p = sh->context;
  
 

  mp_image_t* mpi = mpcodecs_get_image(sh,  p->img_type,
				       MP_IMGFLAG_ACCEPT_STRIDE,
				       sh->disp_w,sh->disp_h);

			
  if(!data || !mpi || len <= 0)
    return NULL;
  
    memset(&dec,0,sizeof(xvid_dec_frame_t));
  dec.version = XVID_VERSION;
  
 

  dec.bitstream = data;
  dec.length = len;

  dec.general |= XVID_LOWDELAY;

 dec.output.csp=p->cs;   
 
 dec.output.plane[0]=mpi->planes[0];
   dec.output.plane[1]= mpi->planes[1];  
  dec.output.plane[2]= mpi->planes[2];
  
  dec.output.stride[0]=mpi->stride[0]; 
  dec.output.stride[1]=mpi->stride[1]; 
   dec.output.stride[2]=mpi->stride[2]; 
  
        
  if(xvid_decore(p->hdl,XVID_DEC_DECODE,&dec,NULL)<0) {
    mp_msg(MSGT_DECVIDEO,MSGL_ERR,"decoding error\n");
    return NULL;
  }
   

    
  

  return mpi;
}
#endif  //have_xvid
