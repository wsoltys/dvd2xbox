#include "d2xpatcher.h"
#include <stdstring.h>

int D2Xpatcher::mXBECount = 0;
char D2Xpatcher::mXBEs[50][512];
int D2Xpatcher::mFATXren = 0;
//char D2Xpatcher::mFATXs[50][50];
int D2Xpatcher::mcheck[50];

 char* D2Xpatcher::p_hexsearch[] = {"744BE8CAFDFFFF85C07D0633C05050EB44F605","E8CAFDFFFF85C07D"};
char* D2Xpatcher::p_hexreplace[] = {"744BE8CAFDFFFF85C0EB0633C05050EB44F605","E8CAFDFFFF85C0EB"};



D2Xpatcher::D2Xpatcher()
{
	p_log = new D2Xlogger();
	pFiles[0] = NULL;
}

D2Xpatcher::~D2Xpatcher()
{
	delete p_log;
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


int D2Xpatcher::findHex(const char* file,char* mtext,int offset)
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

	DPf_H("Checking %s",mtext);

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
				break;
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

int D2Xpatcher::writeHex(const char* file,char* mtext,int offset)
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

int D2Xpatcher::SetMediatype(const char* file,ULONG &mt,char* nmt)
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

int D2Xpatcher::PatchMediaStd(const char* file,int patch)
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

/*
void D2Xpatcher::addXBE(char* file)
{
	strcpy(mXBEs[mXBECount],file);
	mXBECount++;
}
*/

/*
void D2Xpatcher::addFATX(char* file)
{
	strcpy(mFATXs[mFATXren],file);
	mFATXren++;
}*/


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

void D2Xpatcher::getPatchFilesSTR(map<int,string>& array)
{
	char path[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	p_IO.GetXbePath(path);

	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;

	

	// Open dir
	strcat(path,"\\patches\\*");

	hFind = FindFirstFile( path, &wfd);
	if( INVALID_HANDLE_VALUE == hFind )
	{
		array.insert(pair<int,string>(0,"No files"));
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
			array.insert(pair<int,string>(0,wfd.cFileName));
		}
		}

	}
	while(FindNextFile( hFind, &wfd ));
	// Close the find handle.
	FindClose( hFind);
	}

}

int D2Xpatcher::readPatchesfromFile(char* file)
{
	FILE *stream;
	char buffer[1024];
	char seps[]   = "\n";
	char *token;
	patches=0;
	stream = fopen(file,"rt");
	if(stream==NULL)
		return 0;
	while(fread(buffer,sizeof(char),sizeof(buffer),stream))
	{
		token = strtok( buffer, seps );
		while( token != NULL)
		{
		
			if(!strncmp(token,".",1))
			{
				sscanf(token,".%[^;];%[^.].%[^!]",comment[patches],search[patches],replace[patches]);
				patches++;
			}
			token = strtok( NULL, seps );
		}
		
	}
	fclose(stream);
	return 1;
}

void D2Xpatcher::patchXBEfromFile(HDDBROWSEINFO source,char* patchfile,WCHAR** message)
{
	int line=0;
	int count=0;
	int error=0;
	char path[1024];
	
	p_IO.GetXbePath(path);
	DPf_H("XBE Path: %s",path);
	char* p_xbe = strrchr(path,'\\');
	p_xbe[0] = 0;
	sprintf(path,"%s\\patches\\%s",path,patchfile);
	DPf_H("Patch file: %s",path);

	if(!readPatchesfromFile(path))
	{
		message[0] = new WCHAR[50];
		wcscpy(message[0],L"Could not read patchfile.");
		//p_log->WLog(L"Could not read patchfile %hs.",path);
		message[1] = NULL;
		return;
	}
	if(patches == 0)
	{
		message[0] = new WCHAR[strlen(path)];
		message[1] = new WCHAR[40];
		wsprintfW(message[0],L"%hs",path);
		wcscpy(message[1],L"does not contain any readable patches.");
		message[2] = NULL;
		return;
	}
	DPf_H("Found %d patches",patches);
	for(int i=0;i<patches;i++)
	{
	
		int mc_pos=0;
		DPf_H("Searching for %hs ...",search[i]);
		while((mc_pos = findHex(source.item,search[i],mc_pos))>=0)
		{
			if(mc_pos>=0)
			{
				if(!writeHex(source.item,replace[i],mc_pos))
				{
					DPf_H("- Found at position %d and replaced",mc_pos);
					count++;
				} else {
					DPf_H("Error patching file ",source.item);
					error++;
				}
			
			} 

		}
		message[line] = new WCHAR[strlen(comment[i])+20];
		wsprintfW(message[line],L"%hs found %d times.",comment[i],count);
		DPf_H("%hs found %d times.",search[i],count);
		//p_log->WLog(L"%hs found %d times.",search[i],count);
		//p_log->WLog(L"");
		line++;
		count = 0;
	}
	message[line] = NULL;
	return;
}