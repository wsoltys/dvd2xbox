/*****
 *
 * This file is part of libcss.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *****/

#if defined(__NetBSD__) || defined(__OpenBSD__)
# include <sys/dvdio.h>
#elif defined(__linux__)
# include <linux/cdrom.h>
#elif defined(__sun)
# include <sun/dvdio.h>
#else
#include "dvdio.h"
//# error "Need the DVD ioctls"
#endif

#if defined(__sun) && defined(__svr4__)
#define DVD     "/vol/dev/aliases/cdrom0"
#elif defined(__FreeBSD__)
typedef struct dvd_authinfo dvd_authinfo;
# define DVD "/dev/acd0c"
#elif defined (_XBOX)
# define DVD "D:\\"
#else
# define DVD "/dev/cdrom"
#endif

typedef struct dvd_device dvd_device_t;

extern dvd_device_t *DVDOpenDevice(char *dev_name);
//extern dvd_device_t *DVDOpenDeviceFD(int dev_fd);
extern int DVDCloseDevice(dvd_device_t *dvd);

extern int DVDReadStruct(dvd_device_t *dvd, dvd_struct *s);
extern int DVDAuth(dvd_device_t *dvd, dvd_authinfo *a);

