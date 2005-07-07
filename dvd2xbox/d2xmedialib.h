#ifndef D2XMEDIALIB
#define D2XMEDIALIB

#include <xtl.h>
#include "..\..\xbox\tinyxml\tinyxml.h"
#include "stdstring.h"
#include "d2xfont.h"
#include "d2xtexture.h"


class D2Xmedialib
{
protected:

	D2Xfont*	p_Font;
	D2Xtexture*	p_tex;

	int LoadFonts(CStdString& strSkindir);
	int LoadBitmaps(CStdString& strSkindir);

public:

	D2Xmedialib();
	~D2Xmedialib();

	int LoadMedia(CStdString& strSkindir);
	void DrawText(const CStdString name,FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText);
	void SetCursorPosition(FLOAT fX, FLOAT fY );

	void RenderTexture2(CStdString name, FLOAT x, FLOAT y, FLOAT width, FLOAT height);
	float getFontHeight( const CStdString name);
};

#endif