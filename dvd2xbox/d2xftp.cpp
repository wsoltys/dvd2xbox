#include "D2Xftp.h"

ftplib D2Xftp::p_ftplib;
char D2Xftp::startpwd[128];

D2Xftp::D2Xftp()
{
	it = 0;
}

D2Xftp::~D2Xftp()
{
	dirlist.clear();
}

bool D2Xftp::Connect()
{
	g_d2xSettings.generalNotice = 0;
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
	dirlist.clear();
}

HANDLE D2Xftp::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	ftp_dir dir;
	dir.iterator = 0;
	char* ch = DelFTP(lpFileName);
	/*if(ch = strchr(lpFileName,'\\'))
        ch++;
	else
		ch = lpFileName;*/
	//it = 0;
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
		if(dir.filename.empty())
			return INVALID_HANDLE_VALUE;
		//strcpy(lpFindFileData->cFileName,dir[it].filename.c_str());
		strncpy(lpFindFileData->cFileName,dir.filename[0].c_str(),42);
		if(dir.directory[0])
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		else
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		DPf_H("ftp1");
		if(dirlist.size() > it)
			dirlist[it] = dir;
		else
			dirlist.push_back(dir);
		DPf_H("ftp2");
		it++;
		return (HANDLE)this;
	}
	return INVALID_HANDLE_VALUE;
}

BOOL D2Xftp::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	ftp_dir dir;
	dirlist[it-1].iterator++;
	dir = dirlist[it-1];
	/*DPf_H("Number of elements: %d",dirlist.size());
	DPf_H("Number of files: %d",dir.filename.size());
	DPf_H("Iterator: %d",dir.iterator);
	DPf_H("Iterator2: %d",dirlist[dirlist.size()-1].iterator++);*/
	//it++;
	if(dir.iterator >= dir.filename.size())
		return false;
	strncpy(lpFindFileData->cFileName,dir.filename[dir.iterator].c_str(),42);
	//DPf_H("Found file %d: %hs",dir.iterator,dir.filename[dir.iterator].c_str());
	if(dir.directory[dir.iterator])
		lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	else
		lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	return true;
}

BOOL D2Xftp::FindClose(HANDLE hFindFile)
{
	dirlist[it-1].iterator = 0;
	//it = 0;
	//dir.clear();
	dirlist.pop_back();
	it--;
	return true;
}

int D2Xftp::CreateFile(char* path)
{
	if(!Connect())
		return 0;
	char abpath[1024];
	sprintf(abpath,"%s/%s",startpwd,DelFTP(path));
	DPf_H("Trying to open file: %s",abpath);
	return p_ftplib.FtpAccess(abpath,FTPLIB_FILE_WRITE,FTPLIB_IMAGE, &nData);
}

int D2Xftp::ReadFile(char* path)
{
	if(!Connect())
		return 0;
	char abpath[1024];
	sprintf(abpath,"%s/%s",startpwd,DelFTP(path));
	DPf_H("Trying to open file: %s",abpath);
	return p_ftplib.FtpAccess(abpath,FTPLIB_FILE_READ,FTPLIB_IMAGE, &nData);
}

//int D2Xftp::CreateDir(char* path)
//{
//	if(!Connect())
//		return 0;
//	return p_ftplib.FtpAccess(path,FTPLIB_DIR,FTPLIB_IMAGE, &nData);
//}

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
	char tpath[1024];
	strncpy(tpath,DelFTP(path),1023);
	char* file = strrchr(tpath,'/');
	if(file != NULL)
	{
		char path[1024];
		*file = '\0';
		file++;
		sprintf(path,"%s/%s",startpwd,tpath);
		p_ftplib.Chdir(path);
		DPf_H("Get filesize: %hs,%hs",path,file);
		return p_ftplib.Size(file, size, ftplib::ascii);
	}
	else
	{
		DPf_H("Get filesize: %hs",tpath);
		return p_ftplib.Size(tpath, size, ftplib::ascii);
	}
}

int D2Xftp::DeleteFile(char* path)
{
	char tpath[1024];
	strncpy(tpath,DelFTP(path),1023);
	/*char* dir = strchr(tpath,'\\');
	if(dir != NULL)
        dir++;
	else
		dir = tpath;*/
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

int D2Xftp::CreateDir(char* path)
{
	char tpath[1024];
	strncpy(tpath,DelFTP(path),1023);
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

int D2Xftp::DeleteDir(char* path)
{
	char sourcesearch[1024];
	char sourcefile[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	/*char* dir = strchr(path,'\\');
	if(dir != NULL)
	{
		dir++;
		strcpy(sourcesearch,dir);

	} else
        strcpy(sourcesearch,path);*/
	strcpy(sourcesearch,DelFTP(path));


	// Start the find and check for failure.
	hFind = D2Xftp::FindFirstFile( sourcesearch, &wfd );

	DPf_H("DeleteDir main: %hs",path);

	if( INVALID_HANDLE_VALUE == hFind )
	{
		DPf_H("Invalid path: %hs",sourcesearch);
		RmDir( path );
	    return true;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,"/");
			strcat(sourcefile,wfd.cFileName);

			DPf_H("Entry: %hs",sourcefile);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//strcat(sourcefile,"\\");
				// Recursion
				if(!D2Xftp::DeleteDir( sourcefile ))
				{
					return false;
				}
				DPf_H("Called deltree with: %hs",sourcefile);
			
			}
			else
			{
				// delete file
				DPf_H("Called delfile with: %hs",sourcefile);
				if(!D2Xftp::DeleteFile(sourcefile))
					return false;
	
			}

	    }
		while(D2Xftp::FindNextFile( hFind, &wfd ));

	    // Close the find handle.
		D2Xftp::FindClose( hFind );
	}
	RmDir( sourcesearch );
	return true;
}

int D2Xftp::RmDir(char* path)
{
	char tpath[1024];
	strncpy(tpath,DelFTP(path),1023);
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
		DPf_H("path,dir: %hs,%hs",path,dir);
		p_ftplib.Chdir(path);
		/*char temp[50];
		p_ftplib.Pwd(temp,50);
		DPf_H("pwd: %hs",temp);*/
		return p_ftplib.Rmdir(dir);
	}
	else
	{
		DPf_H("dir: %hs",tpath);
		p_ftplib.Chdir(startpwd);
		return p_ftplib.Rmdir(tpath);
	}
}

char* D2Xftp::DelFTP(char* path)
{
	char* dir = strchr(path,'\\');
	if(dir != NULL)
        dir++;
	else
		dir = path;
	return dir;
}

int D2Xftp::Rename(char* path,char* dest)
{
	char tpath[1024];
	strncpy(tpath,DelFTP(path),1023);
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
		return p_ftplib.Rename(dir,dest);
	}
	else
	{
		p_ftplib.Chdir(startpwd);
		return p_ftplib.Rename(tpath,dest);
	}
}