#ifndef D2XDBROWSER
#define D2XDBROWSER

#include <helper.h>
#include "IOSupport.h"
//#include "iso9660.h"
#include <undocumented.h>
#include "d2xtitle.h"
#include "d2xgraphics.h"
#include <stdstring.h>
#include "D2XCDDAripper.h"
#include "d2xfilefactory.h"
#include "d2xtitle.h"

#define BROWSE_DIR		1
#define BROWSE_FILE		2
#define COPY_DVD		3

#define NO_PRESSED		0
#define BUTTON_X		1
#define BUTTON_Y		2
#define BUTTON_START	3
#define BUTTON_RTRIGGER	4
#define BUTTON_LTRIGGER	5
#define BUTTON_WHITE	6
#define BUTTON_BLACK	7
#define BUTTON_B		8


class HDDBROWSEINFO
{
public:
	char	item[1024];
	char	path[1024];
	char	name[128];
	WCHAR	title[43];
	int		type;
	int		button;
	int		size;
	int		track;
	int		mode;
};

class D2Xdbrowser
{
protected:

	char				prevurl[5];
	D2Xfile*			p_file;
	D2Xtitle			p_title;
	int					prev_type;

	// Dir browser
	char				currentdir[1024];
	char				cprevdir[20][1024];
	char*				cDirs[1024];
	char*				cFiles[1024];
	int					cbrowse;
	int					crelbrowse;
	int					coffset;
	int					mdirscount;
	int					mfilescount;
	int					level;
	bool				renew;
	vector<int> browse_item;
	vector<int> relbrowse_item;
	vector<int> offset_item;
	

	//iso9660*			m_pIsoReader;
	CIoSupport			m_cdrom;
	

	HANDLE D2XFindFirstFile(char* lpFileName,LPWIN32_FIND_DATA lpFindFileData,int type);
	BOOL D2XFindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData,int type);
	BOOL D2XFindClose(HANDLE hFindFile,int type);
	int getFilesize(char* filename,int type);
	

public:
	D2Xdbrowser();
	~D2Xdbrowser();

	static bool			renewAll;
	map<int,HDDBROWSEINFO> selected_item;

	HDDBROWSEINFO processDirBrowser(int lines,char* path,XBGAMEPAD gp, XBIR_REMOTE ir,int type);
	bool showDirBrowser(int lines,float x,float y,DWORD fc,DWORD hlfc, CXBFont &font);
	bool resetDirBrowser();
	void Renew();
	bool RenewStatus();
	void ResetCurrentDir();
	map<int,HDDBROWSEINFO> GetSelected();
	/*
	int FTPconnect(char* ip,char* user,char* pwd);
	int FTPclose();
	bool FTPisConnected();
	*/
};

#endif