


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


#ifdef USE_LIBMAD

#include "ad_internal.h"

static ad_info_t info =  {
                             "libmad mpeg audio decoder",
                             "libmad",
                             "A'rpi",
                             "libmad...",
                             "based on Xine's libmad/xine_decoder.c"
                         };

LIBAD_EXTERN(libmad)

#include "../libmad/mad.h"

typedef struct mad_decoder_s
{

    struct mad_synth  synth;
    struct mad_stream stream;
    struct mad_frame  frame;

    int have_frame;

    int               output_sampling_rate;
    int               output_open;
    int               output_mode;

}
mad_decoder_t;

static int preinit(sh_audio_t *sh)
{

    mad_decoder_t *pThis = (mad_decoder_t *) malloc(sizeof(mad_decoder_t));
    memset(pThis,0,sizeof(mad_decoder_t));
    sh->context = pThis;

    mad_synth_init  (&pThis->synth);
    mad_stream_init (&pThis->stream);
    mad_frame_init  (&pThis->frame);

    sh->audio_out_minsize=2*4608;
    sh->audio_in_minsize=4096;

    return 1;
}

static int read_frame(sh_audio_t *sh)
{
    mad_decoder_t *pThis = (mad_decoder_t *) sh->context;
    int len;

    while((len=demux_read_data(sh->ds,&sh->a_in_buffer[sh->a_in_buffer_len],
                               sh->a_in_buffer_size-sh->a_in_buffer_len))>0)
    {
        sh->a_in_buffer_len+=len;
        while(1)
        {
            int ret;
            mad_stream_buffer (&pThis->stream, sh->a_in_buffer, sh->a_in_buffer_len);
            ret=mad_frame_decode (&pThis->frame, &pThis->stream);
            if (pThis->stream.next_frame)
            {
                int num_bytes =
                    (char*)sh->a_in_buffer+sh->a_in_buffer_len - (char*)pThis->stream.next_frame;
                memmove(sh->a_in_buffer, pThis->stream.next_frame, num_bytes);
                //mp_msg(MSGT_DECAUDIO,MSGL_DBG2,"libmad: %d bytes processed\n",sh->a_in_buffer_len-num_bytes);
                sh->a_in_buffer_len = num_bytes;
            }
            if (ret == 0)
                return 1; // OK!!!
            // error! try to resync!
            if(pThis->stream.error==MAD_ERROR_BUFLEN)
                break;
        }
    }
    mp_msg(MSGT_DECAUDIO,MSGL_INFO,"Cannot sync MAD frame\n");
    return 0;
}

static int init(sh_audio_t *sh)
{
    mad_decoder_t *pThis = (mad_decoder_t *) sh->context;

    pThis->have_frame=read_frame(sh);
    if(!pThis->have_frame)
        return 0; // failed to sync...

    sh->channels=(pThis->frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
    sh->samplerate=pThis->frame.header.samplerate;
    sh->i_bps=pThis->frame.header.bitrate/8;

    return 1;
}

static void uninit(sh_audio_t *sh)
{
    mad_decoder_t *pThis = (mad_decoder_t *) sh->context;
    mad_synth_finish (&pThis->synth);
    mad_frame_finish (&pThis->frame);
    mad_stream_finish(&pThis->stream);
    free(sh->context);
}

/* utility to scale and round samples to 16 bits */
static signed int scale(mad_fixed_t sample)
{
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
        sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
        sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static int decode_audio(sh_audio_t *sh,unsigned char *buf,int minlen,int maxlen)
{
    mad_decoder_t *pThis = (mad_decoder_t *) sh->context;
    int len=0;

    while(len<minlen && len+4608<=maxlen)
    {
        if(!pThis->have_frame)
            pThis->have_frame=read_frame(sh);
        if(!pThis->have_frame)
            break; // failed to sync... or EOF
        pThis->have_frame=0;

        mad_synth_frame (&pThis->synth, &pThis->frame);

        {
            unsigned int         nchannels, nsamples;
            mad_fixed_t const   *left_ch, *right_ch;
            struct mad_pcm      *pcm = &pThis->synth.pcm;
            uint16_t            *output = (uint16_t*) buf;

            nchannels = pcm->channels;
            nsamples  = pcm->length;
            left_ch   = pcm->samples[0];
            right_ch  = pcm->samples[1];

            len+=2*nchannels*nsamples;
            buf+=2*nchannels*nsamples;

            while (nsamples--)
            {
                /* output sample(s) in 16-bit signed little-endian PCM */

                *output++ = scale(*left_ch++);

                if (nchannels == 2)
                    *output++ = scale(*right_ch++);

            }
        }
    }

    return len?len:-1;
}

static int control(sh_audio_t *sh,int cmd,void* arg, ...)
{
    mad_decoder_t *pThis = (mad_decoder_t *) sh->context;
    // various optional functions you MAY implement:
    switch(cmd)
    {
    case ADCTRL_RESYNC_STREAM:
        pThis->have_frame=0;
        mad_synth_init  (&pThis->synth);
        mad_stream_init (&pThis->stream);
        mad_frame_init  (&pThis->frame);
        return CONTROL_TRUE;
    case ADCTRL_SKIP_FRAME:
        pThis->have_frame=read_frame(sh);
        return CONTROL_TRUE;
    }
    return CONTROL_UNKNOWN;
}
#endif
