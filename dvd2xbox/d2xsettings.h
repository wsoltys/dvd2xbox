
#ifndef D2XSETTINGS
#define D2XSETTINGS


#include <xtl.h>
#include <memory>
#include <undocumented.h>
#include <string>
#include <stdstring.h>
#include <map>
#include <vector>
//#include "..\..\xbox\stdstring.h"
#include <tinyxml\tinyxml.h>

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480


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
#define D2X_FAT			9
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
#define D2X_NO_FILEMANAGER	330
#define D2X_NO_ISORIPPER	340

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
#define D2X_CALIBRATION	   1200

// Network
#define NETWORK_DASH   0
#define NETWORK_DHCP   1
#define NETWORK_STATIC  2

// Status counter
#define STAT_OFF			0
#define	STAT_GENERAL		1
#define STAT_COPY_GAME		2
#define STAT_COPY_DVD		3
#define STAT_COPY_ISO		4
#define STAT_COPY_CDDA		5
#define STAT_COPY_FAILED	6
#define STAT_COPY_OK		7
#define STAT_COPY_RENAMED	8
#define STAT_START_XBE		9
#define	STAT_SHUTDOWN		10
#define STAT_TRAY_OPEN		11

#define STAT_MAX_COUNTER	11


typedef struct _DVD2XBOX_CFG {
	unsigned int	Version;
	
	char			ftpIP[16];
	char			ftpuser[128];
	char			ftppwd[128];

	char			skin[48];
	FLOAT			ScreenX1;
	FLOAT			ScreenY1;
	FLOAT			ScreenScaleX;
	FLOAT			ScreenScaleY;
} DVD2XBOX_CFG, *PDVD2XBOX_CFG;


class D2Xsettings
{
private:
	static std::auto_ptr<D2Xsettings> sm_inst;
	D2Xsettings();

	TiXmlDocument		xmldoc;
	TiXmlElement*		itemElement;
	std::vector<std::string>	xmlDumpDirs;

	class D2Xrgb
	{
	public:
		D2Xrgb(){red=0;green=0;blue=0;}
		unsigned int red;
		unsigned int green;
		unsigned int blue;
	};
	

	
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
	int			showKeyboard(int type);
	int			showCopyRetryDialog(int type);
	bool		OpenRGBxml(CStdString strFilename);

	struct d2xSettings
	{
	public:
		char		xboxIP[16];
		char		netmask[16];
		char		gateway[16];
		char		DNS[16];
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
		unsigned short	network_assignment;
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
		bool			enableSmartXXRGB;
		CStdString		disable_isoripper;
		CStdString		disable_filemanager;

		// remote control
		bool			remoteControlled;
		CStdString		rm_strApp;
		int				rm_iGCmode;
		int				rm_iVCmode;
		int				rm_iACmode;
		CStdString		rm_strGCdir;
		CStdString		rm_strVCdir;
		CStdString		rm_strACdir;
		int				rm_iGCkeyboard;
		int				rm_iVCkeyboard;
		int				rm_iACkeyboard;
		int				rm_iGCretry;
		int				rm_iVCretry;
		int				rm_iACretry;

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

		D2Xrgb		SmartXXRGB[12];

		FLOAT		ScreenX1;
		FLOAT		ScreenY1;
		FLOAT		ScreenScaleX;
		FLOAT		ScreenScaleY;
	};


};

extern struct D2Xsettings::d2xSettings g_d2xSettings;

void DebugOut(char *message,...);

#endif