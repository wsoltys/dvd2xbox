#include <limits.h>
/* SPUdec.c
   Skeleton of function spudec_process_controll() is from xine sources.
   Further works:
   LGB,... (yeah, try to improve it and insert your name here! ;-)

   Kim Minh Kaplan
   implement fragments reassembly, RLE decoding.
   read brightness from the IFO.

   For information on SPU format see <URL:http://sam.zoy.org/doc/dvd/subtitles/>
   and <URL:http://members.aol.com/mpucoder/DVD/spu.html>

 */
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "cygwin_inttypes.h"
#include "help.orig/help_mp-en.h"

#include "mp_msg.h"
#if ANTIALIASING_ALGORITHM == 2
#include <math.h>
#endif
#include "libvo/video_out.h"
#include "spudec.h"
#include "postproc/swscale.h"

#pragma warning (disable:4018)
#pragma warning (disable:4244)
#pragma warning (disable:4305)

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif


int spu_aamode = 3;
int spu_alignment = 0;
float spu_gaussvar = 1.0;
extern int sub_pos;

typedef struct packet_t packet_t;
struct packet_t
{
    unsigned char *packet;
    unsigned int palette[4];
    unsigned int alpha[4];
    unsigned int control_start;	/* index of start of control data */
    unsigned int current_nibble[2]; /* next data nibble (4 bits) to be
                                         processed (for RLE decoding) for
                                         even and odd lines */
    int deinterlace_oddness;	/* 0 or 1, index into current_nibble */
    unsigned int start_col, end_col;
    unsigned int start_row, end_row;
    unsigned int width, height, stride;
    unsigned int start_pts, end_pts;
    packet_t *next;
};

typedef struct
{
    packet_t *queue_head;
    packet_t *queue_tail;
    unsigned int global_palette[16];
    unsigned int orig_frame_width, orig_frame_height;
    unsigned char* packet;
    size_t packet_reserve;	/* size of the memory pointed to by packet */
    unsigned int packet_offset;	/* end of the currently assembled fragment */
    unsigned int packet_size;	/* size of the packet once all fragments are assembled */
    unsigned int packet_pts;	/* PTS for this packet */
    unsigned int palette[4];
    unsigned int alpha[4];
    unsigned int cuspal[4];
    unsigned int custom;
    unsigned int now_pts;
    unsigned int start_pts, end_pts;
    unsigned int start_col, end_col;
    unsigned int start_row, end_row;
    unsigned int width, height, stride;
    size_t image_size;		/* Size of the image buffer */
    unsigned char *image;		/* Grayscale value */
    unsigned char *aimage;	/* Alpha value */
    unsigned int scaled_frame_width, scaled_frame_height;
    unsigned int scaled_start_col, scaled_start_row;
    unsigned int scaled_width, scaled_height, scaled_stride;
    size_t scaled_image_size;
    unsigned char *scaled_image;
    unsigned char *scaled_aimage;
    int auto_palette; /* 1 if we lack a palette and must use an heuristic. */
    int font_start_level;  /* Darkest value used for the computed font */
    vo_functions_t *hw_spu;
    int spu_changed;
}
spudec_handle_t;

static void spudec_queue_packet(spudec_handle_t *pThis, packet_t *packet)
{
    if (pThis->queue_head == NULL)
        pThis->queue_head = packet;
    else
        pThis->queue_tail->next = packet;
    pThis->queue_tail = packet;
}

static packet_t *spudec_dequeue_packet(spudec_handle_t *pThis)
{
    packet_t *retval = pThis->queue_head;

    pThis->queue_head = retval->next;
    if (pThis->queue_head == NULL)
        pThis->queue_tail = NULL;

    return retval;
}

static void spudec_free_packet(packet_t *packet)
{
    if (packet->packet != NULL)
        free(packet->packet);
    free(packet);
}

static __inline unsigned int get_be16(const unsigned char *p)
{
    return (p[0] << 8) + p[1];
}

static __inline unsigned int get_be24(const unsigned char *p)
{
    return (get_be16(p) << 8) + p[2];
}

static void next_line(packet_t *packet)
{
    if (packet->current_nibble[packet->deinterlace_oddness] % 2)
        packet->current_nibble[packet->deinterlace_oddness]++;
    packet->deinterlace_oddness = (packet->deinterlace_oddness + 1) % 2;
}

static __inline unsigned char get_nibble(packet_t *packet)
{
    unsigned char nib;
    unsigned int *nibblep = packet->current_nibble + packet->deinterlace_oddness;
    if (*nibblep / 2 >= packet->control_start)
    {
        mp_msg(MSGT_SPUDEC,MSGL_WARN, "SPUdec: ERROR: get_nibble past end of packet\n");
        return 0;
    }
    nib = packet->packet[*nibblep / 2];
    if (*nibblep % 2)
        nib &= 0xf;
    else
        nib >>= 4;
    ++*nibblep;
    return nib;
}

static __inline int mkalpha(int i)
{
    /* In mplayer's alpha planes, 0 is transparent, then 1 is nearly
       opaque upto 255 which is transparent */
    switch (i)
    {
    case 0xf:
        return 1;
    case 0:
        return 0;
    default:
        return (0xf - i) << 4;
    }
}

/* Cut the sub to visible part */
static __inline void spudec_cut_image(spudec_handle_t *pThis)
{
    unsigned int fy, ly;
    unsigned int first_y, last_y;
    unsigned char *image;
    unsigned char *aimage;

    if (pThis->stride == 0 || pThis->height == 0)
    {
        return;
    }

    for (fy = 0; fy < pThis->image_size && !pThis->aimage[fy]; fy++)
        ;
    for (ly = pThis->stride * pThis->height-1; ly && !pThis->aimage[ly]; ly--)
        ;
    first_y = fy / pThis->stride;
    last_y = ly / pThis->stride;
    //printf("first_y: %d, last_y: %d\n", first_y, last_y);
    pThis->start_row += first_y;

    // Some subtitles trigger this condition
    if (last_y + 1 > first_y )
    {
        pThis->height = last_y - first_y +1;
    }
    else
    {
        pThis->height = 0;
        pThis->image_size = 0;
        return;
    }

    //  printf("new h %d new start %d (sz %d st %d)---\n\n", pThis->height, pThis->start_row, pThis->image_size, pThis->stride);

    image = malloc(2 * pThis->stride * pThis->height);
    if(image)
    {
        pThis->image_size = pThis->stride * pThis->height;
        aimage = image + pThis->image_size;
        memcpy(image, pThis->image + pThis->stride * first_y, pThis->image_size);
        memcpy(aimage, pThis->aimage + pThis->stride * first_y, pThis->image_size);
        free(pThis->image);
        pThis->image = image;
        pThis->aimage = aimage;
    }
    else
    {
        mp_msg(MSGT_SPUDEC, MSGL_FATAL, "Fatal: update_spu: malloc requested %d bytes\n", 2 * pThis->stride * pThis->height);
    }
}

static void spudec_process_data(spudec_handle_t *pThis, packet_t *packet)
{
    unsigned int cmap[4], alpha[4];
    unsigned int i, x, y;

    pThis->scaled_frame_width = 0;
    pThis->scaled_frame_height = 0;
    pThis->start_col = packet->start_col;
    pThis->end_col = packet->end_col;
    pThis->start_row = packet->start_row;
    pThis->end_row = packet->end_row;
    pThis->height = packet->height;
    pThis->width = packet->width;
    pThis->stride = packet->stride;
    for (i = 0; i < 4; ++i)
    {
        alpha[i] = mkalpha(packet->alpha[i]);
        if (alpha[i] == 0)
            cmap[i] = 0;
        else if (pThis->custom)
        {
            cmap[i] = ((pThis->cuspal[i] >> 16) & 0xff);
            if (cmap[i] + alpha[i] > 255)
                cmap[i] = 256 - alpha[i];
        }
        else
        {
            cmap[i] = ((pThis->global_palette[packet->palette[i]] >> 16) & 0xff);
            if (cmap[i] + alpha[i] > 255)
                cmap[i] = 256 - alpha[i];
        }
    }

    if (pThis->image_size < pThis->stride * pThis->height)
    {
        if (pThis->image != NULL)
        {
            free(pThis->image);
            pThis->image_size = 0;
        }
        pThis->image = malloc(2 * pThis->stride * pThis->height);
        if (pThis->image)
        {
            pThis->image_size = pThis->stride * pThis->height;
            pThis->aimage = pThis->image + pThis->image_size;
        }
    }
    if (pThis->image == NULL)
        return;

    /* Kludge: draw_alpha needs width multiple of 8. */
    if (pThis->width < pThis->stride)
        for (y = 0; y < pThis->height; ++y)
        {
            memset(pThis->aimage + y * pThis->stride + pThis->width, 0, pThis->stride - pThis->width);
            /* FIXME: Why is this one needed? */
            memset(pThis->image + y * pThis->stride + pThis->width, 0, pThis->stride - pThis->width);
        }

    i = packet->current_nibble[1];
    x = 0;
    y = 0;
    while (packet->current_nibble[0] < i
            && packet->current_nibble[1] / 2 < packet->control_start
            && y < pThis->height)
    {
        unsigned int len, color;
        unsigned int rle = 0;
        rle = get_nibble(packet);
        if (rle < 0x04)
        {
            rle = (rle << 4) | get_nibble(packet);
            if (rle < 0x10)
            {
                rle = (rle << 4) | get_nibble(packet);
                if (rle < 0x040)
                {
                    rle = (rle << 4) | get_nibble(packet);
                    if (rle < 0x0004)
                        rle |= ((pThis->width - x) << 2);
                }
            }
        }
        color = 3 - (rle & 0x3);
        len = rle >> 2;
        if (len > pThis->width - x || len == 0)
            len = pThis->width - x;
        /* FIXME have to use palette and alpha map*/
        memset(pThis->image + y * pThis->stride + x, cmap[color], len);
        memset(pThis->aimage + y * pThis->stride + x, alpha[color], len);
        x += len;
        if (x >= pThis->width)
        {
            next_line(packet);
            x = 0;
            ++y;
        }
    }
    spudec_cut_image(pThis);
}


/*
  This function tries to create a usable palette.
  Is searchs how many non-transparent colors are used and assigns different
gray scale values to each color.
  I tested it with four streams and even got something readable. Half of the
times I got black characters with white around and half the reverse.
*/
static void compute_palette(spudec_handle_t *pThis, packet_t *packet)
{
    int used[16],i,cused,start,step,color;

    memset(used, 0, sizeof(used));
    for (i=0; i<4; i++)
        if (packet->alpha[i]) /* !Transparent? */
            used[packet->palette[i]] = 1;
    for (cused=0, i=0; i<16; i++)
        if (used[i])
            cused++;
    if (!cused)
        return;
    if (cused == 1)
    {
        start = 0x80;
        step = 0;
    }
    else
    {
        start = pThis->font_start_level;
        step = (0xF0-pThis->font_start_level)/(cused-1);
    }
    memset(used, 0, sizeof(used));
    for (i=0; i<4; i++)
    {
        color = packet->palette[i];
        if (packet->alpha[i] && !used[color])
        { /* not assigned? */
            used[color] = 1;
            pThis->global_palette[color] = start<<16;
            start += step;
        }
    }
}

static void spudec_process_control(spudec_handle_t *pThis, unsigned int pts100)
{
    int a,b; /* Temporary vars */
    unsigned int date, type;
    unsigned int off;
    unsigned int start_off = 0;
    unsigned int next_off;
    unsigned int start_pts;
    unsigned int end_pts;
    unsigned int current_nibble[2];
    unsigned int control_start;
    unsigned int display = 0;
    unsigned int start_col = 0;
    unsigned int end_col = 0;
    unsigned int start_row = 0;
    unsigned int end_row = 0;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int stride = 0;

    control_start = get_be16(pThis->packet + 2);
    next_off = control_start;
    while (start_off != next_off)
    {
        start_off = next_off;
        date = get_be16(pThis->packet + start_off) * 1024;
        next_off = get_be16(pThis->packet + start_off + 2);
        mp_msg(MSGT_SPUDEC,MSGL_DBG2, "date=%d\n", date);
        off = start_off + 4;
        for (type = pThis->packet[off++]; type != 0xff; type = pThis->packet[off++])
        {
            mp_msg(MSGT_SPUDEC,MSGL_DBG2, "cmd=%d  ",type);
            switch(type)
            {
            case 0x00:
                /* Menu ID, 1 byte */
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Menu ID\n");
                /* shouldn't a Menu ID type force display start? */
                //pThis->start_pts = pts100 + date;
                //pThis->end_pts = UINT_MAX;
                break;
            case 0x01:
                /* Start display */
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Start display!\n");
                start_pts = pts100 + date;
                end_pts = UINT_MAX;
                display = 1;
                break;
            case 0x02:
                /* Stop display */
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Stop display!\n");
                end_pts = pts100 + date;
                break;
            case 0x03:
                /* Palette */
                pThis->palette[0] = pThis->packet[off] >> 4;
                pThis->palette[1] = pThis->packet[off] & 0xf;
                pThis->palette[2] = pThis->packet[off + 1] >> 4;
                pThis->palette[3] = pThis->packet[off + 1] & 0xf;
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Palette %d, %d, %d, %d\n",
                       pThis->palette[0], pThis->palette[1], pThis->palette[2], pThis->palette[3]);
                off+=2;
                break;
            case 0x04:
                /* Alpha */
                pThis->alpha[0] = pThis->packet[off] >> 4;
                pThis->alpha[1] = pThis->packet[off] & 0xf;
                pThis->alpha[2] = pThis->packet[off + 1] >> 4;
                pThis->alpha[3] = pThis->packet[off + 1] & 0xf;
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Alpha %d, %d, %d, %d\n",
                       pThis->alpha[0], pThis->alpha[1], pThis->alpha[2], pThis->alpha[3]);
                off+=2;
                break;
            case 0x05:
                /* Co-ords */
                a = get_be24(pThis->packet + off);
                b = get_be24(pThis->packet + off + 3);
                start_col = a >> 12;
                end_col = a & 0xfff;
                width = (end_col < start_col) ? 0 : end_col - start_col + 1;
                stride = (width + 7) & ~7; /* Kludge: draw_alpha needs width multiple of 8 */
                start_row = b >> 12;
                end_row = b & 0xfff;
                height = (end_row < start_row) ? 0 : end_row - start_row /* + 1 */;
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Coords  col: %d - %d  row: %d - %d  (%dx%d)\n",
                       start_col, end_col, start_row, end_row,
                       width, height);
                off+=6;
                break;
            case 0x06:
                /* Graphic lines */
                current_nibble[0] = 2 * get_be16(pThis->packet + off);
                current_nibble[1] = 2 * get_be16(pThis->packet + off + 2);
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Graphic offset 1: %d  offset 2: %d\n",
                       current_nibble[0] / 2, current_nibble[1] / 2);
                off+=4;
                break;
            case 0xff:
                /* All done, bye-bye */
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"Done!\n");
                return;
                //	break;
            default:
                mp_msg(MSGT_SPUDEC,MSGL_WARN,"spudec: Error determining control type 0x%02x.  Skipping %d bytes.\n",
                       type, next_off - off);
                goto next_control;
            }
        }
next_control:
        if (display)
        {
            packet_t *packet = calloc(1, sizeof(packet_t));
            int i;
            packet->start_pts = start_pts;
            if (end_pts == UINT_MAX && start_off != next_off)
            {
                start_pts = pts100 + get_be16(pThis->packet + next_off) * 1024;
                packet->end_pts = start_pts - 1;
            }
            else
                packet->end_pts = end_pts;
            packet->current_nibble[0] = current_nibble[0];
            packet->current_nibble[1] = current_nibble[1];
            packet->start_row = start_row;
            packet->end_row = end_row;
            packet->start_col = start_col;
            packet->end_col = end_col;
            packet->width = width;
            packet->height = height;
            packet->stride = stride;
            packet->control_start = control_start;
            for (i=0; i<4; i++)
            {
                packet->alpha[i] = pThis->alpha[i];
                packet->palette[i] = pThis->palette[i];
            }
            packet->packet = malloc(pThis->packet_size);
            memcpy(packet->packet, pThis->packet, pThis->packet_size);
            spudec_queue_packet(pThis, packet);
        }
    }
}

static void spudec_decode(spudec_handle_t *pThis, unsigned int pts100)
{
    if(pThis->hw_spu)
    {
        static vo_mpegpes_t packet = { NULL, 0, 0x20, 0 };
        static vo_mpegpes_t *pkg=&packet;
        packet.data = pThis->packet;
        packet.size = pThis->packet_size;
        packet.timestamp = pts100;
        pThis->hw_spu->draw_frame((uint8_t**)&pkg);
    }
    else
        spudec_process_control(pThis, pts100);
}

int spudec_changed(void * pThis)
{
    spudec_handle_t * spu = (spudec_handle_t*)pThis;
    return (spu->spu_changed || spu->now_pts > spu->end_pts);
}

void spudec_assemble(void *pThis, unsigned char *packet, unsigned int len, unsigned int pts100)
{
    spudec_handle_t *spu = (spudec_handle_t*)pThis;
    //  spudec_heartbeat(pThis, pts100);
    if (len < 2)
    {
        mp_msg(MSGT_SPUDEC,MSGL_WARN,"SPUasm: packet too short\n");
        return;
    }
    if ((spu->packet_pts + 10000) < pts100)
    {
        // [cb] too long since last fragment: force new packet
        spu->packet_offset = 0;
    }
    spu->packet_pts = pts100;
    if (spu->packet_offset == 0)
    {
        unsigned int len2 = get_be16(packet);
        // Start new fragment
        if (spu->packet_reserve < len2)
        {
            if (spu->packet != NULL)
                free(spu->packet);
            spu->packet = malloc(len2);
            spu->packet_reserve = spu->packet != NULL ? len2 : 0;
        }
        if (spu->packet != NULL)
        {
            spu->packet_size = len2;
            if (len > len2)
            {
                mp_msg(MSGT_SPUDEC,MSGL_WARN,"SPUasm: invalid frag len / len2: %d / %d \n", len, len2);
                return;
            }
            memcpy(spu->packet, packet, len);
            spu->packet_offset = len;
            spu->packet_pts = pts100;
        }
    }
    else
    {
        // Continue current fragment
        if (spu->packet_size < spu->packet_offset + len)
        {
            mp_msg(MSGT_SPUDEC,MSGL_WARN,"SPUasm: invalid fragment\n");
            spu->packet_size = spu->packet_offset = 0;
            return;
        }
        else
        {
            memcpy(spu->packet + spu->packet_offset, packet, len);
            spu->packet_offset += len;
        }
    }
#if 1
    // check if we have a complete packet (unfortunatelly packet_size is bad
    // for some disks)
    // [cb] packet_size is padded to be even -> may be one byte too long
    if ((spu->packet_offset == spu->packet_size) ||
            ((spu->packet_offset + 1) == spu->packet_size))
    {
        unsigned int x=0,y;
        while(x+4<=spu->packet_offset)
        {
            y=get_be16(spu->packet+x+2); // next control pointer
            mp_msg(MSGT_SPUDEC,MSGL_DBG2,"SPUtest: x=%d y=%d off=%d size=%d\n",x,y,spu->packet_offset,spu->packet_size);
            if(x>=4 && x==y)
            {		// if it points to self - we're done!
                // we got it!
                mp_msg(MSGT_SPUDEC,MSGL_DBG2,"SPUgot: off=%d  size=%d \n",spu->packet_offset,spu->packet_size);
                spudec_decode(spu, pts100);
                spu->packet_offset = 0;
                break;
            }
            if(y<=x || y>=spu->packet_size)
            { // invalid?
                mp_msg(MSGT_SPUDEC,MSGL_WARN,"SPUtest: broken packet!!!!! y=%d < x=%d\n",y,x);
                spu->packet_size = spu->packet_offset = 0;
                break;
            }
            x=y;
        }
        // [cb] packet is done; start new packet
        spu->packet_offset = 0;
    }
#else
if (spu->packet_offset == spu->packet_size)
    {
        spudec_decode(spu, pts100);
        spu->packet_offset = 0;
    }
#endif
}

void spudec_reset(void *pThis)	// called after seek
{
    spudec_handle_t *spu = (spudec_handle_t*)pThis;
    while (spu->queue_head)
        spudec_free_packet(spudec_dequeue_packet(spu));
    spu->now_pts = 0;
    spu->end_pts = 0;
    spu->packet_size = spu->packet_offset = 0;
}

void spudec_heartbeat(void *pThis, unsigned int pts100)
{
    spudec_handle_t *spu = (spudec_handle_t*) pThis;
    spu->now_pts = pts100;

    while (spu->queue_head != NULL && pts100 >= spu->queue_head->start_pts)
    {
        packet_t *packet = spudec_dequeue_packet(spu);
        spu->start_pts = packet->start_pts;
        spu->end_pts = packet->end_pts;
        if (spu->auto_palette)
            compute_palette(spu, packet);
        spudec_process_data(spu, packet);
        spudec_free_packet(packet);
        spu->spu_changed = 1;
    }
}

int spudec_visible(void *pThis)
{
    spudec_handle_t *spu = (spudec_handle_t *)pThis;
    int ret=(spu->start_pts <= spu->now_pts &&
             spu->now_pts < spu->end_pts &&
             spu->height > 0);
    //    printf("spu visible: %d  \n",ret);
    return ret;
}

void spudec_draw(void *pThis, void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride))
{
    spudec_handle_t *spu = (spudec_handle_t *)pThis;
    if (spu->start_pts <= spu->now_pts && spu->now_pts < spu->end_pts && spu->image)
    {
        draw_alpha(spu->start_col, spu->start_row, spu->width, spu->height,
                   spu->image, spu->aimage, spu->stride);
        spu->spu_changed = 0;
    }
}

/* calc the bbox for spudec subs */
void spudec_calc_bbox(void *me, unsigned int dxs, unsigned int dys, unsigned int* bbox)
{
    spudec_handle_t *spu;
    spu = (spudec_handle_t *)me;
    if (spu->orig_frame_width == 0 || spu->orig_frame_height == 0
            || (spu->orig_frame_width == dxs && spu->orig_frame_height == dys))
    {
        bbox[0] = spu->start_col;
        bbox[1] = spu->start_col + spu->width;
        bbox[2] = spu->start_row;
        bbox[3] = spu->start_row + spu->height;
    }
    else if (spu->scaled_frame_width != dxs || spu->scaled_frame_height != dys)
    {
        unsigned int scalex = 0x100 * dxs / spu->orig_frame_width;
        unsigned int scaley = 0x100 * dys / spu->orig_frame_height;
        bbox[0] = spu->start_col * scalex / 0x100;
        bbox[1] = spu->start_col * scalex / 0x100 + spu->width * scalex / 0x100;
        switch (spu_alignment)
        {
        case 0:
            bbox[3] = dys*sub_pos/100 + spu->height * scaley / 0x100;
            if (bbox[3] > dys)
                bbox[3] = dys;
            bbox[2] = bbox[3] - spu->height * scaley / 0x100;
            break;
        case 1:
            if (sub_pos < 50)
            {
                bbox[2] = dys*sub_pos/100 - spu->height * scaley / 0x200;
                if (bbox[2] < 0)
                    bbox[2] = 0;
                bbox[3] = bbox[2] + spu->height;
            }
            else
            {
                bbox[3] = dys*sub_pos/100 + spu->height * scaley / 0x200;
                if (bbox[3] > dys)
                    bbox[3] = dys;
                bbox[2] = bbox[3] - spu->height * scaley / 0x100;
            }
            break;
        case 2:
            bbox[2] = dys*sub_pos/100 - spu->height * scaley / 0x100;
            if (bbox[2] < 0)
                bbox[2] = 0;
            bbox[3] = bbox[2] + spu->height;
            break;
        default: /* -1 */
            bbox[2] = spu->start_row * scaley / 0x100;
            bbox[3] = spu->start_row * scaley / 0x100 + spu->height * scaley / 0x100;
            break;
        }
    }
}
/* transform mplayer's alpha value into an opacity value that is linear */
static __inline int canon_alpha(int alpha)
{
    return alpha ? 256 - alpha : 0;
}

typedef struct
{
    unsigned position;
    unsigned left_up;
    unsigned right_down;
}
scale_pixel;


static void scale_table(unsigned int start_src, unsigned int start_tar, unsigned int end_src, unsigned int end_tar, scale_pixel * table)
{
    unsigned int t;
    unsigned int delta_src = end_src - start_src;
    unsigned int delta_tar = end_tar - start_tar;
    int src = 0;
    int src_step;
    if (delta_src == 0 || delta_tar == 0)
    {
        return;
    }
    src_step = (delta_src << 16) / delta_tar >>1;
    for (t = 0; t<=delta_tar; src += (src_step << 1), t++)
    {
        table[t].position= MIN(src >> 16, end_src - 1);
        table[t].right_down = src & 0xffff;
        table[t].left_up = 0x10000 - table[t].right_down;
    }
}

/* bilinear scale, similar to vobsub's code */
static void scale_image(int x, int y, scale_pixel* table_x, scale_pixel* table_y, spudec_handle_t * spu)
{
    int alpha[4];
    int color[4];
    unsigned int scale[4];
    int base = table_y[y].position * spu->stride + table_x[x].position;
    int scaled = y * spu->scaled_stride + x;
    alpha[0] = canon_alpha(spu->aimage[base]);
    alpha[1] = canon_alpha(spu->aimage[base + 1]);
    alpha[2] = canon_alpha(spu->aimage[base + spu->stride]);
    alpha[3] = canon_alpha(spu->aimage[base + spu->stride + 1]);
    color[0] = spu->image[base];
    color[1] = spu->image[base + 1];
    color[2] = spu->image[base + spu->stride];
    color[3] = spu->image[base + spu->stride + 1];
    scale[0] = (table_x[x].left_up * table_y[y].left_up >> 16) * alpha[0];
    scale[1] = (table_x[x].right_down * table_y[y].left_up >>16) * alpha[1];
    scale[2] = (table_x[x].left_up * table_y[y].right_down >> 16) * alpha[2];
    scale[3] = (table_x[x].right_down * table_y[y].right_down >> 16) * alpha[3];
    spu->scaled_image[scaled] = (color[0] * scale[0] + color[1] * scale[1] + color[2] * scale[2] + color[3] * scale[3])>>24;
    spu->scaled_aimage[scaled] = (scale[0] + scale[1] + scale[2] + scale[3]) >> 16;
    if (spu->scaled_aimage[scaled])
    {
        spu->scaled_aimage[scaled] = 256 - spu->scaled_aimage[scaled];
        if(spu->scaled_aimage[scaled] + spu->scaled_image[scaled] > 255)
            spu->scaled_image[scaled] = 256 - spu->scaled_aimage[scaled];
    }
}

void sws_spu_image(unsigned char *d1, unsigned char *d2, int dw, int dh, int ds,
                   unsigned char *s1, unsigned char *s2, int sw, int sh, int ss)
{
    struct SwsContext *ctx;
    static SwsFilter filter;
    static int firsttime = 1;
    static float oldvar;
    int i;

    if (!firsttime && oldvar != spu_gaussvar)
        sws_freeVec(filter.lumH);
    if (firsttime)
    {
        filter.lumH = filter.lumV =
                          filter.chrH = filter.chrV = sws_getGaussianVec(spu_gaussvar, 3.0);
        sws_normalizeVec(filter.lumH, 1.0);
        firsttime = 0;
        oldvar = spu_gaussvar;
    }

    ctx=sws_getContext(sw, sh, IMGFMT_Y800, dw, dh, IMGFMT_Y800, SWS_GAUSS, &filter, NULL);
    sws_scale(ctx,&s1,&ss,0,sh,&d1,&ds);
    for (i=ss*sh-1; i>=0; i--)
        if (!s2[i])
            s2[i] = 255; //else s2[i] = 1;
    sws_scale(ctx,&s2,&ss,0,sh,&d2,&ds);
    for (i=ds*dh-1; i>=0; i--)
        if (d2[i]==0)
            d2[i] = 1;
        else if (d2[i]==255)
            d2[i] = 0;
    sws_freeContext(ctx);
}

void spudec_draw_scaled(void *me, unsigned int dxs, unsigned int dys, void (*draw_alpha)(int x0,int y0, int w,int h, unsigned char* src, unsigned char *srca, int stride))
{
    spudec_handle_t *spu = (spudec_handle_t *)me;
    scale_pixel *table_x;
    scale_pixel *table_y;
    if (spu->start_pts <= spu->now_pts && spu->now_pts < spu->end_pts)
    {
        if (!(spu_aamode&16) && (spu->orig_frame_width == 0 || spu->orig_frame_height == 0
                                 || (spu->orig_frame_width == dxs && spu->orig_frame_height == dys)))
        {
            if (spu->image)
            {
                draw_alpha(spu->start_col, spu->start_row, spu->width, spu->height,
                           spu->image, spu->aimage, spu->stride);
                spu->spu_changed = 0;
            }
        }
        else
        {
            if (spu->scaled_frame_width != dxs || spu->scaled_frame_height != dys)
            {	/* Resizing is needed */
                /* scaled_x = scalex * x / 0x100
                   scaled_y = scaley * y / 0x100
                   order of operations is important because of rounding. */
                unsigned int scalex = 0x100 * dxs / spu->orig_frame_width;
                unsigned int scaley = 0x100 * dys / spu->orig_frame_height;
                spu->scaled_start_col = spu->start_col * scalex / 0x100;
                spu->scaled_start_row = spu->start_row * scaley / 0x100;
                spu->scaled_width = spu->width * scalex / 0x100;
                spu->scaled_height = spu->height * scaley / 0x100;
                /* Kludge: draw_alpha needs width multiple of 8 */
                spu->scaled_stride = (spu->scaled_width + 7) & ~7;
                if (spu->scaled_image_size < spu->scaled_stride * spu->scaled_height)
                {
                    if (spu->scaled_image)
                    {
                        free(spu->scaled_image);
                        spu->scaled_image_size = 0;
                    }
                    spu->scaled_image = malloc(2 * spu->scaled_stride * spu->scaled_height);
                    if (spu->scaled_image)
                    {
                        spu->scaled_image_size = spu->scaled_stride * spu->scaled_height;
                        spu->scaled_aimage = spu->scaled_image + spu->scaled_image_size;
                    }
                }
                if (spu->scaled_image)
                {
                    unsigned int x, y;
                    /* Kludge: draw_alpha needs width multiple of 8. */
                    if (spu->scaled_width < spu->scaled_stride)
                        for (y = 0; y < spu->scaled_height; ++y)
                        {
                            memset(spu->scaled_aimage + y * spu->scaled_stride + spu->scaled_width, 0,
                                   spu->scaled_stride - spu->scaled_width);
                            /* FIXME: Why is pThis one needed? */
                            memset(spu->scaled_image + y * spu->scaled_stride + spu->scaled_width, 0,
                                   spu->scaled_stride - spu->scaled_width);
                        }
                    if (spu->scaled_width <= 1 || spu->scaled_height <= 1)
                    {
                        goto nothing_to_do
                        ;
                    }
                    switch(spu_aamode&15)
                    {
                    case 4:
                        sws_spu_image(spu->scaled_image, spu->scaled_aimage,
                                      spu->scaled_width, spu->scaled_height, spu->scaled_stride,
                                      spu->image, spu->aimage, spu->width, spu->height, spu->stride);
                        break;
                    case 3:
                        table_x = calloc(spu->scaled_width, sizeof(scale_pixel));
                        table_y = calloc(spu->scaled_height, sizeof(scale_pixel));
                        if (!table_x || !table_y)
                        {
                            mp_msg(MSGT_SPUDEC, MSGL_FATAL, "Fatal: spudec_draw_scaled: calloc failed\n");
                        }
                        scale_table(0, 0, spu->width - 1, spu->scaled_width - 1, table_x);
                        scale_table(0, 0, spu->height - 1, spu->scaled_height - 1, table_y);
                        for (y = 0; y < spu->scaled_height; y++)
                            for (x = 0; x < spu->scaled_width; x++)
                                scale_image(x, y, table_x, table_y, spu);
                        free(table_x);
                        free(table_y);
                        break;
                    case 0:
                        /* no antialiasing */
                        for (y = 0; y < spu->scaled_height; ++y)
                        {
                            int unscaled_y = y * 0x100 / scaley;
                            int strides = spu->stride * unscaled_y;
                            int scaled_strides = spu->scaled_stride * y;
                            for (x = 0; x < spu->scaled_width; ++x)
                            {
                                int unscaled_x = x * 0x100 / scalex;
                                spu->scaled_image[scaled_strides + x] = spu->image[strides + unscaled_x];
                                spu->scaled_aimage[scaled_strides + x] = spu->aimage[strides + unscaled_x];
                            }
                        }
                        break;
                    case 1:
                        {
                            /* Intermediate antialiasing. */
                            for (y = 0; y < spu->scaled_height; ++y)
                            {
                                const unsigned int unscaled_top = y * spu->orig_frame_height / dys;
                                unsigned int unscaled_bottom = (y + 1) * spu->orig_frame_height / dys;
                                if (unscaled_bottom >= spu->height)
                                    unscaled_bottom = spu->height - 1;
                                for (x = 0; x < spu->scaled_width; ++x)
                                {
                                    const unsigned int unscaled_left = x * spu->orig_frame_width / dxs;
                                    unsigned int unscaled_right = (x + 1) * spu->orig_frame_width / dxs;
                                    unsigned int color = 0;
                                    unsigned int alpha = 0;
                                    unsigned int walkx, walky;
                                    unsigned int base, tmp;
                                    if (unscaled_right >= spu->width)
                                        unscaled_right = spu->width - 1;
                                    for (walky = unscaled_top; walky <= unscaled_bottom; ++walky)
                                        for (walkx = unscaled_left; walkx <= unscaled_right; ++walkx)
                                        {
                                            base = walky * spu->stride + walkx;
                                            tmp = canon_alpha(spu->aimage[base]);
                                            alpha += tmp;
                                            color += tmp * spu->image[base];
                                        }
                                    base = y * spu->scaled_stride + x;
                                    spu->scaled_image[base] = alpha ? color / alpha : 0;
                                    spu->scaled_aimage[base] =
                                        alpha * (1 + unscaled_bottom - unscaled_top) * (1 + unscaled_right - unscaled_left);
                                    /* spu->scaled_aimage[base] =
                                      alpha * dxs * dys / spu->orig_frame_width / spu->orig_frame_height; */
                                    if (spu->scaled_aimage[base])
                                    {
                                        spu->scaled_aimage[base] = 256 - spu->scaled_aimage[base];
                                        if (spu->scaled_aimage[base] + spu->scaled_image[base] > 255)
                                            spu->scaled_image[base] = 256 - spu->scaled_aimage[base];
                                    }
                                }
                            }
                        }
                        break;
                    case 2:
                        {
                            /* Best antialiasing.  Very slow. */
                            /* Any pixel (x, y) represents pixels from the original
                               rectangular region comprised between the columns
                               unscaled_y and unscaled_y + 0x100 / scaley and the rows
                               unscaled_x and unscaled_x + 0x100 / scalex

                               The original rectangular region that the scaled pixel
                               represents is cut in 9 rectangular areas like this:
                               
                               +---+-----------------+---+
                               | 1 |        2        | 3 |
                               +---+-----------------+---+
                               |   |                 |   |
                               | 4 |        5        | 6 |
                               |   |                 |   |
                               +---+-----------------+---+
                               | 7 |        8        | 9 |
                               +---+-----------------+---+

                               The width of the left column is at most one pixel and
                               it is never null and its right column is at a pixel
                               boundary.  The height of the top row is at most one
                               pixel it is never null and its bottom row is at a
                               pixel boundary. The width and height of region 5 are
                               integral values.  The width of the right column is
                               what remains and is less than one pixel.  The height
                               of the bottom row is what remains and is less than
                               one pixel.

                               The row above 1, 2, 3 is unscaled_y.  The row between
                               1, 2, 3 and 4, 5, 6 is top_low_row.  The row between 4,
                               5, 6 and 7, 8, 9 is (unsigned int)unscaled_y_bottom.
                               The row beneath 7, 8, 9 is unscaled_y_bottom.

                               The column left of 1, 4, 7 is unscaled_x.  The column
                               between 1, 4, 7 and 2, 5, 8 is left_right_column.  The
                               column between 2, 5, 8 and 3, 6, 9 is (unsigned
                               int)unscaled_x_right.  The column right of 3, 6, 9 is
                               unscaled_x_right. */
                            const double inv_scalex = (double) 0x100 / scalex;
                            const double inv_scaley = (double) 0x100 / scaley;
                            for (y = 0; y < spu->scaled_height; ++y)
                            {
                                const double unscaled_y = y * inv_scaley;
                                const double unscaled_y_bottom = unscaled_y + inv_scaley;
                                const unsigned int top_low_row = MIN(unscaled_y_bottom, unscaled_y + 1.0);
                                const double top = top_low_row - unscaled_y;
                                const unsigned int height = unscaled_y_bottom > top_low_row
                                                            ? (unsigned int) unscaled_y_bottom - top_low_row
                                                            : 0;
                                const double bottom = unscaled_y_bottom > top_low_row
                                                      ? unscaled_y_bottom - floor(unscaled_y_bottom)
                                                      : 0.0;
                                for (x = 0; x < spu->scaled_width; ++x)
                                {
                                    const double unscaled_x = x * inv_scalex;
                                    const double unscaled_x_right = unscaled_x + inv_scalex;
                                    const unsigned int left_right_column = MIN(unscaled_x_right, unscaled_x + 1.0);
                                    const double left = left_right_column - unscaled_x;
                                    const unsigned int width = unscaled_x_right > left_right_column
                                                               ? (unsigned int) unscaled_x_right - left_right_column
                                                               : 0;
                                    const double right = unscaled_x_right > left_right_column
                                                         ? unscaled_x_right - floor(unscaled_x_right)
                                                         : 0.0;
                                    double color = 0.0;
                                    double alpha = 0.0;
                                    double tmp;
                                    unsigned int base;
                                    /* Now use these informations to compute a good alpha,
                                                     and lightness.  The sum is on each of the 9
                                                     region's surface and alpha and lightness.

                                      transformed alpha = sum(surface * alpha) / sum(surface)
                                      transformed color = sum(surface * alpha * color) / sum(surface * alpha)
                                    */
                                    /* 1: top left part */
                                    base = spu->stride * (unsigned int) unscaled_y;
                                    tmp = left * top * canon_alpha(spu->aimage[base + (unsigned int) unscaled_x]);
                                    alpha += tmp;
                                    color += tmp * spu->image[base + (unsigned int) unscaled_x];
                                    /* 2: top center part */
                                    if (width > 0)
                                    {
                                        unsigned int walkx;
                                        for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx)
                                        {
                                            base = spu->stride * (unsigned int) unscaled_y + walkx;
                                            tmp = /* 1.0 * */ top * canon_alpha(spu->aimage[base]);
                                            alpha += tmp;
                                            color += tmp * spu->image[base];
                                        }
                                    }
                                    /* 3: top right part */
                                    if (right > 0.0)
                                    {
                                        base = spu->stride * (unsigned int) unscaled_y + (unsigned int) unscaled_x_right;
                                        tmp = right * top * canon_alpha(spu->aimage[base]);
                                        alpha += tmp;
                                        color += tmp * spu->image[base];
                                    }
                                    /* 4: center left part */
                                    if (height > 0)
                                    {
                                        unsigned int walky;
                                        for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky)
                                        {
                                            base = spu->stride * walky + (unsigned int) unscaled_x;
                                            tmp = left /* * 1.0 */ * canon_alpha(spu->aimage[base]);
                                            alpha += tmp;
                                            color += tmp * spu->image[base];
                                        }
                                    }
                                    /* 5: center part */
                                    if (width > 0 && height > 0)
                                    {
                                        unsigned int walky;
                                        for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky)
                                        {
                                            unsigned int walkx;
                                            base = spu->stride * walky;
                                            for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx)
                                            {
                                                tmp = /* 1.0 * 1.0 * */ canon_alpha(spu->aimage[base + walkx]);
                                                alpha += tmp;
                                                color += tmp * spu->image[base + walkx];
                                            }
                                        }
                                    }
                                    /* 6: center right part */
                                    if (right > 0.0 && height > 0)
                                    {
                                        unsigned int walky;
                                        for (walky = top_low_row; walky < (unsigned int) unscaled_y_bottom; ++walky)
                                        {
                                            base = spu->stride * walky + (unsigned int) unscaled_x_right;
                                            tmp = right /* * 1.0 */ * canon_alpha(spu->aimage[base]);
                                            alpha += tmp;
                                            color += tmp * spu->image[base];
                                        }
                                    }
                                    /* 7: bottom left part */
                                    if (bottom > 0.0)
                                    {
                                        base = spu->stride * (unsigned int) unscaled_y_bottom + (unsigned int) unscaled_x;
                                        tmp = left * bottom * canon_alpha(spu->aimage[base]);
                                        alpha += tmp;
                                        color += tmp * spu->image[base];
                                    }
                                    /* 8: bottom center part */
                                    if (width > 0 && bottom > 0.0)
                                    {
                                        unsigned int walkx;
                                        base = spu->stride * (unsigned int) unscaled_y_bottom;
                                        for (walkx = left_right_column; walkx < (unsigned int) unscaled_x_right; ++walkx)
                                        {
                                            tmp = /* 1.0 * */ bottom * canon_alpha(spu->aimage[base + walkx]);
                                            alpha += tmp;
                                            color += tmp * spu->image[base + walkx];
                                        }
                                    }
                                    /* 9: bottom right part */
                                    if (right > 0.0 && bottom > 0.0)
                                    {
                                        base = spu->stride * (unsigned int) unscaled_y_bottom + (unsigned int) unscaled_x_right;
                                        tmp = right * bottom * canon_alpha(spu->aimage[base]);
                                        alpha += tmp;
                                        color += tmp * spu->image[base];
                                    }
                                    /* Finally mix these transparency and brightness information suitably */
                                    base = spu->scaled_stride * y + x;
                                    spu->scaled_image[base] = alpha > 0 ? color / alpha : 0;
                                    spu->scaled_aimage[base] = alpha * scalex * scaley / 0x10000;
                                    if (spu->scaled_aimage[base])
                                    {
                                        spu->scaled_aimage[base] = 256 - spu->scaled_aimage[base];
                                        if (spu->scaled_aimage[base] + spu->scaled_image[base] > 255)
                                            spu->scaled_image[base] = 256 - spu->scaled_aimage[base];
                                    }
                                }
                            }
                        }
                    }
                    nothing_to_do
:
                    spu->scaled_frame_width = dxs;
                    spu->scaled_frame_height = dys;
                }
            }
            if (spu->scaled_image)
            {
                switch (spu_alignment)
                {
                case 0:
                    spu->scaled_start_row = dys*sub_pos/100;
                    if (spu->scaled_start_row + spu->scaled_height > dys)
                        spu->scaled_start_row = dys - spu->scaled_height;
                    break;
                case 1:
                    spu->scaled_start_row = dys*sub_pos/100 - spu->scaled_height/2;
                    if (sub_pos < 50)
                    {
                        if (spu->scaled_start_row < 0)
                            spu->scaled_start_row = 0;
                    }
                    else
                    {
                        if (spu->scaled_start_row + spu->scaled_height > dys)
                            spu->scaled_start_row = dys - spu->scaled_height;
                    }
                    break;
                case 2:
                    spu->scaled_start_row = dys*sub_pos/100 - spu->scaled_height;
                    if (spu->scaled_start_row < 0)
                        spu->scaled_start_row = 0;
                    break;
                }
                draw_alpha(spu->scaled_start_col, spu->scaled_start_row, spu->scaled_width, spu->scaled_height,
                           spu->scaled_image, spu->scaled_aimage, spu->scaled_stride);
                spu->spu_changed = 0;
            }
        }
    }
    else
    {
        mp_msg(MSGT_SPUDEC,MSGL_DBG2,"SPU not displayed: start_pts=%d  end_pts=%d  now_pts=%d\n",
               spu->start_pts, spu->end_pts, spu->now_pts);
    }
}

void spudec_update_palette(void * pThis, unsigned int *palette)
{
    spudec_handle_t *spu = (spudec_handle_t *) pThis;
    if (spu && palette)
    {
        memcpy(spu->global_palette, palette, sizeof(spu->global_palette));
        if(spu->hw_spu)
            spu->hw_spu->control(VOCTRL_SET_SPU_PALETTE,spu->global_palette);
    }
}

void spudec_set_font_factor(void * pThis, double factor)
{
    spudec_handle_t *spu = (spudec_handle_t *) pThis;
    spu->font_start_level = (int)(0xF0-(0xE0*factor));
}

void *spudec_new_scaled(unsigned int *palette, unsigned int frame_width, unsigned int frame_height)
{
    return spudec_new_scaled_vobsub(palette, NULL, 0, frame_width, frame_height);
}

/* get palette custom color, width, height from .idx file */
void *spudec_new_scaled_vobsub(unsigned int *palette, unsigned int *cuspal, unsigned int custom, unsigned int frame_width, unsigned int frame_height)
{
    spudec_handle_t *pThis = calloc(1, sizeof(spudec_handle_t));
    if (pThis)
    {
        //(fprintf(stderr,"VobSub Custom Palette: %d,%d,%d,%d", pThis->cuspal[0], pThis->cuspal[1], pThis->cuspal[2],pThis->cuspal[3]);
        pThis->packet = NULL;
        pThis->image = NULL;
        pThis->scaled_image = NULL;
        /* XXX Although the video frame is some size, the SPU frame is
           always maximum size i.e. 720 wide and 576 or 480 high */
        pThis->orig_frame_width = 720;
        pThis->orig_frame_height = (frame_height == 480 || frame_height == 240) ? 480 : 576;
        pThis->custom = custom;
        // set up palette:
        pThis->auto_palette = 1;
        if (palette)
        {
            memcpy(pThis->global_palette, palette, sizeof(pThis->global_palette));
            pThis->auto_palette = 0;
        }
        pThis->custom = custom;
        if (custom && cuspal)
        {
            memcpy(pThis->cuspal, cuspal, sizeof(pThis->cuspal));
            pThis->auto_palette = 0;
        }
    }
    else
        mp_msg(MSGT_SPUDEC,MSGL_FATAL, "FATAL: spudec_init: calloc");
    return pThis;
}

void *spudec_new(unsigned int *palette)
{
    return spudec_new_scaled(palette, 0, 0);
}

void spudec_free(void *pThis)
{
    spudec_handle_t *spu = (spudec_handle_t*)pThis;
    if (spu)
    {
        while (spu->queue_head)
            spudec_free_packet(spudec_dequeue_packet(spu));
        if (spu->packet)
            free(spu->packet);
        if (spu->scaled_image)
            free(spu->scaled_image);
        if (spu->image)
            free(spu->image);
        free(spu);
    }
}

void spudec_set_hw_spu(void *pThis, vo_functions_t *hw_spu)
{
    spudec_handle_t *spu = (spudec_handle_t*)pThis;
    if (!spu)
        return;
    spu->hw_spu = hw_spu;
    hw_spu->control(VOCTRL_SET_SPU_PALETTE,spu->global_palette);
}
