#ifndef D2XFILE
#define D2XFILE

#include <xtl.h>
//#include "../d2xlogger.h"

class D2Xfile
{

protected:

	//D2Xlogger	p_log;

public:
	D2Xfile();
	virtual ~D2Xfile();
	virtual int FileOpenWrite(char* filename)=0;
	virtual int FileOpenRead(char* filename)=0;
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote)=0;
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)=0;
	virtual int FileClose()=0;
	virtual DWORD GetFileSize()=0;

};

#endif