#include "D2Xguisettings.h"


D2Xguiset::D2Xguiset()
{
	BuildMenu();

	s_item.itemID = 0;
	s_item.items = SetMenu.size();
	s_item.menuID = 1;
	s_item.menulabel = SetMenu[1].label;

	// Window start values
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
}


D2Xguiset::~D2Xguiset()
{	

}

void D2Xguiset::BuildMenu()
{
	AddMenu(1,"Testmenu",true);
	AddInt(1,1,"int",true,0,10,2);
	AddString(1,2,"string",true,"strvalue1");
	AddString(1,2,"string",true,"strvalue2");

	AddMenu(2,"Testmenu2",true);
	AddInt(2,1,"int21",true,0,10,2);
	AddString(2,2,"string2",true,"strvalue1");
	AddString(2,2,"string2",true,"strvalue2");
	AddInt(2,3,"int23",true,0,10,1);
	AddInt(2,4,"int24",true,0,10,1);
}

int D2Xguiset::ExecuteSettings()
{
	int ret = D2X_GUI_PROCESS;
	return ret;
}



/////////////////////////////////////////////////////////////

bool D2Xguiset::SaveConfig()
{
	map <int, GUISETMENU> :: iterator m_Iter;
	map <int, GUISETITEM> :: iterator i_Iter;
	FILE* stream;

	stream  = fopen( D2X_CONFIG_FILE, "wb" );
	if(stream == NULL) 
		return false;
	
	fwrite(&g_d2xSettings.current_version,1,sizeof(unsigned int),stream);
	
	for(unsigned int m=1; m<=SetMenu.size();m++)
	{
		m_Iter = SetMenu.find(m);
		if(m_Iter == SetMenu.end())
			continue;
		for(unsigned int i=1; i<=m_Iter->second.items.size(); i++)
		{
			i_Iter = m_Iter->second.items.find(i);
			if(i_Iter == m_Iter->second.items.end())
				continue;
			fwrite(&i_Iter->second.index,1,sizeof(int),stream);
		}

	}

	fclose(stream);
	return true;
}

bool D2Xguiset::LoadConfig()
{
	map <int, GUISETMENU> :: iterator m_Iter;
	map <int, GUISETITEM> :: iterator i_Iter;
	FILE* stream;
	unsigned int version;

	stream  = fopen( D2X_CONFIG_FILE, "rb" );
	if(stream == NULL) 
	{
		SaveConfig();
		return true;
	}
	
	fread(&version,1,sizeof(unsigned int),stream);
	if(version != g_d2xSettings.current_version)
	{
		SaveConfig();
		return true;
	}
		
	for(unsigned int m=1; m<=SetMenu.size();m++)
	{
		m_Iter = SetMenu.find(m);
		if(m_Iter == SetMenu.end())
			continue;
		for(unsigned int i=1; i<=m_Iter->second.items.size(); i++)
		{
			i_Iter = m_Iter->second.items.find(i);
			if(i_Iter == m_Iter->second.items.end())
				continue;
			fread(&i_Iter->second.index,1,sizeof(int),stream);
		}

	}

	int it = i_Iter->second.index;

	fclose(stream);
	return true;
}

bool D2Xguiset::AddMenu(int menuID, CStdString label, bool active)
{
	GUISETMENU tmenu;
	map <int, GUISETMENU> :: iterator m_Iter;
	typedef pair <int, GUISETMENU> i_Pair;

	m_Iter = SetMenu.find(menuID);
	if(m_Iter != SetMenu.end())
		return false;

	tmenu.label = label;
	tmenu.active = active;
	tmenu.index = 0;

	SetMenu.insert(i_Pair(menuID,tmenu));

	return true;
}


bool D2Xguiset::AddInt(int menuID, int itemID, CStdString label, bool active, int min, int max, int step)
{
	GUISETITEM titem;	
	char tstr[4];
	map <int, GUISETMENU> :: iterator m_Iter;
	map <int, GUISETITEM> :: iterator i_Iter;
	typedef pair <int, GUISETITEM> i_Pair;
	
	m_Iter = SetMenu.find(menuID);
	if(m_Iter == SetMenu.end())
		return false;

	i_Iter = m_Iter->second.items.find(itemID);

	if(i_Iter != m_Iter->second.items.end())
		return false;

	titem.type = D2X_SET_INT;
	titem.label = label;
	titem.active = active;
	titem.index = 0;

	for(int i=min; i<=max; i+=step)
	{
		itoa(i,tstr,10);
		titem.values.push_back(tstr);
	}

	m_Iter->second.items.insert(i_Pair(itemID,titem));


	return true;
}

bool D2Xguiset::AddString(int menuID, int itemID, CStdString label, bool active, CStdString value)
{
	GUISETITEM titem;	
	map <int, GUISETMENU> :: iterator m_Iter;
	map <int, GUISETITEM> :: iterator i_Iter;
	typedef pair <int, GUISETITEM> i_Pair;

	m_Iter = SetMenu.find(menuID);
	if(m_Iter == SetMenu.end())
		return false;

	i_Iter = m_Iter->second.items.find(itemID);

	if(i_Iter == m_Iter->second.items.end())
	{
		titem.type = D2X_SET_STRING;
		titem.label = label;
		titem.active = active;
		titem.index = 0;
		titem.values.push_back(value);
		m_Iter->second.items.insert(i_Pair(itemID,titem));
	}
	else
	{
		if(i_Iter->second.type != D2X_SET_STRING)
			return false;
		i_Iter->second.values.push_back(value);
	}

	return true;
}

int D2Xguiset::Process(XBGAMEPAD pad)
{
	int ret = D2X_GUI_PROCESS;
	bool pressed = false;
	p_input.update(pad);

	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) 
	{
		if(cbrowse > 1)
			cbrowse--;
		if(crelbrowse>1)
		{
			crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
		pressed = true;
	}
	if((pad.fY1 > 0.5)) {
		Sleep(100);
		if(cbrowse > 1)
			cbrowse--;
		if(crelbrowse>1)
		{
			crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
		pressed = true;
	}

	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
	{
		if(cbrowse < s_item.items)
			cbrowse++;
		if(crelbrowse<SHOWITEMS)
		{
			crelbrowse++;
		} else {
			if(coffset < (s_item.items-SHOWITEMS))
				coffset++;
		}
		pressed = true;
	}
	if(pad.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < s_item.items)
			cbrowse++;
		if(crelbrowse<SHOWITEMS)
		{
			crelbrowse++;
		} else {
			if(coffset < (s_item.items-SHOWITEMS))
				coffset++;
		}
		pressed = true;
	}

	int i = cbrowse;


	if(pressed == true)
	{
		if(s_item.itemID == 0)
		{
			s_item.menuID = i;
			s_item.menulabel = SetMenu[i].label;
		}
		else
		{
			s_item.itemID = i;
			s_item.itemlabel = SetMenu[s_item.menuID].items[i].label;
			s_item.value_index = SetMenu[s_item.menuID].items[i].index;
			s_item.value_label = SetMenu[s_item.menuID].items[i].values[s_item.value_index];
		}
	}

	if(p_input.pressed(GP_A))
	{
		
		if(s_item.itemID == 0)
		{
			if(SetMenu[i].active == true)
			{
				// we're in the main settings menu and wanna jump into a submenu
				s_item.items = SetMenu[i].items.size();
				s_item.menuID = i;
				s_item.menulabel = SetMenu[i].label;
				s_item.itemID = 1;
				s_item.itemlabel = SetMenu[i].items[1].label;
				s_item.value_index = SetMenu[i].items[1].index;
				s_item.value_label = SetMenu[i].items[1].values[s_item.value_index];
				// Window start values
				cbrowse = 1;
				crelbrowse = 1;
				coffset = 0;
			}
		}
		else
		{
			if(SetMenu[s_item.menuID].items[i].active == true)
			{
				// we're in a sub menu, increase index by one
				if(s_item.value_index+1 < SetMenu[s_item.menuID].items[i].values.size())
					s_item.value_index++;
				else
					s_item.value_index = 0;

				SetMenu[s_item.menuID].items[i].index = s_item.value_index;
				s_item.value_label = SetMenu[s_item.menuID].items[i].values[s_item.value_index];

				ret = ExecuteSettings();
			}
		}
	}

	// decrease value index if we're in a submenu and item is active
	if(p_input.pressed(GP_B))
	{
		if((s_item.itemID != 0) && (SetMenu[s_item.menuID].items[i].active == true))
		{
			if(s_item.value_index > 0)
				s_item.value_index--;
			else
				s_item.value_index = SetMenu[s_item.menuID].items[i].values.size()-1;

			SetMenu[s_item.menuID].items[i].index = s_item.value_index;
			s_item.value_label = SetMenu[s_item.menuID].items[i].values[s_item.value_index];

			ret = ExecuteSettings();
		}
	}

	if(p_input.pressed(GP_BACK))
	{
		if(s_item.itemID == 0)
		{
			// we're in the settings main menu, let's go home
			ret = D2X_GUI_BACK;
		}
		else
		{
			// we're in a submenu, let's head over to the settings main menu
			s_item.itemID = 0;
			s_item.itemlabel = "";
			s_item.items = SetMenu.size();
			s_item.menuID = 1;
			s_item.menulabel = SetMenu[1].label;
			// Window start values
			cbrowse = 1;
			crelbrowse = 1;
			coffset = 0;
			SaveConfig();
		}
	}

	return ret;
}

void D2Xguiset::ShowGUISettings(CXBFont &fontb, CXBFont &fonts)
{
	float tmpy=0;
	unsigned int c=0;

	if(s_item.itemID == 0)
	{
		p_graph.RenderGUISettingsMain();

		for(int i=0;i<SHOWITEMS;i++)
		{
			c = i+coffset;
			tmpy = i*fontb.m_fFontHeight;
			if(c >= s_item.items)
				break;
				
			if((i+coffset) == (cbrowse-1))
			{
				fontb.DrawText( START_X_MAIN, START_Y_MAIN+tmpy, HIGHLITE_COLOR_MAIN, SetMenu[c+1].label );
			} else {
				fontb.DrawText( START_X_MAIN, START_Y_MAIN+tmpy, TEXT_COLOR_MAIN, SetMenu[c+1].label );
			}

		} 
	}
	else
	{
		p_graph.RenderGUISettingsSub();

		for(int i=0;i<SHOWITEMS;i++)
		{
			c = i+coffset;
			tmpy = i*fonts.m_fFontHeight;
			if(c >= s_item.items)
				break;
				
			if((i+coffset) == (cbrowse-1))
			{
				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
				fonts.DrawText( START_X_SUB+150, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index]);
			} 
			else 
			{
				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
				fonts.DrawText( START_X_SUB+150, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index] );
			}

		} 
	}
}