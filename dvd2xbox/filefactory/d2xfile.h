#ifndef D2XFILE
#define D2XFILE

#include <xtl.h>
#include <vector>
#include <string>
//#include "../d2xlogger.h"
#include "../d2xsettings.h"

using namespace std;


class ITEMS
{
public:
	ITEMS() {};
	~ITEMS() {};
	int	isDirectory;
	string name;
	string fullpath;
};

typedef vector<ITEMS> VECFILEITEMS;

class D2Xfile
{

protected:

	//D2Xlogger	p_log;

public:
	D2Xfile();
	virtual ~D2Xfile();
	virtual int FileOpenWrite(char* filename)=0;
	virtual int FileOpenRead(char* filename)=0;
	virtual int FileWrite(LPCVOID buffer,DWORD dwrite,DWORD *dwrote)=0;
	virtual int FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)=0;
	virtual int FileClose()=0;
	virtual DWORD GetFileSize()=0;
	virtual int GetDirectory(char* path,VECFILEITEMS &items)=0;
	// dummy function for DVD and UDF
	virtual int DUMMYFileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)=0;
};

#endif