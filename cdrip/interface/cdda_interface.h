/******************************************************************
 * CopyPolicy: GNU Public License 2 applies
 * Copyright (C) 1998 Monty xiphmont@mit.edu
 * and Heiko Eissfeldt heiko@escape.colossus.de
 *
 * Toplevel interface header; applications include this
 *
 ******************************************************************/

#ifndef _cdda_interface_h_
#define _cdda_interface_h_

#ifdef _XBOX
#include <xtl.h>
#endif

#include <sys/types.h>
#include <signal.h>

#define int32_t int

#ifndef CD_FRAMESIZE
#define CD_FRAMESIZE 2048
#endif
#ifndef CD_FRAMESIZE_RAW
#define CD_FRAMESIZE_RAW 2352
#endif
#define CD_FRAMESAMPLES (CD_FRAMESIZE_RAW / 4)

//ADDED BOBBIN007
#define CDDA_MESSAGE_FORGETIT 0

#define MAXTRK 100

#ifndef TOC_TAG
#define TOC_TAG
typedef struct TOC_TAG 
{
  BYTE	_reserved1;
  BYTE	bFlags;
  BYTE	bTrack;
  BYTE	_reserved2;
  DWORD	dwStartSector;
} TOC;
#endif

typedef struct cdrom_drive{
  int opened;
  long nsectors; /* number of sectors that can be read at once */
  void *cdr;     /* pointer to a CDExtract object */
  long bigbuff;
  int tracks;
  TOC disc_toc[MAXTRK];
} cdrom_drive;


#ifdef __cplusplus
extern "C" {
#endif

extern int cdda_speed_set(cdrom_drive *d, int speed);
extern void cdda_verbose_set(cdrom_drive *d,int err_action, int mes_action);
extern long cdda_read(cdrom_drive *d, void *buffer,
		       long beginsector, long sectors);
extern cdrom_drive *cdda_identify(const char *device, int messagedest,
				  char **message);
extern cdrom_drive *cdda_identify_scsi(const char *generic_device, 
				       const char *ioctl_device,
				       int messagedest, char **message);

extern int cdda_close(cdrom_drive *d);
extern int cdda_open(cdrom_drive *d);
extern long cdda_read(cdrom_drive *d, void *buffer,
		       long beginsector, long sectors);

extern long cdda_track_firstsector(cdrom_drive *d,int track);
extern long cdda_track_lastsector(cdrom_drive *d,int track);
extern long cdda_tracks(cdrom_drive *d);
extern long cdda_disc_firstsector(cdrom_drive *d);
extern long cdda_disc_lastsector(cdrom_drive *d);

extern int cdda_sector_gettrack(cdrom_drive *d,long sector);

extern int cdda_track_bitmap(cdrom_drive *d,int track,int bit,int set,int clear);
extern int cdda_track_channels(cdrom_drive *d,int track);
extern int cdda_track_audiop(cdrom_drive *d,int track);
extern int cdda_track_copyp(cdrom_drive *d,int track);
extern int cdda_track_preemp(cdrom_drive *d,int track);

extern void setFirstLastCDExtract(cdrom_drive *d, long firstsector,long lastsector );
#ifdef __cplusplus
}
#endif
//	END BOBBIN007
#endif

