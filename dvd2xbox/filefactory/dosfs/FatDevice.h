#pragma once

#include "dosfs.h"
#include "../../d2xutils.h"
//#include "../../FileItem.h"

#include <map>

#pragma pack(push,1) 

// structure for vfat name entries (unicode)
struct VFAT_DIR_ENTRY
{
  unsigned char sequence;
  unsigned short unicode1[5];
  unsigned char attr_0f;
  unsigned char type_00;
  unsigned char checksum;
  unsigned short unicode2[6];
  unsigned short cluster_0000;
  unsigned short unicode3[2];
};

#pragma pack(pop)

class CSector
{
public:
  CSector(unsigned char *buffer) { fast_memcpy(m_buffer, buffer, SECTOR_SIZE); };

  unsigned char *Get() { return m_buffer; };
private:
  unsigned char m_buffer[SECTOR_SIZE];
};

class CFatDevice
{
public:
  CFatDevice(unsigned long port, unsigned long slot, void *device);
  ~CFatDevice();

  void LogInfo();
  VOLINFO *GetVolume() { return &m_volume; };
  bool IsInPort(unsigned long port, unsigned long slot);
  const char *GetVolumeName() { return m_volumeName; };
  void ReadVolumeName();
  bool GetDirectory(unsigned char unit, const char *directory, VECFILEITEMS *items);
  bool OpenFile(const char *path, FILEINFO *file);

  unsigned long Read(unsigned char *buffer, unsigned long sector, unsigned long count);
protected:
  void*          m_device;
  unsigned char  m_buffer[4096];
  unsigned long  m_bufferSector;

  VOLINFO        m_volume;
  char           m_volumeName[12];  // 11 characters max
  unsigned long  m_port;
  unsigned long  m_slot;

  // FAT caching functions
  bool IsInFAT(unsigned long sector) const;
  bool IsInCache(unsigned long sector, unsigned char *buffer) const;
  void CacheSector(unsigned long sector, unsigned char *buffer);

  typedef map<unsigned long, CSector *> FATCACHE;
  FATCACHE m_fatCache;

  // critical section so that more than 1 thread doesn't screw up our caching
  //CCriticalSection m_criticalSection;
};
