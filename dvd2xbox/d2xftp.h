#ifndef D2XFTP
#define D2XFTP

#include <helper.h>
#include "d2xsettings.h"
#include "..\lib\libftpc\ftplib.h"
//#include "ftplib.h"



class D2Xftp
{
protected:
	static ftplib	p_ftplib;
	vector<ftp_dir> dir;
	int		it;
	netbuf *nData;
	static char	startpwd[128];

public:
	D2Xftp();
	~D2Xftp();

	HANDLE FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindClose(HANDLE hFindFile);
	bool Connect();
	void Close();
	int CreateFile(char* path);
	int CreateDir(char* path);
	int CloseFile();
	int Write(void* buf,int len);
	int Read(void* buf,int max);
	int GetFileSize(char* path,int* size);

};

#endif