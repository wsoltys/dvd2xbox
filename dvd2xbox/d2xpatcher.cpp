#include "d2xpatcher.h"
#include <stdstring.h>

int D2Xpatcher::mXBECount = 0;
char D2Xpatcher::mXBEs[50][512];
int D2Xpatcher::mFATXren = 0;
char D2Xpatcher::mFATXs[50][50];
int D2Xpatcher::mcheck[50];

 char* D2Xpatcher::p_hexsearch[] = {"744BE8CAFDFFFF85C07D0633C05050EB44F605","E8CAFDFFFF85C07D"};
char* D2Xpatcher::p_hexreplace[] = {"744BE8CAFDFFFF85C0EB0633C05050EB44F605","E8CAFDFFFF85C0EB"};



D2Xpatcher::D2Xpatcher()
{
	//p_help = new HelperX();
}

D2Xpatcher::~D2Xpatcher()
{
	//delete p_help;
}


int D2Xpatcher::char2byte(char* ch, BYTE* b)
{
	char tmp[100];
	if(strlen(ch)%2)
		return 1;
	int n_byte = strlen(ch)/2;
	for(int c=0;c<n_byte;c++)
	{
		sprintf(tmp,"%c%c",ch[2*c],ch[2*c+1]);
		b[c] = (BYTE)strtoul(tmp,'\0',16);
	}
	return 0;
}


int D2Xpatcher::findHex(char* file,char* mtext,int offset)
{
	FILE *stream;
	int read;
	#define blocksize 600
	bool found = false;
	BYTE tmp[blocksize];
	BYTE btext[100];
	if(strlen(mtext)%2)
		return -1;
	int n_byte = strlen(mtext)/2;
	
	if(char2byte(mtext,btext))
		return -1;

	stream  = fopen( file, "rb" );
	if(stream==NULL)
		return -2;
	while(!found)
	{
		fseek(stream,offset,SEEK_SET);
		read = fread(tmp,1,blocksize,stream);
		if(read<n_byte) break;
		
		int c2=0;
		for(int c=0;c<read;c++)
		{
			if(btext[c2] != tmp[c])
			{
				c2=0;
				continue;
			}
			if(c2 == n_byte-1)
			{
				offset+=c-n_byte+1;
				found = true;
			}
			c2++;
		}
		if(!found) offset+=(blocksize-n_byte);
	}
	fclose(stream);
	if(found)
		return offset;
	return -1;
}

int D2Xpatcher::writeHex(char* file,char* mtext,int offset)
{
	FILE *stream;
	BYTE btext[100];
	int write;

	if(strlen(mtext)%2)
		return -1;
	int n_byte = strlen(mtext)/2;

	if(char2byte(mtext,btext))
		return -1;

	SetFileAttributes(file,FILE_ATTRIBUTE_NORMAL);
	stream  = fopen( file, "r+b" );
	if(stream==NULL)
		return -2;
	fseek(stream,offset,SEEK_SET);
	write = fwrite(btext,1,n_byte,stream);
	if(write != n_byte)
		return -3;

	fclose(stream);
	return 0;

}

int D2Xpatcher::SetMediatype(char* file,ULONG &mt,char* nmt)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	stream  = fopen( file, "rb" );
	if(stream == NULL) 
		return 0;

	fread(&HS,1,sizeof(HS),stream);
	fseek(stream,HS.XbeHeaderSize,SEEK_SET);
	fread(&HC,1,sizeof(HC),stream);
	fclose(stream);
	mt = HC.MediaTypes;
	if(writeHex(file,nmt,(HS.XbeHeaderSize + 156))!=0)
		return 0;
	return (HS.XbeHeaderSize + 156);
}

int D2Xpatcher::PatchMediaStd(char* file,int patch)
{
	int mc_pos = findHex(file,p_hexsearch[patch],0);
	if(mc_pos>=0)
	{
		if(!writeHex(file,p_hexreplace[patch],mc_pos))
		{
			mcheck[patch]++;
			return FOUND_OK;
		} else {
			return FOUND_ERROR;
		}
		
	} 
	return NOT_FOUND;
}

void D2Xpatcher::addXBE(char* file)
{
	strcpy(mXBEs[mXBECount],file);
	mXBECount++;
}

void D2Xpatcher::addFATX(char* file)
{
	strcpy(mFATXs[mFATXren],file);
	mFATXren++;
}

void D2Xpatcher::reset()
{
	D2Xpatcher::mXBECount = 0;
	D2Xpatcher::mFATXren = 0;
	D2Xpatcher::mcheck[0] = 0;
	D2Xpatcher::mcheck[1] = 0;
}


char** D2Xpatcher::getPatchFiles()
{
	char path[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	p_IO.GetXbePath(path);
	DPf_H("XBE Path: %s",path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;

	for(int i=0;i<pfilescount;i++)
	{
		if(pFiles[i])
		{
			delete[] pFiles[i];
			pFiles[i]=NULL;
		}
	}
	pfilescount = 0;

	// Open dir
	strcat(path,"\\patches\\*");
	DPf_H("Patch dir: %s",path);
	hFind = FindFirstFile( path, &wfd);
	if( INVALID_HANDLE_VALUE == hFind )
	{
		pFiles[pfilescount] = new char[10];	
		strcpy(pFiles[0],"No files");
		pfilescount ++;
	} else {
	do
	{
		if (wfd.cFileName[0]!=0)
	    {
		
		// Only do files
		if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			
		}
		else
		{
				
			pFiles[pfilescount] = new char[strlen(wfd.cFileName)+1];
			strcpy(pFiles[pfilescount],wfd.cFileName);
			pfilescount++;
		}
		}

	}
	while(FindNextFile( hFind, &wfd ));
	// Close the find handle.
	FindClose( hFind);
	}
	pFiles[pfilescount] = NULL;
	DPf_H("count: %d",pfilescount);
	return pFiles;

}