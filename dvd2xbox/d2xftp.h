#ifndef D2XFTP
#define D2XFTP

#include <helper.h>
#include "..\lib\libftpc\ftplib.h"
//#include "ftplib.h"



class D2Xftp
{
protected:
	ftplib	p_ftplib;
	vector<ftp_dir> dir;
	int		it;

public:
	D2Xftp();
	~D2Xftp();

	HANDLE FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindClose(HANDLE hFindFile);
	void Close();
	

};

#endif