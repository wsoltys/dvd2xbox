/*
 * Copyright (C) 2002 Samuel Hocevar <sam@zoy.org>,
 *                    Håkan Hjort <d95hjort@dtek.chalmers.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include "dvdcss.h"

#include "common.h"
#include "css.h"
#include "libdvdcss.h"
#include "ioctl.h"
#include "device.h"

#include "dvd_input.h"
#include "dvd_reader.h"

/* The function pointers that is the exported interface of this file. */
dvd_input_t (*dvdinput_open)  (const char *);
int         (*dvdinput_close) (dvd_input_t);
int         (*dvdinput_seek)  (dvd_input_t, int);
int         (*dvdinput_title) (dvd_input_t, int); 
int         (*dvdinput_read)  (dvd_input_t, void *, int, int);
char *      (*dvdinput_error) (dvd_input_t);


/*
#define DVDcss_open(a) dvdcss_open((char*)(a))
#define DVDcss_close   dvdcss_close
#define DVDcss_seek    dvdcss_seek
#define DVDcss_title   dvdcss_title
#define DVDcss_read    dvdcss_read
#define DVDcss_error   dvdcss_error
*/


dvdcss_handle (*DVDcss_open)  (const char *);
int           (*DVDcss_close) (dvdcss_handle);
int           (*DVDcss_seek)  (dvdcss_handle, int, int);
int           (*DVDcss_title) (dvdcss_handle, int); 
int           (*DVDcss_read)  (dvdcss_handle, void *, int, int);
char *        (*DVDcss_error) (dvdcss_handle);

/* The DVDinput handle, add stuff here for new input methods. */
struct dvd_input_s {
  /* libdvdcss handle */
  dvdcss_handle dvdcss;
  
  /* dummy file input */
  int fd;
};


/**
 * initialize and open a DVD device or file.
 */
static dvd_input_t css_open(const char *target)
{
  dvd_input_t dev;
    
  /* Allocate the handle structure */
  dev = (dvd_input_t) malloc(sizeof(dvd_input_t));
  if(dev == NULL) {
    fprintf(stderr, "libdvdread: Could not allocate memory.\n");
    return NULL;
  }
  
  /* Really open it with libdvdcss */
  dev->dvdcss = DVDcss_open(target);
  if(dev->dvdcss == 0) {
    fprintf(stderr, "libdvdread: Could not open %s with libdvdcss.\n", target);
    free(dev);
    return NULL;
  }
  
  return dev;
}

/**
 * return the last error message
 */
static char *css_error(dvd_input_t dev)
{
  return DVDcss_error(dev->dvdcss);
}

/**
 * seek into the device.
 */
static int css_seek(dvd_input_t dev, int blocks)
{
  /* DVDINPUT_NOFLAGS should match the DVDCSS_NOFLAGS value. */
  return DVDcss_seek(dev->dvdcss, blocks, DVDINPUT_NOFLAGS);
}

/**
 * set the block for the begining of a new title (key).
 */
static int css_title(dvd_input_t dev, int block)
{
  return DVDcss_title(dev->dvdcss, block);
}

/**
 * read data from the device.
 */
static int css_read(dvd_input_t dev, void *buffer, int blocks, int flags)
{
  return DVDcss_read(dev->dvdcss, buffer, blocks, flags);
}

/**
 * close the DVD device and clean up the library.
 */
static int css_close(dvd_input_t dev)
{
  int ret;

  ret = DVDcss_close(dev->dvdcss);

  if(ret < 0)
    return ret;

  free(dev);

  return 0;
}






/**
 * initialize and open a DVD device or file.
 */
static dvd_input_t file_open(const char *target)
{
  dvd_input_t dev;
  
  /* Allocate the library structure */
  dev = (dvd_input_t) malloc(sizeof(dvd_input_t));
  if(dev == NULL) {
    fprintf(stderr, "libdvdread: Could not allocate memory.\n");
    return NULL;
  }
  
  /* Open the device */
  dev->fd = open(target, O_RDONLY|O_BINARY);
  if(dev->fd < 0) {
    perror("libdvdread: Could not open input");
    free(dev);
    return NULL;
  }
  
  return dev;
}

/**
 * return the last error message
 */
static char *file_error(dvd_input_t dev)
{
  /* use strerror(errno)? */
  return (char *)"unknown error";
}

/**
 * seek into the device.
 */
static int file_seek(dvd_input_t dev, int blocks)
{
  off_t pos;

  pos = lseek(dev->fd, (long)(off_t)blocks * (off_t)DVD_VIDEO_LB_LEN, SEEK_SET);
  if(pos < 0) {
      return (int)pos;
  }
  /* assert pos % DVD_VIDEO_LB_LEN == 0 */
  return (int) (pos / DVD_VIDEO_LB_LEN);
}

/**
 * set the block for the begining of a new title (key).
 */
static int file_title(dvd_input_t dev, int block)
{
  return -1;
}

/**
 * read data from the device.
 */
static int file_read(dvd_input_t dev, void *buffer, int blocks, int flags)
{
  size_t len;
  ssize_t ret;
  
  len = (size_t)blocks * DVD_VIDEO_LB_LEN;
  
  while(len > 0) {
    
    ret = read(dev->fd, buffer, len);
    
    if(ret < 0) {
      /* One of the reads failed, too bad.  We won't even bother
       * returning the reads that went ok, and as in the posix spec
       * the file postition is left unspecified after a failure. */
      return ret;
    }
    
    if(ret == 0) {
      /* Nothing more to read.  Return the whole blocks, if any, that we got.
	 and adjust the file possition back to the previous block boundary. */
      size_t bytes = (size_t)blocks * DVD_VIDEO_LB_LEN - len;
      off_t over_read = -((signed)bytes % DVD_VIDEO_LB_LEN);
      /*off_t pos =*/ lseek(dev->fd, (long)over_read, SEEK_CUR);
      /* should have pos % 2048 == 0 */
      return (int) (bytes / DVD_VIDEO_LB_LEN);
    }
    
    len -= ret;
  }

  return blocks;
}

/**
 * close the DVD device and clean up.
 */
static int file_close(dvd_input_t dev)
{
  int ret;

  ret = close(dev->fd);

  if(ret < 0)
    return ret;

  free(dev);

  return 0;
}


/**
 * Setup read functions with either libdvdcss or minimal DVD access.
 */
int dvdinput_setup(void)
{
    DVDcss_open = dvdcss_open;
    DVDcss_close = dvdcss_close;
    DVDcss_title = dvdcss_title;
    DVDcss_seek = dvdcss_seek;
    DVDcss_read = dvdcss_read;
    DVDcss_error = dvdcss_error;

    /*
    char *psz_method = getenv( "DVDCSS_METHOD" );
    char *psz_verbose = getenv( "DVDCSS_VERBOSE" );
    fprintf(stderr, "DVDCSS_METHOD %s\n", psz_method);
    fprintf(stderr, "DVDCSS_VERBOSE %s\n", psz_verbose);
    */
    //    fprintf(stderr, "libdvdread: Using libdvdcss version %s for DVD access\n",
    //	    *dvdcss_version);

    /* libdvdcss wrapper functions */
    dvdinput_open  = css_open;
    dvdinput_close = css_close;
    dvdinput_seek  = css_seek;
    dvdinput_title = css_title;
    dvdinput_read  = css_read;
    dvdinput_error = css_error;
    return 1;

}
