#include "d2xsettings.h"

std::auto_ptr<D2Xsettings> D2Xsettings::sm_inst;

struct D2Xsettings::d2xSettings g_d2xSettings;

D2Xsettings* D2Xsettings::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xsettings> tmp(new D2Xsettings);
        sm_inst = tmp;
    }
    return sm_inst.get();
}

D2Xsettings::D2Xsettings()
{
	g_d2xSettings.generalError = 0;
	g_d2xSettings.generalNotice = 0;
	g_d2xSettings.HomePath[0] = '\0'; 
	g_d2xSettings.current_version = 63;
	g_d2xSettings.enableRMACL = 0;
	strcpy(g_d2xSettings.ConfigPath,"e:\\TDATA\\0FACFAC0\\metai.d2x");
	strcpy(g_d2xSettings.disk_statsPath,"e:\\TDATA\\0FACFAC0\\dstats.d2x");
	strcpy(g_d2xSettings.disk_statsPath_new,"e:\\TDATA\\0FACFAC0\\new_dstats.d2x");
	strcpy(g_d2xSettings.TDATApath,"e:\\TDATA\\0FACFAC0\\");
	g_d2xSettings.cdda_encoder = OGGVORBIS;
	g_d2xSettings.detected_media = UNDEFINED;
	g_d2xSettings.ftpd_enabled = 0;

	// taken from xbmc
	g_d2xSettings.m_iLCDModChip=MODCHIP_SMARTXX;
	g_d2xSettings.m_bLCDUsed=false;
	g_d2xSettings.m_iLCDMode=0;
	g_d2xSettings.m_iLCDColumns=20;
	g_d2xSettings.m_iLCDRows=4;
	g_d2xSettings.m_iLCDBackLight=80;
	g_d2xSettings.m_iLCDBrightness=100;
	g_d2xSettings.m_iLCDType=LCD_MODE_TYPE_LCD;
	g_d2xSettings.m_iLCDAdress[0]=0x0;
	g_d2xSettings.m_iLCDAdress[1]=0x14;
	g_d2xSettings.m_iLCDAdress[2]=0x40;
	g_d2xSettings.m_iLCDAdress[3]=0x54;
	g_d2xSettings.m_iContrast = 100;
	
}

// Online settings

void D2Xsettings::ReadCFG(PDVD2XBOX_CFG cfg)
{
	FILE* stream;
	if((GetFileAttributes(g_d2xSettings.ConfigPath) == -1))
	{
		WriteDefaultCFG(cfg);
	}
	stream  = fopen( g_d2xSettings.ConfigPath, "rb" );
	if(stream == NULL) 
		return;
	fread(cfg,1,sizeof(DVD2XBOX_CFG),stream); 
	fclose(stream);
	if(g_d2xSettings.current_version != cfg->Version)
		WriteDefaultCFG(cfg);

	g_d2xSettings.enableRMACL = cfg->EnableRMACL;
	g_d2xSettings.cdda_encoder = cfg->cdda_encoder;
	g_d2xSettings.mp3_mode = cfg->mp3_mode;
	g_d2xSettings.mp3_bitrate = cfg->mp3_bitrate;
	g_d2xSettings.detect_media_change = cfg->detect_media_change;
	if(cfg->useLCD != LCD_NONE)
	{
		g_d2xSettings.m_bLCDUsed = true;
		if(cfg->useLCD == MODCHIP_SMARTXX)
            g_d2xSettings.m_iLCDModChip = MODCHIP_SMARTXX;
		else if(cfg->useLCD == MODCHIP_XENIUM)
            g_d2xSettings.m_iLCDModChip = MODCHIP_XENIUM;
		else
			g_d2xSettings.m_bLCDUsed = false;
	}
	else
		g_d2xSettings.m_bLCDUsed = false;

	strcpy(g_d2xSettings.ftpIP, cfg->ftpIP);
	strcpy(g_d2xSettings.ftppwd, cfg->ftppwd);
	strcpy(g_d2xSettings.ftpuser, cfg->ftpuser);
	g_d2xSettings.ogg_quality = cfg->OggQuality;
}

void D2Xsettings::WriteDefaultCFG(PDVD2XBOX_CFG cfg)
{
	cfg->EnableACL = 1;
	cfg->EnableRMACL = 0;
	cfg->EnableAutoeject = 1;
	cfg->EnableLEDcontrol = 0;
	cfg->EnableAutopatch = 0;
	cfg->EnableF = false;
	cfg->EnableG = false;
	cfg->EnableNetwork = 0;
	cfg->Enableftpd = 0;
	cfg->OggQuality = 0.5;
	cfg->mp3_mode = 0; // 0 = stereo, 1 = jstereo
	cfg->mp3_bitrate = 192;
	cfg->WriteLogfile = 0;
	cfg->Version = g_d2xSettings.current_version;
	cfg->cdda_encoder = MP3LAME;
	cfg->useLCD = LCD_NONE;
	cfg->detect_media_change = true;
	strcpy(cfg->ftpIP,"192.168.1.1");
	strcpy(cfg->ftpuser,"xbox");
	strcpy(cfg->ftppwd,"xbox");
	WriteCFG(cfg);
}
 
void D2Xsettings::WriteCFG(PDVD2XBOX_CFG cfg)
{
	FILE* stream;
	stream  = fopen( g_d2xSettings.ConfigPath, "wb" );
	if(stream == NULL) 
		return;
	fwrite(cfg,1,sizeof(DVD2XBOX_CFG),stream);
	fclose(stream);
}

// XML settings


void D2Xsettings::getXMLValue(const char* root, const char* key, char* xml_value, const std::string default_value)
{
	TiXmlNode* node = 0;
	TiXmlElement* itemElement2 = 0;
	{
		itemElement2 = itemElement->FirstChildElement(root);
		if(itemElement2)
		{
			node = itemElement2->FirstChild(key);
			if(node)
			{
				std::string s_value=node->FirstChild()->Value();
				if(s_value.size() && (s_value != "-"))
					strcpy(xml_value,s_value.c_str());
			}
		}
	}
	if(strlen(xml_value)==0)
		strcpy(xml_value,default_value.c_str());
	
	return;
}

void D2Xsettings::getXMLValueUS(const char* root, const char* key, unsigned short& xml_value, int default_value)
{
	TiXmlNode* node = 0;
	TiXmlElement* itemElement2 = 0;
	{
		itemElement2 = itemElement->FirstChildElement(root);
		if(itemElement2)
		{
			node = itemElement2->FirstChild(key);
			if(node)
			{
				std::string s_value=node->FirstChild()->Value();
				if(s_value.size() && (s_value != "-"))
					xml_value = atoi(s_value.c_str());
			}
		}
	}
	if(xml_value==0)
		xml_value = default_value;
	
	return;
}

void D2Xsettings::getDumpDirs(std::map<int,std::string> &ddirs,PDVD2XBOX_CFG cfg)
{
	ddirs.clear();
	int x = 0;
	char tempdir[1024];
	
	for(int i=0;i<xmlDumpDirs.size();i++)
	{
		strcpy(tempdir,xmlDumpDirs[i].c_str());
		
		if(!_strnicmp(tempdir,"e:",2) || 
		(!_strnicmp(tempdir,"f:",2) && cfg->EnableF) ||
		(!_strnicmp(tempdir,"g:",2) && cfg->EnableG) ||
		!_strnicmp(tempdir,"c:",2) ||
		!_strnicmp(tempdir,"x:",2) ||
		!_strnicmp(tempdir,"y:",2) ||
		!_strnicmp(tempdir,"z:",2) /*||
		(!_strnicmp(tempdir,"ftp:",4) && cfg->EnableNetwork) ||
		(!_strnicmp(tempdir,"smb:",4) && cfg->EnableNetwork)*/)
		{
			ddirs.insert(std::pair<int,std::string>(x,tempdir)); 
			x++;
		}

	}
}

int D2Xsettings::readXML(char* file)
{
	char test[50];
	TiXmlDocument xmldoc( file );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;
	itemElement = xmldoc.RootElement();
	if( !itemElement )
		return 0;

	TiXmlElement* itemElement2 = 0;
	TiXmlNode* node = 0;
	{
		itemElement2 = itemElement->FirstChildElement("dumpdirs");
		if(itemElement2)
		{
			for( node = itemElement2->FirstChild( "dir" );
			node;
			node = node->NextSibling( "dir" ) )
			{
				xmlDumpDirs.push_back(node->FirstChild()->Value());	
				//CStdString strFileName = itemElement2->Attribute("name");
			}
		}
	}
	{
		itemElement2 = itemElement->FirstChildElement("gamedirs");
		if(itemElement2)
		{
			for( node = itemElement2->FirstChild( "dir" );
			node;
			node = node->NextSibling( "dir" ) )
			{
				g_d2xSettings.xmlGameDirs.push_back(node->FirstChild()->Value());
			}
		}
	}

	//main
	getXMLValueUS("main","autodetectHDD",g_d2xSettings.autodetectHDD,0);
	getXMLValue("main","trackformat",g_d2xSettings.trackformat,"${TRACK}-${TRACKARTIST}-${TITLE}");

	//network
	getXMLValue("network","xboxip",g_d2xSettings.xboxIP,"");
	getXMLValue("network","netmask",g_d2xSettings.netmask,"");
	getXMLValue("network","gateway",g_d2xSettings.gateway,"");
	getXMLValue("network","cddbip",g_d2xSettings.cddbIP,"195.37.77.133");

	getXMLValue("smb","workgroup",g_d2xSettings.smbWorkgroup,"WORKGROUP");
	getXMLValue("smb","winsserver",g_d2xSettings.winsserver,"");
	getXMLValue("smb","url",g_d2xSettings.smbUrl,"-");

	getXMLValue("ftpserver","username",g_d2xSettings.ftpduser,"xbox");
	getXMLValue("ftpserver","password",g_d2xSettings.ftpd_pwd,"xbox");
	getXMLValueUS("ftpserver","max_users",g_d2xSettings.ftpd_max_user,4);


	return 1;
}

void DebugOut(char *message,...)
{
	char expanded_message[1024];
	va_list tGlop;
	// Expand the varable argument list into the text buffer
	va_start(tGlop,message);
	if(vsprintf(expanded_message,message,tGlop)==-1)
	{
		// Fatal overflow, lets abort
		return;
	}
	va_end(tGlop);
	OutputDebugString(expanded_message);
}