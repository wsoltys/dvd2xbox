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
	g_d2xSettings.generalDialog = 0;
	g_d2xSettings.HomePath[0] = '\0'; 
	g_d2xSettings.current_version = 72;

	strcpy(g_d2xSettings.ConfigPath,"e:\\TDATA\\0FACFAC0\\metai.d2x");
	strcpy(g_d2xSettings.disk_statsPath,"e:\\TDATA\\0FACFAC0\\dstats.d2x");
	strcpy(g_d2xSettings.disk_statsPath_new,"e:\\TDATA\\0FACFAC0\\new_dstats.d2x");
	strcpy(g_d2xSettings.TDATApath,"e:\\TDATA\\0FACFAC0\\");

	g_d2xSettings.detected_media = UNDEFINED;
	
	g_d2xSettings.strskin = "Project_Mayhem_III";
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

	

	strcpy(g_d2xSettings.ftpIP, cfg->ftpIP);
	strcpy(g_d2xSettings.ftppwd, cfg->ftppwd);
	strcpy(g_d2xSettings.ftpuser, cfg->ftpuser);

	g_d2xSettings.strskin = cfg->skin;
}

void D2Xsettings::WriteDefaultCFG(PDVD2XBOX_CFG cfg)
{
	cfg->Version = g_d2xSettings.current_version;
	strcpy(cfg->ftpIP,"192.168.1.1");
	strcpy(cfg->ftpuser,"xbox");
	strcpy(cfg->ftppwd,"xbox");
	strcpy(cfg->skin,g_d2xSettings.strskin);
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

void D2Xsettings::getXMLValue(const char* root, const char* key, CStdString& xml_value, const std::string default_value)
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
					xml_value = s_value;
			}
		}
	}
	if(strlen(xml_value)==0)
		xml_value = default_value;
	
	return;
}

void D2Xsettings::getXMLValueUS(const char* root, const char* key, unsigned short& xml_value, int default_value)
{
	TiXmlNode* node = 0;
	xml_value=9999;
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
	if(xml_value==9999)
		xml_value = default_value;
	
	return;
}


void D2Xsettings::getDumpDirs(std::map<int,std::string> &ddirs)
{
	ddirs.clear();
	int x = 0;
	char tempdir[1024];
	
	for(int i=0;i<xmlDumpDirs.size();i++)
	{
		strcpy(tempdir,xmlDumpDirs[i].c_str());
		
		if(!_strnicmp(tempdir,"e:",2) || 
		(!_strnicmp(tempdir,"f:",2) && g_d2xSettings.useF) ||
		(!_strnicmp(tempdir,"g:",2) && g_d2xSettings.useG) ||
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
	{
		itemElement2 = itemElement->FirstChildElement("smb");
		if(itemElement2)
		{
			itemElement2 = itemElement2->FirstChildElement("shares");
			if(itemElement2)
			{
				for( node = itemElement2->FirstChild( "share" );
				node;
				node = node->NextSibling( "share" ) )
				{
					g_d2xSettings.xmlSmbUrls.insert(pair<CStdString,CStdString>(node->FirstChild("name")->FirstChild()->Value(),node->FirstChild("url")->FirstChild()->Value())); 
				}
			}
		}
	}

	//main
	unsigned short autodetectHDD;
	getXMLValueUS("main","autodetectHDD",autodetectHDD,1);
	if(autodetectHDD == 0)
	{
		// xml settings overwrite online settings if disabled
		g_d2xSettings.autodetectHDD = 0;
	}
	getXMLValue("main","textExtensions",g_d2xSettings.strTextExt,"txt|nfo|cfg|ini");
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
	getXMLValue("ftpserver","nickname",g_d2xSettings.strAutoDetectNick,"dvd2xbox");

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