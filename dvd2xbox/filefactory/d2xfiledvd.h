#ifndef D2XFFDVD
#define D2XFFDVD


#include "d2xfile.h"
#include "..\..\lib\libdvdread\dvd_reader.h"
#include <stdstring.h>
#include "../d2xfilefactory.h"



class D2XfileDVD : public D2Xfile
{
protected:
	
	dvd_file_t*				vob;
	uint64_t				fileOffset;
	uint64_t				bfileSize;
	dvd_reader_t*			dvd;
	D2Xfile*				p_file;
	D2Xfile*				p_udf;
	int						is_vob;

	int OpenDVD();
	void CloseDVD();

public:
	D2XfileDVD();
	virtual ~D2XfileDVD();
	
	virtual int FileOpenWrite(char* filename, int mode=OPEN_MODE_NORMAL, DWORD size=NULL);
	virtual int FileOpenRead(char* filename, int mode=OPEN_MODE_NORMAL);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name) {return 0;};
	
	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);

	virtual __int64 FileSeek(long offset, int origin){return 0;};
	virtual int DeleteFile(char* filename){return 0;};
	virtual int DeleteDirectory(char* filename){return 0;};
	virtual int MoveItem(char* source, char* dest){return 0;};
	virtual int GetType();

};

#endif