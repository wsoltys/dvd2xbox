#pragma once

#include <vector>
#include "../dvd2xbox/d2xfilefactory.h"
//#include "../FileItem.h"
//#include "../Settings.h"  // for VECSHARES

class CFatDevice;

class CMemoryUnitManager
{
public:
  CMemoryUnitManager();

  // update the memory units (plug, unplug)
  bool Update();

  bool IsDriveValid(char Drive);    // for backward compatibility
                                    // with fatx drives in filezilla

  unsigned long ReadFatDevice(unsigned char unit, unsigned char *buffer, unsigned long sector, unsigned long count);
  unsigned long WriteFatDevice(unsigned char unit, unsigned char *buffer, unsigned long sector, unsigned long count);

  bool GetDirectory(unsigned char unit, const CStdString &path, VECFILEITEMS *items);
  bool OpenFile(unsigned char unit, const CStdString &path, bool openForWrite, void *handle);
  unsigned int ReadFile(void *handle, unsigned char *buffer, unsigned int length);
  void SeekFile(void *handle, unsigned int position);
  void GetMemoryUnitShares(vector<CStdString> &shares);

private:
  bool HasFatDevice(unsigned long port, unsigned long slot);
  bool MountFatDevice(unsigned long port, unsigned long slot);
  void UnMountFatDevice(unsigned long port, unsigned long slot);
  
  void MountUnits(unsigned long device);
  int  CheckFirstSectorForLBR(unsigned char unit);

  void DumpImage(const CStdString &path, unsigned char unit, unsigned long sectors);

  char m_FatXDriveLetters[4][2];

  std::vector<CFatDevice *> m_fatUnits;

  bool m_initialized;
};

extern CMemoryUnitManager g_memoryUnitManager;
