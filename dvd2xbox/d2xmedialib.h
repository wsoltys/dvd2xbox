#ifndef D2XMEDIALIB
#define D2XMEDIALIB

#include <xtl.h>
#include "..\..\xbox\tinyxml\tinyxml.h"
#include "stdstring.h"


class D2Xmedialib
{
protected:



public:

	D2Xmedialib();
	~D2Xmedialib();

	int LoadMedia(CStdString& strFilename);
};

#endif