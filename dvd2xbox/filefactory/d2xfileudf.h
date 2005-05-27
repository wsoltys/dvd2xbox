#ifndef D2XFFUDF
#define D2XFFUDF


#include "d2xfile.h"
#include "..\d2xutils.h"




class D2XfileUDF : public D2Xfile
{
protected:
	
	HANDLE			hFile;
	D2Xutils		p_utils;

public:
	D2XfileUDF();
	virtual ~D2XfileUDF();
	
	virtual int FileOpenWrite(char* filename, int mode=OPEN_MODE_NORMAL, DWORD size=NULL);
	virtual int FileOpenRead(char* filename, int mode=OPEN_MODE_NORMAL);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name);

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);

	virtual DWORD FileSeek(long offset, int origin);
	virtual int DeleteFile(char* filename);
	virtual int DeleteDirectory(char* filename);
	virtual int MoveItem(char* source, char* dest);
	virtual int GetType();
};

#endif