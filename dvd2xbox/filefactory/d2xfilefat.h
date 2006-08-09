#ifndef D2XFFFAT
#define D2XFFFAT


#include "d2xfile.h"
#include "..\d2xutils.h"
#include "dosfs/dosfs.h"




class D2XfileFAT : public D2Xfile
{
protected:
	
	D2Xutils		p_utils;
	bool			m_bOpened;
	FILEINFO		m_fileInfo;


public:
	D2XfileFAT();
	virtual ~D2XfileFAT();
	
	virtual int FileOpenWrite(char* filename, int mode=OPEN_MODE_NORMAL, DWORD size=NULL);
	virtual int FileOpenRead(char* filename, int mode=OPEN_MODE_NORMAL);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name);

	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead){return 0;};

	virtual __int64 FileSeek(long offset, int origin);
	virtual int DeleteFile(char* filename);
	virtual int DeleteDirectory(char* filename);
	virtual int MoveItem(char* source, char* dest);
	virtual int GetType();
	
};

#endif