#include "D2Xftp.h"


D2Xftp::D2Xftp()
{
}

D2Xftp::~D2Xftp()
{
}




HANDLE D2Xftp::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	char* ch = strchr(lpFileName,'\\');
	ch++;
	it = 0;
	if(!p_ftplib.isConnected())
	{
	if(!p_ftplib.Connect("192.168.1.30",21))
		return INVALID_HANDLE_VALUE;
	if(!p_ftplib.Login("wiso","Warp99"))
		return INVALID_HANDLE_VALUE;
		//{
			/*if(p_ftplib.D2XDir(dir,ch))
			{
				if(dir.empty())
					return INVALID_HANDLE_VALUE;
				strncpy(lpFindFileData->cFileName,dir[it].filename.c_str(),42);
				if(dir[it].directory)
					lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
				else
					lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
				return (HANDLE)this;
			}*/
		//}
		//p_ftplib.Quit();
	//}
	}
	if(p_ftplib.D2XDir(dir,ch))
	{
		if(dir.empty())
			return INVALID_HANDLE_VALUE;
		strncpy(lpFindFileData->cFileName,dir[it].filename.c_str(),42);
		if(dir[it].directory)
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		else
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		return (HANDLE)this;
	}
	return INVALID_HANDLE_VALUE;
}

BOOL D2Xftp::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	it++;
	if(it >= dir.size())
		return false;
	strncpy(lpFindFileData->cFileName,dir[it].filename.c_str(),42);
	if(dir[it].directory)
		lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	else
		lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	return true;
}

BOOL D2Xftp::FindClose(HANDLE hFindFile)
{
	it = 0;
	dir.clear();
	return true;
}

void D2Xftp::Close()
{
	p_ftplib.Quit();
}