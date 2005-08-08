#include "D2Xgraphics.h"


D2Xgraphics::D2Xgraphics(CXBFont* m_Font2)
{
	m_Font = m_Font2;
	p_tex = NULL;
	p_tex = D2Xtexture::Instance();
	icon = 1;
}

D2Xgraphics::D2Xgraphics()
{
	p_tex = NULL;
	p_tex = D2Xtexture::Instance();
}

D2Xgraphics::~D2Xgraphics()
{
}

VOID D2Xgraphics::RenderMainFrames()
{
	// Header
	DrawRect( 50, 20, 590, 100, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 20, 590, 100, COLOUR_DARK_GREEN );
	// Main
	DrawRect( 50, 110, 590, 420, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 110, 590, 420, COLOUR_DARK_GREEN );
	// Footer
	DrawRect( 50, 430, 590, 460, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 430, 590, 460, COLOUR_DARK_GREEN );
}
VOID D2Xgraphics::RenderBrowserBig()
{
	// Header
	DrawRect( 50, 20, 590, 80, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 20, 590, 80, COLOUR_DARK_GREEN );
	// Main
	DrawRect( 50, 90, 590, 420, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 90, 590, 420, COLOUR_DARK_GREEN );
	// Footer
	DrawRect( 50, 430, 590, 460, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 430, 590, 460, COLOUR_DARK_GREEN );
}


VOID D2Xgraphics::RenderBigFrame()
{
	DrawRect( 50, 20, 590, 460, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 20, 590, 460, COLOUR_DARK_GREEN );
}

VOID D2Xgraphics::RenderKeyBoardBG()
{
	DrawRect( 50, 190, 590, 470, COLOUR_DARK_GREEN, COLOUR_DARK_GREEN );
	//DrawRectOutline( 60, 200, 580, 450, COLOUR_DARK_GREEN );
}

VOID D2Xgraphics::RenderHelpFrame()
{
	DrawRect( 0, 0, 640, 480, COLOUR_BLACK, COLOUR_BLACK );
}

VOID D2Xgraphics::RenderPopup()
{
	DrawRect( 50, 150, 590, 285, COLOUR_BLACK,COLOUR_BLACK );
	DrawRectOutline( 50, 150, 590, 285, COLOUR_WHITE );
}

VOID D2Xgraphics::RenderProgressBar(float y,float percent)
{
	WCHAR temp[30];
	wsprintfW(temp,L"%3.0f %%",percent);
	DrawRect( 50, y, 5.9*percent, y+14, COLOUR_WHITE,COLOUR_WHITE );
	DrawRect( 295, y, 345, y+14, COLOUR_BLACK,COLOUR_BLACK );
	DrawRectOutline( 50, y, 590, y+14, COLOUR_WHITE );
	m_Font->DrawText(300, y, 0xffffffff, temp  );
}

VOID D2Xgraphics::RenderBrowserFrames(int active)
{
	// Header
	DrawRect( 50, 20, 590, 80, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 20, 590, 80, COLOUR_DARK_GREEN );
	// Main
	DrawRect( 50, 90, 315, 420, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 90, 315, 420, COLOUR_DARK_GREEN );
	if(active == 1)
		DrawRectOutline( 50, 90, 315, 420, COLOUR_RED );
	DrawRect( 325, 90, 590, 420, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 325, 90, 590, 420, COLOUR_DARK_GREEN );
	if(active == 2)
		DrawRectOutline( 325, 90, 590, 420, COLOUR_RED );
	// Footer
	DrawRect( 50, 430, 590, 460, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 430, 590, 460, COLOUR_DARK_GREEN );
}

VOID D2Xgraphics::RenderBrowserBar(float x,float y,float h)
{
	DrawRect( x-4.0, y, x+259.0, y+h, COLOUR_WHITE, COLOUR_WHITE );
	DrawRectOutline( x-4.0, y, x+259.0, y+h, COLOUR_WHITE );
}

VOID D2Xgraphics::RenderBar(float x,float y,float h, float b)
{
	DrawRect( x, y, x+b, y+h, COLOUR_BLACK, COLOUR_BLACK );
	DrawRectOutline( x, y, x+b, y+h, COLOUR_BLACK );
}

VOID D2Xgraphics::RenderCustomBar(float x,float y,float h, float b, DWORD outline, DWORD fill)
{
	DrawRect( x, y, x+b, y+h, fill, fill );
	DrawRectOutline( x, y, x+b, y+h, outline );
}

VOID D2Xgraphics::RenderBrowserBarSelected(float x,float y,float h)
{
	DrawRect( x-4.0, y, x+259.0, y+h, SEMITRANS_RED, SEMITRANS_RED );
	DrawRectOutline( x-4.0, y, x+259.0, y+h, COLOUR_RED );
}

VOID D2Xgraphics::RenderBrowserPopup(int active)
{
	if(active == 2)
	{
		DrawRect( 50, 90, 315, 420, COLOUR_BLACK, COLOUR_BLACK );
		DrawRectOutline( 50, 90, 315, 420, COLOUR_WHITE );
	}
	if(active == 1)
	{
		DrawRect( 325, 90, 590, 420, COLOUR_BLACK, COLOUR_BLACK );
		DrawRectOutline( 325, 90, 590, 420, COLOUR_WHITE );
	}
}

VOID D2Xgraphics::RenderSmallPopup()
{
	DrawRect( 200, 150, 440, 260, COLOUR_BLACK,COLOUR_BLACK );
	DrawRectOutline( 200, 150, 440, 260, COLOUR_WHITE );
}

VOID D2Xgraphics::RenderGameListBackground()
{
	// upper mainframe
	DrawRect( 40, 40, 440, 320, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( 40, 40, 440, 320, COLOUR_DARK_GREEN ); 

	// upper right frame
	DrawRect( 460, 40, 600, 320, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( 460, 40, 600, 320, COLOUR_DARK_GREEN );

	// bottom status
	DrawRect( 300, 340, 600, 440, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( 300, 340, 600, 440, COLOUR_DARK_GREEN );
}

VOID D2Xgraphics::RenderGUISettingsMain(int x1,int y1,int x2,int y2)
{
	DrawRect( 0,20,640,60, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( 0,20,640,60, COLOUR_DARK_GREEN );

	DrawRect( x1,y1,x2,y2, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( x1,y1,x2,y2, COLOUR_DARK_GREEN );
}

VOID D2Xgraphics::RenderGUISettingsSub(int x1,int y1,int x2,int y2)
{
	DrawRect( 0,20,640,60, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( 0,20,640,60, COLOUR_DARK_GREEN );

	DrawRect( x1,y1,x2,y2, SEMITRANS_GREEN,SEMITRANS_GREEN );
	DrawRectOutline( x1,y1,x2,y2, COLOUR_DARK_GREEN );
}

//-----------------------------------------------------------------------------
// Name: DrawLine()
// Desc: 
//-----------------------------------------------------------------------------
VOID D2Xgraphics::DrawLine( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwColor )
{
    SCREENVERTEX v[2];
    v[0].pos = D3DXVECTOR4( x1, y1, 0.5f, 1.0f );   v[0].color = dwColor;
    v[1].pos = D3DXVECTOR4( x2, y2, 0.5f, 1.0f );   v[1].color = dwColor;
    
    // Render the line
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    g_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX) ;
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, v, sizeof(SCREENVERTEX) );
}




//-----------------------------------------------------------------------------
// Name: DrawRectOutline()
// Desc: 
//-----------------------------------------------------------------------------
VOID D2Xgraphics::DrawRectOutline( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                                   DWORD dwColor )
{
    DrawLine( x1, y1, x1, y2, dwColor );
    DrawLine( x1, y1, x2, y1, dwColor );
    DrawLine( x2, y1, x2, y2, dwColor );
    DrawLine( x1, y2, x2, y2, dwColor );
}




//-----------------------------------------------------------------------------
// Name: DrawRect()
// Desc: 
//-----------------------------------------------------------------------------
VOID D2Xgraphics::DrawRect( FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, 
                            DWORD dwStartColor, DWORD dwEndColor )
{
    SCREENVERTEX v[4];
    v[0].pos = D3DXVECTOR4( x1-0.5f, y1-0.5f, 0.5f, 1.0f );  v[0].color = dwStartColor;
    v[1].pos = D3DXVECTOR4( x2-0.5f, y1-0.5f, 0.5f, 1.0f );  v[1].color = dwStartColor;
    v[2].pos = D3DXVECTOR4( x1-0.5f, y2-0.5f, 0.5f, 1.0f );  v[2].color = dwEndColor;
    v[3].pos = D3DXVECTOR4( x2-0.5f, y2-0.5f, 0.5f, 1.0f );  v[3].color = dwEndColor;
    
    // Render the rectangle
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    g_pd3dDevice->SetVertexShader( D3DFVF_SCREENVERTEX );
    g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(SCREENVERTEX) );
}

//int D2Xgraphics::LoadTextures()
//{
//	if(p_tex == NULL)
//		return 0;
//
//	p_tex->LoadTexture("d:\\Media\\images\\background.png",0);
//	p_tex->LoadTexture("d:\\Media\\images\\dumphdd.png",1,0xFF000000);
//	p_tex->LoadTexture("d:\\Media\\images\\dumpsmb.png",2,0xFF000000);
//	p_tex->LoadTexture("d:\\Media\\images\\fm.png",3,0xFF000000);
//	p_tex->LoadTexture("d:\\Media\\images\\gm.png",4,0xFF000000);
//	p_tex->LoadTexture("d:\\Media\\images\\settings.png",5,0xFF000000);
//	p_tex->LoadTexture("d:\\Media\\images\\shutdown.png",6,0xFF000000);
//
//	return 1;
//}

VOID D2Xgraphics::RenderBackground()
{
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.5f, 1.0f );
	p_tex->RenderTexture2("background",0,0,640,480);
}

VOID D2Xgraphics::SetIcon(int ico)
{
	icon = ico;
}

//VOID D2Xgraphics::RenderMainMenuIcons()
//{
//	p_tex->RenderTexture(400,250,128,128,icon);
//}

VOID D2Xgraphics::ScreenSaver()
{
	DrawRect( 0, 0, 640, 480, SEMITRANS_BLACK2, SEMITRANS_BLACK2 );
}