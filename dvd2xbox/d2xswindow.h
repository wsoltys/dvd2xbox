#ifndef D2XSCROLLWINDOW
#define D2XSCROLLWINDOW

//#include <helper.h>
#include <xbApplicationEx.h>
#include <XBFont.h>

#define NO_PRESSED		0
#define BUTTON_X		1
#define BUTTON_Y		2
#define BUTTON_START	3
#define BUTTON_RTRIGGER	4
#define BUTTON_LTRIGGER	5
#define BUTTON_WHITE	6
#define BUTTON_BLACK	7
#define BUTTON_B		8
#define BUTTON_A		9

struct SWININFO
{
	char	item[1024];
	int		button;
};


class D2Xswin
{
protected:

	//HelperX*		p_help;
	SWININFO		info;

	int				showlines;
	char*			items[1024];
	int				itemscount;
	bool			sort;
	int				cbrowse;
	int				crelbrowse;
	int				coffset;
	
public:
	D2Xswin();
	~D2Xswin();

	void initScrollWindow(char** array,int lines2show,bool sortitems);
	SWININFO processScrollWindow(XBGAMEPAD pad);
	void showScrollWindow(float x,float y,int width,DWORD fc,DWORD hlfc, CXBFont &font);


};

#endif