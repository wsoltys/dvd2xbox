#include "d2xutils.h"
#include <stdstring.h>
#include <helper.h>



D2Xutils::D2Xutils()
{

}

D2Xutils::~D2Xutils()
{

}

ULONG D2Xutils::getTitleID(char* path)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	stream  = fopen( path, "rb" );
	if(stream != NULL) {
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,1,sizeof(HC),stream);
		fclose(stream);
		return HC.TitleId;
	}	
	return 0;
}

int D2Xutils::char2byte(char* ch, BYTE* b)
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


int D2Xutils::findHex(char* file,char* mtext,int offset)
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

int D2Xutils::writeHex(char* file,char* mtext,int offset)
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


void D2Xutils::addSlash(char* source)
{
	if(source[strlen(source)-1] != '\\')
		strcat(source,"\\");
}