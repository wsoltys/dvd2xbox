#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <stdstring.h>
#include <undocumented.h>
#include <iosupport.h>
#include <map>
#include "d2xfilefactory.h"
#include "d2xsettings.h"
#include <xbfontEx.h>
#include <xkhdd.h>
#include <xbApplicationEx.h>
#include "d2xSmartXXRGB.h"

#define FATX_LENGTH		42


#define P_RED    0xf70c   //SmartXX V3 port for PWM red output
#define P_GREEN  0xf70d   //SmartXX V3 port for PWM green output
#define P_BLUE   0xf70e   //SmartXX V3 port for PWM blue output
#define P_STATUS 0xf702   //Status LED port

// for 'cherry' patching
typedef enum
{
  COUNTRY_NULL = 0,
  COUNTRY_USA,
  COUNTRY_JAP,
  COUNTRY_EUR
} F_COUNTRY;

typedef enum
{
  VIDEO_NULL = 0,
  VIDEO_NTSCM,
  VIDEO_NTSCJ,
  VIDEO_PAL50,
  VIDEO_PAL60
} F_VIDEO;

static BYTE OriginalData[57]=
{
  0x55,0x8B,0xEC,0x81,0xEC,0x04,0x01,0x00,0x00,0x8B,0x45,0x08,0x3D,0x04,0x01,0x00,
  0x00,0x53,0x75,0x32,0x8B,0x4D,0x18,0x85,0xC9,0x6A,0x04,0x58,0x74,0x02,0x89,0x01,
  0x39,0x45,0x14,0x73,0x0A,0xB8,0x23,0x00,0x00,0xC0,0xE9,0x59,0x01,0x00,0x00,0x8B,
  0x4D,0x0C,0x89,0x01,0x8B,0x45,0x10,0x8B,0x0D
};

static BYTE PatchData[70]=
{
  0x55,0x8B,0xEC,0xB9,0x04,0x01,0x00,0x00,0x2B,0xE1,0x8B,0x45,0x08,0x53,0x3B,0xC1,
  0x74,0x0C,0x49,0x3B,0xC1,0x75,0x2F,0xB8,0x00,0x03,0x80,0x00,0xEB,0x05,0xB8,0x04,
  0x00,0x00,0x00,0x50,0x8B,0x4D,0x18,0x6A,0x04,0x58,0x85,0xC9,0x74,0x02,0x89,0x01,
  0x8B,0x4D,0x0C,0x89,0x01,0x59,0x8B,0x45,0x10,0x89,0x08,0x33,0xC0,0x5B,0xC9,0xC2,
  0x14,0x00,0x00,0x00,0x00,0x00
};

extern "C"	int __stdcall MmQueryAddressProtect(void *Adr);
extern "C"	void __stdcall MmSetAddressProtect(void *Adr, int Size, int Type);

// iso ripper
typedef struct _DISK_GEOMETRY {
    LARGE_INTEGER Cylinders;
    DWORD MediaType;
    DWORD TracksPerCylinder;
    DWORD SectorsPerTrack;
    DWORD BytesPerSector;
} DISK_GEOMETRY, *PDISK_GEOMETRY;

#define IOCTL_CDROM_GET_DRIVE_GEOMETRY	0x2404c
#define DEFAULT_ISO_SLICE_SIZE		0xffe00000
#define EXTRA_SPACE_REQ			(2*1024*1024)
#define STATUS_DATA_ERROR		0xc000003e

static unsigned long copy_level_size[] = { 4*1024*1024, 131072, 2048 };

#define MAX_COPY_LEVEL	(sizeof(copy_level_size) / sizeof(unsigned long) - 1)

// iso ripper end


class D2Xutils
{
protected:

	CStdString strhdd;
	CStdString strdvd;
	DWORD			DiskFreeLastAccess;
	ULARGE_INTEGER	ulFreeAvail;

	
public:
	D2Xutils();
	~D2Xutils();

	map<char,string> driveMappingEx;

	static void	mapDrives(map<int,string>& drives);
	ULONG		getTitleID(char* path);
	int			getXBECert(char* filename);
	bool		writeTitleName(char* path,const WCHAR* title);
	int			char2byte(char* ch, BYTE* b);
	void		str2byte(string item,BYTE* buf);
	void		str2hex(string item,char* buf);
	int			findHex(const char* file,char* mtext,int offset);
	int			writeHex(const char* file,char* mtext,int offset);
	static void	addSlash(CStdString& strText);
	static void	addSlash2(CStdString& strText);
	static void	addSlash(char* source);
	void		addSlash2(char* source);
	bool		DelTree(char *path);
	void		DelPersistentData(ULONG titleID);
	int			SetMediatype(const char* file,ULONG &mt,char* nmt);
	int			SetGameRegion(const char* file,ULONG &mt,char* nmt);
	void		getHomePath(char* path);
	//__inline void	getFatxName(char* pattern);
	void		getFatxNameW(WCHAR* pattern);
	static bool	isdriveD(char* path);
	bool		getfreeDiskspaceMB(char* drive,char* size);
	bool		getfreeDiskspaceMB(char* drive,int& size);
	int			getfreeDiskspaceMB(char* drive);
	//void		LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters=NULL);
	int			IsDrivePresent( char* cDrive );
	void		GetHDDModel(CStdString& strModel);
	void		GetDVDModel(CStdString& strModel);
	bool		IsEthernetConnected();
	void		RemapHomeDir(char* path);
	static const char*	getMapValue(map<CStdString,CStdString>& map,CStdString key);
	static bool	cmpExtension(CStdString strFilename, CStdString strExt);
	static int	IsVideoExt(char* cFilename);
	static void	Reboot();
	static bool isTextExtension(CStdString strFilename);
	static LONGLONG QueryVolumeInformation(HANDLE h=NULL);
	static bool IsSmbPath(char* cDestPath);
	/*static void SetSmartXXRGB(unsigned int status);
	static void SetLastRGB();*/
	static int	getFilesize(char* filename);

	_XBE_CERTIFICATE	xbecert;
	//_XBE_HEADER			xbeheader;

	// MXM utils
	HRESULT		MakePath( LPCTSTR szPath );
	CStdString  PathSlasher( LPCTSTR szPath, bool bSlashIt );

	// XBMC
	static void		SetBrightnessContrastGamma(float Brightness, float Contrast, float Gamma, bool bImmediate);
	static CStdString	GetNextFilename(const char* fn_template, int max);
	static void		FlashScreen(bool bImmediate, bool bOn);
	static void		TakeScreenshot(const char* fn, bool flashScreen);
	static void		TakeScreenshot();
	static void		Unicode2Ansi(const wstring& wstrText,CStdString& strName);
	static CStdString SmartXXModCHIP();
	static void		GetHomePath(CStdString& strPath);
	static void		RunXBE(const char* szPath, char* szParameters = NULL, F_VIDEO ForceVideo=VIDEO_NULL, F_COUNTRY ForceCountry=COUNTRY_NULL);
	static bool		PatchCountryVideo(F_COUNTRY Country, F_VIDEO Video);
	static void		LaunchXbe(const char* szPath, const char* szXbe, const char* szParameters=NULL, F_VIDEO ForceVideo=VIDEO_NULL, F_COUNTRY ForceCountry=COUNTRY_NULL); 

};



// XBMC
void fast_memcpy(void* d, const void* s, unsigned n);
void fast_memset(void* d, int c, unsigned n);
void usleep(int t);

// dvd2xbox
__inline void getFatxName(char* pattern)
{
	char f_name[256]={0};
	int c = 0;

	for(unsigned int i=0;i<strlen(pattern);i++)
	{
		if(isalnum(pattern[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern[i]) )
		{
			f_name[c] = pattern[i];
			++c;
		}
	}
	f_name[c] = '\0';

	fast_memset(pattern,0,strlen(pattern)+1);

	if(strlen(f_name) > FATX_LENGTH)
	{
		char* c;
		c = strrchr(f_name,'.');
		if(c != 0)
		{
			strncpy(pattern,f_name,FATX_LENGTH-strlen(c));
			strcat(pattern,c);
		} else {
			strncpy(pattern,f_name,FATX_LENGTH);
			pattern[FATX_LENGTH] = '\0';
		}
	} else {
		strcpy(pattern,f_name);
	}
	return;
}

__inline void getFatxNameStr(CStdString& pattern)
{
	CStdString f_name;
	char cfile[128];

	for(unsigned int i=0;i<pattern.size();i++)
	{
		if(isalnum(pattern.c_str()[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern.c_str()[i]))
		{
			f_name.push_back(pattern[i]);
		}
	}

	fast_memset(cfile,0,128);
	if(f_name.size() > FATX_LENGTH)
	{
		char* c;
		c = strrchr(f_name.c_str(),'.');
		if(c != 0)
		{
			strncpy(cfile,f_name.c_str(),FATX_LENGTH-strlen(c));
			strcat(cfile,c);
		} else {
			strncpy(cfile,f_name.c_str(),FATX_LENGTH);
			pattern[FATX_LENGTH] = '\0';
		}
		pattern = CStdString(cfile);
	} else {
		pattern = f_name;
	}
	return;
}

#endif