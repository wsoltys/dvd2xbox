//-----------------------------------------------------------------------------
// File: Debug.h
//
// Desc: Simple debug message output function
//
// Hist: 17/6/02 Creation
//
//-----------------------------------------------------------------------------

#ifndef XBOXDEBUG_H
#define XBOXDEBUG_H

#include <stdio.h>
#include <stdarg.h>
#include <helper.h>

#define DEBUG_BUFFERS	20

#define VARARG_BUFFSIZE	1024

class CXBoxDebug 
{
public:
	CXBoxDebug(DWORD fgc,DWORD bgc,float xpos, float ypos)
	{
		mX=xpos;
		mY=ypos;
		dwTopColorI=fgc;
		dwBottomColorI=bgc;
		mMsgNo=0;		
		m_Font.Create("debugfont.xpr");
	}

	~CXBoxDebug()
	{
		for(int loop=0;loop<DEBUG_BUFFERS;loop++)
		{
			delete mMsgs[loop];
			mMsgs[loop]=NULL;
		}
		mMsgNo=0;
		m_Font.Destroy();
	}

	

	void delhistory()
	{
		for(int loop=0;loop<DEBUG_BUFFERS;loop++)
		{
			if(mMsgs[loop])
			{
				delete mMsgs[loop];
        		mMsgs[loop]=NULL;
			} else {
				break;
			}
		}
		mMsgNo=0;
	}

	void Message(WCHAR *message,...)
	{
		WCHAR expanded_message[VARARG_BUFFSIZE];
		va_list tGlop;

		// Expand the varable argument list into the text buffer
		va_start(tGlop,message);
		if(vswprintf(expanded_message,message,tGlop)==-1)
		{
			// Fatal overflow, lets abort
			return;
		}
		va_end(tGlop);

		// First see if we have space otherwise a shift is in order
		if(mMsgNo>=DEBUG_BUFFERS)
		{
			// Delete 1st message as it will be lost
			delete mMsgs[0];

			// Shift all messages up one
			for(int loop=0;loop<DEBUG_BUFFERS-1;loop++)
			{
				mMsgs[loop]=mMsgs[loop+1];
			}
			// Fix at end point to be doubly sure
			mMsgNo=DEBUG_BUFFERS-1;
		}

		//Create & copy our new buffer
		mMsgs[mMsgNo]=new WCHAR[wcslen(expanded_message)+1];
		wcscpy(mMsgs[mMsgNo],expanded_message);

		// Move the message pointer on
		mMsgNo++;
	}


	void MessageInstant(WCHAR *message,...)
	{
		WCHAR expanded_message[VARARG_BUFFSIZE];
		va_list tGlop;

		// Expand the varable argument list into the text buffer
		va_start(tGlop,message);
		if(vswprintf(expanded_message,message,tGlop)==-1)
		{
			// Fatal overflow, lets abort
			return;
		}
		va_end(tGlop);

		// Register our message
		Message(expanded_message);

		// Pretty Blue background
		RenderGradientBackground(dwTopColorI, dwBottomColorI);
		//m_BackGround.Render( &m_Font, 0, 0 );

		// Output messages
		Display();
		g_pd3dDevice->Present(NULL,NULL,NULL,NULL);
	}


	void Display(void)
	{
		float tmpy=mY;
		m_Font.Begin();
		for(int loop=0;loop<mMsgNo;loop++)
		{
			m_Font.DrawText( mX,tmpy,0xffffffff, mMsgs[loop] );
			tmpy+=m_Font.m_fFontHeight + 1;
		}
		m_Font.End();	
	}

	void RenderGradientBackground( DWORD dwTopColor, DWORD dwBottomColor )
	{
	    // First time around, allocate a vertex buffer
		static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
	    if( g_pVB == NULL )
		{
			g_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &g_pVB );
	        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
		    BACKGROUNDVERTEX* v;
			g_pVB->Lock( 0, 0, (BYTE**)&v, 0L );
	        v[0].p = D3DXVECTOR4(   0 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[0].color = dwTopColor;
		    v[1].p = D3DXVECTOR4( 640 - 0.5f,   0 - 0.5f, 1.0f, 1.0f );  v[1].color = dwTopColor;
			v[2].p = D3DXVECTOR4(   0 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[2].color = dwBottomColor;
	        v[3].p = D3DXVECTOR4( 640 - 0.5f, 480 - 0.5f, 1.0f, 1.0f );  v[3].color = dwBottomColor;
		    g_pVB->Unlock();
		}

	    // Set states
		g_pd3dDevice->SetTexture( 0, NULL );
	    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
	    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ); 
		g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
	    g_pd3dDevice->SetStreamSource( 0, g_pVB, 5*sizeof(FLOAT) );

		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	    // Clear the zbuffer
		g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );
	}

public:
	CXBFont	m_Font;
	WCHAR *mMsgs[DEBUG_BUFFERS];
	int	mMsgNo;
	float mX;
	float mY;
	DWORD dwTopColorI;
	DWORD dwBottomColorI;

};


#endif