#include "D2Xfileudf.h"


D2XfileUDF::D2XfileUDF()
{
	hFile=NULL;
}

D2XfileUDF::~D2XfileUDF()
{

}


bool D2XfileUDF::FileOpenWrite(char* filename)
{
	hFile = CreateFile( filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		p_log.WLog(L"Can't open %hs for UDF write",filename);
		return false;
	}
	return true;
}

bool D2XfileUDF::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD &dwWrote)
{
	return WriteFile(hFile,buffer,dwWrite,&dwWrote,NULL);
}

bool D2XfileUDF::FileClose()
{
	if(hFile==NULL)
		return false;
	
	return CloseHandle(hFile);
}