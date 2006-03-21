#ifndef D2XGAMEMANAGER
#define D2XGAMEMANAGER

#include "d2xsettings.h"
#include "d2xutils.h"
#include "d2xinput.h"
#include "d2xgraphics.h"
#include "d2xswindow.h"
#include "d2xfilefactory.h"
#include "d2xmedialib.h"
#include <xbApplicationEx.h>
#include <XBFont.h>
#include <vector>
#include <string>

#define	SHOWGAMES		14
#define TEXT_COLOR		0xffffffff
#define HIGHLITE_COLOR	0xffffff00
#define HIGHLITE_POPUP	0xffff0000
#define INFO_TEXT		0xff404040
#define	START_X			50
#define START_Y			50

#define MODE_SHOWLIST					1111
#define MODE_OPTIONS					2222
#define MODE_POPUP						3333
#define MODE_DELETE_SAVES				4444
#define MODE_DELETE_GAME				5555
#define MODE_DELETE_GAMESAVES			6666
#define MODE_DELETE_SAVES_PROGRESS		7777
#define MODE_DELETE_GAME_PROGRESS		8888
#define MODE_DELETE_GAMESAVES_PROGRESS	9999
#define MODE_NO_GAMES_FOUND				1112

#define PROCESS_RESCAN	777
#define PROCESS_ON		888
#define PROCESS_BACK	999


struct INFOitem
{
	CStdString		cdrive;
	CStdString		isizeMB;

	CStdString		title;
	CStdString		TitleId;
	CStdString		files;
	CStdString		dirs;
	CStdString		sizeMB;
	
	CStdString		total_files;
	CStdString		total_dirs;
	CStdString		total_MB;
	CStdString		total_items;
	CStdString		full_path;
	int				gm_mode;
	bool			top_items;
	bool			bottom_items;

};


struct GMheader
{
	char			token[5];
	int				total_files;
	int				total_dirs;
	int				total_MB;
	unsigned short	total_items;
};

struct GMitem
{
	WCHAR			title[42];
	ULONG			TitleId;
	char			full_path[256];
	unsigned short	files;
	unsigned short	dirs;
	unsigned short	sizeMB;
};


struct GMlist
{
	GMheader		header;
	std::vector<GMitem>	item;
};

struct SortTitles
{
  bool operator()(const GMitem& a, const GMitem& b)
  {
	  /*unsigned int to_compare = __min(wcslen(a.title),wcslen(b.title));
	  return _wcsnicmp(a.title,b.title,to_compare) <= 0 ? true : false;*/
	  return _wcsicmp(a.title,b.title) <= 0 ? true : false;
  }
};

struct FreeMB
{
	char	cdrive;
	int		isizeMB;	
};


class D2XGM
{
protected:

	D2Xutils	p_utils;
	D2Xinput*	p_input;
	D2Xgraphics	p_graph;
	D2Xswin*	p_swin;
	D2Xfile*	p_file;
	//D2Xgui*		p_gui;
	D2Xmedialib	p_ml;
	GMitem		global_item;
	GMlist		global_list;
	FreeMB		global_freeMB;
	float		g_x;
	float		g_y;
	int				i_vspace;
	unsigned short	start_x;
	unsigned short	start_y;
	unsigned short	v_space;

	int			addItem(GMitem item);
	int			deleteItem(char* full_path);
	int			readItem(int ID, GMitem* item);
	int			readHeader(GMheader* header);
	int			ScanHardDrive(const char* path);
		
	LONGLONG	CountItemSize(char *path);

	// Window
	int				cbrowse;
	int				crelbrowse;
	int				coffset;

	int				gm_mode;
	map<int,string>	gm_options;
	SWININFO		sinfo;
	WCHAR			temp[64];
	int				showlines;

public:
	D2XGM();
	~D2XGM();

	INFOitem	info;
	
	void		DeleteStats();
	void		ScanDisk();
	int			PrepareList();
	int			AddGameToList(char* full_path);
	int			ProcessGameManager(XBGAMEPAD* pad, XBIR_REMOTE* ir);
	//void		ShowGameManager(CXBFont &font);
	void		ShowGameMenu(float x,float y,int width,int widthpx,int vspace,int lines,DWORD fc,DWORD hlfc,const CStdString& font, DWORD dwFlags=0L, bool scroll=false);
	void		getInfo(INFOitem* i);
	D2Xswin*	getWindowObject();

	void		getXY(float* posX, float* posY);
	void		getOrigin(float* posX, float* posY);
	int			getItems(int* vspace=NULL);
};

#endif