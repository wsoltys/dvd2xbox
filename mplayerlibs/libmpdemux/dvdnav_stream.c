#include "config.h"

#ifdef USE_DVDNAV
#include "../libmpdvdkit2/dvdnav_internal.h"
#include <stdlib.h>
#include <stdio.h>
#ifndef _XBOX
#include <unistd.h>
#include "../linux/timer.h"
#include "../input/input.h"
#endif
#include <string.h>
#include "mp_msg.h"
#include "stream.h"
#include "dvdnav_stream.h"
#include "../libvo/video_out.h"
#include "../spudec.h"
#include "../libmpdemux/mp_cmd.h"

int dvd_nav_skip_opening=0;     /* skip opening stalls? */
int osd_show_dvd_nav_delay=0;   /* count down for dvd nav text on OSD */
char dvd_nav_text[50];          /* for reporting stuff to OSD */
int osd_show_dvd_nav_highlight; /* show highlight area */
int osd_show_dvd_nav_sx;        /* start x .... */
int osd_show_dvd_nav_ex;
int osd_show_dvd_nav_sy;
int osd_show_dvd_nav_ey;
int dvd_nav_still=0;            /* are we on a still picture? */

extern unsigned long		GetTimer();

dvdnav_priv_t * new_dvdnav_stream(char * filename)
{
    char * title_str;
    dvdnav_priv_t *dvdnav_priv;

    if (!filename)
        return NULL;

    if (!(dvdnav_priv=(dvdnav_priv_t*)calloc(1,sizeof(*dvdnav_priv))))
        return NULL;

    if (!(dvdnav_priv->filename=strdup(filename)))
    {
        free(dvdnav_priv);
        return NULL;
    }

    if(dvdnav_open(&(dvdnav_priv->dvdnav),dvdnav_priv->filename)!=DVDNAV_STATUS_OK)
    {
        free(dvdnav_priv->filename);
        free(dvdnav_priv);
        return NULL;
    }

    if (!dvdnav_priv->dvdnav)
    {
        free(dvdnav_priv);
        return NULL;
    }

    dvdnav_stream_ignore_timers(dvdnav_priv,dvd_nav_skip_opening);

    /* turn on dvdnav caching */
    dvdnav_set_readahead_flag(dvdnav_priv->dvdnav,1);

    /* report the title?! */
    if (dvdnav_get_title_string(dvdnav_priv->dvdnav,&title_str)==DVDNAV_STATUS_OK)
    {
        mp_msg(MSGT_OPEN,MSGL_INFO,"Title: '%s'\n",title_str);
    }

    return dvdnav_priv;
}

int dvdnav_stream_reset(dvdnav_priv_t * dvdnav_priv)
{
    if (!dvdnav_priv)
        return 0;

    //  if (dvdnav_reset(dvdnav_priv->dvdnav)!=DVDNAV_STATUS_OK)
    return 0;

    dvdnav_priv->started=0;

    return 1;
}

void free_dvdnav_stream(dvdnav_priv_t * dvdnav_priv)
{
    if (!dvdnav_priv)
        return;

    dvdnav_close(dvdnav_priv->dvdnav);
    dvdnav_priv->dvdnav=NULL;
    free(dvdnav_priv);
}

void dvdnav_stream_ignore_timers(dvdnav_priv_t * dvdnav_priv, int ignore)
{
    if (!dvdnav_priv)
        return;

    dvdnav_priv->ignore_timers=ignore;
}

int dvdnav_stream_sleeping(dvdnav_priv_t * dvdnav_priv)
{
    unsigned int now;

    if (!dvdnav_priv)
        return 0;

    if(dvdnav_priv->sleeping)
    {
        now=GetTimer();
        while(dvdnav_priv->sleeping>1 || now<dvdnav_priv->sleep_until)
        {
            //        mp_msg(0,0,"%s %u<%u\n",__FUNCTION__,now,dvdnav_priv->sleep_until);
            //        usec_sleep(1000); /* 1ms granularity */
            return 1;
        }
        dvdnav_still_skip(dvdnav_priv->dvdnav); // continue past...
        dvdnav_priv->sleeping=0;
        mp_msg(0,0,"%s: woke up!\n",__FUNCTION__);
    }
    dvd_nav_still=0;
    mp_msg(0,0,"%s: active\n",__FUNCTION__);
    return 0;
}

void dvdnav_stream_sleep(dvdnav_priv_t * dvdnav_priv, int seconds)
{
    if (!dvdnav_priv)
        return;

    if (!dvdnav_priv->started)
        return;

    dvdnav_priv->sleeping=0;
    switch (seconds)
    {
    case 0:
        return;
    case 0xff:
        mp_msg(0,0, "Sleeping indefinately\n" );
        dvdnav_priv->sleeping=2;
        break;
    default:
        mp_msg(0,0, "Sleeping %d sec(s)\n", seconds );
        dvdnav_priv->sleep_until = GetTimer();// + seconds*1000000;
        dvdnav_priv->sleeping=1;
        break;
    }
    //if (dvdnav_priv->started) dvd_nav_still=1;
}

void dvdnav_stream_add_event(dvdnav_priv_t* dvdnav_priv, int event, unsigned char *buf, int len)
{
    //mp_msg(0,0,"%s: %d\n",__FUNCTION__,event);

    dvdnav_event_t * dvdnav_event;
    mp_cmd_t * cmd;

    if (!dvdnav_priv->started)
        return;

    if (!(dvdnav_event=(dvdnav_event_t*)calloc(1,sizeof(*dvdnav_event))))
    {
        mp_msg(0,0,"%s: dvdnav_event: out of memory!\n",__FUNCTION__);
        return;
    }
    dvdnav_event->event=event;
    dvdnav_event->details=calloc(1,len);
    memcpy(dvdnav_event->details,buf,len);
    dvdnav_event->len=len;

    if (!(cmd = (mp_cmd_t*)calloc(1,sizeof(*cmd))))
    {
        mp_msg(0,0,"%s: mp_cmd_t: out of memory!\n",__FUNCTION__);
        free(dvdnav_event->details);
        free(dvdnav_event);
        return;
    }
    cmd->id=MP_CMD_DVDNAV_EVENT; // S+event;
    cmd->name=strdup("dvdnav_event"); // FIXME: do I really need a 'name'?
    cmd->nargs=1;
    cmd->args[0].v.v=dvdnav_event;

    mp_input_queue_cmd(cmd);
}

void dvdnav_stream_read(dvdnav_priv_t * dvdnav_priv, unsigned char *buf, int *len)
{
    int event = DVDNAV_NOP;

    if (!len)
        return;
    *len=-1;
    if (!dvdnav_priv)
        return;
    if (!buf)
        return;

    if (dvd_nav_still)
    {
        mp_msg(0,0,"%s: got a stream_read while I should be asleep!\n",__FUNCTION__);
        *len=0;
        return;
    }

    if (dvdnav_get_next_block(dvdnav_priv->dvdnav,buf,&event,len)!=DVDNAV_STATUS_OK)
    {
        mp_msg(0,0, "Error getting next block from DVD (%s)\n",dvdnav_err_to_string(dvdnav_priv->dvdnav) );
        *len=-1;
    }
    else if (event!=DVDNAV_BLOCK_OK)
    {

        // need to handle certain events internally (like skipping stills)
        switch (event)
        {
        case DVDNAV_STILL_FRAME:
            {
                dvdnav_still_event_t *still_event = (dvdnav_still_event_t*)(buf);

                mp_msg(0,0,"************************************************************ STILL \n");

                //if (dvdnav_priv->started) dvd_nav_still=1;
                //else
                dvdnav_still_skip(dvdnav_priv->dvdnav); // don't let dvdnav stall on this image

                break;
            }
        }

        // got an event, repeat the read
        dvdnav_stream_add_event(dvdnav_priv,event,buf,*len);
        *len=0;
    }
    //  mp_msg(0,0,"%s: got %d\n",__FUNCTION__,*len);
}

void dvdnav_stream_fullstart(dvdnav_priv_t * dvdnav_priv)
{
    if (dvdnav_priv && !dvdnav_priv->started)
    {
        dvdnav_stream_reset(dvdnav_priv);
        dvdnav_priv->started=1;
    }
}

unsigned int * dvdnav_stream_get_palette(dvdnav_priv_t * dvdnav_priv)
{
    if (!dvdnav_priv)
    {
        mp_msg(0,0,"%s: NULL dvdnav_priv\n",__FUNCTION__);
        return NULL;
    }
    if (!dvdnav_priv->dvdnav)
    {
        mp_msg(0,0,"%s: NULL dvdnav_priv->dvdnav\n",__FUNCTION__);
        return NULL;
    }
    if (!dvdnav_priv->dvdnav->vm)
    {
        mp_msg(0,0,"%s: NULL dvdnav_priv->dvdnav->vm\n",__FUNCTION__);
        return NULL;
    }
    if (!dvdnav_priv->dvdnav->vm->state.pgc)
    {
        mp_msg(0,0,"%s: NULL dvdnav_priv->dvdnav->vm->state.pgc\n",__FUNCTION__);
        return NULL;
    }
    return dvdnav_priv->dvdnav->vm->state.pgc->palette;
}

unsigned int dvdnav_stream_dvdnav_menu_call(dvdnav_priv_t * dvdnav_priv, DVDMenuID_t menu)
{
	return dvdnav_menu_call(dvdnav_priv->dvdnav, menu);
}

#endif /* USE_DVDNAV */
