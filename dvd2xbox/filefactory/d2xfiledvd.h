#ifndef D2XFFDVD
#define D2XFFDVD


#include "d2xfile.h"
#include "..\..\lib\libdvdread\dvd_reader.h"
#include <stdstring.h>
#include "../d2xfilefactory.h"



class D2XfileDVD : public D2Xfile
{
protected:
	
	dvd_file_t*		vob;
	dvd_reader_t*	dvd;
	uint64_t		fileOffset;
	D2Xfile*		p_file;
	int				is_vob;

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
	virtual int FileUDFRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead);
};

#endif