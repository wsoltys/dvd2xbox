#include "D2Xfiledvd.h"


D2XfileDVD::D2XfileDVD()
{
	dvd = NULL;
	vob = NULL;
	p_file = NULL;
	is_vob = 0;
}

D2XfileDVD::~D2XfileDVD()
{
	if(vob)
		DVDCloseFile(vob);
	if(dvd)
		DVDClose(dvd);
	if(p_file)
		delete p_file;

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


int D2XfileDVD::FileOpenWrite(char* filename)
{
	return 0;
}

int D2XfileDVD::FileOpenRead(char* filename)
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
	D2Xff factory;

	if(strstr(filename,".vob") || strstr(filename,".VOB"))
	{
		is_vob = 1;
		vob = DVDOpenSingleFile(dvd,filename);
		if(!vob)
			return 0;

		p_file = this;
	}
	else
	{
		is_vob = 0;
		p_file = factory.Create(UDF);
		p_file->FileOpenRead(filename);
	}

	return 1;
}

int D2XfileDVD::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return 0;
}

int D2XfileDVD::FileUDFRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	*dwRead = (DWORD)DVDReadBlocks(vob,fileOffset,dwToRead,(LPBYTE)buffer);
	fileOffset += *dwRead;
	return 1; 
}

int D2XfileDVD::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	return p_file->FileUDFRead(buffer,dwToRead,dwRead); 
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
		delete p_file;
		p_file = NULL;
	}
	fileOffset = 0;
	return 1;
}

DWORD D2XfileDVD::GetFileSize()
{
	if(dvd == NULL)
	{
		if(!OpenDVD())
			return 0;
	}
	if(is_vob)
		return (DWORD)DVDFileSize(vob);
	else
		return p_file->GetFileSize();
}


int D2XfileDVD::GetDirectory(char* path, VECFILEITEMS &items)
{
	if(dvd == NULL)
	{
		if(!OpenDVD())
			return 0;
	}

	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	items.clear();
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

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	items.push_back(temp_item);
	return 1;
}

