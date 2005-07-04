#ifndef D2XGRAPHICS
#define D2XGRAPHICS

//#include <helper.h>
#include <xtl.h>
#include <XBFont.h>
#include "xbApplicationEx.h"
#include "d2xtexture.h"

struct SCREENVERTEX
{
    D3DXVECTOR4 pos;   // The transformed position for the vertex point.
    DWORD       color; // The vertex color. 
};
#define D3DFVF_SCREENVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

// Some colors
#define SEMITRANS_BLACK     0xC0000000
#define COLOUR_BLACK        0xff000000
#define COLOUR_WHITE        0xffffffff
#define COLOUR_YELLOW       0xffffff00
#define COLOUR_RED          0xffff0000
#define COLOUR_DARK_RED     0xff500000
#define COLOUR_GREY         0xff404040
#define COLOUR_LIGHT_BLUE   0xff6060ff
#define COLOUR_GREEN		0xff006600
#define COLOUR_DARK_GREEN	0xff003300
#define SEMITRANS_GREEN		0xC0003300
#define SEMITRANS_RED		0x90ff0000
#define SEMITRANS_BLACK2	0xE0000000

// bitmaps
#define BM_DUMP_TO_HDD			1
#define BM_DUMP_TO_SMB			2
#define BM_FILEMANAGER			3
#define BM_GAMEMANAGER			4
#define	BM_SETTINGS				5
#define	BM_BOOT_TO_DASH			6

class D2Xgraphics
{
protected:
	CXBFont*    m_Font;
	D2Xtexture*	p_tex;
	int			icon;


public:
	D2Xgraphics(CXBFont* m_Font2);
	D2Xgraphics();
	~D2Xgraphics();

	VOID RenderMainFrames();
	VOID RenderBrowserBig();
	VOID RenderBigFrame();
	VOID RenderPopup();
	VOID RenderHelpFrame();
	VOID RenderKeyBoardBG();
	VOID RenderProgressBar(float y,float percent);
	VOID RenderBrowserFrames(int active);
	VOID RenderBrowserBar(float x,float y,float h);
	VOID RenderBar(float x,float y,float h, float b);
	VOID RenderCustomBar(float x,float y,float h, float b, DWORD outline, DWORD fill);
	VOID RenderBrowserBarSelected(float x,float y,float h);
	VOID RenderBrowserPopup(int active);
	VOID RenderSmallPopup();
	VOID RenderGameListBackground();
	VOID RenderGUISettingsMain(int x1,int y1,int x2,int y2);
	VOID RenderGUISettingsSub(int x1,int y1,int x2,int y2);
	VOID ScreenSaver();
	VOID DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,DWORD dwStartColor, DWORD dwEndColor );
	VOID DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,DWORD dwColor );
	VOID DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwColor );
	
	int LoadTextures();
	VOID SetIcon(int ico);
	VOID RenderBackground();
	VOID RenderMainMenuIcons();

};

#endif