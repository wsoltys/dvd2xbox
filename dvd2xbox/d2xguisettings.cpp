#include "D2Xguisettings.h"


D2Xguiset::D2Xguiset()
{
	SetMenu.clear();
	BuildMenu();

	s_item.itemID = 0;
	s_item.items = SetMenu.size();
	s_item.menuID = 1;
	s_item.menulabel = SetMenu[1].label;

	// Window start values
	cbrowse = 1;
}


D2Xguiset::~D2Xguiset()
{	

}

void D2Xguiset::BuildMenu()
{
	AddMenu(1,"General",true);
	AddString(1,1,"Autodetect Partitions",true,1,"no");
	AddString(1,1,"Autodetect Partitions",true,1,"yes");
	AddString(1,2,"Enable F: Partition",false,0,"no");
	AddString(1,2,"Enable F: Partition",false,0,"yes");
	AddString(1,3,"Enable G: Partition",false,0,"no");
	AddString(1,3,"Enable G: Partition",false,0,"yes");
	AddString(1,4,"Enable Logfile",true,0,"no");
	AddString(1,4,"Enable Logfile",true,0,"yes");
	AddString(1,5,"Enable ACL processing",true,1,"no");
	AddString(1,5,"Enable ACL processing",true,1,"yes");
	AddString(1,6,"Enable ACL deletion",true,0,"no");
	AddString(1,6,"Enable ACL deletion",true,0,"yes");
	AddString(1,7,"Enable Autoeject",true,1,"no");
	AddString(1,7,"Enable Autoeject",true,1,"yes");
	AddString(1,8,"Enable LED control",true,0,"no");
	AddString(1,8,"Enable LED control",true,0,"yes");	
	AddString(1,9,"Enable Screensaver",true,0,"off");
	AddString(1,9,"Enable Screensaver",true,0,"1 min");
	AddString(1,9,"Enable Screensaver",true,0,"2 min");
	AddString(1,9,"Enable Screensaver",true,0,"3 min");
	AddString(1,9,"Enable Screensaver",true,0,"4 min");
	AddString(1,9,"Enable Screensaver",true,0,"5 min");

	AddMenu(2,"Audio",true);
	AddString(2,1,"Encoder",true,0,"MP3");
	AddString(2,1,"Encoder",true,0,"OggVorbis");
	AddString(2,1,"Encoder",true,0,"WAV");
	AddString(2,2,"Ogg Quality",true,1,"low");
	AddString(2,2,"Ogg Quality",true,1,"medium");
	AddString(2,2,"Ogg Quality",true,1,"high");
	AddString(2,3,"Mode",true,0,"Stereo");
	AddString(2,3,"Mode",true,0,"Joint Stereo");
	AddInt(2,4,"Bitrate",true,192,64,384,64);

	AddMenu(3,"LCD",true);
	AddString(3,1,"Enable LCD",true,0,"no");
	AddString(3,1,"Enable LCD",true,0,"yes");
	AddString(3,2,"Modchip",true,0,"SmartXX");
	AddString(3,2,"Modchip",true,0,"Xexuter3");
	AddString(3,2,"Modchip",true,0,"Xenium");
	AddString(3,3,"Type",true,0,"LCD-KS0073");
	AddString(3,3,"Type",true,0,"LCD-HD44780");
	AddString(3,3,"Type",true,0,"VFD");
	AddInt(3,4,"Columns",true,20,6,40,1);
	AddInt(3,5,"Rows",true,4,1,10,1);
	AddHex(3,6,"Line1 Address",true,0,0,252,4);
	AddHex(3,7,"Line2 Address",true,20,0,252,4);
	AddHex(3,8,"Line3 Address",true,64,0,252,4);
	AddHex(3,9,"Line4 Address",true,84,0,252,4);
	AddInt(3,10,"Backlight",true,80,1,100,1);
	AddInt(3,11,"Contrast",true,80,1,100,1);

	if(p_utils.IsEthernetConnected() == true)
	{
		AddMenu(4,"Network",true);
		AddString(4,1,"Enable Network",true,0,"no");
		AddString(4,1,"Enable Network",true,0,"yes");
		AddString(4,2,"Enable FTP Server",false,0,"no");
		AddString(4,2,"Enable FTP Server",false,0,"yes");
	}
	else
		AddMenu(4,"Network",false);

	AddMenu(5,"Restore Defaults",true);

	
}

int D2Xguiset::ExecuteSettings()
{
	int ret = D2X_GUI_PROCESS;

	if(s_item.menuID == 1)
	{
		switch(s_item.itemID)
		{
		case 1:
			if(s_item.value_index == 1)
			{
				CheckingPartitions();
				ret = D2X_GUI_MAPDRIVES;
			}
			break;
		case 2:
			SetItemByIndex(1,2,s_item.value_index);
			ret = D2X_GUI_MAPDRIVES;
			break;
		case 3:
			SetItemByIndex(1,3,s_item.value_index);
			ret = D2X_GUI_MAPDRIVES;
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == 2)
	{
	}
	else if(s_item.menuID == 3)
	{
		switch(s_item.itemID)
		{
		case 1:
			ret = s_item.value_index ? D2X_GUI_START_LCD : D2X_GUI_STOP_LCD;
			break;
		case 10:
		case 11:
			ret = D2X_GUI_SET_LCD;
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == 4)
	{
		switch(s_item.itemID)
		{
		case 1:
			if(s_item.value_index == 0)
				SetItemByIndex(4,2,0);
			ret = s_item.value_index ? D2X_GUI_START_NET : D2X_GUI_STOP_NET;
			break;
		case 2:
			ret = s_item.value_index ? D2X_GUI_START_FTPD : D2X_GUI_STOP_FTPD;
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == 5)
	{
	/*	SaveConfig();
		AnnounceSettings();*/
	}
	AnnounceSettings();
	return ret;
}

void D2Xguiset::AnnounceSettings()
{
	g_d2xSettings.autodetectHDD = GetIndexByItem(1,1);
	g_d2xSettings.useF = GetIndexByItem(1,2);
	g_d2xSettings.useG = GetIndexByItem(1,3);
	g_d2xSettings.WriteLogfile = GetIndexByItem(1,4);
	g_d2xSettings.enableACL = GetIndexByItem(1,5);
	g_d2xSettings.enableRMACL = GetIndexByItem(1,6);
	g_d2xSettings.enableAutoeject = GetIndexByItem(1,7);
	g_d2xSettings.enableLEDcontrol = GetIndexByItem(1,8);
	g_d2xSettings.ScreenSaver = GetIndexByItem(1,9);

	if(GetIndexByItem(2,1) == 0)
		g_d2xSettings.cdda_encoder = MP3LAME;
	else if(GetIndexByItem(2,1) == 1)
		g_d2xSettings.cdda_encoder = OGGVORBIS;
	else if(GetIndexByItem(2,1) == 2)
		g_d2xSettings.cdda_encoder = WAV;

	if(GetIndexByItem(2,2)==0)
		g_d2xSettings.ogg_quality = 0.1;
	else if(GetIndexByItem(2,2)==1)
		g_d2xSettings.ogg_quality = 0.5;
	else if(GetIndexByItem(2,2)==2)
		g_d2xSettings.ogg_quality = 1.0;
	g_d2xSettings.mp3_mode = GetIndexByItem(2,3);
	g_d2xSettings.mp3_bitrate = GetIntValueByItem(2,4);

	g_d2xSettings.m_bLCDUsed = GetIndexByItem(3,1);
	if(GetIndexByItem(3,2) == 2)
		g_d2xSettings.m_iLCDModChip = MODCHIP_XENIUM;
	else if(GetIndexByItem(3,2) == 1)
		g_d2xSettings.m_iLCDModChip = MODCHIP_XECUTER3;
	else
        g_d2xSettings.m_iLCDModChip = MODCHIP_SMARTXX;
	
    g_d2xSettings.m_iLCDType=GetIndexByItem(3,3);
	g_d2xSettings.m_iLCDColumns=GetIntValueByItem(3,4);
	g_d2xSettings.m_iLCDRows=GetIntValueByItem(3,5);
	g_d2xSettings.m_iLCDAdress[0]=GetIntValueByItem(3,6);
	g_d2xSettings.m_iLCDAdress[1]=GetIntValueByItem(3,7);
	g_d2xSettings.m_iLCDAdress[2]=GetIntValueByItem(3,8);
	g_d2xSettings.m_iLCDAdress[3]=GetIntValueByItem(3,9);
	g_d2xSettings.m_iLCDBackLight=GetIntValueByItem(3,10);
	g_d2xSettings.m_iContrast = GetIntValueByItem(3,11);

	g_d2xSettings.network_enabled = GetIndexByItem(4,1);
	g_d2xSettings.ftpd_enabled = GetIndexByItem(4,2);



	SetStatus(1,2,!GetIndexByItem(1,1));
	SetStatus(1,3,!GetIndexByItem(1,1));

	/*SetStatus(3,2,GetIndexByItem(3,1));
	SetStatus(3,3,GetIndexByItem(3,1));
	SetStatus(3,4,GetIndexByItem(3,1));
	SetStatus(3,5,GetIndexByItem(3,1));
	SetStatus(3,6,GetIndexByItem(3,1));
	SetStatus(3,7,GetIndexByItem(3,1));
	SetStatus(3,8,GetIndexByItem(3,1));
	SetStatus(3,9,GetIndexByItem(3,1));
	SetStatus(3,10,GetIndexByItem(3,1));
	SetStatus(3,11,GetIndexByItem(3,1));
	SetStatus(3,12,GetIndexByItem(3,1));
	SetStatus(3,13,GetIndexByItem(3,1));*/

	SetStatus(4,2,GetIndexByItem(4,1));
	
}


/////////////////////////////////////////////////////////////

void D2Xguiset::CheckingPartitions()
{
	SetItemByIndex(1,2,p_utils.IsDrivePresent("F:\\"));
	SetItemByIndex(1,3,p_utils.IsDrivePresent("G:\\"));
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
		AnnounceSettings();
		return true;
	}
	
	fread(&version,1,sizeof(unsigned int),stream);
	if(version != g_d2xSettings.current_version)
	{
		SaveConfig();
		AnnounceSettings();
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

	AnnounceSettings();

	return true;
}

int D2Xguiset::GetIndexByItem(int menuid, int itemid)
{
	return SetMenu[menuid].items[itemid].index;
}

int  D2Xguiset::GetIntValueByItem(int menuid, int itemid)
{
	return SetMenu[menuid].items[itemid].v_int[SetMenu[menuid].items[itemid].index];
}

void D2Xguiset::SetItemByIndex(int menuid, int itemid, int index)
{
	SetMenu[menuid].items[itemid].index = index;
	AnnounceSettings();
}

void D2Xguiset::SetStatus(int menuid, int itemid, bool status)
{
	SetMenu[menuid].items[itemid].active = status;
}

bool D2Xguiset::GetStatus(int menuid, int itemid)
{
	return SetMenu[menuid].items[itemid].active;
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
	tmenu.elements = 0;

	SetMenu.insert(i_Pair(menuID,tmenu));

	return true;
}


bool D2Xguiset::AddInt(int menuID, int itemID, CStdString label, bool active, int default_value, int min, int max, int step)
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

	int count = 0;
	for(int i=min; i<=max; i+=step)
	{
		itoa(i,tstr,10);
		titem.values.push_back(tstr);
		titem.v_int.push_back(i);
		if(i == default_value)
			titem.index = count;
		count++;
	}

	m_Iter->second.items.insert(i_Pair(itemID,titem));
	m_Iter->second.elements++;

	return true;
}

bool D2Xguiset::AddHex(int menuID, int itemID, CStdString label, bool active, int default_value, int min, int max, int step)
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

	titem.type = D2X_SET_HEX;
	titem.label = label;
	titem.active = active;
	titem.index = 0;

	int count = 0;
	for(int i=min; i<=max; i+=step)
	{
		//itoa(i,tstr,16);
		sprintf(tstr,"%02X",i);
		titem.values.push_back(tstr);
		titem.v_int.push_back(i);
		if(i == default_value)
			titem.index = count;
		count++;
	}

	m_Iter->second.items.insert(i_Pair(itemID,titem));
	m_Iter->second.elements++;

	return true;
}

bool D2Xguiset::AddString(int menuID, int itemID, CStdString label, bool active, int index, CStdString value)
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
		titem.index = index;
		titem.values.push_back(value);
		m_Iter->second.items.insert(i_Pair(itemID,titem));
		m_Iter->second.elements++;
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

		pressed = true;
	}
	if((pad.fY1 > 0.5)) {
		Sleep(100);
		if(cbrowse > 1)
			cbrowse--;
		
		pressed = true;
	}

	if(pad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
	{
		if(cbrowse < s_item.items)
			cbrowse++;
		
		pressed = true;
	}
	if(pad.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < s_item.items)
			cbrowse++;
		
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
				if(SetMenu[i].elements > 0)
				{
					s_item.items = SetMenu[i].elements;
					s_item.menuID = i;
					s_item.menulabel = SetMenu[i].label;
					s_item.itemID = 1;
					s_item.itemlabel = SetMenu[i].items[1].label;
					s_item.value_index = SetMenu[i].items[1].index;
					s_item.value_label = SetMenu[i].items[1].values[s_item.value_index];
					// Window start values
					cbrowse = 1;

				}
				else
					ExecuteSettings();
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

				//SetMenu[s_item.menuID].items[i].index = s_item.value_index;
				SetItemByIndex(s_item.menuID,i,s_item.value_index);
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
			// Window start values
			cbrowse = s_item.menuID;

			s_item.itemID = 0;
			s_item.itemlabel = "";
			s_item.items = SetMenu.size();
			
			SaveConfig();
		}
	}

	return ret;
}

void D2Xguiset::ShowGUISettings(CXBFont &fontb, CXBFont &fonts)
{
	float tmpy=0;

	map <int, GUISETITEM> :: iterator i_Iter;
	map <int, GUISETMENU> :: iterator m_Iter;

	if(s_item.itemID == 0)
	{
		p_graph.RenderGUISettingsMain();

		for(int c=0;c<s_item.items;c++)
		{
		
			m_Iter = SetMenu.find(c+1);
			if(m_Iter == SetMenu.end())
				continue;

			tmpy = c*fontb.m_fFontHeight;
							
			if(c == (cbrowse-1))
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

		for(int c=0;c<s_item.items;c++)
		{

			i_Iter = SetMenu[s_item.menuID].items.find(c+1);
			if(i_Iter == SetMenu[s_item.menuID].items.end())
				continue;

			tmpy = c*fonts.m_fFontHeight;
				
			if(c == (cbrowse-1))
			{
				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
				fonts.DrawText( START_X_SUB+SPACE_X_SUB, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index]);
			} 
			else 
			{
				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
				fonts.DrawText( START_X_SUB+SPACE_X_SUB, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index] );
			}

		} 
	}

}