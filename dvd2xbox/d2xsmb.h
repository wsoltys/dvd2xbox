#ifndef D2XSMB
#define D2XSMB

//#include "../lib/libsmb/xbLibSmb.h"
#include <FileSMB.h>
#include <stdstring.h>
#include "d2xsettings.h"

class D2Xsmb
{
protected:

	//SMB*				smb;
	//SMBdirent*			dirEnt;
	//int					smbfd;
	struct smbc_dirent* dirEnt;
	int fd;
	CStdString strRoot;

	
	
public:
	D2Xsmb();
	~D2Xsmb();

	HANDLE FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindClose(HANDLE hFindFile);
	

};

#endif