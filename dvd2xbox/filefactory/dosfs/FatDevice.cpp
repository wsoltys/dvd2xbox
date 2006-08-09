#include <xtl.h>

//#include <Undocumented.h>
#include "FatDevice.h"

static CRITICAL_SECTION m_criticalSection;

static int cacheFat = 0;
static int cacheHit = 0;
static int cacheMiss = 0;

#define IRP_MJ_READ                     0x02

extern "C"
{
	XBOXAPI
NTSTATUS WINAPI
IoSynchronousFsdRequest(
    IN ULONG MajorFunction,
    IN PDEVICE_OBJECT DeviceObject,
    IN OUT PVOID Buffer OPTIONAL,
    IN ULONG Length OPTIONAL,
    IN PLARGE_INTEGER StartingOffset OPTIONAL
    );
};


CFatDevice::CFatDevice(unsigned long port, unsigned long slot, void *device)
{
  m_port = port;
  m_slot = slot;
  m_device = device;
  m_volumeName[0] = 0;
  m_bufferSector = 0xffffffff;
  InitializeCriticalSection(&m_criticalSection);
}

CFatDevice::~CFatDevice()
{
  // destroy our FAT cache
  for (FATCACHE::iterator it = m_fatCache.begin(); it != m_fatCache.end(); it++)
    delete (*it).second;
  m_fatCache.clear();
  DeleteCriticalSection(&m_criticalSection);
}

void CFatDevice::LogInfo()
{
  DebugOut(" Volume label '%-11.11s'", m_volume.label);
  DebugOut(" Separate label '%s'", m_volumeName);
  DebugOut(" %d sector/s per cluster, %d reserved sector/s, volume total %d sectors.", m_volume.secperclus, m_volume.reservedsecs, m_volume.numsecs);
  DebugOut(" %d sectors per FAT, first FAT at sector #%d, root dir at #%d.",m_volume.secperfat,m_volume.fat1,m_volume.rootdir);
  DebugOut(" (For FAT32, the root dir is a CLUSTER number, FAT12/16 it is a SECTOR number)");
  DebugOut(" %d root dir entries, data area commences at sector #%d.",m_volume.rootentries,m_volume.dataarea);
  DebugOut(" %d clusters (%d bytes) in data area, filesystem IDd as ", m_volume.numclusters, m_volume.numclusters * m_volume.secperclus * SECTOR_SIZE);
  if (m_volume.filesystem == FAT12)
    DebugOut(" FAT12.");
  else if (m_volume.filesystem == FAT16)
    DebugOut(" FAT16.");
  else if (m_volume.filesystem == FAT32)
    DebugOut(" FAT32.");
  else
    DebugOut(" [unknown]");
}

bool CFatDevice::IsInPort(unsigned long port, unsigned long slot)
{
  return m_port == port && m_slot == slot;
}

bool CFatDevice::IsInFAT(unsigned long sector) const
{
  return (sector >= m_volume.fat1 && sector < m_volume.rootdir);
}

void CFatDevice::CacheSector(unsigned long sector, unsigned char *buffer)
{
  FATCACHE::const_iterator it = m_fatCache.find(sector);
  if (it == m_fatCache.end())
    m_fatCache.insert(pair<unsigned long, CSector *>(sector, new CSector(buffer)));
}

bool CFatDevice::IsInCache(unsigned long sector, unsigned char *buffer) const
{
  FATCACHE::const_iterator it = m_fatCache.find(sector);
  if (it != m_fatCache.end())
  {
    cacheFat++;
    fast_memcpy(buffer, (*it).second, SECTOR_SIZE);
    return true;
  }
  return false;
}

unsigned long CFatDevice::Read(unsigned char *buffer, unsigned long sector, unsigned long count)
{
  // lock it down - one access at a time please as we're dealing with
  // a common buffer
  //CSingleLock lock(m_criticalSection);
	EnterCriticalSection(&m_criticalSection);

  // check FAT cache first
  if (IsInFAT(sector) && IsInCache(sector, buffer))
  {
	LeaveCriticalSection(&m_criticalSection);
	return 0;
  }

  if (m_bufferSector <= sector && sector < m_bufferSector + PAGE_SIZE/SECTOR_SIZE)
  { // yes
    fast_memcpy(buffer, m_buffer + (sector-m_bufferSector)*SECTOR_SIZE, SECTOR_SIZE);
    cacheHit++;
	LeaveCriticalSection(&m_criticalSection);
    return 0;
  }
  __int64 sectorStart = sector * SECTOR_SIZE;
  __int64 sectorPageStart = sectorStart & ~(PAGE_SIZE - 1);
  long sectorPageOffset = (long)(sectorStart - sectorPageStart);

	LARGE_INTEGER StartingOffset;
	StartingOffset.QuadPart = sectorPageStart;
	NTSTATUS status = IoSynchronousFsdRequest(IRP_MJ_READ, (PDEVICE_OBJECT)m_device, m_buffer, PAGE_SIZE, &StartingOffset);

  if (NT_SUCCESS(status))
  { // yay :)
    cacheMiss++;
    fast_memcpy(buffer, m_buffer + sectorPageOffset, SECTOR_SIZE);
    m_bufferSector = (unsigned long)(sectorPageStart / SECTOR_SIZE);
//    CLog::MemDump(buffer, SECTOR_SIZE);
    // cache this read if it's in the FAT
    for (unsigned long i = 0; i < PAGE_SIZE / SECTOR_SIZE; i++)
      if (IsInFAT(i + m_bufferSector))
        CacheSector(i + m_bufferSector, m_buffer + i * SECTOR_SIZE);
	LeaveCriticalSection(&m_criticalSection);
    return 0;
  }

  DebugOut(" Error reading sector %u from fat (port %u, slot %u): %u", sector, m_port, m_slot, status);
  LeaveCriticalSection(&m_criticalSection);
  return 1;
}

void CFatDevice::ReadVolumeName()
{
  DIRINFO di;
  DIRENT de;
  di.scratch = new BYTE[SECTOR_SIZE];
	if (DFS_OpenDir(&m_volume, (uint8_t*)"", &di))
		return;
	while (!DFS_GetNext(&m_volume, &di, &de))
  {
    if (de.attr == ATTR_VOLUME_ID)
    {
      strncpy(m_volumeName, (char *)de.name, 11);
      m_volumeName[11] = 0;
      return;
    }
  }
}

bool CFatDevice::GetDirectory(unsigned char unit, const char *directory, VECFILEITEMS *items)
{
  DIRINFO di;
  DIRENT de;

  di.scratch = new BYTE[SECTOR_SIZE];
	if (DFS_OpenDir(&m_volume, (uint8_t*)directory, &di)) {
    DebugOut(" Error opening directory %s", directory);
		return false;
	}
  CStdStringW vfatName;
  unsigned short vfatSequence = 0;
  unsigned char vfatChecksum = 0;
  items->clear();
  ITEMS temp_item;

  while (!DFS_GetNext(&m_volume, &di, &de))
  {
		if (de.name[0])
    {
      if ((de.attr & ATTR_LONG_NAME) == ATTR_LONG_NAME)
      { // long filename
        VFAT_DIR_ENTRY *vfat = (VFAT_DIR_ENTRY*)&de;
        // check it's a vfat entry
        if (vfat->attr_0f != 0x0f || vfat->cluster_0000 != 0x0000 || vfat->type_00 != 0x00)
        { // invalid entry
          continue;
        }
        if ((vfat->sequence & 0x40) == 0x40)
        { // last entry
          vfatName.Empty();
          vfatSequence = (vfat->sequence & 0x1f);
          vfatChecksum = vfat->checksum;
        }
        if (vfat->checksum == vfat->checksum && (vfat->sequence & 0x1f) == vfatSequence && vfatSequence)
        {
          WCHAR unicode[14];
          memcpy(unicode, vfat->unicode1, 10);
          memcpy(unicode + 5, vfat->unicode2, 12);
          memcpy(unicode + 11, vfat->unicode3, 4);
          unicode[13] = 0;
          vfatName = unicode + vfatName;
          vfatSequence--;
        }
        continue;
      }
      CStdString name;
      name.Format("%-8.8s", de.name);
      name.TrimRight(' ');
      if (name.Equals(".") || name.Equals(".."))
        continue;
      if ((de.attr & ATTR_VOLUME_ID) == ATTR_VOLUME_ID)
        continue;
      if ((de.attr & ATTR_DIRECTORY) == 0)
      { // filename
        CStdString extension;
        extension.Format(".%-3.3s", de.name + 8);
        name += extension;
      }
      // path
      CStdString path;
      if (strlen(directory))
        path.Format("mem%c://%s/%s", unit, directory, name);
      else
        path.Format("mem%c://%s", unit, name);
      // do we have a vfatName here?
      if (!vfatSequence && vfatName.size())
      { // yes, check the checksum
        //g_charsetConverter.utf16toUTF8(vfatName, name);
		  name = vfatName;
        vfatSequence = 0;
        vfatName.Empty();
      }
      //CFileItem *item = new CFileItem(name);
      //item->m_strPath = path;
      //item->m_bIsFolder = (de.attr & ATTR_DIRECTORY) == ATTR_DIRECTORY;
	  temp_item.name = name;
	  temp_item.fullpath = path;
	  temp_item.isDirectory = (de.attr & ATTR_DIRECTORY) == ATTR_DIRECTORY;
      // file size
      /*if ((de.attr & ATTR_DIRECTORY) == 0)
        item->m_dwSize = *((unsigned long *)(&de.filesize_0));*/
      // Currently using the last write time
      /*int day = (de.wrtdate_l & 0x1f);
      int month = ((de.wrtdate_l & 0xe0) >> 5) | ((de.wrtdate_h & 1) << 3);
      int year = 1980 + ((de.wrtdate_h & 0xfe) >> 1);
      int second = (de.wrttime_l & 0x1f) << 1;
      int minute = ((de.wrttime_l & 0xe0) >> 5) | ((de.wrttime_h & 0x07) << 3);
      int hour = (de.wrtdate_h & 0xf8) >> 3;
      item->m_dateTime.SetDateTime(year, month, day, hour, minute, second);*/

      //items.Add(item);
	  items->push_back(temp_item);
    }
	}
  return true;
}

bool CFatDevice::OpenFile(const char *path, FILEINFO *file)
{
  BYTE sector[SECTOR_SIZE];
	if (DFS_OK != DFS_OpenFile(&m_volume, (uint8_t*)path, DFS_READ, sector, file))
  {
    DebugOut(" Error opening file %s", path);
		return false;
	}
  return true;
}