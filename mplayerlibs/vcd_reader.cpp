#ifdef __cplusplus
extern "C" {
#endif

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "mp_msg.h"

	/* CD-ROM address types (cdrom_tocentry.cdte_format) */
#define VCD_SECTOR_DATA 2324
#define VCD_SECTOR_OFFS 24
#define VCD_SECTOR_SIZE 2352
#define	CDROM_LEADOUT	0xAA
#define CDROM_LBA 0x01 		/* logical block: first frame is #0 */
#define CDROM_MSF 0x02 		/* minute-second-frame: binary. not bcd here!*/
#define CDROMREADTOCHDR		0x5305 /* Read TOC header (cdrom_tochdr) */
#define CDROMREADTOCENTRY	0x5306 /* Read TOC entry (cdrom_tocentry) */
#define CD_HEAD_SIZE          4 /* header (address) bytes per raw data frame */
#define CD_SYNC_SIZE         12 /* 12 sync bytes per raw data frame */
#define CD_FRAMESIZE       2048 /* bytes per frame, "cooked" mode */
#define CD_FRAMESIZE_RAW   2352 /* bytes per frame, "raw" mode */
#define CD_FRAMESIZE_RAW0 (CD_FRAMESIZE_RAW-CD_SYNC_SIZE-CD_HEAD_SIZE) /*2336*/
#define CD_FRAMESIZE_RAW1 (CD_FRAMESIZE_RAW-CD_SYNC_SIZE) 	/*2340*/

static char vcd_buf[VCD_SECTOR_SIZE];

	struct cdrom_msf 
{
	unsigned char	cdmsf_min0;	/* start minute */
	unsigned char	cdmsf_sec0;	/* start second */
	unsigned char	cdmsf_frame0;	/* start frame */
	unsigned char	cdmsf_min1;	/* end minute */
	unsigned char	cdmsf_sec1;	/* end second */
	unsigned char	cdmsf_frame1;	/* end frame */
};

struct	cdrom_tochdr 	
	{
	unsigned char	cdth_trk0;	/* start track */
	unsigned char	cdth_trk1;	/* end track */
	};

struct cdrom_msf0		
{
	unsigned char	minute;
	unsigned char	second;
	unsigned char	frame;
};

union cdrom_addr		
{
	struct cdrom_msf0	msf;
	int			lba;
};

struct cdrom_tocentry 
{
	unsigned char	cdte_track;
	unsigned char	cdte_adr	;
	unsigned char	cdte_ctrl	;
	unsigned char	cdte_format;
	union cdrom_addr cdte_addr;
	unsigned char	cdte_datamode;
};

static struct cdrom_tocentry vcd_entry;


//******************************************************************************************
int read_sectortoc(struct cdrom_tocentry* entry)
{
	//CDROMREADTOCHDR
	return -1;
}

//******************************************************************************************
int read_tochdr(struct cdrom_tochdr *tochdr)
{
	//CDROMREADTOCHDR
	return -1;
}

//******************************************************************************************
int read_sectorraw(char *pbuffer)
{
	//CDROMREADRAW
	return -1;
}
						
//******************************************************************************************
unsigned int vcd_get_msf()
{
    return vcd_entry.cdte_addr.msf.frame +
           (vcd_entry.cdte_addr.msf.second+
            vcd_entry.cdte_addr.msf.minute*60)*75;
}

//******************************************************************************************
void vcd_set_msf(unsigned int sect)
{
    vcd_entry.cdte_addr.msf.frame=sect%75;
    sect=sect/75;
    vcd_entry.cdte_addr.msf.second=sect%60;
    sect=sect/60;
    vcd_entry.cdte_addr.msf.minute=sect;

}

//******************************************************************************************
void vcd_read_toc(int fd)
{
    struct cdrom_tochdr tochdr;
    int i;
    if (read_tochdr(&tochdr)==-1) //CDROMREADTOCHDR
    {
        mp_msg(0,0,"read CDROM toc header: ");
        return;
    }
    for (i=tochdr.cdth_trk0 ; i<=tochdr.cdth_trk1 ; i++)
    {
        struct cdrom_tocentry tocentry;

        tocentry.cdte_track  = i;
        tocentry.cdte_format = CDROM_MSF;

        if (read_sectortoc(&tocentry)==-1)//CDROMREADTOCHDR
				{
            mp_msg(0,0,"read CDROM toc entry: ");
            return;
        }

        mp_msg(MSGT_OPEN,MSGL_INFO,"track %02d:  adr=%d  ctrl=%d  format=%d  %02d:%02d:%02d  mode: %d\n",
               (int)tocentry.cdte_track,
               (int)tocentry.cdte_adr,
               (int)tocentry.cdte_ctrl,
               (int)tocentry.cdte_format,
               (int)tocentry.cdte_addr.msf.minute,
               (int)tocentry.cdte_addr.msf.second,
               (int)tocentry.cdte_addr.msf.frame,
               (int)tocentry.cdte_datamode
              );
    }
}

//******************************************************************************************
int vcd_get_track_end(int fd,int track)
{
  struct cdrom_tochdr tochdr;
  if (read_tochdr(&tochdr)==-1)									//CDROMREADTOCHDR
  {
      mp_msg(0,0,"read CDROM toc header: ");
      return -1;
  }
  vcd_entry.cdte_format = CDROM_MSF;
  vcd_entry.cdte_track  = track < tochdr.cdth_trk1?(track+1) : CDROM_LEADOUT;
  if (read_sectortoc(&vcd_entry))						//CDROMREADTOCENTRY
  {
      mp_msg(0,0,"ioctl dif2");
      return -1;
  }
  return VCD_SECTOR_DATA*vcd_get_msf();
}

//******************************************************************************************
int  vcd_seek_to_track(int fd,int track)
{

  vcd_entry.cdte_format = CDROM_MSF;
  vcd_entry.cdte_track  = track;
  if (!read_sectortoc(&vcd_entry)) return -1; //CDROMREADTOCENTRY
  return VCD_SECTOR_DATA*vcd_get_msf();
}

//******************************************************************************************
int vcd_read(int fd,char *mem)
{
    memcpy(vcd_buf,&vcd_entry.cdte_addr.msf,sizeof(struct cdrom_msf));
    if(read_sectorraw(vcd_buf)==-1) //CDROMREADRAW
        return 0; // EOF?
    memcpy(mem,&vcd_buf[VCD_SECTOR_OFFS],VCD_SECTOR_DATA);

    vcd_entry.cdte_addr.msf.frame++;
    if (vcd_entry.cdte_addr.msf.frame==75)
    {
        vcd_entry.cdte_addr.msf.frame=0;
        vcd_entry.cdte_addr.msf.second++;
        if (vcd_entry.cdte_addr.msf.second==60)
        {
            vcd_entry.cdte_addr.msf.second=0;
            vcd_entry.cdte_addr.msf.minute++;
        }
    }

    return VCD_SECTOR_DATA;
}

//******************************************************************************************
void vcd_close(int fd)
{
}

//******************************************************************************************
int vcd_open(char* szDevice ,int iWhence)
{
	return -1;
}


#ifdef __cplusplus
}
#endif