#ifndef D2XDRIVESTATUS
#define D2XDRIVESTATUS

#include <helper.h>
#include <undocumented.h>
#include <xtl.h>
#include <io.h>
#include <IOSupport.h>
#include "iso9660.h"
#include <dvd_reader.h>
#include <cdiosupport.h>

#define UNKNOWN			1
#define DVD				2
#define GAME			3
#define ISO				4
#define CDDA			5
#define VCD				6
#define SVCD			7
#define UDF				20
#define SMBDIR			21
#define FTP				22
#define UDF2SMB			23

//using namespace XISO9660;
using namespace MEDIA_DETECT;

class D2Xdstatus
{
protected:
	CIoSupport			m_IO;
	//static CCdInfo*		m_pCdInfo;
	LONGLONG			dvdsize;
	//LONGLONG			CountDVDsize(char *path);
	DWORD				GetTrayState();
	DWORD				m_dwTrayState;
	DWORD				m_dwTrayCount;
	DWORD				m_dwLastTrayState;

public:
	D2Xdstatus();
	~D2Xdstatus();


	void GetDriveState(WCHAR *m_scdstat,int& type);


};

#endif