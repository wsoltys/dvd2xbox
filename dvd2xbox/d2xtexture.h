// taken from http://www.xbdev.net/tuts/subpages/prt4/index.php and XBMC

#ifndef D2XTEXTURE
#define D2XTEXTURE

#include <xtl.h>
#include "xbApplicationEx.h"
#include "stdstring.h"
#include <map>
#include <xbresource.h>
#include "d2xsettings.h"

#define TEXTURE_BUFFER	10

#define D3DFVF_CUSTOMVERTEX		(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class D2Xtexture
{
protected:

	D2Xtexture();
	static std::auto_ptr<D2Xtexture> sm_inst;

	LPDIRECT3DTEXTURE8 pTexture[TEXTURE_BUFFER];

	map<CStdString,LPDIRECT3DTEXTURE8> mapTexture;

	struct CUSTOMVERTEX
	{
		D3DXVECTOR4 p;
		D3DCOLOR col;
		FLOAT tu,tv;
	};

	struct SCREENVERTEX
	{
		D3DXVECTOR4 pos;   // The transformed position for the vertex point.
		DWORD       color; // The vertex color. 
	};


public:

	static D2Xtexture* Instance();
	
	/*int LoadTexture(const CStdString& strFilename,int iID,DWORD dwColorKey=0);
	void RenderTexture(float x, float y, float width, float height,int iID);*/

	int LoadFirstTextureFromXPR(const CStdString& strFileName,const CStdString& name,DWORD dwColorKey);
	int LoadTexturesFromXPR(const CStdString& strFileName);
	int LoadTexture2(const CStdString& strFilename,const CStdString& name,DWORD dwColorKey=0);
	int UnloadTexture(const CStdString& strIconName);
	void RenderTexture2(const CStdString& name, float x, float y, float width, float height, bool scaled=true);
	bool IsTextureLoaded(const CStdString& strName);
};

#endif