#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <stdstring.h>
#include <undocumented.h>

#define CFG_FILE			"e:\\TDATA\\0FACFAC0\\metai.d2x"
#define CURRENT_VERSION		54

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

class D2Xutils
{
protected:

	
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

	void		ReadCFG(PDVD2XBOX_CFG cfg);
	void		WriteCFG(PDVD2XBOX_CFG cfg);
	void		WriteDefaultCFG(PDVD2XBOX_CFG cfg);
	// MXM utils
	HRESULT		MakePath( LPCTSTR szPath );
	CStdString  PathSlasher( LPCTSTR szPath, bool bSlashIt );

};

#endif