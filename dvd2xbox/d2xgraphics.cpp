#include "D2Xgraphics.h"


D2Xgraphics::D2Xgraphics(CXBFont* m_Font2)
{
	m_Font = m_Font2;
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

VOID D2Xgraphics::RenderBigFrame()
{
	DrawRect( 50, 35, 590, 445, SEMITRANS_GREEN, SEMITRANS_GREEN );
	DrawRectOutline( 50, 35, 590, 445, COLOUR_DARK_GREEN );
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
	DrawRect( 50, y, 590*(percent/100), y+14, COLOUR_WHITE,COLOUR_WHITE );
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