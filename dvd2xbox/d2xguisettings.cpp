#include "D2Xguisettings.h"


D2Xguiset::D2Xguiset()
{
	BuildMenu();

	s_item.itemID = 0;
	s_item.items = SetMenu.size();
	s_item.menuID = 1;
	s_item.menulabel = SetMenu[1].label;
}


D2Xguiset::~D2Xguiset()
{	

}

void D2Xguiset::BuildMenu()
{
	AddMenu(1,"Testmenu",true);
	AddInt(1,1,"test1",true,0,10,2);
	AddString(1,2,"test2",true,"bla");
	AddString(1,2,"test2",true,"bla2");
}

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
	tmenu.index = 1;

	SetMenu.insert(i_Pair(menuID,tmenu));

	return true;
}


bool D2Xguiset::AddInt(int menuID, int itemID, CStdString label, bool active, int min, int max, int step)
{
	GUISETITEM titem;	
	CStdString tint;
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
	titem.index = 1;

	for(int i=min; i<=max; i+=step)
	{
		tint = i;
		titem.values.push_back(tint);
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
		titem.index = 1;
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
	}

	int i = cbrowse+1;

	if(s_item.itemID == 0)
	{
		s_item.menuID = i;
		s_item.menulabel = SetMenu[i].label;
	}
	else
	{
		s_item.itemID = i;
		s_item.itemlabel = SetMenu[s_item.menuID].items[i].label;
	}

	if(p_input.pressed(GP_A))
	{
		
		if(s_item.itemID == 0)
		{
			if(SetMenu[i].active == true)
			{
				// we're in the main settings menu and wanna jumb into a submenu
				s_item.items = SetMenu[i].items.size();
				s_item.menuID = i;
				s_item.menulabel = SetMenu[i].label;
				s_item.itemID = 1;
				s_item.itemlabel = SetMenu[i].items[1].label;
			}
		}
		else
		{
			if(SetMenu[s_item.menuID].items[i].active == true)
			{
				// we're in a sub menu, increase index by one
				if(s_item.value_index < SetMenu[s_item.menuID].items[i].values.size())
					s_item.value_index++;
				else
					s_item.value_index = 1;

				s_item.value_label = SetMenu[s_item.menuID].items[i].values[s_item.value_index];
			}
		}
	}

	// decrease value index if we're in a submenu and item is active
	if(p_input.pressed(GP_B))
	{
		if((s_item.itemID == 0) && (SetMenu[s_item.menuID].items[i].active == true))
		{
			if(s_item.value_index > 1)
				s_item.value_index--;
			else
				s_item.value_index = SetMenu[s_item.menuID].items[i].values.size();

			s_item.value_label = SetMenu[s_item.menuID].items[i].values[s_item.value_index];
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
		}
	}

	return ret;
}