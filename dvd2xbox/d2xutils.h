#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <stdstring.h>
#include <undocumented.h>


class D2Xutils
{
protected:
	char	memfile[1024];

	
public:
	D2Xutils();
	~D2Xutils();

	ULONG		getTitleID(char* path);
	int			char2byte(char* ch, BYTE* b);
	int			findHex(char* file,char* mtext,int offset);
	int			writeHex(char* file,char* mtext,int offset);
	void		addSlash(char* source);
	bool		DelTree(char *path);
	int			SetMediatype(char* file,ULONG &mt,char* nmt);

	int			GetItemValue(char* key,char* value);
	//int			SetItemValue(char* key,char* value);
	// MXM utils
	HRESULT		MakePath( LPCTSTR szPath );
	CStdString  PathSlasher( LPCTSTR szPath, bool bSlashIt );

};

#endif