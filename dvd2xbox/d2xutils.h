#ifndef D2XUTILS
#define D2XUTILS

#include <xtl.h>
#include <undocumented.h>


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
	

};

#endif