#include <xtl.h>

#include "../CDExtract.h"
#include "../Undocumented.h"

#define DriveD "\\??\\D:"
#define CdRom "\\Device\\Cdrom0"
STRING DDeviceName = { strlen(CdRom), strlen(CdRom)+1, CdRom };
STRING DSymbolicLinkName = { strlen(DriveD), strlen(DriveD)+1, DriveD };

// Interface for Monty's paranoia library:
// return the number of sectors?

long cdda_read(cdrom_drive *d, void *buffer, long beginsector, long sectors)
{
	int nRetries = 0;

	CCDExtract *cdr = (CCDExtract*)d->cdr;

	// when 100 sectors have be read without a problem, revert to default speed
	if (  ( ( beginsector - cdr->GetLastSectorSpeedAdjusted() ) > 500  ) &&
			(-1 != cdr->GetLastSectorSpeedAdjusted() ) )
	{
		if ( cdr->GetCurrentSpeed() != cdr->GetSpeed()  )
		{
			cdr->SetCDSpeed(  );
			cdr->GetLastSectorSpeedAdjusted( -1 );
		}

	}

	if ( ( FALSE == cdr->ReadCdRomSector( (BYTE*)buffer, beginsector, sectors, FALSE ) ) 
			&& ( nRetries< 10 ) )
	{
		// read error, reduce spead if possible
		int nSpeed = cdr->GetCurrentSpeed();

		if ( nSpeed > 1)
		{
			::Sleep(1000);
			cdr->GetLastSectorSpeedAdjusted( beginsector );
			cdr->SetCDSpeed( 1 );

		}

		// do a dummy read at the begin sector to flush cache buffer
		cdr->ReadCdRomSector( (BYTE*)buffer, cdr->GetBeginSector(), sectors, FALSE );

//		nRetries++;
		sectors = -1;
	}

	/* return -999 incase the abort button has been pressed */
	if ( cdr->GetAbort() )
	{
		sectors = -999;
	}
	
  return sectors;
}

//	ADDED BOBBIN007

cdrom_drive *cdda_identify(const char *device, int messagedest,char **messages){
  cdrom_drive *d=NULL;

	IoDeleteSymbolicLink(&DSymbolicLinkName);
	IoCreateSymbolicLink(&DSymbolicLinkName, &DDeviceName);

	CDRomSettings::SetIniFileName( "test.ini" );
  d = (cdrom_drive*)malloc( sizeof( cdrom_drive ) );
	d->cdr = (void*) new CCDExtract;
  return(d);
  
}
cdrom_drive *cdda_identify_scsi(const char *generic_device, 
				const char *ioctl_device, int messagedest,
				char **messages){
  cdrom_drive *d=NULL;

  d = (cdrom_drive*)malloc( sizeof( cdrom_drive ) );
	d->cdr = (void*) new CCDExtract;

  d->nsectors=-1;
  return(d);
  
}

int cdda_track_bitmap(cdrom_drive *d,int track,int bit,int set,int clear){
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }

  if (track == 0)
    track = 1; /* map to first track number */

  if(track<1 || track>d->tracks){
    OutputDebugString("401: Invalid track number\n");
    return(-1);
  }
  if ((d->disc_toc[track-1].bFlags & bit))
    return(set);
  else
    return(clear);
}


int cdda_track_channels(cdrom_drive *d,int track){
  return(cdda_track_bitmap(d,track,8,4,2));
}

int cdda_track_audiop(cdrom_drive *d,int track){
  return(cdda_track_bitmap(d,track,4,0,1));
}

int cdda_track_copyp(cdrom_drive *d,int track){
  return(cdda_track_bitmap(d,track,2,1,0));
}

int cdda_track_preemp(cdrom_drive *d,int track){
  return(cdda_track_bitmap(d,track,1,1,0));
}

/* doubles as "cdrom_drive_free()" */
int cdda_close(cdrom_drive *d){
  if(d){
    CCDExtract *cdr = (CCDExtract*)d->cdr;
    if(cdr)
      cdr->EnableCdda( FALSE ); //->enable_cdda(d,0);

/*    _clean_messages(d);
    if(d->cdda_device_name)free(d->cdda_device_name);
    if(d->ioctl_device_name)free(d->ioctl_device_name);
    if(d->drive_model)free(d->drive_model);
    if(d->cdda_fd!=-1)close(d->cdda_fd);
    if(d->ioctl_fd!=-1 && d->ioctl_fd!=d->cdda_fd)close(d->ioctl_fd);
    if(d->sg)free(d->sg);
*/    
	delete cdr;
	free(d);
  }
  CR_DeInit();
  IoDeleteSymbolicLink(&DSymbolicLinkName);
  return(0);
}

/* finish initializing the drive! */
int cdda_open(cdrom_drive *d){
  int ret;
  if(!d)return(0);

  CCDExtract *cdr = (CCDExtract*)d->cdr;

	// Check if low level CD-ROM drivers are intialized properly
	if (cdr->IsAvailable() )
	{
		// Obtain the specs of the SCSI devices and select the proper CD Device
		cdr->GetCDRomDevices();
	}
	else
	{
		cdr->Clear();
		delete cdr;
		cdr=NULL;
		return CDEX_ERROR;
	}

	if (CR_GetNumCDROM()<1)
	{
		cdr->Clear();
		delete cdr;
		cdr=NULL;
		return CDEX_ERROR;
	}

	// Set drive zero as default
	cdr->SetActiveCDROM( 0 );

	cdr->SetCDSpeed( 2 );

	CR_LoadSettings();
	CR_SaveSettings();

	if ( cdr->ReadToc() != CDEX_OK )
	  return -100;

  CToc toc = cdr->GetToc();
  d->tracks = toc.GetNumTracks();

  for (int i = 0; i <= d->tracks; i++ ) {
	d->disc_toc[i].dwStartSector = toc.GetStartSector( i );
	d->disc_toc[i].bFlags = toc.GetFlags( i );
	d->disc_toc[i].bTrack = toc.GetTrackNumber( i );
  }
/*  switch(d->interface){
  case GENERIC_SCSI:  
    if((ret=scsi_init_drive(d)))
      return(ret);
    break;
  case COOKED_IOCTL:  
    if((ret=cooked_init_drive(d)))
      return(ret);
    break;
#ifdef CDDA_TEST
  case TEST_INTERFACE:  
    if((ret=test_init_drive(d)))
      return(ret);
    break;
#endif
  default:
    cderror(d,"100: Interface not supported\n");
    return(-100);
  }
 */ 
  /* Check TOC, enable for CDDA */
  
  /* Some drives happily return a TOC even if there is no disc... */
  {
    int i;
    for(i=0;i<d->tracks;i++)
      if(d->disc_toc[i].dwStartSector<0 ||
	 d->disc_toc[i+1].dwStartSector==0){
	d->opened=0;
//	cderror(d,"009: CDROM reporting illegal table of contents\n");
	return(-9);
      }
  }

  cdr->EnableCdda( TRUE );
  d->opened = 1;
  d->nsectors = cdr->GetNumReadSectors();
  /*  d->select_speed(d,d->maxspeed); most drives are full speed by default */
//  if(d->bigendianp==-1)d->bigendianp=data_bigendianp(d);
  return(0);
}

int cdda_speed_set(cdrom_drive *d, int speed)
{
  CCDExtract *cdr = (CCDExtract*)d->cdr;
  cdr->SetSpeed( 2 );
  return 1;
}

void cdda_verbose_set(cdrom_drive *d,int err_action, int mes_action){
/*  d->messagedest=mes_action;
  d->errordest=err_action;*/
}

long cdda_track_firstsector(cdrom_drive *d,int track){
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }

  if (track == 0) {
    if (d->disc_toc[0].dwStartSector == 0) {
      /* first track starts at lba 0 -> no pre-gap */
      OutputDebugString("401: Invalid track number\n");
      return(-1);
    }
    else {
      return 0; /* pre-gap of first track always starts at lba 0 */
    }
  }

  if(track<0 || track>d->tracks){
    OutputDebugString("401: Invalid track number\n");
    return(-1);
  }
  return(d->disc_toc[track-1].dwStartSector);
}

long cdda_disc_firstsector(cdrom_drive *d){
  int i;
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }

  /* look for an audio track */
  for(i=0;i<d->tracks;i++)
    if(cdda_track_audiop(d,i+1)==1) {
      if (i == 0) /* disc starts at lba 0 if first track is an audio track */
       return 0;
      else
       return(cdda_track_firstsector(d,i+1));
    }

  OutputDebugString("403: No audio tracks on disc\n");  
  return(-1);
}

long cdda_track_lastsector(cdrom_drive *d,int track){
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }

  if (track == 0) {
    if (d->disc_toc[0].dwStartSector == 0) {
      /* first track starts at lba 0 -> no pre-gap */
      OutputDebugString("401: Invalid track number\n");
      return(-1);
    }
    else {
      return d->disc_toc[0].dwStartSector-1;
    }
  }

  if(track<1 || track>d->tracks){
    OutputDebugString("401: Invalid track number\n");
    return(-1);
  }
  /* Safe, we've always the leadout at disc_toc[tracks] */
  return(d->disc_toc[track].dwStartSector-1);
}

long cdda_disc_lastsector(cdrom_drive *d){
  int i;
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }

  /* look for an audio track */
  for(i=d->tracks-1;i>=0;i--)
    if(cdda_track_audiop(d,i+1)==1)
      return(cdda_track_lastsector(d,i+1));

  OutputDebugString("403: No audio tracks on disc\n");  
  return(-1);
}

long cdda_tracks(cdrom_drive *d){
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }
  return(d->tracks);
}

int cdda_sector_gettrack(cdrom_drive *d,long sector){
  if(!d->opened){
    OutputDebugString("400: Device not open\n");
    return(-1);
  }else{
    int i;

    if (sector < d->disc_toc[0].dwStartSector)
      return 0; /* We're in the pre-gap of first track */

    for(i=0;i<d->tracks;i++){
      if(d->disc_toc[i].dwStartSector<=sector &&
	 d->disc_toc[i+1].dwStartSector>sector)
	return (i+1);
    }

    OutputDebugString("401: Invalid track number\n");
    return -1;
  }
}

void setFirstLastCDExtract(cdrom_drive *d, long firstsector,long lastsector ) {
	CCDExtract* pExtr = (CCDExtract*) d->cdr;
	pExtr->SetupTrackExtract( firstsector, lastsector + 1 );
}

//	END BOBBIN007