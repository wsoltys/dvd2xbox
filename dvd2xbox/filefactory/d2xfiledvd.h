#ifndef D2XFFISO
#define D2XFFISO


#include "d2xfile.h"
#include "..\..\lib\libdvdread\dvd_reader.h"
#include <stdstring.h>




class D2XfileDVD : public D2Xfile
{
protected:
	
	dvd_file_t*		vob;
	dvd_reader_t*	dvd;
	uint64_t		fileOffset;

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
	virtual DWORD GetFileSize();
	virtual int GetDirectory(char* path,VECFILEITEMS &items);
};

#endif