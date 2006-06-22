#ifndef D2XFONT
#define D2XFONT

#include <xtl.h>
#include <xbfontEx.h>
#include <XBResource.h>
#include <vector>
#include <map>
#include "stdstring.h"
#include "d2xsettings.h"

#define D2XFONT_RIGHT		0x00000001
#define D2XFONT_LEFT		0x00000002
#define D2XFONT_CENTER		0x00000004
#define D2XFONT_VERTICAL	0x00000008

#define D2XFONT_SCROLL_TIME		10	// time in ms between move
#define D2XFONT_CACHE_TIME		100 // time in ms after last access cache will be deleted

struct _SCROLL
{
	CStdStringW		strScrolled;
	FLOAT			pixel_x;
	DWORD			last_access;
};


class D2Xfont
{
protected:

	D2Xfont();
	static std::auto_ptr<D2Xfont>	sm_inst;
	FLOAT							fCursorX,fCursorY;

	map<CStdString,CXBFont*>			mapFont;
	map<CStdString,CXBPackedResource*>	mapRes;
	map<CStdStringW,_SCROLL>			mapScroll;

	void getScrollText(const CStdString& name,const CStdStringW& strText, CStdStringW& strText2, FLOAT fMaxPixelWidth, FLOAT *pixel_x);


public:

	static D2Xfont* Instance();
	int LoadFont(const CStdString& strFilename,const CStdString& name);
	void DrawText(const CStdString& name, DWORD dwColor, const CStdStringW& strText);
	void DrawText( const CStdString& name, FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags=0L, DWORD dwFlags2=0L,FLOAT fMaxPixelWidth = 0.0f, bool scroll=false );
	void SetCursorPosition(FLOAT fX, FLOAT fY );
	float getFontHeight( const CStdString& name);
	float getFontWidth( const CStdString& name,const CStdStringW& text);
	CXBFont* getFontObj( const CStdString& name);

	int LoadResource(const CStdString& strFilename,const CStdString& name);
	LPDIRECT3DTEXTURE8 GetTexture(const CStdString& name, DWORD dwOffset);
	void clearScrollCache();

};

#endif