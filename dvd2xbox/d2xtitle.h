#ifndef D2XTITLE
#define D2XTITLE

//#include <helper.h>
//#include <undocumented.h>
//#include "..\lib\libcdripx\cdripxlib.h"
#include "..\Xcddb\cddb.h"
#include <dvd_reader.h>
#include "d2xsettings.h"
#include "d2xutils.h"
#include "D2XCDDAripper.h"

class D2Xtitle
{
protected:
	//HelperX*		p_help;
	//CCDRipX			p_cdripx;
	D2Xutils		p_utils;
	Xcddb			m_cddb;
	//CIoSupport		io;

	int				cddbquery;
	int				tocentries;

	

public:

	static int		i_network;
	//static char		c_cddbip[20];
	static char		disk_artist[1024];
	static char		disk_title[1024];
	static char*	track_title[100];
	static char*	track_artist[100];
	static char		filemask[1024];
	static char		dirmask[1024];

	D2Xtitle();
	~D2Xtitle();

	bool getCDDATrackTitle(char* file,int track);
	bool getCDDADiskTitle(char* title);
	int getXBETitle(char* file,WCHAR* m_GameTitle);
	bool getDVDTitle(char* title);

	char* GetNextPath(char *drive,int type);
	void getvalidFilename(char* path,char *name,char* ext);
	void setMask(char file,char dir);

};

#endif