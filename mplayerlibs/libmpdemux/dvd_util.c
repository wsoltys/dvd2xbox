#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "config.h"


#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
//#include <sys/ioctl.h>

#include "dvd_util.h"
#pragma warning (disable:4244)
#pragma warning (disable:4018)

#if defined(__linux__)

#include <fcntl.h>
#include <linux/fs.h>

#ifndef FIBMAP
#define FIBMAP 1
#endif

int DVDPath2LBA(char *path)
{
	int fd, lba = 0;

	if ((fd = open(path, O_RDONLY)) == -1) {
		perror("DVD vob file");
		return -1;
	}

#ifdef __FreeBSD__
	if (ioctl (fd, FIOGETLBA, &lba)) {
		perror ("ioctl FIOGETLBA");
#else
	if (ioctl(fd, FIBMAP, &lba) != 0) {
		perror ("ioctl FIBMAP");
#endif
		close(fd);
		return -1;
	}

	close(fd);

	return lba;
}
#elif defined(__sun) && defined(__svr4__)

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>

static unsigned long iso9660_get32(unsigned char *s)
{
    return s[0] + (s[1]<<8) + (s[2]<<16) + (s[3]<<24);
}


/*
 * Return LBA for first sector of a file on a DVD.
 * 
 * We assume that the file is located on a DVD using the
 * ISO9660 filesystem.  The file's first sector is extracted
 * from the extent location from the file's parent directory.
 */
int DVDPath2LBA (char *path)
{
    char *p_dup = strdup(path);
    char *dir, *file;
    char *s;
    struct stat stb;
    FILE *dir_f;
    int lbs = 2048;
    unsigned char buf[256];
    char iso_filename[256];
    int c;
    int lba = -1;

    /* first make sure that the file is really on an iso9660 fs */
    if (stat(path, &stb)) {
	fprintf(stderr, "DVD vob file (%s): %s", path, strerror(errno));
	return -1;
    }
    if (strcmp(stb.st_fstype, "hsfs")) {
	fprintf(stderr, "DVD vob file %s not on iso9660 filesystem (%s)\n",
		path, stb.st_fstype);
	return -1;
    }

    /* construct path name for the file parent dir */
    file = strrchr(p_dup, '/');
    if (file) {
	*file++ = 0;
	dir = p_dup;
    } else {
	dir = ".";
	file = p_dup;
    }

    /* iso9660 stores filenames in upper case on the media */
    for (s = file; *s; s++)
	if (islower(*s & 0xff)) 
	    *s = toupper(*s & 0xff);

    /*
     * scan the file's parent directory, try to find the iso9660
     * directory entry for the given path
     */
    dir_f = fopen(dir, "r");
    if (dir_f == NULL) {
	fprintf(stderr, "cannot open iso9660 parent dir %s: %s",
		dir, strerror(errno));
    } else {
	
	while ((c = fgetc(dir_f)) != EOF) {
	    off_t pos = ftell(dir_f);

	    if (!c) {
		/* no more dirents in this block, skip to next block */
		if ((pos&(lbs-1)) != 0
		    && fseek(dir_f, lbs - (pos&(lbs-1)), 1) == EOF) {
		    perror("fseek in iso9660 parent dir");
		    break;
		}
		continue;
	    }
	    buf[0] = c;
	    if (fread(buf+1, buf[0]-1, 1, dir_f) != 1) {
		fprintf(stderr, "dir read error\n");
		break;
	    }

	    /* skip . and .. */
	    if (buf[32] == 1 && buf[33] <= 1)
		continue;

	    memcpy(iso_filename, buf+33, buf[32]);
	    iso_filename[buf[32]] = 0;
	    /* printf("filename: %.*s\n", buf[32], buf+33); */

	    /* ignore file version information */
	    if ((s = strrchr(iso_filename, ';')) != NULL)
		*s = 0;
	    if (strcmp(file, iso_filename) == 0) {
		/*
		 * directory entry for file found, extract the
		 * extent location from it 
		 */
		lba = iso9660_get32(buf+2);
		/* printf("found it, lba: %d\n", lba); */
		break;
	    }
	}
		
	fclose(dir_f);
    }
    free(p_dup);

    return lba;
}
#else

#include <errno.h>

#define DVD_UDF_VERSION 19991124


/***********************************************************************************/
/* The length of one Logical Block of a DVD Video                                  */
/***********************************************************************************/
#define DVD_VIDEO_LB_LEN 2048

/***********************************************************************************/
/* reads Logical Block of the disc or image                                        */
/*   lb_number: disc-absolute logical block number                                 */
/*   block_count: number of 2048 byte blocks to read                               */
/*   data: pointer to enough allocated memory                                      */
/*   returns number of read bytes on success, 0 on error                           */
/***********************************************************************************/
int _UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data);

/***********************************************************************************/
/* looks for a file on the UDF disc/imagefile                                      */
/*   filename: absolute pathname on the UDF filesystem, starting with '/'          */
/*   returns absolute LB number, or 0 on error                                     */
/***********************************************************************************/
unsigned long int _UDFFindFile(char *filename);

/***********************************************************************************/
/* Initializes the CSS process with the drive                                      */
/* returns positive AGID on success, -1 on error, -2 if no CSS support in CD-ROM driver */
/***********************************************************************************/
int _UDFCSSRequestAGID(void);

/***********************************************************************************/
/* Post host challenge (10 bytes) into drive                                       */
/* and retreive drive response (5 bytes) for host                                  */
/* returns 0 on success, -1 on error, -2 if no CSS support in CD-ROM driver        */
/***********************************************************************************/
int _UDFCSSDriveAuth(char *data);

/***********************************************************************************/
/* Retreive drive challenge (10 bytes)                                             */
/* returns 0 on success, -1 on error, -2 if no CSS support in CD-ROM driver        */
/***********************************************************************************/
int _UDFCSSHostAuthChallenge(char *data);

/***********************************************************************************/
/* Post host response (5 bytes) into drive                                         */
/* returns 0 on success, -1 on error, -2 if no CSS support in CD-ROM driver        */
/***********************************************************************************/
int _UDFCSSHostAuthResponse(char *data);

/***********************************************************************************/
/* Retreive disc key (2048 byte) from drive                                        */
/* returns 0 on success, -1 on error, -2 if no CSS support in CD-ROM driver        */
/***********************************************************************************/
int _UDFCSSDiscKey(char *data);

/***********************************************************************************/
/* Retreive title key (5 byte) from drive                                          */
/* lba: absolute number of logical block containing the title key                  */
/* returns 0 on success, -1 on error, -2 if no CSS support in CD-ROM driver        */
/***********************************************************************************/
int _UDFCSSTitleKey(unsigned long int lba, char *data);

/***********************************************************************************/
/* opens block device or image file                                                */
/*   filename: path to the DVD ROM block device or to the image file in UDF format */
/*   returns fileno() of the file on success, or -1 on error                       */
/***********************************************************************************/
int _UDFOpenDisc(char *filename);

#ifndef u8
#define u8 unsigned char
#endif

#ifndef u16
#define u16 unsigned int
#endif

#ifndef u32
#define u32 unsigned long int
#endif

#ifndef u64
#define u64 unsigned long long int
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define MAX_FILE_LEN 2048

HANDLE dvdromfile=NULL;  // CD/DVD-ROM block device or image file

int __cdecl stricmp(const char *s1, const char *s2)
{
    while(*s1)
        if(*s1++ != *s2++)
            return -1;
    return 0;
}

struct Partition {
  int valid;
  char VolumeDesc[128];
  u16 Flags;
  u16 Number;
  char Contents[32];
  u32 AccessType;
  u32 Start;
  u32 Length;
} partition;

struct AD {
  u32 Location;
  u32 Length;
  u8 Flags;
  u16 Partition;
};

// for direct data access, LSB first
#define GETN1(p) ((u8)data[p])
#define GETN2(p) ((u16)data[p]|((u16)data[(p)+1]<<8))
#define GETN3(p) ((u32)data[p]|((u32)data[(p)+1]<<8)|((u32)data[(p)+2]<<16))
#define GETN4(p) ((u32)data[p]|((u32)data[(p)+1]<<8)|((u32)data[(p)+2]<<16)|((u32)data[(p)+3]<<24))
#define GETN(p,n,target) memcpy(target,&data[p],n)

// reads absolute Logical Block of the disc
// returns number of read bytes on success, 0 on error
int _UDFReadLB(unsigned long int lb_number, unsigned int block_count, unsigned char *data) {
  LONGLONG pos;
  ULONG offsetHigh;
  ULONG bytesRead;

  if (dvdromfile==NULL) return 0;
  pos=(LONGLONG)lb_number*(LONGLONG)DVD_VIDEO_LB_LEN;
      
  offsetHigh = (ULONG)(pos>>32);
  if(SetFilePointer(
        (HANDLE)dvdromfile,(ULONG)pos, &offsetHigh, FILE_BEGIN) == (unsigned long)(-1) 
        && GetLastError())
    return 0;
  if(!ReadFile((HANDLE)dvdromfile, data, block_count*DVD_VIDEO_LB_LEN,&bytesRead,0))
    bytesRead = 0;
  return bytesRead;
}

int Unicodedecode(u8 *data, int len, char *target) {
  int p=1,i=0;
  if ((data[0]==8) || (data[0]==16)) do {
    if (data[0]==16) p++;  // ignore MSB of unicode16
    if (p<len) {
      target[i++]=data[p++];
    }
  } while (p<len);
  target[i]='\0';
  return 0;
}

int _UDFEntity(u8 *data, u8 *Flags, char *Identifier) {
  Flags[0]=data[0];
  strncpy(Identifier,&data[1],5);
  return 0;
}

int _UDFDescriptor(u8 *data, u16 *TagID) {
  TagID[0]=GETN2(0);
  // TODO: check CRC 'n stuff
  return 0;
}

int _UDFExtentAD(u8 *data, u32 *Length, u32 *Location) {
  Length[0]  =GETN4(0);
  Location[0]=GETN4(4);
  return 0;
}

int _UDFShortAD(u8 *data, struct AD *ad) {
  ad->Length=GETN4(0);
  ad->Flags=ad->Length>>30;
  ad->Length&=0x3FFFFFFF;
  ad->Location=GETN4(4);
  ad->Partition=partition.Number;  // use number of current partition
  return 0;
}

int _UDFLongAD(u8 *data, struct AD *ad) {
  ad->Length=GETN4(0);
  ad->Flags=ad->Length>>30;
  ad->Length&=0x3FFFFFFF;
  ad->Location=GETN4(4);
  ad->Partition=GETN2(8);
  //GETN(10,6,Use);
  return 0;
}

int _UDFExtAD(u8 *data, struct AD *ad) {
  ad->Length=GETN4(0);
  ad->Flags=ad->Length>>30;
  ad->Length&=0x3FFFFFFF;
  ad->Location=GETN4(12);
  ad->Partition=GETN2(16);
  //GETN(10,6,Use);
  return 0;
}

int _UDFICB(u8 *data, u8 *FileType, u16 *Flags) {
  FileType[0]=GETN1(11);
  Flags[0]=GETN2(18);
  return 0;
}


int _UDFPartition(u8 *data, u16 *Flags, u16 *Number, char *Contents, u32 *Start, u32 *Length) {
  Flags[0]=GETN2(20);
  Number[0]=GETN2(22);
  GETN(24,32,Contents);
  Start[0]=GETN4(188);
  Length[0]=GETN4(192);
  return 0;
}

// reads the volume descriptor and checks the parameters
// returns 0 on OK, 1 on error
int _UDFLogVolume(u8 *data, char *VolumeDescriptor) {
  u32 lbsize,MT_L,N_PM;
  //u8 type,PM_L;
  //u16 sequence;
  //int i,p;
  Unicodedecode(&data[84],128,VolumeDescriptor);
  lbsize=GETN4(212);  // should be 2048
  MT_L=GETN4(264);  // should be 6
  N_PM=GETN4(268);  // should be 1
  if (lbsize!=DVD_VIDEO_LB_LEN) return 1;
  /*
  Partition1[0]=0;
  p=440;
  for (i=0; i<N_PM; i++) {
    type=GETN1(p);
    PM_L=GETN1(p+1);
    if (type==1) {
      sequence=GETN2(p+2);
      if (sequence==1) {
        Partition1[0]=GETN2(p+4);
        return 0;
      }
    }
    p+=PM_L;
  }
  return 1;
  */
  return 0;
}

int _UDFFileEntry(u8 *data, u8 *FileType, struct AD *ad) {
  u8 filetype;
  u16 flags;
  u32 L_EA,L_AD;
  int p;

  _UDFICB(&data[16],&filetype,&flags);
  FileType[0]=filetype;
  L_EA=GETN4(168);
  L_AD=GETN4(172);
  p=176+L_EA;
  while (p<176+L_EA+L_AD) {
    switch (flags&0x0007) {
      case 0: _UDFShortAD(&data[p],ad); p+=8;  break;
      case 1: _UDFLongAD(&data[p],ad);  p+=16; break;
      case 2: _UDFExtAD(&data[p],ad);   p+=20; break;
      case 3:
        switch (L_AD) {
          case 8:  _UDFShortAD(&data[p],ad); break;
          case 16: _UDFLongAD(&data[p],ad);  break;
          case 20: _UDFExtAD(&data[p],ad);   break;
        }
        p+=L_AD;
        break;
      default: p+=L_AD; break;
    }
  }
  return 0;
}

int _UDFFileIdentifier(u8 *data, u8 *FileCharacteristics, char *FileName, struct AD *FileICB) {
  u8 L_FI;
  u16 L_IU;
  
  FileCharacteristics[0]=GETN1(18);
  L_FI=GETN1(19);
  _UDFLongAD(&data[20],FileICB);
  L_IU=GETN2(36);
  if (L_FI) Unicodedecode(&data[38+L_IU],L_FI,FileName);
  else FileName[0]='\0';
  return 4*((38+L_FI+L_IU+3)/4);
}

// Maps ICB to FileAD
// ICB: Location of ICB of directory to scan
// FileType: Type of the file
// File: Location of file the ICB is pointing to
// return 1 on success, 0 on error;
int _UDFMapICB(struct AD ICB, u8 *FileType, struct AD *File) {
  u8 LogBlock[DVD_VIDEO_LB_LEN];
  u32 lbnum;
  u16 TagID;

  lbnum=partition.Start+ICB.Location;
  do {
    if (!_UDFReadLB(lbnum++,1,LogBlock)) TagID=0;
    else _UDFDescriptor(LogBlock,&TagID);
    if (TagID==261) {
      _UDFFileEntry(LogBlock,FileType,File);
      //printf("Found File entry type %d at LB %ld, %ld bytes long\n",FileType[0],File->Location,File->Length);
      return 1;
    };
  } while ((lbnum<=partition.Start+ICB.Location+(ICB.Length-1)/DVD_VIDEO_LB_LEN) && (TagID!=261));
  return 0;
}
  
// Dir: Location of directory to scan
// FileName: Name of file to look for
// FileICB: Location of ICB of the found file
// return 1 on success, 0 on error;
int _UDFScanDir(struct AD Dir, char *FileName, struct AD *FileICB) {
  u8 LogBlock[DVD_VIDEO_LB_LEN];
  u32 lbnum;
  u16 TagID;
  u8 filechar;
  char filename[MAX_FILE_LEN];
  int p;
  
  // Scan dir for ICB of file
  lbnum=partition.Start+Dir.Location;
  do {
    if (!_UDFReadLB(lbnum++,1,LogBlock)) TagID=0;
    else {
      p=0;
      while (p<DVD_VIDEO_LB_LEN) {
        _UDFDescriptor(&LogBlock[p],&TagID);
        if (TagID==257) {
          p+=_UDFFileIdentifier(&LogBlock[p],&filechar,filename,FileICB);
          //printf("Found ICB for file '%s' at LB %ld, %ld bytes long\n",filename,FileICB->Location,FileICB->Length);
          if (!stricmp(FileName,filename)) return 1;
        } else p=DVD_VIDEO_LB_LEN;
      }
    }
  } while (lbnum<=partition.Start+Dir.Location+(Dir.Length-1)/DVD_VIDEO_LB_LEN);
  return 0;
}

// looks for partition on the disc
//   partnum: number of the partition, starting at 0
//   part: structure to fill with the partition information
//   return 1 if partition found, 0 on error;
int _UDFFindPartition(int partnum, struct Partition *part) {
  u8 LogBlock[DVD_VIDEO_LB_LEN],Anchor[DVD_VIDEO_LB_LEN];
  u32 lbnum,MVDS_location,MVDS_length;
  u16 TagID;
  //u8 Flags;
  //char Identifier[6];
  u32 lastsector;
  int i,terminate,volvalid;

  // Recognize Volume
  /*
  lbnum=16;
  do {
    if (!_UDFReadLB(lbnum++,1,LogBlock)) strcpy(Identifier,"");
    else _UDFEntity(LogBlock,&Flags,Identifier);
    printf("Looking for NSR02 at LB %ld, found %s\n",lbnum-1,Identifier);
  } while ((lbnum<=256) && strcmp("NSR02",Identifier));
  if (strcmp("NSR02",Identifier))  printf("Could not recognize volume. Bad.\n");
  else printf("Found %s at LB %ld. Good.\n",Identifier,lbnum-1);
  */

  // Find Anchor
  lastsector=0;
  lbnum=256;   // try #1, prime anchor
  terminate=0;
  while (1) {  // loop da loop
    if (_UDFReadLB(lbnum,1,Anchor)) {
      _UDFDescriptor(Anchor,&TagID);
    } else TagID=0;
    if (TagID!=2) {             // not an anchor?
      if (terminate) return 0;  // final try failed 
      if (lastsector) {         // we already found the last sector
        lbnum=lastsector;       // try #3, alternative backup anchor
        terminate=1;            // but that's just about enough, then!
      } else {
        // TODO: find last sector of the disc (this is optional)
        if (lastsector) lbnum=lastsector-256; // try #2, backup anchor
        else return 0;          // unable to find last sector
      }
    } else break;               // it is an anchor! continue...
  }
  _UDFExtentAD(&Anchor[16],&MVDS_length,&MVDS_location);  // main volume descriptor
  //printf("MVDS at LB %ld thru %ld\n",MVDS_location,MVDS_location+(MVDS_length-1)/DVD_VIDEO_LB_LEN);
  
  part->valid=0;
  volvalid=0;
  part->VolumeDesc[0]='\0';
  i=1;
  do {
    // Find Volume Descriptor
    lbnum=MVDS_location;
    do {
      if (!_UDFReadLB(lbnum++,1,LogBlock)) TagID=0;
      else _UDFDescriptor(LogBlock,&TagID);
      //printf("Looking for Descripors at LB %ld, found %d\n",lbnum-1,TagID);
      if ((TagID==5) && (!part->valid)) {  // Partition Descriptor
        //printf("Partition Descriptor at LB %ld\n",lbnum-1);
        _UDFPartition(LogBlock,&part->Flags,&part->Number,part->Contents,&part->Start,&part->Length);
        part->valid=(partnum==part->Number);
        //printf("Partition %d at LB %ld thru %ld\n",part->Number,part->Start,part->Start+part->Length-1);
      } else if ((TagID==6) && (!volvalid)) {  // Logical Volume Descriptor
        //printf("Logical Volume Descriptor at LB %ld\n",lbnum-1);
        if (_UDFLogVolume(LogBlock,part->VolumeDesc)) {  
          //TODO: sector size wrong!
        } else volvalid=1;
//printf("Logical Volume Descriptor: %s\n",part->VolumeDesc);  // name of the disc
      }
    } while ((lbnum<=MVDS_location+(MVDS_length-1)/DVD_VIDEO_LB_LEN) && (TagID!=8) && ((!part->valid) || (!volvalid)));
    if ((!part->valid) || (!volvalid)) _UDFExtentAD(&Anchor[24],&MVDS_length,&MVDS_location);  // backup volume descriptor
  } while (i-- && ((!part->valid) || (!volvalid)));
  return (part->valid);  // we only care for the partition, not the volume
}

// looks for a file on the UDF disc/imagefile
// filename has to be the absolute pathname on the UDF filesystem, starting with /
// returns absolute LB number, or 0 on error
unsigned long int _UDFFindFile(char *filename) {
  u8 LogBlock[DVD_VIDEO_LB_LEN];
  u32 lbnum;
  u16 TagID;
  struct AD RootICB,File,ICB;
  char tokenline[MAX_FILE_LEN];
  char *token;
  u8 filetype;
  
  int Partition=0;  // this is the standard location for DVD Video
  
  tokenline[0]='\0';
  // Very DVD-Video specific:
  //if (filename[0]!='/') strcat(tokenline,"/VIDEO_TS/");
  strcat(tokenline,filename);

  // Find partition
  if (!_UDFFindPartition(Partition,&partition)) return 0;
  printf("Found partition!\n");
  
  // Find root dir ICB
  lbnum=partition.Start;
  do {
    if (!_UDFReadLB(lbnum++,1,LogBlock)) TagID=0;
    else _UDFDescriptor(LogBlock,&TagID);
    //printf("Found TagID %d at LB %ld\n",TagID,lbnum-1);
    if (TagID==256) {  // File Set Descriptor
      _UDFLongAD(&LogBlock[400],&RootICB);
    }
  } while ((lbnum<partition.Start+partition.Length) && (TagID!=8) && (TagID!=256));
  if (TagID!=256) return 0;
  if (RootICB.Partition!=Partition) return 0;
  
  // Find root dir
  if (!_UDFMapICB(RootICB,&filetype,&File)) return 0;
  if (filetype!=4) return 0;  // root dir should be dir
  printf("Root Dir found at %ld\n",File.Location);

  // Tokenize filepath
  token=strtok(tokenline,"\\");
  while (token != NULL) {
    printf("looking for token %s\n",token);
    if (!_UDFScanDir(File,token,&ICB)) return 0;
    if (!_UDFMapICB(ICB,&filetype,&File)) return 0;
    token=strtok(NULL,"\\");
  }
  return partition.Start+File.Location;
}

struct dvd_device {
    HANDLE		fd;	/* dvd device filedesc for dvd ioctl */
};

int DVDPath2LBA (dvd_device_t* dev, char *path)
{
	char* localpath = strchr(path, '\\');
	dvdromfile = dev->fd;
	return _UDFFindFile(localpath);
}
#endif

