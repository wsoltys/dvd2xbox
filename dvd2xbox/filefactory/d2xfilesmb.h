#ifndef D2XFFSMB
#define D2XFFSMB


#include "d2xfile.h"
#include <FileSMB.h>
#include <stdstring.h>
#include "..\d2xutils.h"



class D2XfileSMB : public D2Xfile
{
protected:
	
	CFileSMB		p_smb;
	D2Xutils		p_utils;
	CStdString		strRoot;

	void			GetPath(char* dest, char* path);
	char			temp_source[512];
	char			temp_dest[512];

public:
	D2XfileSMB();
	virtual ~D2XfileSMB();
	
	virtual int FileOpenWrite(char* filename);
	virtual int FileOpenRead(char* filename);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name);

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead){return 0;};

	virtual DWORD FileSeek(long offset, int origin);
	virtual int DeleteFile(char* filename);
	virtual int DeleteDirectory(char* filename);
	virtual int MoveItem(char* source, char* dest);
};

#endif