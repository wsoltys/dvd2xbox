// taken from http://www.xbdev.net/tuts/subpages/prt4/index.php and XBMC

#ifndef D2XTEXTURE
#define D2XTEXTURE

#include <xtl.h>
#include "xbApplicationEx.h"
#include "stdstring.h"

#define TEXTURE_BUFFER	10

#define D3DFVF_CUSTOMVERTEX		(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class D2Xtexture
{
protected:

	D2Xtexture();
	static std::auto_ptr<D2Xtexture> sm_inst;

	LPDIRECT3DTEXTURE8 pTexture[TEXTURE_BUFFER];
	LPDIRECT3DTEXTURE8 pTexture2;
	//LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer;

	struct CUSTOMVERTEX
	{
		D3DXVECTOR4 p;
		D3DCOLOR col;
		FLOAT tu,tv;
	};


public:

	static D2Xtexture* Instance();
	
	int LoadTexture(const CStdString& strFilename,int iID,DWORD dwColorKey=0);
	void RenderTexture(float x, float y, float width, float height,int iID);

};

#endif