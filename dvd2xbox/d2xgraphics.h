#ifndef D2XGRAPHICS
#define D2XGRAPHICS

#include <helper.h>

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

class D2Xgraphics
{
protected:
	CXBFont*     m_Font;


public:
	D2Xgraphics(CXBFont* m_Font2);
	~D2Xgraphics();

	VOID RenderMainFrames();
	VOID RenderBigFrame();
	VOID RenderPopup();
	VOID RenderHelpFrame();
	VOID RenderProgressBar(float y,float percent);
	VOID RenderBrowserFrames(int active);
	VOID DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,DWORD dwStartColor, DWORD dwEndColor );
	VOID DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2,DWORD dwColor );
	VOID DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, DWORD dwColor );
	

};

#endif