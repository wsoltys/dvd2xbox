#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <stdstring.h>
#include <undocumented.h>



class D2Xutils
{
protected:

	
public:
	D2Xutils();
	~D2Xutils();

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
	int			SetMediatype(const char* file,ULONG &mt,char* nmt);
	int			SetGameRegion(const char* file,ULONG &mt,char* nmt);
	void		getHomePath(char* path);
	void		getFatxName(char* pattern);
	void		getFatxName(WCHAR* pattern);
	bool		isdriveD(char* path);
	bool		getfreeDiskspaceMB(char* drive,char* size);
	bool		getfreeDiskspaceMB(char* drive,int& size);
	void		LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters=NULL);

	_XBE_CERTIFICATE	xbecert;
	//_XBE_HEADER			xbeheader;

	// MXM utils
	HRESULT		MakePath( LPCTSTR szPath );
	CStdString  PathSlasher( LPCTSTR szPath, bool bSlashIt );

	// XBMC
	void		Unicode2Ansi(const wstring& wstrText,CStdString& strName);

};

#endif