#ifndef D2XFILE
#define D2XFILE

#include <xtl.h>
#include <vector>
#include <string>
//#include "../d2xlogger.h"
#include "../d2xsettings.h"
//#include "d2xfileitem.h"
#include <StdString.h>

#define OPEN_MODE_SEQ		10
#define OPEN_MODE_NORMAL	20

using namespace std;


class ITEMS
{
public:
	ITEMS() {};
	~ITEMS() {};
	bool		isDirectory;
	CStdString	name;
	CStdString	fullpath;
	CStdString	strLabel;
};

typedef vector<ITEMS> VECFILEITEMS;

class D2Xfile
{

protected:

	//D2Xlogger	p_log;

public:
	D2Xfile();
	virtual ~D2Xfile();
	virtual int FileOpenWrite(char* filename, int mode=OPEN_MODE_NORMAL, DWORD size=NULL)=0;
	//virtual int FileOpenWrite(const D2XFileItem& item, int mode=OPEN_MODE_NORMAL, DWORD size=NULL) {return 0;};
	virtual int FileOpenRead(char* filename, int mode=OPEN_MODE_NORMAL)=0;
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote)=0;
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)=0;
	virtual int FileClose()=0;
	virtual DWORD GetFileSize(char* filename=NULL)=0;
	virtual int GetDirectory(char* path,VECFILEITEMS *items)=0;
	virtual int CreateDirectory(char* name)=0;
	// dummy function for DVD and UDF
	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)=0;

	virtual __int64 FileSeek(long offset, int origin)=0;
	virtual int DeleteFile(char* filename)=0;
	virtual int DeleteDirectory(char* filename)=0;
	virtual int MoveItem(char* source, char* dest)=0;
	virtual int GetType()=0;
};

#endif