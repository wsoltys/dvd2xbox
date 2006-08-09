//#include "../stdafx.h"

//#include <Undocumented.h>
#include "../dvd2xbox/filefactory/dosfs/FatDevice.h"
#include "MemoryUnitManager.h"

// Undocumented stuff

typedef STRING OBJECT_STRING;
typedef PSTRING POBJECT_STRING;

#define IRP_MJ_READ                     0x02

#define FAT_VOLUME_NAME_LENGTH          32
#define FAT_ONLINE_DATA_LENGTH          2048

extern "C"
{
	XBOXAPI NTSTATUS WINAPI 
MU_CreateDeviceObject(
    IN  ULONG            Port,
    IN  ULONG            Slot,
    IN  POBJECT_STRING  DeviceName
    );

	XBOXAPI VOID WINAPI 
MU_CloseDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );

	XBOXAPI PDEVICE_OBJECT WINAPI
MU_GetExistingDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );

	XBOXAPI
NTSTATUS WINAPI
IoSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL
    );

XBOXAPI
DWORD
WINAPI
XMountMURootA(
   IN DWORD dwPort,
   IN DWORD dwSlot,
   OUT PCHAR pchDrive
   );
};

typedef struct _PARTITION_INFORMATION {
    LARGE_INTEGER StartingOffset;
    LARGE_INTEGER PartitionLength;
    ULONG HiddenSectors;
    ULONG PartitionNumber;
    UCHAR PartitionType;
    BOOLEAN BootIndicator;
    BOOLEAN RecognizedPartition;
    BOOLEAN RewritePartition;
} PARTITION_INFORMATION, *PPARTITION_INFORMATION;

#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK
#define IOCTL_DISK_GET_PARTITION_INFO   CTL_CODE(IOCTL_DISK_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)


CMemoryUnitManager g_memoryUnitManager;

CMemoryUnitManager::CMemoryUnitManager()
{
  m_initialized = false;
}

bool CMemoryUnitManager::Update()
{
  DWORD newdev = 0;
  DWORD deaddev = 0;
  if (!m_initialized)
  {
    newdev = XGetDevices(XDEVICE_TYPE_MEMORY_UNIT);
    m_initialized = true;
  }
  else if (!XGetDeviceChanges(XDEVICE_TYPE_MEMORY_UNIT, &newdev, &deaddev))
    return false;

  for (DWORD i = 0; i < 4; ++i)
  {
    if (deaddev & (1 << i))
    {
      if (HasFatDevice(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT))
      {
        UnMountFatDevice(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT);
        DebugOut( "Fat Memory unit in port %d (top) removed", i + 1);
      }
      else if (m_FatXDriveLetters[i][0])
      {
        XUnmountMU(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT);
        DebugOut( "Memory unit in port %d (top) removed, %c: unmounted", i + 1, m_FatXDriveLetters[i][0]);
        m_FatXDriveLetters[i][0] = 0;
      }
    }
    if (deaddev & (1 << (i + 16)))
    {
      if (HasFatDevice(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT))
      {
        UnMountFatDevice(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT);
        DebugOut( "Fat Memory unit in port %d (bottom) removed", i + 1);
      }
      else if (m_FatXDriveLetters[i][1])
      {
        XUnmountMU(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT);
        DebugOut( "Memory unit in port %d (bottom) removed, %c: unmounted", i + 1, m_FatXDriveLetters[i][1]);
        m_FatXDriveLetters[i][1] = 0;
      }
    }
  }

  MountUnits(newdev);
  return (newdev || deaddev);
}

void CMemoryUnitManager::MountUnits(unsigned long device)
{
  DebugOut( "Attempting to mount memory unit device %d", device);
  for (DWORD i = 0; i < 4; ++i)
  {
    if (device & (1 << i))
    {
      char c;
      DWORD r = XMountMURootA(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (top), mounted as fatx: %c:", i + 1, m_FatXDriveLetters[i][0] = toupper(c));
      else
      {
        DebugOut("Found memory unit in port %d (top), unable to mount as fatx: %u", i + 1, r);
        MountFatDevice(XDEVICE_PORT0 + i, XDEVICE_TOP_SLOT);
      }
    }
    if (device & (1 << (i + 16)))
    {
      char c;
      DWORD r = XMountMURootA(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT, &c);
      if (r == ERROR_SUCCESS)
        DebugOut( "Found memory unit in port %d (bottom), mounted as fatx: %c:", i + 1, m_FatXDriveLetters[i][1] = toupper(c));
      else
      {
        DebugOut("Found memory unit in port %d (bottom), unable to mount as fatx: %u", i + 1, r);
        MountFatDevice(XDEVICE_PORT0 + i, XDEVICE_BOTTOM_SLOT);
      }
    }
  }
}

bool CMemoryUnitManager::IsDriveValid(char Drive)
{
  Drive = toupper(Drive);
  for (int i = 0; i < 4; ++i)
  {
    if (m_FatXDriveLetters[i][0] == Drive || m_FatXDriveLetters[i][1] == Drive)
      return true;
  }
  return false;
}

bool CMemoryUnitManager::HasFatDevice(unsigned long port, unsigned long slot)
{
  for (unsigned int i = 0; i < m_fatUnits.size(); i++)
    if (m_fatUnits[i]->IsInPort(port, slot))
      return true;
  return false;
}

bool CMemoryUnitManager::MountFatDevice(unsigned long port, unsigned long slot)
{
	NTSTATUS Status;
	CHAR szDeviceName[64];
	OBJECT_STRING DeviceName;
	PDEVICE_OBJECT DeviceObject;

	//
	// Setup the string buffer
	//
	DeviceName.Length = 0;
	//lie - so we have guaranteed space for a back slash (lie leave room for NULL, and a '\\')
	DeviceName.MaximumLength = sizeof(szDeviceName)/sizeof(CHAR)-2;
	DeviceName.Buffer = szDeviceName;

	//
	//  create the device object
	//
  DebugOut(" Creating MU device (port %i, slot %i)", port, slot);
	Status = MU_CreateDeviceObject(port, slot, &DeviceName);

	if(NT_SUCCESS(Status))
	{
    DebugOut(" Getting MU device (port %i, slot %i)", port, slot);
		DeviceObject = MU_GetExistingDeviceObject(port, slot);
    DebugOut(" Reading MU partition info (port %i, slot %i)", port, slot);
  	PARTITION_INFORMATION PartitionInformation;
	  Status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO,
            DeviceObject, NULL, 0, &PartitionInformation,
            sizeof(PARTITION_INFORMATION), NULL, FALSE);

    if (!NT_SUCCESS(Status))
      return false;

    uint8_t sector[SECTOR_SIZE];
    uint8_t unit = m_fatUnits.size();

    CFatDevice *fatDevice = new CFatDevice(port, slot, DeviceObject);
    m_fatUnits.push_back(fatDevice);

    // TEST: Dump first 32k of image to make sure we're reading valid data (yuck!)
    DumpImage("Z:\\image.bin", unit, 64);

    DebugOut(" Reading first sector to determine type");
    uint32_t lbrSector = 0;
    int ret = CheckFirstSectorForLBR(unit);
    if (ret)
    {
      DebugOut(" First sector failed LBR test (%i)", ret);
      // read the MBR
	    uint32_t psize;
	    uint8_t pactive, ptype;
      // 0 is the partition number
      lbrSector = DFS_GetPtnStart(unit, sector, 0, &pactive, &ptype, &psize);
      if (lbrSector == 0xFFFFFFFF)
      { // fail
        DebugOut(" No MBR found - assuming none exists");
        lbrSector = 0;
      }
      else
      {
        DebugOut(" Partition 0 start sector 0x%-08.8lX active %-02.2hX type %-02.2hX size %-08.8lX\n", lbrSector, pactive, ptype, psize);
      }
    }
    if (DFS_GetVolInfo(unit, sector, lbrSector, fatDevice->GetVolume()))
    {
      DebugOut(" Error getting volume information\n");
      UnMountFatDevice(port, slot);
      return false;
    }

    fatDevice->ReadVolumeName();
    fatDevice->LogInfo();
    return true;
	}
  return false;
}

void CMemoryUnitManager::UnMountFatDevice(unsigned long port, unsigned long slot)
{
  for (vector<CFatDevice *>::iterator it = m_fatUnits.begin(); it != m_fatUnits.end(); ++it)
  {
    CFatDevice *fatDevice = *it;
    if (fatDevice->IsInPort(port, slot))
    {
    	MU_CloseDeviceObject(port, slot);
      m_fatUnits.erase(it);
      delete fatDevice;
      return;
    }
  }
}

bool CMemoryUnitManager::GetDirectory(unsigned char unit, const CStdString &path, VECFILEITEMS *items)
{
  if (unit >= m_fatUnits.size())
  {
    DebugOut(" Attempt to get a directory on an unmounted memory unit (%s)", path.c_str());
    return false;
  }
  return m_fatUnits[unit]->GetDirectory('0' + unit, path.c_str(), items);
}

bool CMemoryUnitManager::OpenFile(unsigned char unit, const CStdString &path, bool openForWrite, void *handle)
{
  if (unit >= m_fatUnits.size())
  {
    DebugOut(" Attempt to open a file on an unmounted memory unit (%s)", path.c_str());
    return false;
  }
  return m_fatUnits[unit]->OpenFile(path.c_str(), (PFILEINFO)handle);
}

unsigned int CMemoryUnitManager::ReadFile(void *handle, unsigned char *buffer, unsigned int length)
{
  unsigned char sector[SECTOR_SIZE];
  unsigned int amountRead = 0;
	if (DFS_OK != DFS_ReadFile((PFILEINFO)handle, sector, buffer, &amountRead, length))
  {
    DebugOut(" Error reading file");
		return 0;
	}
  return amountRead;
}

void CMemoryUnitManager::SeekFile(void *handle, unsigned int position)
{
  unsigned char sector[SECTOR_SIZE];
  DFS_Seek((PFILEINFO)handle, position, sector);
}

unsigned long CMemoryUnitManager::ReadFatDevice(unsigned char unit, unsigned char *buffer, unsigned long sector, unsigned long count)
{
  if (unit >= m_fatUnits.size())
  {
    DebugOut(" Attempt to read from fat unit %i when it doesn't exist", unit);
    return 1;
  }
  // read our device
  return m_fatUnits[unit]->Read(buffer, sector, count);
}

unsigned long CMemoryUnitManager::WriteFatDevice(unsigned char unit, unsigned char *buffer, unsigned long sector, unsigned long count)
{
  DebugOut(" WriteFatDevice() called when we have no support for it :(");
  return 1;
}

#define IsPowerOfTwo(x) !((x-1) & x)

int CMemoryUnitManager::CheckFirstSectorForLBR(unsigned char unit)
{
  BYTE sector[SECTOR_SIZE];

	PLBR lbr = (PLBR) sector;

	if(DFS_ReadSector(unit,sector,0,1))
    return -1;

  // ok, let's do some checks
  // check 1: number of fats nonzero
  if (lbr->bpb.numfats == 0)
    return 1;
  // check 2: cluster size a power of 2
  if (!IsPowerOfTwo(lbr->bpb.secperclus))
    return 2;
  // check 3: logical sector size is a power of 2, > 512
  unsigned short bytespersec = *(unsigned short *)&lbr->bpb.bytepersec_l;
  if (!IsPowerOfTwo(bytespersec) || bytespersec < 512)
    return 3;
  // check 4: number of rootdir entries is a multiple of number of dirs per logical sector
  //int rootEntries = (((int)lbr->bpb.rootentries_h) << 8) + lbr->bpb.rootentries_l;
  //lbr->bpb.
  // check 5: disk geometry has nonzero number of heads and sectors per track
  if (*(unsigned short *)&lbr->bpb.heads_l == 0)
    return 5;
  if (*(unsigned short *)&lbr->bpb.secpertrk_l == 0)
    return 5;
  // check 6: number of reserved sectors non-zero
  if (*(unsigned short *)&lbr->bpb.reserved_l == 0)
    return 6;
  // check 7: media descripter is f0 or f8->ff
  if ((lbr->bpb.mediatype & 0xf0) != 0xf0)
    return 7;
  if ((lbr->bpb.mediatype & 0x0f) != 0 && (lbr->bpb.mediatype & 0x08) != 0x08)
    return 7;

  // ok :)
  return 0;
}

void CMemoryUnitManager::DumpImage(const CStdString &path, unsigned char unit, unsigned long sectors)
{
  DebugOut( " Dumping image to %s", path.c_str());
  BYTE buffer[SECTOR_SIZE];
  D2Xfile*	p_file;
  D2Xff factory;
  p_file = factory.Create(UDF);
  //if (file.OpenForWrite(path, true, true))
  if(p_file->FileOpenWrite((char*)path.c_str()))
  {
    for (unsigned int i = 0; i < sectors; i++)
    {
      if (ReadFatDevice(unit, buffer, i, 1))
        break;  // error
      //file.Write(buffer, SECTOR_SIZE);
	  DWORD dwWrote;
	  p_file->FileWrite(buffer,SECTOR_SIZE,&dwWrote);
    }
  }
  //file.Close();
  p_file->FileClose();
  delete p_file;
}

void CMemoryUnitManager::GetMemoryUnitShares(vector<CStdString> &shares)
{
  for (unsigned int i = 0; i < m_fatUnits.size(); i++)
  {
    /*CShare share;*/
    CStdString volumeName = m_fatUnits[i]->GetVolumeName();
	CStdString share;
    volumeName.TrimRight(' ');
    //share.strName.Format("Memory Unit %i (%s)", i + 1, volumeName.c_str());
    share.Format("mem%i://", i);
    shares.push_back(share);
  }
}