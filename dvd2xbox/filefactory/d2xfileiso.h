#ifndef D2XFFISO
#define D2XFFISO


#include "d2xfile.h"
#include <iso9660.h>
#include <stdstring.h>




class D2XfileISO : public D2Xfile
{
protected:
	
	iso9660 mISO;
	HANDLE	fh;

public:
	D2XfileISO();
	virtual ~D2XfileISO();
	
	virtual int FileOpenWrite(char* filename);
	virtual int FileOpenRead(char* filename);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize();
	virtual int GetDirectory(char* path,VECFILEITEMS &items);
};

#endif