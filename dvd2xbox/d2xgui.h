#ifndef D2XGUI
#define D2XGUI

#include <xtl.h>
#include "stdstring.h"
#include "d2xmedialib.h"


class D2Xgui
{
protected:

	D2Xmedialib*	p_ml;


public:
	D2Xgui();
	~D2Xgui();

	int LoadSkin(CStdString strSkinName);


};

#endif