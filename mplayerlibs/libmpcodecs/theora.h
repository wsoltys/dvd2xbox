/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggTheora SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the Xiph.Org Foundation http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

  function: 
  last mod: $Id: theora.h,v 1.1 2003/11/04 18:28:02 wiso Exp $

 ********************************************************************/

#ifndef _O_THEORA_H_
#define _O_THEORA_H_

#include "../../lib/ogg/ogg.h"

typedef struct {
    int   y_width;
    int   y_height;
    int   y_stride;

    int   uv_width;
    int   uv_height;
    int   uv_stride;

    char *y;
    char *u;
    char *v;

} yuv_buffer;

typedef struct{
  ogg_uint32_t  width;
  ogg_uint32_t  height;
  ogg_uint32_t  fps_numerator;
  ogg_uint32_t  fps_denominator;
  ogg_uint32_t  aspect_numerator;
  ogg_uint32_t  aspect_denominator;
  int           target_bitrate;
  int           quality;
  int           quick_p;  /* quick encode/decode */

  /* decode only */
  unsigned char version_major;
  unsigned char version_minor;
  unsigned char version_subminor;

  /* encode only */
  int           dropframes_p;
  int           keyframe_auto_p;
  ogg_uint32_t  keyframe_frequency;
  ogg_uint32_t  keyframe_frequency_force;  /* also used for decode init to
					      get granpos shift correct */
  ogg_uint32_t  keyframe_data_target_bitrate;
  ogg_int32_t   keyframe_auto_threshold;
  ogg_uint32_t  keyframe_mindistance;
  ogg_int32_t   noise_sensitivity;
  ogg_int32_t   sharpness;

} theora_info;

typedef struct{
  theora_info *i;
  ogg_int64_t granulepos;

  void *internal_encode;
  void *internal_decode;

} theora_state;

typedef struct theora_comment{
  char **user_comments;
  int   *comment_lengths;
  int    comments;
  char  *vendor;                                                          

} theora_comment;

#define OC_FAULT       -1
#define OC_EINVAL      -10
#define OC_BADHEADER   -20
#define OC_NOTFORMAT   -21
#define OC_VERSION     -22
#define OC_IMPL        -23
#define OC_BADPACKET   -24

extern const char *theora_version_string(void);
extern ogg_uint32_t theora_version_number(void);
extern int theora_encode_init(theora_state *th, theora_info *c);
extern int theora_encode_YUVin(theora_state *t, yuv_buffer *yuv);
extern int theora_encode_packetout( theora_state *t, int last_p, 
				    ogg_packet *op);
extern int theora_encode_header(theora_state *t, ogg_packet *op);
extern int theora_encode_comment(theora_comment *tc, ogg_packet *op);
extern int theora_decode_header(theora_info *c, ogg_packet *op);
extern int theora_decode_comment(theora_comment *tc, ogg_packet *op); 
extern int theora_decode_init(theora_state *th, theora_info *c);
extern int theora_decode_packetin(theora_state *th,ogg_packet *op);
extern int theora_decode_YUVout(theora_state *th,yuv_buffer *yuv);
extern double theora_granule_time(theora_state *th,ogg_int64_t granulepos);
extern void theora_clear(theora_state *t);

extern void theora_comment_init(theora_comment *tc);
extern void theora_comment_add(theora_comment *tc, char *comment);
extern void theora_comment_add_tag(theora_comment *tc,
                                       char *tag, char *value);
extern char *theora_comment_query(theora_comment *tc, char *tag, int count);
extern int   theora_comment_query_count(theora_comment *tc, char *tag);
extern void  theora_comment_clear(theora_comment *tc);



#endif /* _O_THEORA_H_ */

