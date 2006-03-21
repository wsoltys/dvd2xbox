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
#include "d2xmedialib.h"
#include "d2xinput.h"
#include <map>

#define BROWSE_DIR		100
#define BROWSE_FILE		200
#define COPY_DVD		300
#define	SELECT_DRIVE	400

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
	bool	top_items;
	bool	bottom_items;
};

class D2Xdbrowser
{
protected:

	D2Xmedialib			p_ml;
	D2Xinput*			p_input;
	char				prevurl[5];
	D2Xfile*			p_file;
	D2Xtitle			p_title;
	int					prev_type;
	float				b_x;
	float				b_y;
	unsigned short		i_vspace;
	unsigned short		start_x;
	unsigned short		start_y;
	unsigned short		v_space;
	HDDBROWSEINFO		info;

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
	int					show_lines;
	map<int,string>		drives;
	

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
	bool showDirBrowser2(float x,float y,int width,int widthpx,int vspace,int lines, DWORD fc,DWORD hlfc,DWORD sfc, const CStdString& font, DWORD dwFlags=0L, bool scroll=false);
	bool resetDirBrowser();
	void Renew();
	bool RenewStatus();
	void ResetCurrentDir();
	map<int,HDDBROWSEINFO> GetSelected();

	void getXY(float* posX, float* posY);
	void getOrigin(float* posX, float* posY);
	int getItems(int* vspace=NULL);
	void getInfo(HDDBROWSEINFO* info);

};

#endif