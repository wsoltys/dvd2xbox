#include "D2Xftp.h"


D2Xftp::D2Xftp()
{
}

D2Xftp::~D2Xftp()
{
}

int D2Xftp::connect(char* ip,char* user,char* pwd)
{
	p_ftp.DoOpen(ip);
	if(!p_ftp.Connected)
		return 0;
	char login[30];
	sprintf(login,"user %s:%s",user,pwd);
	p_ftp.DoLogin(login);
	return 1;
}

int D2Xftp::close()
{
	p_ftp.DoClose();
	if(p_ftp.Connected)
		return 0;
	return 1;
}

bool D2Xftp::isConnected()
{
	if(p_ftp.Connected)
		return true;
	else
        return false;
}

void D2Xftp::changeDir(char* path)
{
	if(!strcmp(path,"ftp:"))
		return;
	char* npath = new char[strlen(path)]; 
	//sprintf(cpath,"cd %s",path+4);
	strcpy(npath,path+4);
	for(unsigned int i=0;i < strlen(npath);i++)
	{
		if(npath[i] == '\\')
			npath[i] = '/';
	}
	DPf_H(path);
	if(strlen(npath) < strlen(lastDir))
	{
		// cdup
	}
	else
	{
	}
	if(lastDir)
		delete[] lastDir;
	lastDir = new char[strlen(path)+1];
	strcpy(lastDir,path);
	//p_ftp.DoCD(path);
	
}


HANDLE D2Xftp::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	char rights[32];
	int code;
	char User[32];
	char Group[32];
	char month[32];
	char add[32];
	int day;
	char filename[512];
	int size;

	int length;
	
	char buffer[16*1024];
	struct inbuffer *inbuff;
	struct inbuffer *first;
	struct inbuffer *last;
	
	char path[1024];
	totallength=0;
	
	strcpy(path,lpFileName);
	//changeDir(path);
	
	p_ftp.DoList("dir");

	inbuff = (struct inbuffer*)malloc2(sizeof(struct inbuffer));
	first = inbuff;
	last = inbuff;

	length = p_ftp.ReadDataMsg(buffer,16*1024);
	if( length == -1 )
		length =0;
	inbuff->buffer = (char *)malloc2( strlen( buffer ));
	inbuff->next = 0;
	memcpy( inbuff->buffer, buffer, strlen( buffer ));
	inbuff->length = length;
	totallength = length;
	length =0;
	do
	{
		length = p_ftp.ReadDataMsg(buffer,16*1024);
		if( length > 0 )
		{
			inbuff = (struct inbuffer*)malloc2(sizeof(struct inbuffer));
			last->next = inbuff;
			last = inbuff;

			inbuff->length = length;
			totallength += length;

			inbuff->buffer = (char *)malloc2( length+1);
			memset(inbuff->buffer,0,length+1 );
			memcpy( inbuff->buffer, buffer, length);
			inbuff->next = 0;
		}
	}while( length != 0 );

	fullbuffer = (char *)malloc2( totallength + 10);

	
	memset(fullbuffer,0, totallength + 9);
	memcpy(fullbuffer, first->buffer, first->length );

	t = first->length;
	inbuff = first->next;
	free2(first->buffer);
	free2((char *)first);
	while( inbuff )
	{
		memcpy(fullbuffer+t, inbuff->buffer, inbuff->length );
		t+=inbuff->length;
		first = inbuff;
		inbuff = inbuff->next;
		free2(first->buffer);
		free2((char *)first);
	}

	tt2 = tt = fullbuffer;

	//while( ((fullbuffer+totallength)-tt2)>20 )
	{
		tt2=strstr( tt,"\r\n");
		memset(buffer2,0,1024);
		if( tt2 )
		{
			memcpy(buffer2,tt,(tt2-tt) +2 );
			tt = tt2+2;
		}
		else
		{
			free2( fullbuffer );
			return INVALID_HANDLE_VALUE;
		}
		tt2=buffer2;
		if( buffer2[0] == 'l' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			
		}
		if( buffer2[0] == 'd' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			DPf_H("found dir: %s",filename);
			strcpy(lpFindFileData->cFileName,filename);
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		}
		if( buffer2[0] == '-' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			DPf_H("found file: %s",filename);
			strcpy(lpFindFileData->cFileName,filename);
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		}
		while( ( tt2[0] == '\n' ) || ( tt2[0] == '\r' )) tt2++;
	}

	return (HANDLE)this;
}

BOOL D2Xftp::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	char rights[32];
	int code;
	char User[32];
	char Group[32];
	char month[32];
	char add[32];
	int day;
	char filename[512];
	int size;

	if( ((fullbuffer+totallength)-tt2)>20 )
	{
		tt2=strstr( tt,"\r\n");
		memset(buffer2,0,1024);
		if( tt2 )
		{
			memcpy(buffer2,tt,(tt2-tt) +2 );
			tt = tt2+2;
		}
		else
		{
			free2( fullbuffer );
			return false;
		}
		tt2=buffer2;
		if( buffer2[0] == 'l' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			
		}
		if( buffer2[0] == 'd' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			DPf_H("found dir: %s",filename);
			strcpy(lpFindFileData->cFileName,filename);
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		}
		if( buffer2[0] == '-' )
		{
			sscanf(tt2,"%s %d %s     %s  %d %s %d %s %s\n",rights, &code, User, Group, &size, month, &day, add, filename);
			DPf_H("found file: %s",filename);
			strcpy(lpFindFileData->cFileName,filename);
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		}
		while( ( tt2[0] == '\n' ) || ( tt2[0] == '\r' )) tt2++;
	} else 
	{
		free2( fullbuffer );
		return false;
	}
	return true;
}

BOOL D2Xftp::FindClose(HANDLE hFindFile)
{
	(void)p_ftp.GetReply();
	char buffer[1024];
	p_ftp.GetPWD(buffer);
	return true;
}