#ifndef D2XGAMEMANAGER
#define D2XGAMEMANAGER

#include "d2xsettings.h"
#include "d2xutils.h"
#include "d2xinput.h"
#include <xbApplicationEx.h>
#include <XBFont.h>
#include <vector>

#define	SHOWGAMES		10
#define TEXT_COLOR		0xffffffff
#define HIGHLITE_COLOR	0xffffff00
#define	START_X			40
#define START_Y			40



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
	  unsigned int to_compare = __min(wcslen(a.title),wcslen(b.title));
	  return _wcsnicmp(a.title,b.title,to_compare) <= 0 ? true : false;
  }
};


class D2XGM
{
protected:

	D2Xutils	p_utils;
	D2Xinput	p_input;
	GMitem		global_item;
	GMlist		global_list;

	int			addItem(GMitem item);
	int			deleteItem(int ID);
	int			readItem(int ID, GMitem* item);
	int			readHeader(GMheader* header);
	int			ScanHardDrive(const char* path);
		
	LONGLONG	CountItemSize(char *path);

	// Window
	int				cbrowse;
	int				crelbrowse;
	int				coffset;

public:
	D2XGM();
	~D2XGM();

	/*int operator < (const GMitem& a,const GMitem& b) {
		return _wcsicmp(a.title,b.title);
	}*/
	
	
	void		DeleteStats();
	void		ScanDisk();
	int			PrepareList();
	GMitem		ProcessGameManager(XBGAMEPAD pad);
	void		ShowGameManager(CXBFont &font);
};

#endif