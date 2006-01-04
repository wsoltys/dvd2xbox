#ifndef D2XFONT
#define D2XFONT

#include <xtl.h>
#include <XBFont.h>
#include <XBResource.h>
#include <vector>
#include <map>
#include "stdstring.h"

#define D2XFONT_RIGHT	0x00000001
#define D2XFONT_LEFT	0x00000002
#define D2XFONT_CENTER  0x00000004


class D2Xfont
{
protected:

	D2Xfont();
	static std::auto_ptr<D2Xfont>	sm_inst;
	FLOAT							fCursorX,fCursorY;

	map<CStdString,CXBFont*>			mapFont;
	map<CStdString,CXBPackedResource*>	mapRes;


public:

	static D2Xfont* Instance();
	int LoadFont(const CStdString& strFilename,const CStdString& name);
	void DrawText(const CStdString& name, DWORD dwColor, const CStdStringW& strText);
	void DrawText( const CStdString& name, FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags=0L, DWORD dwFlags2=0L,FLOAT fMaxPixelWidth = 0.0f );
	void SetCursorPosition(FLOAT fX, FLOAT fY );
	float getFontHeight( const CStdString& name);
	float getFontWidth( const CStdString& name,const CStdStringW& text);
	CXBFont* getFontObj( const CStdString& name);

	int LoadResource(const CStdString& strFilename,const CStdString& name);
	LPDIRECT3DTEXTURE8 GetTexture(const CStdString& name, DWORD dwOffset);

};

#endif