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
	bool		writeTitleName(char* path,const WCHAR* title);
	int			char2byte(char* ch, BYTE* b);
	int			findHex(const char* file,char* mtext,int offset);
	int			writeHex(const char* file,char* mtext,int offset);
	void		addSlash(char* source);
	bool		DelTree(char *path);
	int			SetMediatype(const char* file,ULONG &mt,char* nmt);
	void		getHomePath(char* path);

	// MXM utils
	HRESULT		MakePath( LPCTSTR szPath );
	CStdString  PathSlasher( LPCTSTR szPath, bool bSlashIt );

};

#endif