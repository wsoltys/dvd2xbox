#ifndef D2XPATCHER
#define D2XPATCHER

#include <helper.h>
#include <iosupport.h>
#include <undocumented.h>
#include "d2xdbrowser.h"
#include "d2xlogger.h"

#define ERROR		0
#define NOT_FOUND	1
#define FOUND_OK	2
#define FOUND_ERROR	3

class D2Xpatcher
{
protected:
	int char2byte(char* ch, BYTE* b);
	int findHex(const char* file,char* mtext,int offset);
	int writeHex(const char* file,char* mtext,int offset);
	//HelperX*			p_help;
	CIoSupport			p_IO;
	char*				pFiles[1024];
	int					pfilescount;

	char				comment[20][100];
	char				search[20][100];
	char				replace[20][100];
	int					patches;
	int					readPatchesfromFile(char* file);

	D2Xlogger*			p_log;

public:
	D2Xpatcher();
	~D2Xpatcher();
	static char*			p_hexsearch[100];
	static char*			p_hexreplace[100];
	static int				mXBECount;
	static char				mXBEs[50][512];
	static int				mFATXren;
	//static char				mFATXs[50][50];
	static int				mcheck[50];
	int SetMediatype(const char* file,ULONG &mt,char* nmt);
	//int PatchMediaStd(char* file,WCHAR* message[]);
	int PatchMediaStd(const char* file,int patch);
	//static void addXBE(char* file);
	//static void addFATX(char* file);
	static void reset();
	char** getPatchFiles();
	void patchXBEfromFile(HDDBROWSEINFO source,char* patchfile,WCHAR** message);

};

#endif