#ifndef D2XDRIVESTATUS
#define D2XDRIVESTATUS

#include <helper.h>
#include <undocumented.h>
#include <xtl.h>
#include <io.h>
#include <IOSupport.h>
//#include "iso9660.h"
//#include "..\lib\libdvdread\dvd_reader.h"
#include <cdiosupport.h>
#include "d2xsettings.h"
#include "d2xfilefactory.h"

using namespace MEDIA_DETECT;

class D2Xdstatus
{
protected:
	CIoSupport			m_IO;
	static LONGLONG		dvdsize;
	LONGLONG			CountDVDsize(char *path);
	DWORD				GetTrayState();
	DWORD				m_dwTrayState;
	DWORD				m_dwTrayCount;
	DWORD				m_dwLastTrayState;
	D2Xfile*			p_file;

public:
	D2Xdstatus();
	~D2Xdstatus();


	void GetDriveState(WCHAR *m_scdstat,int& type);
	void DetectMedia(WCHAR *m_scdstat,int& type);
	int	 countMB(char* drive);


};

#endif