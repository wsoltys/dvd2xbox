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


int D2XfileUDF::FileOpenWrite(char* filename)
{
	FileClose();
	hFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		//p_log.WLog(L"Can't open %hs for UDF write",filename);
		return 0;
	}
	return 1;
}

int D2XfileUDF::FileOpenRead(char* filename)
{
	FileClose();
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

DWORD D2XfileUDF::GetFileSize()
{
	return ::GetFileSize(hFile,NULL);
}