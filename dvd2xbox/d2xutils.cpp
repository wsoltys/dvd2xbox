#include "d2xutils.h"

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


bool D2Xutils::writeTitleName(char* path,const WCHAR* title)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	wcsncpy(HC.TitleName,title,40);

	stream  = fopen( path, "r+b" );
	if(stream != NULL) {
		fseek(stream,0,SEEK_SET);
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fwrite(&HC,sizeof(HC),1,stream);
		fclose(stream);
		return 1;
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


int D2Xutils::findHex(const char* file,char* mtext,int offset)
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

	//DPf_H("Checking %s",mtext);

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

int D2Xutils::writeHex(const char* file,char* mtext,int offset)
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

bool D2Xutils::DelTree(char *path)
{
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"\\*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
		RemoveDirectory( path );
	    return true;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,"\\");
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//strcat(sourcefile,"\\");
				// Recursion
				if(!DelTree( sourcefile ))
				{
					if(sourcesearch != NULL)
					{
						delete sourcesearch;
						sourcesearch = NULL;
					}
					if(sourcefile != NULL)
					{
						delete sourcefile;
						sourcefile = NULL;
					}
					return false;
				}
				DPf_H("Called deltree with: %hs",sourcefile);
				//if(!RemoveDirectory(sourcefile))
				//	return false;
				DPf_H("Called removedir with: %hs",sourcefile);
			
			}
			else
			{
				// delete file
				DPf_H("Called delfile with: %hs",sourcefile);
				if(!DeleteFile(sourcefile))
					return false;
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	RemoveDirectory( path );
	if(sourcesearch != NULL)
	{
		delete sourcesearch;
		sourcesearch = NULL;
	}
	if(sourcefile != NULL)
	{
		delete sourcefile;
		sourcefile = NULL;
	}
	return true;
}

void D2Xutils::getHomePath(char* path)
{
	CIoSupport p_IO;
	p_IO.GetXbePath(path);
	if(strlen(path) > 2)
	{
		char* p_xbe = strrchr(path,'\\');
		p_xbe[0] = 0;
	} else
	{
		strcpy(path,"e:\\");
	}
}

// MXM utils

HRESULT D2Xutils::MakePath( LPCTSTR szPath )
{
	HRESULT hr = E_FAIL;

	// Generate path from start to finish!
	CStdString sPath(szPath);

	sPath = PathSlasher( szPath, false );
#if 0
	int iPos = sPath.GetLength()-1;
	// Strip trailing backslash...
	if ( iPos >= 0 && sPath[iPos] == _T('\\') )
	{
		sPath = sPath.Left( iPos );
	}
#endif
	DWORD dwAttr = GetFileAttributes( sPath );

	if ( sPath.GetLength() > 2 )
	{
		if ( dwAttr == (-1))
		{
			// OK, let's step through the process of making a path
			if ( _istalpha( sPath[0] ) && sPath[1] == _T(':') && sPath[2] == _T('\\') )
			{
				// OK so far... let's see if we need to go further:
				if ( sPath.GetLength() > 3 )
				{
					// Call MakePath on parent directory....
					CStdString sParent;
					int iPos = sPath.ReverseFind( _T("\\") );
					if ( iPos >= 2 )
					{
						sParent = sPath.Left( iPos );
						hr = MakePath( sParent );
					}
					else
					{
						// else parent was root...
						hr = S_OK;
					}
					dwAttr = GetFileAttributes( sPath );
					if ( (int)dwAttr == (-1) )
					{
						if ( SUCCEEDED(hr) && CreateDirectory( sPath, NULL ) )
						{
							hr = S_OK;
						}
						else
						{
							hr = E_FAIL;
						}
					}
					else
					{
						if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
						{
							hr = S_OK;
						}
						else
						{
							hr = E_FAIL;
						}
					}
				}
				else
				{
					//Drive root, just say it's "OK"
					hr = S_OK;
				}
			}
			// Else: Fail!
		}
		else if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
		{
			// Already exists... that's fine.
			hr = S_OK;
		}
		// Else, failed. Must be file. We cannot make a path!
	}
	return hr;
}

CStdString D2Xutils::PathSlasher( LPCTSTR szPath, bool bSlashIt )
{
	CStdString sReturn;
	int iLen;

	if ( szPath && (iLen = _tcslen( szPath )) )
	{
		iLen--;
		if ( szPath[iLen] == _T('\\') )
		{
			if ( !bSlashIt )
			{
				sReturn = CStdString( szPath, iLen );
			}
			else
			{
				sReturn = szPath;
			}
		}
		else
		{
			if ( bSlashIt )
			{
				sReturn = szPath;
				sReturn += _T('\\');
			}
			else
			{
				sReturn = szPath;
			}
		}
		if ( !bSlashIt ) // Special case: Root directory MUST be slashed
		{
			if ( sReturn.GetLength() == 2 && sReturn[1] == _T(':') && _istalpha( sReturn[0]) )
			{
				sReturn += _T('\\');
			}
		}
	}
	return sReturn;
}

int D2Xutils::SetMediatype(const char* file,ULONG &mt,char* nmt)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;
	char rnmt[10];

	if(strlen(nmt) != 8)
		return 0;

	sprintf(rnmt,"%c%c%c%c%c%c%c%c",nmt[6],nmt[7],nmt[4],nmt[5],nmt[2],nmt[3],nmt[0],nmt[1]);

	stream  = fopen( file, "rb" );
	if(stream == NULL) 
		return 0;

	fread(&HS,1,sizeof(HS),stream);
	fseek(stream,HS.XbeHeaderSize,SEEK_SET);
	fread(&HC,1,sizeof(HC),stream);
	fclose(stream);
	mt = HC.MediaTypes;
	if(writeHex(file,rnmt,(HS.XbeHeaderSize + 156))!=0)
		return 0;
	return (HS.XbeHeaderSize + 156);
}


// emulated for smb
char* getenv(const char* szKey)
{
	return NULL;
}