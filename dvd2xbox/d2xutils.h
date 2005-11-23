#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <stdstring.h>
#include <undocumented.h>
#include <iosupport.h>
#include <map>
#include "d2xfilefactory.h"
#include "d2xsettings.h"
#include <XBFont.h>
#include <xkhdd.h>
#include <xbApplicationEx.h>

#define FATX_LENGTH		42


class D2Xutils
{
protected:

	CStdString strhdd;
	CStdString strdvd;

	
public:
	D2Xutils();
	~D2Xutils();

	map<char,string> driveMappingEx;

	ULONG		getTitleID(char* path);
	int			getXBECert(char* filename);
	bool		writeTitleName(char* path,const WCHAR* title);
	int			char2byte(char* ch, BYTE* b);
	void		str2byte(string item,BYTE* buf);
	void		str2hex(string item,char* buf);
	int			findHex(const char* file,char* mtext,int offset);
	int			writeHex(const char* file,char* mtext,int offset);
	void		addSlash(char* source);
	void		addSlash2(char* source);
	bool		DelTree(char *path);
	void		DelPersistentData(ULONG titleID);
	int			SetMediatype(const char* file,ULONG &mt,char* nmt);
	int			SetGameRegion(const char* file,ULONG &mt,char* nmt);
	void		getHomePath(char* path);
	//__inline void	getFatxName(char* pattern);
	void		getFatxNameW(WCHAR* pattern);
	bool		isdriveD(char* path);
	bool		getfreeDiskspaceMB(char* drive,char* size);
	bool		getfreeDiskspaceMB(char* drive,int& size);
	int			getfreeDiskspaceMB(char* drive);
	void		LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters=NULL);
	int			IsDrivePresent( char* cDrive );
	void		GetHDDModel(CStdString& strModel);
	void		GetDVDModel(CStdString& strModel);
	bool		IsEthernetConnected();
	void		RemapHomeDir(char* path);
	const char*	getMapValue(map<CStdString,CStdString>& map,CStdString key);

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

};



// XBMC
void fast_memcpy(void* d, const void* s, unsigned n);
void fast_memset(void* d, int c, unsigned n);
void usleep(int t);

// dvd2xbox
__inline void getFatxName(char* pattern)
{
	char f_name[256];
	int c = 0;

	for(unsigned int i=0;i<strlen(pattern);i++)
	{
		if(isalnum(pattern[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern[i]))
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