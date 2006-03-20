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
	int		iRead;

public:
	D2XfileISO();
	virtual ~D2XfileISO();
	
	virtual int FileOpenWrite(char* filename, int mode=OPEN_MODE_NORMAL, DWORD size=NULL);
	virtual int FileOpenRead(char* filename, int mode=OPEN_MODE_NORMAL);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name) {return 0;};

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead){return 0;};

	virtual __int64 FileSeek(long offset, int origin){return 0;};
	virtual int DeleteFile(char* filename){return 0;};
	virtual int DeleteDirectory(char* filename){return 0;};
	virtual int MoveItem(char* source, char* dest){return 0;};
	virtual int GetType();
};

#endif