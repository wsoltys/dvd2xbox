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
	
	virtual int FileOpenWrite(char* filename);
	virtual int FileOpenRead(char* filename);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize();
	virtual int GetDirectory(char* path,VECFILEITEMS &items);

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
};

#endif