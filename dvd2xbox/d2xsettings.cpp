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
	g_d2xSettings.HomePath[0] = '\0'; 
	g_d2xSettings.current_version = 54;
	g_d2xSettings.enableRMACL = 0;
	strcpy(g_d2xSettings.ConfigPath,"e:\\TDATA\\0FACFAC0\\metai.d2x");
	strcpy(g_d2xSettings.TDATApath,"e:\\TDATA\\0FACFAC0\\");
	
}

// Online settings

void D2Xsettings::ReadCFG(PDVD2XBOX_CFG cfg)
{
	FILE* stream;
	if((GetFileAttributes(g_d2xSettings.ConfigPath) == -1))
	{
		WriteDefaultCFG(cfg);
		return;
	}
	stream  = fopen( g_d2xSettings.ConfigPath, "rb" );
	if(stream == NULL) 
		return;
	fread(cfg,1,sizeof(DVD2XBOX_CFG),stream); 
	fclose(stream);
	if(g_d2xSettings.current_version > cfg->Version)
		WriteDefaultCFG(cfg);

	g_d2xSettings.enableRMACL = cfg->EnableRMACL;
}

void D2Xsettings::WriteDefaultCFG(PDVD2XBOX_CFG cfg)
{
	cfg->EnableACL = 1;
	cfg->EnableRMACL = 0;
	cfg->EnableAutoeject = 1;
	cfg->EnableAutopatch = 1;
	cfg->EnableF = false;
	cfg->EnableG = false;
	cfg->EnableNetwork = 0;
	cfg->OggQuality = 0.5;
	cfg->WriteLogfile = 0;
	cfg->Version = g_d2xSettings.current_version;
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
int D2Xsettings::readIni(char* file)
{
	FILE* fh;
	int read;
	simplexml			*rootptr;
	simplexml			*ptr;

	if((fh=fopen(file,"rt"))==NULL)
		return 1;
	read = fread(XMLbuffer,sizeof(char),XML_BUFFER,fh);
	fclose(fh);
	if(read < 10)
		return 1;
	rootptr = new simplexml(XMLbuffer);

	ptr = rootptr->child("network");

    strcpy(g_d2xSettings.cddbIP,(char*)ptr->child("cddbip")->value());
	
    strcpy(g_d2xSettings.netmask,(char*)ptr->child("netmask")->value());

    strcpy(g_d2xSettings.gateway,(char*)ptr->child("gateway")->value());

    strcpy(g_d2xSettings.nameserver,(char*)ptr->child("nameserver")->value());

    strcpy(g_d2xSettings.xboxIP,(char*)ptr->child("xboxip")->value());

	if(!strncmp(g_d2xSettings.gateway,"-",1))
		strcpy(g_d2xSettings.gateway,"\0");
	if(!strncmp(g_d2xSettings.xboxIP,"-",1))
		strcpy(g_d2xSettings.xboxIP,"\0");
	if(!strncmp(g_d2xSettings.netmask,"-",1))
		strcpy(g_d2xSettings.netmask,"\0");



	// smb
	ptr = rootptr->child("smb");


    strcpy(g_d2xSettings.smbHostname,(char*)ptr->child("hostname")->value());

    strcpy(g_d2xSettings.smbUsername,(char*)ptr->child("username")->value());

    strcpy(g_d2xSettings.smbPassword,(char*)ptr->child("password")->value());

    strcpy(g_d2xSettings.smbDomain,(char*)ptr->child("domain")->value());

    strcpy(g_d2xSettings.smbShare,(char*)ptr->child("share")->value());


	if((strncmp(g_d2xSettings.smbDomain,"-",1) != 0) && (strncmp(g_d2xSettings.smbUsername,"-",1) != 0))
		sprintf(g_d2xSettings.smbDomainUser,"%s;%s",g_d2xSettings.smbDomain,g_d2xSettings.smbUsername);
	else if(strncmp(g_d2xSettings.smbUsername,"-",1) != 0)
		strcpy(g_d2xSettings.smbDomainUser,g_d2xSettings.smbUsername);
	else
		strcpy(g_d2xSettings.smbDomainUser,"\0");

	return 0;
}	

/*
char* D2Xsettings::getIniValue(const char* root, const char* key)
{
	ptr = rootptr->child(root);
	char* value = (char*)ptr->child(key)->value();
	if(value == NULL)
		return 0;
	else
		return value;
	
	//return ptr->child(key)->value();
}
*/