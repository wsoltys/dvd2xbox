#ifndef D2XPATCHER
#define D2XPATCHER

#include <helper.h>
#include <undocumented.h>

#define ERROR		0
#define NOT_FOUND	1
#define FOUND_OK	2
#define FOUND_ERROR	3

class D2Xpatcher
{
protected:
	int char2byte(char* ch, BYTE* b);
	int findHex(char* file,char* mtext,int offset);
	int writeHex(char* file,char* mtext,int offset);

public:
	D2Xpatcher();
	~D2Xpatcher();
	static char*			p_hexsearch[100];
	static char*			p_hexreplace[100];
	static int				mXBECount;
	static char				mXBEs[50][512];
	static int				mFATXren;
	static char				mFATXs[50][50];
	static int				mcheck[50];
	int SetMediatype(char* file,ULONG &mt,char* nmt);
	//int PatchMediaStd(char* file,WCHAR* message[]);
	int PatchMediaStd(char* file,int patch);
	static void addXBE(char* file);
	static void addFATX(char* file);
	static void reset();

};

#endif