#ifndef D2XFILE
#define D2XFILE

#include <xtl.h>
#include "../d2xlogger.h"

class D2Xfile
{

protected:

	D2Xlogger	p_log;

public:
	D2Xfile();
	virtual ~D2Xfile();
	virtual bool FileOpenWrite(char* filename)=0;
	virtual bool FileWrite(LPCVOID buffer,DWORD dwrite,DWORD &dwrote)=0;
	virtual bool FileClose()=0;

};

#endif