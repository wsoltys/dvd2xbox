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
	
	virtual int FileOpenWrite(char* filename);
	virtual int FileOpenRead(char* filename);
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote);
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
	virtual int FileClose();
	virtual DWORD GetFileSize(char* filename=NULL);
	virtual int GetDirectory(char* path,VECFILEITEMS *items);
	virtual int CreateDirectory(char* name) {return 0;};
	
	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);

};

#endif