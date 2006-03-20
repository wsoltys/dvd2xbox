#include "D2Xfileudf.h"


D2XfileUDF::D2XfileUDF()
{
	hFile=NULL;
}

D2XfileUDF::~D2XfileUDF()
{
	if(hFile != NULL)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
}


int D2XfileUDF::FileOpenWrite(char* filename, int mode, DWORD size)
{
	FileClose();
	if(mode == OPEN_MODE_SEQ)
		hFile = CreateFile( filename, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 0);
	else
    	hFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );

	if (hFile==NULL)
	{
		//p_log.WLog(L"Can't open %hs for UDF write",filename);
		return 0;
	}

	if(size != NULL)
	{
		// pre allocate file
		LARGE_INTEGER l_size;
		l_size.QuadPart = size;
		VERIFY(SetFilePointerEx(hFile, l_size, NULL, FILE_BEGIN));
		SetEndOfFile(hFile);
		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	}

	return 1;
}

int D2XfileUDF::FileOpenRead(char* filename, int mode)
{
	FileClose();
	if(mode == OPEN_MODE_SEQ)
		hFile = CreateFile( filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
	else
    	hFile = CreateFile( filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

	if (hFile==NULL)
	{
		//p_log.WLog(L"Can't open %hs for UDF read",filename);
		return 0;
	}
	return 1;
}

int D2XfileUDF::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return WriteFile(hFile,buffer,dwWrite,dwWrote,NULL);
}

int D2XfileUDF::DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	// for fileDVD
	return ReadFile(hFile,buffer,dwToRead,dwRead,NULL);
}

int D2XfileUDF::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	return ReadFile(hFile,buffer,dwToRead,dwRead,NULL);
}

int D2XfileUDF::FileClose()
{
	if(hFile==NULL)
		return false;

	int ret = CloseHandle(hFile);
	hFile = NULL;
	
	return ret;
}

DWORD D2XfileUDF::GetFileSize(char* filename)
{
	return ::GetFileSize(hFile,NULL);
}

int D2XfileUDF::GetDirectory(char* path, VECFILEITEMS *items)
{
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

int D2XfileUDF::CreateDirectory(char* name)
{
	int ret = ::CreateDirectory(name,NULL);
	return 1;
}

__int64 D2XfileUDF::FileSeek(long offset, int origin)
{
	DWORD dwptr = SetFilePointer (hFile, offset, NULL, origin); 
	if (dwptr == INVALID_SET_FILE_POINTER)
		return 0;
	else
		return dwptr;
}

int D2XfileUDF::DeleteFile(char* filename)
{
	return ::DeleteFile(filename);
}

int D2XfileUDF::DeleteDirectory(char* filename)
{
	return p_utils.DelTree(filename);
}

int D2XfileUDF::MoveItem(char* source, char* dest)
{
	return MoveFileEx(source,dest,MOVEFILE_COPY_ALLOWED);;
}

int D2XfileUDF::GetType()
{
	return UDF;
}