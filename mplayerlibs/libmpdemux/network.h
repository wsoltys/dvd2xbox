/*
 * Network layer for MPlayer
 * by Bertrand BAUDET <bertrand_baudet@yahoo.com>
 * (C) 2001, MPlayer team.
 */

#ifndef __NETWORK_H
#define __NETWORK_H

#include <fcntl.h>
#ifdef _XBOX
#include <xtl.h>
#include "url.h"
#else
#include <winsock.h>
#endif
#define BUFFER_SIZE		2048

typedef enum {
    streaming_stopped_e,
    streaming_playing_e
} streaming_status;

typedef struct streaming_control
{
    URL_t *url;
    streaming_status status;
    int buffering;	// boolean
    unsigned int prebuffer_size;
    char *buffer;
    unsigned int buffer_size;
    unsigned int buffer_pos;
    unsigned int bandwidth;	// The downstream available
    int (*streaming_read)( int fd, char *buffer, int buffer_size, struct streaming_control *stream_ctrl );
    int (*streaming_seek)( int fd, off_t pos, struct streaming_control *stream_ctrl );
    int (*streaming_close)( int fd, struct streaming_control *stream_ctrl );
    void *data;
}
streaming_ctrl_t;


#endif
