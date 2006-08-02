//#include "../stdafx.h"
#include "..\dvd2xbox\d2xsettings.h"

#include "MemUnit.h"

typedef enum _MEDIA_TYPE {
    Unknown,                // Format is unknown
    F5_1Pt2_512,            // 5.25", 1.2MB,  512 bytes/sector
    F3_1Pt44_512,           // 3.5",  1.44MB, 512 bytes/sector
    F3_2Pt88_512,           // 3.5",  2.88MB, 512 bytes/sector
    F3_20Pt8_512,           // 3.5",  20.8MB, 512 bytes/sector
    F3_720_512,             // 3.5",  720KB,  512 bytes/sector
    F5_360_512,             // 5.25", 360KB,  512 bytes/sector
    F5_320_512,             // 5.25", 320KB,  512 bytes/sector
    F5_320_1024,            // 5.25", 320KB,  1024 bytes/sector
    F5_180_512,             // 5.25", 180KB,  512 bytes/sector
    F5_160_512,             // 5.25", 160KB,  512 bytes/sector
    RemovableMedia,         // Removable media other than floppy
    FixedMedia,             // Fixed hard disk media
    F3_120M_512,            // 3.5", 120M Floppy
    F3_640_512,             // 3.5" ,  640KB,  512 bytes/sector
    F5_640_512,             // 5.25",  640KB,  512 bytes/sector
    F5_720_512,             // 5.25",  720KB,  512 bytes/sector
    F3_1Pt2_512,            // 3.5" ,  1.2Mb,  512 bytes/sector
    F3_1Pt23_1024,          // 3.5" ,  1.23Mb, 1024 bytes/sector
    F5_1Pt23_1024,          // 5.25",  1.23MB, 1024 bytes/sector
    F3_128Mb_512,           // 3.5" MO 128Mb   512 bytes/sector
    F3_230Mb_512,           // 3.5" MO 230Mb   512 bytes/sector
    F8_256_128              // 8",     256KB,  128 bytes/sector
} MEDIA_TYPE, *PMEDIA_TYPE;

typedef struct _DISK_GEOMETRY {
    LARGE_INTEGER Cylinders;
    MEDIA_TYPE MediaType;
    DWORD TracksPerCylinder;
    DWORD SectorsPerTrack;
    DWORD BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

#define IOCTL_SUBCMD_GET_INFO 0
#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define IOCTL_DISK_GET_DRIVE_GEOMETRY   CTL_CODE(IOCTL_DISK_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)


static char DriveLetters[4][2];

void InitMemoryUnits()
{
  DWORD dev = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);

  for (DWORD i = 0; i < 4; ++i)
  {
    if (dev & (1 << i))
    {
      char c;
      DWORD r = XMountMU(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (top), mounted as: %c:", i + 1, DriveLetters[i][0] = toupper(c));
      else
        DebugOut( "Found memory unit in port %d (top), unable to mount: %u", i + 1, r);
    }
    if (dev & (1 << (i + 16)))
    {
      char c;
      DWORD r = XMountMU(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (bottom), mounted as: %c:", i + 1, DriveLetters[i][1] = toupper(c));
      else
        DebugOut( "Found memory unit in port %d (bottom), unable to mount: %u", i + 1, r);
    }
  }
}

void UpdateMemoryUnits()
{
  DWORD newdev, deaddev;
  if (!XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &newdev, &deaddev))
    return ;

  for (DWORD i = 0; i < 4; ++i)
  {
    if (deaddev & (1 << i))
    {
      if (DriveLetters[i][0])
      {
        XUnmountMU(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT);
        DebugOut( "Memory unit in port %d (top) removed, %c: unmounted", i + 1, DriveLetters[i][0]);
        DriveLetters[i][0] = 0;
      }
    }
    if (deaddev & (1 << (i + 16)))
    {
      if (DriveLetters[i][1])
      {
        XUnmountMU(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT);
        DebugOut( "Memory unit in port %d (bottom) removed, %c: unmounted", i + 1, DriveLetters[i][1]);
        DriveLetters[i][1] = 0;
      }
    }
  }

  for (DWORD i = 0; i < 4; ++i)
  {
    if (newdev & (1 << i))
    {
      char c;
      DWORD r = XMountMU(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (top), mounted as: %c:", i + 1, DriveLetters[i][0] = toupper(c));
      else
	  {
        DebugOut( "Found memory unit in port %d (top), unable to mount: %u", i + 1, r);

#ifdef _DEBUG
		ANSI_STRING				a_file;
		OBJECT_ATTRIBUTES	obj_attr;
		IO_STATUS_BLOCK		io_stat_block;
		HANDLE						handle;
		unsigned int geom_ioctl_cmd_in_buf[100];
		DISK_GEOMETRY DiskGeometry;

		RtlInitAnsiString(&a_file, "\\Device\\MU_7");
		obj_attr.RootDirectory = 0;
		obj_attr.ObjectName = &a_file;
		obj_attr.Attributes = OBJ_CASE_INSENSITIVE;

		unsigned int stat = NtOpenFile(&handle, (GENERIC_READ|0x00100000), &obj_attr, &io_stat_block, (FILE_SHARE_READ|FILE_SHARE_WRITE), 0x10);

		if (stat != STATUS_SUCCESS) 
		{
			DebugOut("Cant NtOpen mem unit");
		}
		memset(geom_ioctl_cmd_in_buf, 0, sizeof(geom_ioctl_cmd_in_buf));
		geom_ioctl_cmd_in_buf[0] = IOCTL_SUBCMD_GET_INFO;

		stat = NtDeviceIoControlFile(handle, 0, 0, 0, &io_stat_block,
			IOCTL_DISK_GET_DRIVE_GEOMETRY,
			geom_ioctl_cmd_in_buf, sizeof(geom_ioctl_cmd_in_buf),
			&DiskGeometry, sizeof(DiskGeometry));
		if (stat == STATUS_SUCCESS)
		{
			DebugOut("MemUnit Sectors: %d", DiskGeometry.Cylinders.LowPart);
		}
		else
			DebugOut("MemUnit: Cant stat Sectors");
#endif

	  }
    }
    if (newdev & (1 << (i + 16)))
    {
      char c;
      DWORD r = XMountMU(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (bottom), mounted as: %c:", i + 1, DriveLetters[i][1] = toupper(c));
      else
        DebugOut( "Found memory unit in port %d (bottom), unable to mount: %u", i + 1, r);
    }
  }
}

bool MU_IsDriveValid(char Drive)
{
  Drive = toupper(Drive);
  for (int i = 0; i < 4; ++i)
  {
    if (DriveLetters[i][0] == Drive || DriveLetters[i][1] == Drive)
      return true;
  }
  return false;
}
