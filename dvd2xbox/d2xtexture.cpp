#include "D2Xtexture.h"

std::auto_ptr<D2Xtexture> D2Xtexture::sm_inst;

D2Xtexture::D2Xtexture()
{
	for(int i=0;i<TEXTURE_BUFFER;i++)
		pTexture[i] = NULL;

}


D2Xtexture* D2Xtexture::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xtexture> tmp(new D2Xtexture);
        sm_inst = tmp;
    }
    return sm_inst.get();
}

int D2Xtexture::LoadTexture(const CStdString& strFilename,int iID,DWORD dwColorKey)
{
	if (D3DXCreateTextureFromFileEx(g_pd3dDevice,strFilename.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			D3DX_DEFAULT , D3DX_DEFAULT, dwColorKey, NULL, NULL, &pTexture[iID]) == D3D_OK)
		return 1;
	else
		return 0;
}

void D2Xtexture::RenderTexture(float x, float y, float width, float height, int iID)
{
	if(pTexture[iID] != NULL)
	{

		D3DSURFACE_DESC desc;
		pTexture[iID]->GetLevelDesc( 0, &desc );

		CUSTOMVERTEX cvVertices[4];
		LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer = NULL;

		cvVertices[0].p   = D3DXVECTOR4( x ,	y +height, 0.5f, 1.0f);
		cvVertices[0].col = 0xFFFFFFFF ;
		cvVertices[0].tu  = 0;
		cvVertices[0].tv  = 1;
		cvVertices[1].p   = D3DXVECTOR4( x , y , 0.5f , 1.0f);
		cvVertices[1].col = 0xFFFFFFFF ;
		cvVertices[1].tu  = 0;
		cvVertices[1].tv  = 0;
		cvVertices[2].p   = D3DXVECTOR4( x+width, y+height , 0.5f, 1.0f );
		cvVertices[2].col = 0xFFFFFFFF ;
		cvVertices[2].tu  = 1;
		cvVertices[2].tv  = 1;
		cvVertices[3].p   = D3DXVECTOR4( x +width, y , 0.5f, 1.0f);
		cvVertices[3].col = 0xFFFFFFFF ;
		cvVertices[3].tu  = 1;
		cvVertices[3].tv  = 0;

		VOID* pVertices;
		g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer);
		
		//Get a pointer to the vertex buffer vertices and lock the vertex buffer
		g_pVertexBuffer->Lock(0, sizeof(cvVertices), (BYTE**)&pVertices, 0);
	    
		//Copy our stored vertices values into the vertex buffer
		memcpy(pVertices, cvVertices, sizeof(cvVertices));
	    
		//Unlock the vertex buffer
		g_pVertexBuffer->Unlock();

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);

		//Turn off lighting because we are specifying that our vertices have textures colour
		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		
		// Render the image
		g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer,  sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

		//Set our background to use our texture buffer
		g_pd3dDevice->SetTexture(0, pTexture[iID]);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		g_pVertexBuffer->Release();
	}
	else
	{
		SCREENVERTEX v[4];
		v[0].pos = D3DXVECTOR4( x-0.5f, y-0.5f, 0.5f, 1.0f );  v[0].color = 0xff000000;
		v[1].pos = D3DXVECTOR4( x+width-0.5f, y-0.5f, 0.5f, 1.0f );  v[1].color = 0xff000000;
		v[2].pos = D3DXVECTOR4( x-0.5f, y+height-0.5f, 0.5f, 1.0f );  v[2].color = 0xff000000;
		v[3].pos = D3DXVECTOR4( x+width-0.5f, y+height-0.5f, 0.5f, 1.0f );  v[3].color = 0xff000000;
	    
		// Render the rectangle
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
		g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(SCREENVERTEX) );
	}

}