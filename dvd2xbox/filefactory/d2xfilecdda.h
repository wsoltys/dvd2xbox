#ifndef D2XFFCDDA
#define D2XFFCDDA


#include "d2xfile.h"
#include <stdstring.h>
#include "..\..\lib\libcdrip\cdrip.h"




class D2XfileCDDA : public D2Xfile
{
protected:
	
	BYTE* pbtStream;
	BOOL bAbort;
	LONG nNumBytesRead;

	int DeInit();

public:
	D2XfileCDDA();
	virtual ~D2XfileCDDA();
	
	virtual int FileOpenWrite(char* filename);
	virtual int FileOpenRead(char* filename);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name) {return 0;};

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead){return 0;};

	virtual DWORD FileSeek(long offset, int origin){return 0;};
	virtual int DeleteFile(char* filename){return 0;};
	virtual int DeleteDirectory(char* filename){return 0;};
	virtual int MoveItem(char* source, char* dest){return 0;};
};

#endif