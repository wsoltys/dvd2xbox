#ifndef D2XGUISETTINGS
#define D2XGUISETTINGS

#include <xtl.h>
#include <xbApplicationEx.h>
#include <XBFont.h>
#include <stdstring.h>
#include <vector>
#include <map>
#include "d2xsettings.h"
#include "d2xinput.h"
#include "d2xgraphics.h"

#define	SHOWITEMS			10
#define D2X_CONFIG_FILE		"e:\\TDATA\\0FACFAC0\\settings.d2x"
#define	D2X_SET_STRING		10
#define	D2X_SET_INT			20
#define D2X_GUI_PROCESS		100
#define D2X_GUI_BACK		110

#define TEXT_COLOR_MAIN			0xffffffff
#define HIGHLITE_COLOR_MAIN		0xffffff00
#define TEXT_COLOR_SUB			0xffffffff
#define HIGHLITE_COLOR_SUB		0xffffff00

#define	START_X_MAIN			250
#define START_Y_MAIN			180
#define	START_X_SUB				180
#define START_Y_SUB				160



class D2Xguiset
{
protected:

	struct GUISETITEM
	{
		int	type;
		CStdStringW label;
		bool active;
		int index;
		vector <CStdStringW> values;
	};

	struct GUISETMENU
	{
		CStdStringW label;
		bool active;
		int index;
		map <int,GUISETITEM> items; 
	};

	struct SELECTED
	{
		CStdStringW		menulabel;
		int				menuID;
		CStdStringW		itemlabel;
		int				itemID;
		int				value_index;
		CStdStringW		value_label;
		int				items;
	};


	map<int,GUISETMENU>	SetMenu;
	SELECTED			s_item;
	D2Xinput			p_input;
	D2Xgraphics			p_graph;

	void BuildMenu();
	int ExecuteSettings();
	bool AddMenu(int menuID, CStdString label, bool active);
	bool AddInt(int menuID, int itemID, CStdString label, bool active, int min, int max, int step);
	bool AddString(int menuID, int itemID, CStdString label, bool active, CStdString value);

	// browser
	int				cbrowse;
	int				crelbrowse;
	int				coffset;

public:
	D2Xguiset();
	~D2Xguiset();

	
	bool	SaveConfig();
	bool	LoadConfig();
	int		Process(XBGAMEPAD pad);
	void	ShowGUISettings(CXBFont &fontb, CXBFont &fonts);
};

#endif