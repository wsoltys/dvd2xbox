#include "D2Xfileftp.h"

ftplib D2XfileFTP::p_ftplib;
char D2XfileFTP::startpwd[128];

D2XfileFTP::D2XfileFTP()
{
	memset(fileopen,0,1024);
}

D2XfileFTP::~D2XfileFTP()
{
	p_ftplib.Quit();
}

void D2XfileFTP::FormPath(char* path, char* ret_path)
{
	char* f;
	if(!_strnicmp(path,"ftp:",4))
		strcpy(ret_path,path+5);
	else
		strcpy(ret_path,path);
	
	while((f = strchr(ret_path,'\\')) != NULL)
	{
		*f = '/';
	}
}

bool D2XfileFTP::Connect()
{
	g_d2xSettings.generalNotice = 0;
	if(!p_ftplib.isConnected())
	{
		memset(startpwd,0,128);
		if(!p_ftplib.Connect(g_d2xSettings.ftpIP,21))
		{
			g_d2xSettings.generalError = FTP_COULD_NOT_CONNECT;
			return false;
		}
		if(!p_ftplib.Login(g_d2xSettings.ftpuser,g_d2xSettings.ftppwd))
		{
			g_d2xSettings.generalError = FTP_COULD_NOT_LOGIN;
			return false;
		}
		if(!p_ftplib.Pwd(startpwd,128))
		{
			g_d2xSettings.generalError = FTP_COULD_NOT_LOGIN;
			return false;
		}
	} 
	return true;
}

int D2XfileFTP::FileOpenWrite(char* filename)
{
	if(!Connect())
		return 0;
	if(strlen(fileopen) > 1)
		FileClose();
	char wopath[1024];
	FormPath(filename,wopath);
	sprintf(fileopen,"%s/%s",startpwd,wopath);
	return p_ftplib.FtpAccess(fileopen,FTPLIB_FILE_WRITE,FTPLIB_IMAGE, &nData);
}

int D2XfileFTP::FileOpenRead(char* filename)
{
	if(!Connect())
		return 0;
	if(strlen(fileopen) > 1)
		FileClose();
	char wopath[1024];
	FormPath(filename,wopath);
	sprintf(fileopen,"%s/%s",startpwd,wopath);
	return p_ftplib.FtpAccess(fileopen,FTPLIB_FILE_READ,FTPLIB_IMAGE, &nData);
}

int D2XfileFTP::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	*dwWrote = (DWORD)p_ftplib.FtpWrite((void*)buffer,dwWrite,nData);
	if(*dwWrote == -1 )
		return 0;
	return 1;
}


int D2XfileFTP::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	*dwRead = (DWORD)p_ftplib.FtpRead(buffer,dwToRead,nData);
	if(*dwRead == -1 )
		return 0;
	return 1;
}

int D2XfileFTP::FileClose()
{
	memset(fileopen,0,1024);
	return p_ftplib.FtpClose(nData);
}

DWORD D2XfileFTP::GetFileSize(char* filename)
{
	// Crappy ftplib needs GetFileSize to be called before FileOpenRead
	if(!Connect())
		return 0;
	if(filename == NULL)
		return 0;

	char path[1024];
	int filesize;
	int ret;
	
	FormPath(filename,path);

	char* file = strrchr(path,'/');
	if(file != NULL)
	{
		char wopath[1024];
		*file = '\0';
		file++;
		sprintf(wopath,"%s/%s",startpwd,path);
		p_ftplib.Chdir(wopath);
		ret = p_ftplib.Size(file, &filesize, ftplib::ascii);
	}
	else
	{
		ret = p_ftplib.Size(path, &filesize, ftplib::ascii);
	}
	if(ret)
		return (DWORD)filesize;
	else
		return 0;
}

int D2XfileFTP::GetDirectory(char* path, VECFILEITEMS *items)
{
	ftp_dir dir;
	items->clear();
	ITEMS temp_item;

	if(!Connect())
		return 0;

	char wopath[1024];
	char temp[1024];
	
	FormPath(path,temp);
	if(strlen(temp) >= 1)
        sprintf(wopath,"%s/%s",startpwd,temp);
	else
		strcpy(wopath,startpwd);
	
	if(strlen(wopath) >= 1)
	{
		if(!p_ftplib.Chdir(wopath))
			return 0;
	}

	if(!p_ftplib.D2XDir(dir,""))
		return 0;
	
	for(int i=0;i<dir.filename.size();i++)
	{
		if(dir.directory[i])
		{
			temp_item.fullpath = string(wopath);
			temp_item.name = string(dir.filename[i].c_str());
			temp_item.isDirectory = 1;
		}
		else
		{
			temp_item.fullpath = string(wopath);
			temp_item.name = string(dir.filename[i].c_str());
			temp_item.isDirectory = 0;
		}
		items->push_back(temp_item);
	}

	return 1;
}

int D2XfileFTP::CreateDirectory(char* name)
{
	if(!Connect())
		return 0;
	char tpath[1024];
	//strncpy(tpath,DelFTP(path),1023);
	FormPath(name,tpath);
	char* dir = strrchr(tpath,'/');
	if((dir != NULL) && (strlen(dir)<=1))
	{
		*dir = '\0';
		dir = strrchr(tpath,'/');
	}
	if(dir != NULL)
	{
		char path[1024];
		*dir = '\0';
		dir++;
		sprintf(path,"%s/%s",startpwd,tpath);
		p_ftplib.Chdir(path);
		return p_ftplib.Mkdir(dir);
	}
	else
	{
		p_ftplib.Chdir(startpwd);
		return p_ftplib.Mkdir(tpath);
	}
}

int D2XfileFTP::DeleteFile(char* filename)
{
	if(!Connect())
		return 0;
	char tpath[1024];
	FormPath(filename,tpath);
	char* file = strrchr(tpath,'/');
	if(file != NULL)
	{
		*file = '\0';
		file++;
		p_ftplib.Chdir(tpath);
		return p_ftplib.Delete(file);
	}
	else
		return p_ftplib.Delete(tpath);
}