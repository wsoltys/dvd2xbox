#ifndef D2XSETTINGS
#define D2XSETTINGS

#include <xtl.h>
#include <memory>
#include <undocumented.h>
#include "simplexml.h"

#define CFG_FILE			"e:\\TDATA\\0FACFAC0\\metai.d2x"
#define CURRENT_VERSION		54
#define XML_BUFFER			2048

#define UNKNOWN			1
#define DVD				2
#define GAME			3
#define ISO				4
#define CDDA			5
#define VCD				6
#define SVCD			7
#define UDF				20
#define SMBDIR			21
#define FTP				22
#define UDF2SMB			23

typedef struct _DVD2XBOX_CFG {
	// 20 Bytes
	unsigned int	Version;
	bool			EnableF;
	bool			EnableG;
	unsigned short	WriteLogfile;
	unsigned short	EnableACL;
	unsigned short	EnableAutopatch;
	unsigned short	EnableAutoeject;
	unsigned short	EnableNetwork;
	float			OggQuality;
} DVD2XBOX_CFG, *PDVD2XBOX_CFG;


class D2Xsettings
{
private:
	static std::auto_ptr<D2Xsettings> sm_inst;
	D2Xsettings() {};

	char				XMLbuffer[XML_BUFFER];
	
public:
	static D2Xsettings* Instance();
	
	// Online settings
	void		ReadCFG(PDVD2XBOX_CFG cfg);
	void		WriteCFG(PDVD2XBOX_CFG cfg);
	void		WriteDefaultCFG(PDVD2XBOX_CFG cfg);
	// XML settings
	int			readIni(char* file);
	//char* getIniValue(const char* root,const char* key);

	struct d2xSettings
	{
	public:
		char		xboxIP[16];
		char		netmask[16];
		char		gateway[16];
		char		nameserver[16];
		char		cddbIP[16];

		char		smbHostname[128];
		char		smbUsername[16];
		char		smbPassword[16];
		char		smbDomain[128];
		char		smbShare[128];
		char		smbDomainUser[256];

	};


};

extern struct D2Xsettings::d2xSettings g_d2xSettings;

#endif