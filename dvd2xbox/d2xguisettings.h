#ifndef D2XGUISETTINGS
#define D2XGUISETTINGS

#include <xtl.h>
#include <xbApplicationEx.h>
#include <stdstring.h>
#include <vector>
#include <map>
#include "d2xsettings.h"
#include "d2xinput.h"

#define	SHOWITEMS			10
#define D2X_CONFIG_FILE		"e:\\TDATA\\0FACFAC0\\settings.d2x"
#define	D2X_SET_STRING		10
#define	D2X_SET_INT			20
#define D2X_GUI_PROCESS		100
#define D2X_GUI_BACK		110



class D2Xguiset
{
protected:

	struct GUISETITEM
	{
		int	type;
		CStdString label;
		bool active;
		int index;
		vector <CStdString> values;
	};

	struct GUISETMENU
	{
		CStdString label;
		bool active;
		int index;
		map <int,GUISETITEM> items; 
	};

	struct SELECTED
	{
		CStdString		menulabel;
		unsigned int	menuID;
		CStdString		itemlabel;
		unsigned int	itemID;
		unsigned int	value_index;
		CStdString		value_label;
		unsigned int	items;
	};


	map<int,GUISETMENU>	SetMenu;
	SELECTED			s_item;
	D2Xinput			p_input;

	void BuildMenu();
	bool AddMenu(int menuID, CStdString label, bool active);
	bool AddInt(int menuID, int itemID, CStdString label, bool active, int min, int max, int step);
	bool AddString(int menuID, int itemID, CStdString label, bool active, CStdString value);

	// browser
	unsigned int	cbrowse;
	int				crelbrowse;
	int				coffset;

public:
	D2Xguiset();
	~D2Xguiset();

	
	bool SaveConfig();
	bool LoadConfig();
	int Process(XBGAMEPAD pad);
};

#endif