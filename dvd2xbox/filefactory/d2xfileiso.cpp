#include "D2Xfileiso.h"


D2XfileISO::D2XfileISO()
{

}

D2XfileISO::~D2XfileISO()
{
	
}


int D2XfileISO::FileOpenWrite(char* filename)
{
	return 0;
}

int D2XfileISO::FileOpenRead(char* filename)
{
	mISO.CloseFile();
	if ((fh = mISO.OpenFile(filename)) == INVALID_HANDLE_VALUE)
		return 0;
	return 1;
}

int D2XfileISO::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return 0;
}

int D2XfileISO::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	*dwRead = (DWORD)mISO.ReadFile(1,(LPBYTE)buffer,dwToRead);
	return 1; 
}

int D2XfileISO::FileClose()
{
	mISO.CloseFile();
	return 1;
}

DWORD D2XfileISO::GetFileSize()
{
	return (DWORD)mISO.GetFileSize();
}

int D2XfileISO::GetDirectory(char* path, VECFILEITEMS &items)
{
	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	items.clear();
	ITEMS temp_item;

	strcpy(sourcesearch,path);

	// Start the find and check for failure.
	memset(&wfd,0,sizeof(wfd));
	hFind = mISO.FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return 0;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			if (wfd.cFileName[0]==0)
				continue;
			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				CStdString strDir=wfd.cFileName;
				if (strDir != "." && strDir != "..")
				{
					temp_item.fullpath = string(path);
					temp_item.name = string(wfd.cFileName);
					temp_item.isDirectory = 1;
				}
			}
			else
			{
				temp_item.fullpath = string(path);
				temp_item.name = string(wfd.cFileName);
				temp_item.isDirectory = 0;
			}

	    }
	    while(mISO.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    mISO.FindClose( hFind );
		hFind = NULL;
	}
	items.push_back(temp_item);
	return 1;
}
