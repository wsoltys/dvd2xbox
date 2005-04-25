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
#include "d2xutils.h" 

#define D2X_CONFIG_FILE		"e:\\TDATA\\0FACFAC0\\settings.d2x"
#define	D2X_SET_STRING		10
#define	D2X_SET_INT			20
#define	D2X_SET_HEX			30

#define D2X_GUI_PROCESS		100
#define D2X_GUI_BACK		110
#define D2X_GUI_MAPDRIVES	120
#define	D2X_GUI_START_NET	130
#define	D2X_GUI_STOP_NET	140
#define	D2X_GUI_START_LCD	150
#define	D2X_GUI_STOP_LCD	160
#define	D2X_GUI_START_FTPD	170
#define	D2X_GUI_STOP_FTPD	180
#define D2X_GUI_SET_LCD		190		

#define TEXT_COLOR_MAIN			0xffffffff
#define HIGHLITE_COLOR_MAIN		0xffffff00
#define TEXT_COLOR_SUB			0xffffffff
#define HIGHLITE_COLOR_SUB		0xffffff00

#define	START_X_MAIN			250
#define START_Y_MAIN			180
#define	START_X_SUB				180
#define START_Y_SUB				160
#define SPACE_X_SUB				200



class D2Xguiset
{
protected:

	struct GUISETITEM
	{
		int	type;
		CStdStringW label;
		bool active;
		int index;
		vector <int> v_int;
		vector <CStdStringW> values;
	};

	struct GUISETMENU
	{
		CStdStringW label;
		bool active;
		int index;
		int elements;
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
	D2Xutils			p_utils;

	void BuildMenu();
	void AnnounceSettings();
	int	 ExecuteSettings();
	void SetItemByIndex(int menuid, int itemid, int index);
	int  GetIndexByItem(int menuid, int itemid);
	int  GetIntValueByItem(int menuid, int itemid);
	void SetStatus(int menuid, int itemid, bool status);
	bool GetStatus(int menuid, int itemid);
	bool AddMenu(int menuID, CStdString label, bool active);
	bool AddInt(int menuID, int itemID, CStdString label, bool active, int default_value, int min, int max, int step);
	bool AddHex(int menuID, int itemID, CStdString label, bool active, int default_value, int min, int max, int step);
	bool AddString(int menuID, int itemID, CStdString label, bool active, int index, CStdString value);

	// browser
	int				cbrowse;

public:
	D2Xguiset();
	~D2Xguiset();

	
	bool	SaveConfig();
	bool	LoadConfig();
	int		Process(XBGAMEPAD pad);
	void	ShowGUISettings(CXBFont &fontb, CXBFont &fonts);
	void	CheckingPartitions();
};

#endif