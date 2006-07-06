#include "D2Xguisettings.h"


D2Xguiset::D2Xguiset()
{
	p_input = D2Xinput::Instance();
	p_ml = D2Xmedialib::Instance();
	SetMenu.clear();
	i_skin = 0;
	BuildMenu();

	s_item.itemID = 0;
	s_item.showID = 1;
	s_item.items = SetMenu.size();
	s_item.menuID = 1;
	s_item.menulabel = SetMenu[1].label;

	// Window start values
	cbrowse = 1;
	p_xbad = NULL;

}


D2Xguiset::~D2Xguiset()
{	

}

void D2Xguiset::BuildMenu()
{
	AddMenu(SET_GENERAL,"General",true);
	AddString(SET_GENERAL,1,"Autodetect Partitions",true,1,"no");
	AddString(SET_GENERAL,1,"Autodetect Partitions",true,1,"yes");
	AddString(SET_GENERAL,2,"Enable F: Partition",false,0,"no");
	AddString(SET_GENERAL,2,"Enable F: Partition",false,0,"yes");
	AddString(SET_GENERAL,3,"Enable G: Partition",false,0,"no");
	AddString(SET_GENERAL,3,"Enable G: Partition",false,0,"yes");
	AddString(SET_GENERAL,4,"Enable Logfile",true,0,"no");
	AddString(SET_GENERAL,4,"Enable Logfile",true,0,"yes");
	AddString(SET_GENERAL,5,"Enable ACL processing",true,1,"no");
	AddString(SET_GENERAL,5,"Enable ACL processing",true,1,"yes");
	AddString(SET_GENERAL,6,"Enable ACL RM/MV/EP",true,0,"no");
	AddString(SET_GENERAL,6,"Enable ACL RM/MV/EP",true,0,"yes");
	AddString(SET_GENERAL,7,"Enable Autoeject",true,1,"no");
	AddString(SET_GENERAL,7,"Enable Autoeject",true,1,"yes");
	AddString(SET_GENERAL,8,"Enable LED control",true,0,"no");
	AddString(SET_GENERAL,8,"Enable LED control",true,0,"yes");	
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"off");
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"1 min");
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"2 min");
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"3 min");
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"4 min");
	AddString(SET_GENERAL,9,"Enable Screensaver",true,0,"5 min");
	AddString(SET_GENERAL,10,"Enable Media detection",true,1,"no");
	AddString(SET_GENERAL,10,"Enable Media detection",true,1,"yes");
	AddInt(SET_GENERAL,11,"Failed Copy Retries",true,3,0,20,1);
	AddInt(SET_GENERAL,12,"Copy Read Retries",true,3,0,20,1);
	AddString(SET_GENERAL,13,"Blank video files",true,0,"no");
	AddString(SET_GENERAL,13,"Blank video files",true,0,"yes");
	AddString(SET_GENERAL,14,"Enable unlocker",true,0,"no");
	AddString(SET_GENERAL,14,"Enable unlocker",true,0,"yes");
	AddString(SET_GENERAL,15,"Enable SmartXX RGB LED",true,0,"no");
	AddString(SET_GENERAL,15,"Enable SmartXX RGB LED",true,0,"yes");

	AddMenu(SET_AUDIO,"Audio",true);
	AddString(SET_AUDIO,1,"Encoder",true,0,"MP3");
	AddString(SET_AUDIO,1,"Encoder",true,0,"OggVorbis");
	AddString(SET_AUDIO,1,"Encoder",true,0,"WAV");
	AddString(SET_AUDIO,2,"Ogg Quality",true,1,"low");
	AddString(SET_AUDIO,2,"Ogg Quality",true,1,"medium");
	AddString(SET_AUDIO,2,"Ogg Quality",true,1,"high");
	AddString(SET_AUDIO,3,"Mode",true,0,"Stereo");
	AddString(SET_AUDIO,3,"Mode",true,0,"Joint Stereo");
	AddInt(SET_AUDIO,4,"Bitrate",true,192,64,384,64);

	AddMenu(SET_LCD,"LCD",true);
	AddString(SET_LCD,1,"Enable LCD",true,0,"no");
	AddString(SET_LCD,1,"Enable LCD",true,0,"yes");
	AddString(SET_LCD,2,"Modchip",true,0,"SmartXX");
	AddString(SET_LCD,2,"Modchip",true,0,"Xexuter3");
	AddString(SET_LCD,2,"Modchip",true,0,"Xenium");
	AddString(SET_LCD,3,"Type",true,0,"LCD-KS0073");
	AddString(SET_LCD,3,"Type",true,0,"LCD-HD44780");
	AddString(SET_LCD,3,"Type",true,0,"VFD");
	AddInt(SET_LCD,4,"Columns",true,20,6,40,1);
	AddInt(SET_LCD,5,"Rows",true,4,1,10,1);
	AddHex(SET_LCD,6,"Line1 Address",true,0,0,252,4);
	AddHex(SET_LCD,7,"Line2 Address",true,20,0,252,4);
	AddHex(SET_LCD,8,"Line3 Address",true,64,0,252,4);
	AddHex(SET_LCD,9,"Line4 Address",true,84,0,252,4);
	AddInt(SET_LCD,10,"Backlight",true,80,1,100,1);
	AddInt(SET_LCD,11,"Contrast",true,80,1,100,1);
	AddString(SET_LCD,12,"Enable Scrolling",true,1,"no");
	AddString(SET_LCD,12,"Enable Scrolling",true,1,"yes");


	if(p_utils.IsEthernetConnected() == true)
	{
		AddMenu(SET_NETWORK,"Network",true);
		AddString(SET_NETWORK,1,"Enable Network",true,0,"no");
		AddString(SET_NETWORK,1,"Enable Network",true,0,"yes");
		AddString(SET_NETWORK,2,"Network mode",true,0,"dash");
		AddString(SET_NETWORK,2,"Network mode",true,0,"DHCP");
		AddString(SET_NETWORK,2,"Network mode",true,0,"static");
		AddString(SET_NETWORK,3,"Enable FTP Server",false,0,"no");
		AddString(SET_NETWORK,3,"Enable FTP Server",false,0,"yes");
		AddString(SET_NETWORK,4,"Enable xbox autodetection",true,0,"no");
		AddString(SET_NETWORK,4,"Enable xbox autodetection",true,0,"yes");
		AddString(SET_NETWORK,5,"Send username/password",true,1,"no");
		AddString(SET_NETWORK,5,"Send username/password",true,1,"yes");
	}
	else
		AddMenu(SET_NETWORK,"Network",false);

	if(getSkins(v_skins) == true)
	{
		AddMenu(SET_SKINS,"Select Skin",true);
		for(int i=0;i<v_skins.size();i++)
		{
			AddString(SET_SKINS,1,"Available Skins",true,i_skin,v_skins[i]);
		}
		AddString(SET_SKINS,2,"Load Skin",true,0,"");
	}
	else
		AddMenu(SET_SKINS,"Select Skin",false);

	AddMenu(SET_CALIBRATE,"Calibrate Screen",true);
	AddMenu(SET_RESTORE,"Restore Defaults",true);

	
}

bool D2Xguiset::getSkins(vector<CStdString>& v2_skins)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	int i = 0;
	CStdString	strTemp;
	
	hFind = FindFirstFile( "D:\\skins\\*", &wfd);

	if( INVALID_HANDLE_VALUE == hFind )
	{
		return false;
	} 
	else 
	{
		do
		{
			if (wfd.cFileName[0]!=0)
			{
				// only directories
				if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					// check if it is a dvd2xbox skin
					strTemp.Format("D:\\skins\\%s\\filemanager.xml",wfd.cFileName);
					if(GetFileAttributes(strTemp.c_str()) != -1)
					{
						v2_skins.push_back(wfd.cFileName);
						if(!_stricmp(wfd.cFileName,g_d2xSettings.strskin))
							i_skin = i;
						i++;
					}
				}
			}

		}
		while(FindNextFile( hFind, &wfd ));
		// Close the find handle.
		FindClose( hFind);
	}
	return true;
}

void D2Xguiset::StartAutoDetect()
{
	p_xbad = new D2Xxbautodetect();
	p_xbad->Create();
	DebugOut("Autodetection Thread started\n");
}

void D2Xguiset::StopAutoDetect()
{
	if(p_xbad != NULL)
	{
		p_xbad->StopThread();
		delete p_xbad;
		p_xbad = NULL;
		DebugOut("Autodetection Thread stopped\n");
	}
}

int D2Xguiset::ExecuteSettings()
{
	int ret = D2X_GUI_PROCESS;

	if(s_item.menuID == SET_GENERAL)
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
			SetItemByIndex(SET_GENERAL,2,s_item.value_index);
			ret = D2X_GUI_MAPDRIVES;
			break;
		case 3:
			SetItemByIndex(SET_GENERAL,3,s_item.value_index);
			ret = D2X_GUI_MAPDRIVES;
			break;
		case 10:
			ret = s_item.value_index ? D2X_GUI_START_MEDIAD : D2X_GUI_STOP_MEDIAD;
			break;
		case 15:
			D2Xutils::SetSmartXXRGB(s_item.value_index ? STAT_GENERAL : STAT_OFF);
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == SET_AUDIO)
	{
	}
	else if(s_item.menuID == SET_LCD)
	{
		switch(s_item.itemID)
		{
		case 1:
			ret = s_item.value_index ? D2X_GUI_START_LCD : D2X_GUI_STOP_LCD;
			break;
		case 2:
			if(GetIndexByItem(SET_LCD,1))
				ret = D2X_GUI_RESTART_LCD;
			break;
		case 10:
		case 11:
			if(GetIndexByItem(SET_LCD,1))
				ret = D2X_GUI_SET_LCD;
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == SET_NETWORK)
	{
		switch(s_item.itemID)
		{
		case 1:
			/*if(s_item.value_index == 0)
			{
				SetItemByIndex(4,3,0);
				SetItemByIndex(4,4,0);
				StopAutoDetect();
			}*/
			ret = s_item.value_index ? D2X_GUI_START_NET : D2X_GUI_STOP_NET;
			break;
		case 3:
			if(s_item.value_index == 0)
			{
				//SetItemByIndex(4,3,0);
				//StopAutoDetect();
			}
			ret = s_item.value_index ? D2X_GUI_START_FTPD : D2X_GUI_STOP_FTPD;
			break;
		case 4:
			if(s_item.value_index == 1)
				StartAutoDetect();
			else
				StopAutoDetect();
			
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == SET_SKINS)
	{
		switch(s_item.itemID)
		{
		case 1:
			{
				g_d2xSettings.strskin = v_skins[GetIndexByItem(SET_SKINS,1)];
				ret = D2X_GUI_SAVE_SKIN;
			}
			break;
		case 2:
			{
				g_d2xSettings.strskin = v_skins[GetIndexByItem(SET_SKINS,1)];
				SaveConfig();
				ret = D2X_GUI_RESTART;
			}
			break;
		default:
			break;
		}
	}
	else if(s_item.menuID == SET_CALIBRATE)
	{
		ret = D2X_GUI_CALIBRATION;
	}
	else if(s_item.menuID == SET_RESTORE)
	{
		DeleteFile(D2X_CONFIG_FILE);
		DeleteFile(g_d2xSettings.ConfigPath);
		p_utils.Reboot();
	}
	AnnounceSettings();
	return ret;
}

void D2Xguiset::AnnounceSettings()
{
	g_d2xSettings.autodetectHDD = GetIndexByItem(SET_GENERAL,1);
	g_d2xSettings.useF = GetIndexByItem(SET_GENERAL,2);
	g_d2xSettings.useG = GetIndexByItem(SET_GENERAL,3);
	g_d2xSettings.WriteLogfile = GetIndexByItem(SET_GENERAL,4);
	g_d2xSettings.enableACL = GetIndexByItem(SET_GENERAL,5);
	g_d2xSettings.enableRMACL = GetIndexByItem(SET_GENERAL,6);
	g_d2xSettings.enableAutoeject = GetIndexByItem(SET_GENERAL,7);
	g_d2xSettings.enableLEDcontrol = GetIndexByItem(SET_GENERAL,8);
	g_d2xSettings.ScreenSaver = GetIndexByItem(SET_GENERAL,9);
	g_d2xSettings.detect_media_change = GetIndexByItem(SET_GENERAL,10);
	g_d2xSettings.autoCopyRetries = GetIndexByItem(SET_GENERAL,11);
	g_d2xSettings.autoReadRetries = GetIndexByItem(SET_GENERAL,12);
	g_d2xSettings.replaceVideo = GetIndexByItem(SET_GENERAL,13);
	g_d2xSettings.enableUnlocker = GetIndexByItem(SET_GENERAL,14);
	g_d2xSettings.enableSmartXXRGB = GetIndexByItem(SET_GENERAL,15);

	if(GetIndexByItem(SET_AUDIO,1) == 0)
		g_d2xSettings.cdda_encoder = MP3LAME;
	else if(GetIndexByItem(SET_AUDIO,1) == 1)
		g_d2xSettings.cdda_encoder = OGGVORBIS;
	else if(GetIndexByItem(SET_AUDIO,1) == 2)
		g_d2xSettings.cdda_encoder = WAV;

	if(GetIndexByItem(SET_AUDIO,2)==0)
		g_d2xSettings.ogg_quality = 0.1;
	else if(GetIndexByItem(SET_AUDIO,2)==1)
		g_d2xSettings.ogg_quality = 0.5;
	else if(GetIndexByItem(SET_AUDIO,2)==2)
		g_d2xSettings.ogg_quality = 1.0;
	g_d2xSettings.mp3_mode = GetIndexByItem(SET_AUDIO,3);
	g_d2xSettings.mp3_bitrate = GetIntValueByItem(SET_AUDIO,4);

	g_d2xSettings.m_bLCDUsed = GetIndexByItem(SET_LCD,1);
	if(GetIndexByItem(SET_LCD,2) == 2)
		g_d2xSettings.m_iLCDModChip = MODCHIP_XENIUM;
	else if(GetIndexByItem(SET_LCD,2) == 1)
		g_d2xSettings.m_iLCDModChip = MODCHIP_XECUTER3;
	else
        g_d2xSettings.m_iLCDModChip = MODCHIP_SMARTXX;
	
    g_d2xSettings.m_iLCDType=GetIndexByItem(SET_LCD,3);
	g_d2xSettings.m_iLCDColumns=GetIntValueByItem(SET_LCD,4);
	g_d2xSettings.m_iLCDRows=GetIntValueByItem(SET_LCD,5);
	g_d2xSettings.m_iLCDAdress[0]=GetIntValueByItem(SET_LCD,6);
	g_d2xSettings.m_iLCDAdress[1]=GetIntValueByItem(SET_LCD,7);
	g_d2xSettings.m_iLCDAdress[2]=GetIntValueByItem(SET_LCD,8);
	g_d2xSettings.m_iLCDAdress[3]=GetIntValueByItem(SET_LCD,9);
	g_d2xSettings.m_iLCDBackLight=GetIntValueByItem(SET_LCD,10);
	g_d2xSettings.m_iContrast = GetIntValueByItem(SET_LCD,11);
	g_d2xSettings.enableLCDScrolling = GetIndexByItem(SET_LCD,12);

	g_d2xSettings.network_enabled = GetIndexByItem(SET_NETWORK,1);
	g_d2xSettings.network_assignment = GetIndexByItem(SET_NETWORK,2);
	g_d2xSettings.ftpd_enabled = GetIndexByItem(SET_NETWORK,3);
	g_d2xSettings.autodetect_enabled = GetIndexByItem(SET_NETWORK,4);
	g_d2xSettings.autodetect_send_pwd = GetIndexByItem(SET_NETWORK,5);

	SetStatus(SET_GENERAL,2,!GetIndexByItem(SET_GENERAL,1));
	SetStatus(SET_GENERAL,3,!GetIndexByItem(SET_GENERAL,1));

	SetStatus(SET_NETWORK,3,GetIndexByItem(SET_NETWORK,1));
}


/////////////////////////////////////////////////////////////

void D2Xguiset::CheckingPartitions()
{
	SetItemByIndex(SET_GENERAL,2,p_utils.IsDrivePresent("F:\\"));
	SetItemByIndex(SET_GENERAL,3,p_utils.IsDrivePresent("G:\\"));
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
		fclose(stream);
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

int D2Xguiset::Process(XBGAMEPAD* pad,XBIR_REMOTE* ir)
{
	int ret = D2X_GUI_PROCESS;
	bool pressed = false;
	//p_input->update(pad,ir);

	if(p_input->pressed(C_UP)) 
	{
		if(cbrowse > 1)
			cbrowse--;

		pressed = true;
	}
	//if((pad->fY1 > 0.5)) 
	if(p_input->pressed(GP_LTRIGGER_P) || (pad->fY1 > 0.5))
	{
		//Sleep(100);
		if(cbrowse > 1)
			cbrowse--;
		
		pressed = true;
	}

	if(p_input->pressed(C_DOWN)) 
	{
		if(cbrowse < s_item.items)
			cbrowse++;
		
		pressed = true;
	}
	//if(pad->fY1 < -0.5) 
	if(p_input->pressed(GP_RTRIGGER_P) || (pad->fY1 < -0.5) )
	{
		//Sleep(100);
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

	if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT) || p_input->pressed(IR_RIGHT))
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
					s_item.showID = i+1;
					s_item.menulabel = SetMenu[i].label;
					s_item.itemID = 1;
					s_item.itemlabel = SetMenu[i].items[1].label;
					s_item.value_index = SetMenu[i].items[1].index;
					s_item.value_label = SetMenu[i].items[1].values[s_item.value_index];
					// Window start values
					cbrowse = 1;

				}
				else
					ret = ExecuteSettings();
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
	if(p_input->pressed(GP_B)||p_input->pressed(IR_LEFT))
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

	if(p_input->pressed(GP_BACK))
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
			s_item.showID = 1;
			s_item.itemlabel = "";
			s_item.items = SetMenu.size();
			
			SaveConfig();
		}
	}

	// stupid workaround
	map <int, GUISETITEM> :: iterator i_Iter;
	map <int, GUISETMENU> :: iterator m_Iter;

	if(s_item.itemID == 0)
	{

		for(int c=0;c<s_item.items;c++)
		{
		
			m_Iter = SetMenu.find(c+1);
			if(m_Iter == SetMenu.end())
				continue;

			short tmpy = c*i_vspace;
							
			if(c == (cbrowse-1))
			{
				gs_x = start_x;
				gs_y = start_y+tmpy;
			} 

		} 
	}
	else
	{


		for(int c=0;c<s_item.items;c++)
		{

			i_Iter = SetMenu[s_item.menuID].items.find(c+1);
			if(i_Iter == SetMenu[s_item.menuID].items.end())
				continue;

			short tmpy = c*i_vspace;
				
			if(c == (cbrowse-1))
			{
				gs_x = start_x;
				gs_y = start_y+tmpy;

			} 

		} 
	}

	return ret;
}

//void D2Xguiset::ShowGUISettings(CXBFont &fontb, CXBFont &fonts)
//{
//	float tmpy=0;
//
//	map <int, GUISETITEM> :: iterator i_Iter;
//	map <int, GUISETMENU> :: iterator m_Iter;
//
//	if(s_item.itemID == 0)
//	{
//		p_graph.RenderGUISettingsMain(0,START_Y_MAIN-20,640,START_Y_MAIN+s_item.items*fontb.m_fFontHeight+20);
//
//		fontb.DrawText(40,30,TEXT_COLOR_MAIN,L"Settings:");
//
//		for(int c=0;c<s_item.items;c++)
//		{
//		
//			m_Iter = SetMenu.find(c+1);
//			if(m_Iter == SetMenu.end())
//				continue;
//
//			tmpy = c*fontb.m_fFontHeight;
//							
//			if(c == (cbrowse-1))
//			{
//				p_graph.RenderBar(0, START_Y_MAIN+tmpy,fontb.m_fFontHeight,640);
//				fontb.DrawText( START_X_MAIN, START_Y_MAIN+tmpy, HIGHLITE_COLOR_MAIN, SetMenu[c+1].label );
//			} else {
//				fontb.DrawText( START_X_MAIN, START_Y_MAIN+tmpy, TEXT_COLOR_MAIN, SetMenu[c+1].label );
//			}
//
//		} 
//	}
//	else
//	{
//		p_graph.RenderGUISettingsSub(0,START_Y_SUB-20,640,START_Y_SUB+s_item.items*fonts.m_fFontHeight+20);
//
//		fontb.DrawText(40,30,TEXT_COLOR_MAIN,L"Settings:");
//		fontb.DrawText(140,30,TEXT_COLOR_MAIN,s_item.menulabel.c_str());
//
//		for(int c=0;c<s_item.items;c++)
//		{
//
//			i_Iter = SetMenu[s_item.menuID].items.find(c+1);
//			if(i_Iter == SetMenu[s_item.menuID].items.end())
//				continue;
//
//			tmpy = c*fonts.m_fFontHeight;
//				
//			if(c == (cbrowse-1))
//			{
//				p_graph.RenderBar(0, START_Y_SUB+tmpy,fonts.m_fFontHeight,640);
//				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
//				fonts.DrawText( START_X_SUB+SPACE_X_SUB, START_Y_SUB+tmpy, HIGHLITE_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index]);
//			} 
//			else 
//			{
//				fonts.DrawText( START_X_SUB, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].label );
//				fonts.DrawText( START_X_SUB+SPACE_X_SUB, START_Y_SUB+tmpy, TEXT_COLOR_SUB, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index] );
//			}
//
//		} 
//	}
//
//}

// gui stuff

int	D2Xguiset::getShowID()
{
	return s_item.showID;
}

void D2Xguiset::getXY(float* posX, float* posY)
{
	*posX = gs_x;
	*posY = gs_y;
}

void D2Xguiset::getOrigin(float* posX, float* posY)
{
	*posX = start_x;
	*posY = start_y;
}

int D2Xguiset::getItems(int* vspace)
{
	if(vspace != NULL)
		*vspace = v_space;

	return s_item.items;
}


void D2Xguiset::ShowGUISettings2(float x,float y,int hspace,int width, int widthpx,int vspace,DWORD fc,DWORD hlfc,const CStdString& font, DWORD dwFlags, bool scroll)
{
	float tmpy=0;

	map <int, GUISETITEM> :: iterator i_Iter;
	map <int, GUISETMENU> :: iterator m_Iter;


	// workaround
	if(vspace == 0)
        i_vspace = p_ml->getFontHeight(font);
	else
		i_vspace = vspace;

	v_space = i_vspace;

	start_x = x;
	start_y = y;

	if(s_item.itemID == 0)
	{

		for(int c=0;c<s_item.items;c++)
		{
		
			m_Iter = SetMenu.find(c+1);
			if(m_Iter == SetMenu.end())
				continue;

			tmpy = c*i_vspace;
							
			if(c == (cbrowse-1))
			{
				/*gs_x = x;
				gs_y = y+tmpy;*/
				p_ml->DrawText(font, x, y+tmpy, hlfc, SetMenu[c+1].label, XBFONT_TRUNCATED, dwFlags, (float)widthpx, scroll );
			} else {
				p_ml->DrawText(font, x, y+tmpy, fc, SetMenu[c+1].label , XBFONT_TRUNCATED, dwFlags, (float)widthpx, false);
			}

		} 
	}
	else
	{


		for(int c=0;c<s_item.items;c++)
		{

			i_Iter = SetMenu[s_item.menuID].items.find(c+1);
			if(i_Iter == SetMenu[s_item.menuID].items.end())
				continue;

			tmpy = c*i_vspace;
				
			if(c == (cbrowse-1))
			{
				/*gs_x = x;
				gs_y = y+tmpy;*/

				p_ml->DrawText(font, x, y+tmpy, hlfc, SetMenu[s_item.menuID].items[c+1].label );
				p_ml->DrawText(font, x+hspace, y+tmpy, hlfc, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index], scroll);
			} 
			else 
			{
				p_ml->DrawText(font, x, y+tmpy, fc, SetMenu[s_item.menuID].items[c+1].label );
				p_ml->DrawText(font, x+hspace, y+tmpy, fc, SetMenu[s_item.menuID].items[c+1].values[SetMenu[s_item.menuID].items[c+1].index],false );
			}

		} 
	}

}