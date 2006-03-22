#ifndef D2XMEDIALIB
#define D2XMEDIALIB

#include <xtl.h>
#include "..\..\xbox\tinyxml\tinyxml.h"
#include "stdstring.h"
#include "d2xfont.h"
#include "d2xtexture.h"
#include "d2xguisounds.h"
#include <xbeheader.h>


class D2Xmedialib
{
protected:

	D2Xfont*		p_Font;
	D2Xtexture*		p_tex;
	D2Xguisounds*	p_wav;

	map<CStdString,bool> mapTextureStatus;
	map<CStdString,CStdString> mapSoundKeyMap;

	int LoadFonts(CStdString& strSkindir);
	int LoadBitmaps(CStdString& strSkindir);

	D2Xmedialib();
	static std::auto_ptr<D2Xmedialib> sm_inst;
	

public:

	static D2Xmedialib* Instance();

	int LoadMedia(CStdString& strSkindir);
	void DrawText(const CStdString name,FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags=0L, DWORD dwFlags2=0L,FLOAT fMaxPixelWidth = 0.0f, bool scroll=false );
	void SetCursorPosition(FLOAT fX, FLOAT fY );

	int LoadXBEIcon(CStdString strXBEPath, CStdString strIconName);
	int LoadTextureFromTitleID(const CStdString& titleid, const CStdString& alt_FileName, const CStdString& name,DWORD dwColorKey);
	int UnloadTexture(CStdString strIconName);
	void UnloadGameIcons();
	void RenderTexture2(CStdString name, FLOAT x, FLOAT y, FLOAT width, FLOAT height);
	float getFontHeight( const CStdString& name);
	float getFontWidth( const CStdString& name,const CStdStringW& text);
	CXBFont* getFontObj( const CStdString& name);
	LPDIRECT3DTEXTURE8 GetTexture(const CStdString& name, DWORD dwOffset);
	bool IsTextureLoaded(const CStdString strName);
	void SetTextureStatus(const CStdString& strTextureName,bool status);
	bool getTextureStatus(const CStdString& strTextureName);
	void clearScrollCache();

	int LoadSounds(CStdString& strSkindir);
	void DoSoundWork();

	int LoadSoundMap(CStdString& strSkindir);
	void PlayKeySound(CStdString strAction);
	void PlaySound(CStdString strName);
};

#endif