
#ifndef D2XSETTINGS
#define D2XSETTINGS


#include <xtl.h>
#include <memory>
#include "..\..\xbox\undocumented.h"
#include <string>
#include <stdstring.h>
#include <map>
#include <vector>
//#include "..\..\xbox\stdstring.h"
#include "..\..\xbox\tinyxml\tinyxml.h"



#define XML_BUFFER			2048

#define UNDEFINED		0
#define UNKNOWN_		1
#define DVD				2
#define GAME			3
#define ISO				4
#define CDDA			5
#define VCD				6
#define SVCD			7
#define RAR				8
#define UDF				20
#define D2X_SMB			21
#define FTP				22
#define UDF2SMB			23
#define DVD2SMB			24
#define ISO2SMB			25
#define VCD2SMB			26
#define SVCD2SMB		27
#define SMB2UDF			28
#define X2SMB			29
#define UDF2FTP			30
#define FTP2UDF			31
#define COPYFAILED		32
#define	X2FTP			33
#define DVD2IMAGE		34
#define DVD2ISORIPPER	35

#define MP3LAME		   100
#define OGGVORBIS	   110
#define WAV			   120

#define	LCD_NONE		  0
#define MODCHIP_SMARTXX   1
#define MODCHIP_XENIUM    2
#define MODCHIP_XECUTER3  3

#define LCD_TYPE_LCD_KS0073  0
#define LCD_TYPE_LCD_HD44780 1
#define LCD_TYPE_VFD         2

// General errors
#define COULD_NOT_AUTH_DVD		100
#define SMBTYPE_NOT_SUPPORTED	110
#define FTP_COULD_NOT_CONNECT	120
#define FTP_COULD_NOT_LOGIN		130
#define FTPTYPE_NOT_SUPPORTED	140
#define NO_DVD2XBOX_XMLFILE		150


// General notice
#define FTP_CONNECT			200
#define DELETING			210
#define SCANNING			220
#define	REBOOTING			230

// General dialogs
#define	D2X_DRIVE_NOT_READY	300
#define D2X_DRIVE_NO_DISC	310
#define D2X_NO_NETWORK		320

// blank video types
#define	D2X_BIK				400
#define	D2X_SFD				410
#define	D2X_WMV				420
#define	D2X_XMV				430

// Context counter
#define D2X_MAINMENU		  0
#define D2X_GAMEMANAGER		750
#define D2X_SETTINGS	   1100
#define D2X_FILEMANAGER		 20
#define	D2X_SMBCOPY			500
#define D2X_DISCCOPY		  1
#define D2X_DISCCOPY_RM		 13


typedef struct _DVD2XBOX_CFG {
	unsigned int	Version;
	
	char			ftpIP[16];
	char			ftpuser[128];
	char			ftppwd[128];

	char			skin[48];
} DVD2XBOX_CFG, *PDVD2XBOX_CFG;


class D2Xsettings
{
private:
	static std::auto_ptr<D2Xsettings> sm_inst;
	D2Xsettings();

	TiXmlDocument		xmldoc;
	TiXmlElement*		itemElement;
	std::vector<std::string>	xmlDumpDirs;
	

	
public:
	static D2Xsettings* Instance();
	
	// Online settings 
	void		ReadCFG(PDVD2XBOX_CFG cfg);
	void		WriteCFG(PDVD2XBOX_CFG cfg);
	void		WriteDefaultCFG(PDVD2XBOX_CFG cfg);
	// XML settings
	int			readXML(char* file);
	void		getXMLValue(const char* root, const char* key, CStdString& xml_value, const std::string default_value);
	void		getXMLValue(const char* root, const char* key, char* xml_value, const std::string default_value);
	void		getXMLValueUS(const char* root, const char* key, unsigned short& xml_value, int default_value);
	void		getDumpDirs(std::map<int,std::string> &ddirs);

	// remote control xml
	bool		OpenRCxml(CStdString strFilename);

	struct d2xSettings
	{
	public:
		char		xboxIP[16];
		char		netmask[16];
		char		gateway[16];
		char		cddbIP[16];

		char		smbUrl[256];
		char		smbWorkgroup[128];
		char		winsserver[16];


		int			generalError;
		int			generalNotice;
		int			generalDialog;
		char		HomePath[1024];
		unsigned int current_version;
		char		ConfigPath[128];
		char		disk_statsPath[128];
		char		disk_statsPath_new[128];
		char		TDATApath[1024];
		char		trackformat[128];
		unsigned int enableRMACL;
		unsigned short cdda_encoder;
		float	ogg_quality;
		unsigned short	mp3_mode;
		unsigned short	mp3_bitrate;
		unsigned short	detect_media_change;
		unsigned short	detect_media;
		unsigned short  detected_media;
		unsigned short	autodetectHDD;
		unsigned short	useF;
		unsigned short	useG;
		unsigned short	network_enabled;
		unsigned short	autodetect_enabled;
		unsigned short	autodetect_send_pwd;
		unsigned short	ftpd_enabled;
		unsigned short	ScreenSaver;
		unsigned short	WriteLogfile;
		unsigned short	enableACL;
		unsigned short	enableAutoeject;
		unsigned short	enableLEDcontrol;
		unsigned short	enableLCDScrolling;
		unsigned short	autoCopyRetries;
		unsigned short	autoReadRetries;
		unsigned short	replaceVideo;
		unsigned short	enableUnlocker;

		// remote control
		unsigned short	remoteControlled;
		CStdString		rm_strApp;
		int				rm_iGCmode;
		int				rm_iVCmode;
		CStdString		rm_strGCdir;
		CStdString		rm_strVCdir;

		// taken from xbmc
		bool      m_bLCDUsed;
		int       m_iLCDColumns;
		int       m_iLCDRows;
		int       m_iLCDAdress[4];
		int       m_iLCDBackLight;
		int       m_iLCDType;
		int       m_iLCDBrightness;
		int       m_iLCDModChip;
		int		  m_iContrast;

		// ftp
		char		ftpIP[17];
		char		ftpuser[129];
		char		ftppwd[129];

		// ftp server
		char		ftpduser[129];
		char		ftpd_pwd[129];
		unsigned short	ftpd_max_user;

		std::vector<std::string>	xmlGameDirs;
		std::map<CStdString,CStdString>	xmlSmbUrls;
		std::map<CStdString,CStdString>	autoFTPstr;

		CStdString	strskin;
		CStdString	strFTPNick;
		char		strAutoDetectNick[129];
		WCHAR	localIP[32];
		CStdString	strTextExt;
	};


};

extern struct D2Xsettings::d2xSettings g_d2xSettings;

void DebugOut(char *message,...);

#endif