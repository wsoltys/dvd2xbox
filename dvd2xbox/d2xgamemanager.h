#ifndef D2XGAMEMANAGER
#define D2XGAMEMANAGER

#include "d2xsettings.h"
#include "d2xfilefactory.h"
#include "d2xutils.h"

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

class D2XGM
{
protected:

	//D2Xfile*	p_file;
	D2Xutils	p_utils;
	GMitem		global_item;

	int			addItem(GMitem item);
	
	
	LONGLONG	CountItemSize(char *path);

public:
	D2XGM();
	~D2XGM();

	void		DeleteStats();
	int			ScanHardDrive(char* path);
	int			readItem(int ID, GMitem* item);
	int			readHeader(GMheader* header);
};

#endif