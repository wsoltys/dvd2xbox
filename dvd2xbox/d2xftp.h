#ifndef D2XFTP
#define D2XFTP

#include <helper.h>
#include "ftp.h"
#include "handler.h"

class D2Xftp
{
protected:
	ftp		p_ftp;
	char	szBuffer[1025];  /* buffer used to read/write */
	struct inbuffer
	{
		char	*buffer;
		struct inbuffer *next;
		int		length;
	};
	char *fullbuffer;
	char *tt;
	int t;
	char	*tt2, *tt3;
	int totallength;
	char buffer2[1024];
	void changeDir(char* path);
	char* lastDir;

public:
	D2Xftp();
	~D2Xftp();

	int connect(char* ip,char* user,char* pwd);
	int close();
	bool isConnected();
	HANDLE FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData);
	BOOL FindClose(HANDLE hFindFile);
	

};

#endif