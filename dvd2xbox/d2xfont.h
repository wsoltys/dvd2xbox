#ifndef D2XFONT
#define D2XFONT

#include <xtl.h>
#include <XBFont.h>
#include <vector>
#include <map>
#include "stdstring.h"


class D2Xfont
{
protected:

	D2Xfont();
	static std::auto_ptr<D2Xfont>	sm_inst;
	std::vector<CXBFont>			v_font;
	FLOAT							fCursorX,fCursorY;

	map<CStdString,CXBFont>			mapFont;


public:

	static D2Xfont* Instance();
	int LoadFont(const CStdString& strFilename,const CStdString& name);
	void DrawText(const CStdString& name, DWORD dwColor, const CStdStringW& strText);
	void SetCursorPosition(FLOAT fX, FLOAT fY );

};

#endif