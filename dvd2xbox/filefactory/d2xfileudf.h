#ifndef D2XFFUDF
#define D2XFFUDF


#include "d2xfile.h"




class D2XfileUDF : public D2Xfile
{
protected:

	HANDLE			hFile;
	

public:
	D2XfileUDF();
	virtual ~D2XfileUDF();
	
	virtual bool FileOpenWrite(char* filename);
	virtual bool FileWrite(LPCVOID buffer,DWORD dwrite,DWORD &dwrote);
	virtual bool FileClose();
};

#endif