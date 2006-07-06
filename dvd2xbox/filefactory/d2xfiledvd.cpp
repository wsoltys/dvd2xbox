#include "D2Xfiledvd.h"


D2XfileDVD::D2XfileDVD()
{
	dvd = NULL;
	vob = NULL;
	p_file = NULL;
	is_vob = 0;
	D2Xff factory;
	p_udf = factory.Create(UDF);
}

D2XfileDVD::~D2XfileDVD()
{
	if(p_udf)
	{
		delete p_udf;
		p_udf = NULL;
	}
	if(vob)
		DVDCloseFile(vob);
	if(dvd)
		DVDClose(dvd);

}

int D2XfileDVD::OpenDVD()
{
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(dvd == NULL)
	{
		OutputDebugString("Could not authenticate DVD");
		g_d2xSettings.generalError = COULD_NOT_AUTH_DVD;
		return 0;
	}
	return 1;
}

void D2XfileDVD::CloseDVD()
{
	DVDClose(dvd);
	dvd = NULL;
}


int D2XfileDVD::FileOpenWrite(char* filename, int mode, DWORD size)
{
	return 0;
}

int D2XfileDVD::FileOpenRead(char* filename, int mode)
{
	if(dvd == NULL)
	{
		if(!OpenDVD())
			return 0;
	}
	if(vob != NULL)
	{
        DVDCloseFile(vob);
		vob = NULL;
	}


	fileOffset = 0;
	bfileSize   = 0;

	if(strstr(filename,".vob") || strstr(filename,".VOB"))
	{
		is_vob = 1;
		vob = DVDOpenSingleFile(dvd,filename);
		if(!vob)
			return 0;

		p_file = this;
		bfileSize = DVDFileSize(vob);
	}
	else
	{
		is_vob = 0;
		p_file = p_udf;
		p_file->FileOpenRead(filename);
	}
	return 1;
}

int D2XfileDVD::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return 0;
}

int D2XfileDVD::DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	int ret = 1;
	DWORD dwToReadBlocks = int(dwToRead/2048);
	int iRead = (DWORD)DVDReadBlocks(vob,fileOffset,dwToReadBlocks,(LPBYTE)buffer);
	
	if(iRead != -1)
	{
		ret = 1;
		*dwRead = iRead;
	}
	else 
	{
		ret = 2;
		*dwRead = dwToReadBlocks;
	}


	if((fileOffset + *dwRead) > bfileSize)
		*dwRead = DWORD(bfileSize - fileOffset);

	fileOffset += *dwRead;
	*dwRead*=2048;
	return ret; 
}


int D2XfileDVD::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	return p_file->DUMMYFileRead(buffer,dwToRead,dwRead); 
}

int D2XfileDVD::FileClose()
{
	if(is_vob)
	{
		DVDCloseFile(vob);
		vob = NULL;
	}
	else
	{
		p_file->FileClose();
		p_file = NULL;
	}
	fileOffset = 0;
	return 1;
}

DWORD D2XfileDVD::GetFileSize(char* filename)
{
	if(dvd == NULL)
	{
		if(!OpenDVD())
			return 0;
	}
	if(is_vob)
		return (DWORD)(DVDFileSize(vob)*2048);
	else
		return p_file->GetFileSize();
}


int D2XfileDVD::GetDirectory(char* path, VECFILEITEMS *items)
{
	if(dvd == NULL)
	{
		if(!OpenDVD())
			return 0;
	}

	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	items->clear();
	ITEMS temp_item;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return 0;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				temp_item.fullpath = string(path);
				temp_item.name = string(wfd.cFileName);
				temp_item.isDirectory = 1;
			}
			else
			{
				temp_item.fullpath = string(path);
				temp_item.name = string(wfd.cFileName);
				temp_item.isDirectory = 0;
			}
			items->push_back(temp_item);
	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	
	return 1;
}

int D2XfileDVD::GetType()
{
	return DVD;
}
//
//__int64 D2XfileDVD::FileSeek(long offset, int origin)
//{
//}