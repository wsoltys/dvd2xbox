
#ifndef D2XSETTINGS
#define D2XSETTINGS


//#include "..\lib\libtinyxml\tinyxml.h"
#include <xtl.h>
#include <memory>
#include <undocumented.h>
#include "simplexml.h"

#define XML_BUFFER			2048

#define UNDEFINED		0
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
#define DVD2SMB			24
#define ISO2SMB			25
#define VCD2SMB			26
#define SVCD2SMB		27

#define OGGVORBIS	   100
#define MP3LAME		   110
#define WAV			   120

#define MODCHIP_SMARTXX   0
#define MODCHIP_XENIUM    1
#define	NONE			  2
#define LCD_MODE_TYPE_LCD 0   
#define LCD_MODE_TYPE_VFD 1

// General errors

#define COULD_NOT_AUTH_DVD	100
#define TYPE_NOT_SUPPORTED	110

typedef struct _DVD2XBOX_CFG {
	unsigned int	Version;
	bool			EnableF;
	bool			EnableG;
	unsigned short	WriteLogfile;
	unsigned short	EnableACL;
	unsigned short	EnableRMACL;
	unsigned short	EnableAutopatch;
	unsigned short	EnableAutoeject;
	unsigned short	EnableNetwork;
	unsigned short	cdda_encoder;
	float			OggQuality;
	unsigned short	mp3_mode;
	unsigned short	mp3_bitrate;
	unsigned short	useLCD;
	unsigned short	detect_media_change;
} DVD2XBOX_CFG, *PDVD2XBOX_CFG;


class D2Xsettings
{
private:
	static std::auto_ptr<D2Xsettings> sm_inst;
	D2Xsettings();

	char				XMLbuffer[XML_BUFFER];
	simplexml			*rootptr;
	simplexml			*ptr;

	
public:
	static D2Xsettings* Instance();
	
	// Online settings 
	void		ReadCFG(PDVD2XBOX_CFG cfg);
	void		WriteCFG(PDVD2XBOX_CFG cfg);
	void		WriteDefaultCFG(PDVD2XBOX_CFG cfg);
	// XML settings
	int			readIni(char* file);
	const char* getIniValue(const char* root,const char* key);
	const char* getIniValue(const char* root,const char* key,int iter);
	int			getIniChilds(const char* root);

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

		int			generalError;
		char		HomePath[1024];
		unsigned int current_version;
		char		ConfigPath[1024];
		char		disk_statsPath[1024];
		char		TDATApath[1024];
		unsigned int enableRMACL;
		unsigned short cdda_encoder;
		unsigned short	mp3_mode;
		unsigned short	mp3_bitrate;
		unsigned short	detect_media_change;
		unsigned short	detect_media;
		// taken from xbmc
		bool      m_bLCDUsed;
		int       m_iLCDColumns;
		int       m_iLCDRows;
		int       m_iLCDAdress[4];
		int       m_iLCDMode;
		int       m_iLCDBackLight;
		int       m_iLCDType;
		int       m_iLCDBrightness;
		int       m_iLCDModChip;
	};


};

extern struct D2Xsettings::d2xSettings g_d2xSettings;

#endif