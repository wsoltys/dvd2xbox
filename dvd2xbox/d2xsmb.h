#ifndef D2XSMB
#define D2XSMB

#include "..\lib\libsmb\smb++.h"
#include "helper.h"

class D2Xsmb
{
protected:

	SMB*				smb;
	SMBdirent*			dirEnt;
	int					smbfd;

	
	
public:
	D2Xsmb();
	~D2Xsmb();

	HANDLE FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindClose(HANDLE hFindFile);
	void SetLogin(const char* strLogin, const char* strPassword);
	

};

#endif