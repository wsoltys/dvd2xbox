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

// Online settings

void D2Xsettings::ReadCFG(PDVD2XBOX_CFG cfg)
{
	FILE* stream;
	if((GetFileAttributes(CFG_FILE) == -1))
	{
		WriteDefaultCFG(cfg);
		return;
	}
	stream  = fopen( CFG_FILE, "rb" );
	if(stream == NULL) 
		return;
	fread(cfg,1,sizeof(DVD2XBOX_CFG),stream); 
	fclose(stream);
	if(CURRENT_VERSION > cfg->Version)
		WriteDefaultCFG(cfg);
}

void D2Xsettings::WriteDefaultCFG(PDVD2XBOX_CFG cfg)
{
	cfg->EnableACL = 1;
	cfg->EnableAutoeject = 1;
	cfg->EnableAutopatch = 1;
	cfg->EnableF = false;
	cfg->EnableG = false;
	cfg->EnableNetwork = 0;
	cfg->OggQuality = 0.5;
	cfg->WriteLogfile = 0;
	cfg->Version = CURRENT_VERSION;
	WriteCFG(cfg);
}
 
void D2Xsettings::WriteCFG(PDVD2XBOX_CFG cfg)
{
	FILE* stream;
	stream  = fopen( CFG_FILE, "wb" );
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

	g_d2xSettings.generalError = 0;
	g_d2xSettings.HomePath[0] = '\0'; 
	

	ptr = rootptr->child("network");
	if(ptr->child("cddbip")->value() != NULL)
        strcpy(g_d2xSettings.cddbIP,(char*)ptr->child("cddbip")->value());
	else
		strcpy(g_d2xSettings.cddbIP,"\0");
	if(ptr->child("netmask")->value() != NULL)
        strcpy(g_d2xSettings.netmask,(char*)ptr->child("netmask")->value());
	else
		strcpy(g_d2xSettings.netmask,"\0");
	if(ptr->child("gateway")->value() != NULL)
        strcpy(g_d2xSettings.gateway,(char*)ptr->child("gateway")->value());
	else
		strcpy(g_d2xSettings.gateway,"\0");
	if(ptr->child("nameserver")->value() != NULL)
        strcpy(g_d2xSettings.nameserver,(char*)ptr->child("nameserver")->value());
	else
		strcpy(g_d2xSettings.nameserver,"\0");
	if(ptr->child("xboxip")->value() != NULL)
        strcpy(g_d2xSettings.xboxIP,(char*)ptr->child("xboxip")->value());
	else
		strcpy(g_d2xSettings.xboxIP,"\0");

	// smb
	ptr = rootptr->child("smb");

	if(ptr->child("hostname")->value() != NULL)
        strcpy(g_d2xSettings.smbHostname,(char*)ptr->child("hostname")->value());
	else
		strcpy(g_d2xSettings.smbHostname,"\0");

	if(ptr->child("username")->value() != NULL)
        strcpy(g_d2xSettings.smbUsername,(char*)ptr->child("username")->value());
	else
		strcpy(g_d2xSettings.smbUsername,"\0");

	if(ptr->child("password")->value() != NULL)
        strcpy(g_d2xSettings.smbPassword,(char*)ptr->child("password")->value());
	else
		strcpy(g_d2xSettings.smbPassword,"\0");

	if(ptr->child("domain")->value() != NULL)
        strcpy(g_d2xSettings.smbDomain,(char*)ptr->child("domain")->value());
	else
		strcpy(g_d2xSettings.smbDomain,"\0");

	if(ptr->child("share")->value() != NULL)
        strcpy(g_d2xSettings.smbShare,(char*)ptr->child("share")->value());
	else
		strcpy(g_d2xSettings.smbShare,"\0");

	if((char*)ptr->child("domain")->value() != NULL)
		sprintf(g_d2xSettings.smbDomainUser,"%s;%s",g_d2xSettings.smbDomain,g_d2xSettings.smbUsername);
	else
		strcpy(g_d2xSettings.smbDomainUser,g_d2xSettings.smbUsername);

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