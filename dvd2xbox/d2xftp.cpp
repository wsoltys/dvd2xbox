#include "D2Xftp.h"

ftplib D2Xftp::p_ftplib;
char D2Xftp::startpwd[128];

D2Xftp::D2Xftp()
{
}

D2Xftp::~D2Xftp()
{
}

bool D2Xftp::Connect()
{
	if(!p_ftplib.isConnected())
	{
		memset(startpwd,0,128);
		if(!p_ftplib.Connect(g_d2xSettings.ftpIP,21))
			return false;
		if(!p_ftplib.Login(g_d2xSettings.ftpuser,g_d2xSettings.ftppwd))
			return false;
		if(!p_ftplib.Pwd(startpwd,128))
			return false;
	} 
	return true;
}

void D2Xftp::Close()
{
	p_ftplib.Quit();
}

HANDLE D2Xftp::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	char* ch = strchr(lpFileName,'\\');
	ch++;
	it = 0;
	char path[1024];
	
	if(!Connect())
		return INVALID_HANDLE_VALUE;

	
	sprintf(path,"%s/%s",startpwd,ch);
	DPf_H("PWD: %s",path);
	
	if(strlen(path) >= 1)
	{
		if(!p_ftplib.Chdir(path))
			return INVALID_HANDLE_VALUE;
	}
	

	if(p_ftplib.D2XDir(dir,""))
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

int D2Xftp::CreateFile(char* path)
{
	if(!Connect())
		return 0;
	char abpath[1024];
	sprintf(abpath,"%s/%s",startpwd,path);
	DPf_H("Trying to open file: %s",abpath);
	return p_ftplib.FtpAccess(abpath,FTPLIB_FILE_WRITE,FTPLIB_IMAGE, &nData);
}

int D2Xftp::CreateDir(char* path)
{
	if(!Connect())
		return 0;
	return p_ftplib.FtpAccess(path,FTPLIB_DIR,FTPLIB_IMAGE, &nData);
}

int D2Xftp::CloseFile()
{
	return p_ftplib.FtpClose(nData);
}

int D2Xftp::Write(void* buf,int len)
{
	return p_ftplib.FtpWrite(buf,len,nData);
}

int D2Xftp::Read(void* buf,int max)
{
	return p_ftplib.FtpRead(buf,max,nData);
}

int D2Xftp::GetFileSize(char* path,int* size)
{
	return p_ftplib.Size(path, size, ftplib::ascii);
}
