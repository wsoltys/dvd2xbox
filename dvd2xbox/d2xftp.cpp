#include "D2Xftp.h"


D2Xftp::D2Xftp()
{
}

D2Xftp::~D2Xftp()
{
}




HANDLE D2Xftp::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	p_ftplib.Connect("192.168.1.30:21");
	p_ftplib.Login("wiso","Warp99");


	p_ftplib.Quit();
	return (HANDLE)this;
}

BOOL D2Xftp::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	
	return true;
}

BOOL D2Xftp::FindClose(HANDLE hFindFile)
{
	
	return true;
}