#include "D2Xtexture.h"

std::auto_ptr<D2Xtexture> D2Xtexture::sm_inst;
static CRITICAL_SECTION m_criticalSection;

D2Xtexture::D2Xtexture()
{
	InitializeCriticalSection(&m_criticalSection);

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

//int D2Xtexture::LoadTexture(const CStdString& strFilename,int iID,DWORD dwColorKey)
//{
//	if (D3DXCreateTextureFromFileEx(g_pd3dDevice,strFilename.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
//			D3DX_DEFAULT , D3DX_DEFAULT, dwColorKey, NULL, NULL, &pTexture[iID]) == D3D_OK)
//		return 1;
//	else
//		return 0;
//}
//
//void D2Xtexture::RenderTexture(float x, float y, float width, float height, int iID)
//{
//	if(pTexture[iID] != NULL)
//	{
//
//		D3DSURFACE_DESC desc;
//		pTexture[iID]->GetLevelDesc( 0, &desc );
//
//		CUSTOMVERTEX cvVertices[4];
//		LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer = NULL;
//
//		cvVertices[0].p   = D3DXVECTOR4( x ,	y +height, 0.5f, 1.0f);
//		cvVertices[0].col = 0xFFFFFFFF ;
//		cvVertices[0].tu  = 0;
//		cvVertices[0].tv  = 1;
//		cvVertices[1].p   = D3DXVECTOR4( x , y , 0.5f,1.0f);
//		cvVertices[1].col = 0xFFFFFFFF ;
//		cvVertices[1].tu  = 0;
//		cvVertices[1].tv  = 0;
//		cvVertices[2].p   = D3DXVECTOR4( x+width, y+height , 0.5f, 1.0f);
//		cvVertices[2].col = 0xFFFFFFFF ;
//		cvVertices[2].tu  = 1;
//		cvVertices[2].tv  = 1;
//		cvVertices[3].p   = D3DXVECTOR4( x +width, y , 0.5f, 1.0f);
//		cvVertices[3].col = 0xFFFFFFFF ;
//		cvVertices[3].tu  = 1;
//		cvVertices[3].tv  = 0;
//
//		VOID* pVertices;
//		g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVertexBuffer);
//		
//		//Get a pointer to the vertex buffer vertices and lock the vertex buffer
//		g_pVertexBuffer->Lock(0, sizeof(cvVertices), (BYTE**)&pVertices, 0);
//	    
//		//Copy our stored vertices values into the vertex buffer
//		memcpy(pVertices, cvVertices, sizeof(cvVertices));
//	    
//		//Unlock the vertex buffer
//		g_pVertexBuffer->Unlock();
//
//		/*g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
//		g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);*/
//
//		//Turn off lighting because we are specifying that our vertices have textures colour
//		//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//
//		
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
//		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
//		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
//		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
//		g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
//		g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
//		g_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
//		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
//		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//		//g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
//		
//		// Render the image
//		g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer,  sizeof(CUSTOMVERTEX));
//		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);
//
//
//		//Set our background to use our texture buffer
//		g_pd3dDevice->SetTexture(0, pTexture[iID]);
//		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
//		//g_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );
//		g_pVertexBuffer->Release();
//	}
//	else
//	{
//		SCREENVERTEX v[4];
//		v[0].pos = D3DXVECTOR4( x-0.5f, y-0.5f, 0.5f, 1.0f );  v[0].color = 0xff000000;
//		v[1].pos = D3DXVECTOR4( x+width-0.5f, y-0.5f, 0.5f, 1.0f );  v[1].color = 0xff000000;
//		v[2].pos = D3DXVECTOR4( x-0.5f, y+height-0.5f, 0.5f, 1.0f );  v[2].color = 0xff000000;
//		v[3].pos = D3DXVECTOR4( x+width-0.5f, y+height-0.5f, 0.5f, 1.0f );  v[3].color = 0xff000000;
//	    
//		// Render the rectangle
//		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
//		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
//		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//
//		g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
//		g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(SCREENVERTEX) );
//	}
//
//}

int D2Xtexture::LoadFirstTextureFromXPR(const CStdString& strFileName, const CStdString& name,DWORD dwColorKey)
{
	map<CStdString,LPDIRECT3DTEXTURE8>::iterator ibmp;
	ibmp = mapTexture.find(name.c_str());
	if(ibmp != mapTexture.end())
		return 0;

	CXBPackedResource* pPackedResource = new CXBPackedResource();
	if ( SUCCEEDED( pPackedResource->Create( strFileName.c_str(), 1, NULL ) ) )
	{
		LPDIRECT3DTEXTURE8 pTexture;
		LPDIRECT3DTEXTURE8 m_pTexture;
		D3DSURFACE_DESC descSurface;

		pTexture = pPackedResource->GetTexture((DWORD)0);

		if (pTexture)
		{
			if ( SUCCEEDED( pTexture->GetLevelDesc( 0, &descSurface ) ) )
			{
				int iHeight = descSurface.Height;
				int iWidth = descSurface.Width;
				DWORD dwFormat = descSurface.Format;
				g_pd3dDevice->CreateTexture( iWidth,
					iHeight,
					1,
					0,
					D3DFMT_A8R8G8B8,
					0,
					&m_pTexture);

				LPDIRECT3DSURFACE8 pSrcSurface = NULL;
				LPDIRECT3DSURFACE8 pDestSurface = NULL;

				pTexture->GetSurfaceLevel( 0, &pSrcSurface );
				m_pTexture->GetSurfaceLevel( 0, &pDestSurface );

				D3DXLoadSurfaceFromSurface( pDestSurface, NULL, NULL,
											pSrcSurface, NULL, NULL,
											D3DX_DEFAULT, D3DCOLOR( 0 ) );

				mapTexture.insert(pair<CStdString,LPDIRECT3DTEXTURE8>(name.c_str(),m_pTexture));
				pSrcSurface->Release();
				pDestSurface->Release();
				pTexture->Release();
			}
			
		}
		delete pPackedResource;
		return 1;
	}
	else
	{
		delete pPackedResource;
		return 0;
	}
}

int D2Xtexture::LoadTexture2(const CStdString& strFilename,const CStdString& name,DWORD dwColorKey)
{
	map<CStdString,LPDIRECT3DTEXTURE8>::iterator ibmp;
	ibmp = mapTexture.find(name.c_str());
	if(ibmp != mapTexture.end())
		return 0;

	LPDIRECT3DTEXTURE8 pTexture;
	//pTexture = new LPDIRECT3DTEXTURE8();
	if (D3DXCreateTextureFromFileEx(g_pd3dDevice,strFilename.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			D3DX_DEFAULT , D3DX_DEFAULT, dwColorKey, NULL, NULL, &pTexture) == D3D_OK)
	{
		mapTexture.insert(pair<CStdString,LPDIRECT3DTEXTURE8>(name.c_str(),pTexture));
		return 1;
	}
	else
		return 0;
}

int D2Xtexture::UnloadTexture(const CStdString& strIconName)
{
	map<CStdString,LPDIRECT3DTEXTURE8>::iterator ibmp;

	ibmp = mapTexture.find(strIconName.c_str());

	if(ibmp != mapTexture.end())
	{
		EnterCriticalSection(&m_criticalSection);
		SAFE_RELEASE(ibmp->second);
		mapTexture.erase(ibmp);
		LeaveCriticalSection(&m_criticalSection);
	}
	return 1;
}

void D2Xtexture::RenderTexture2(const CStdString& name, float x, float y, float width, float height)
{
	map<CStdString,LPDIRECT3DTEXTURE8>::iterator ibmp;

	ibmp = mapTexture.find(name.c_str());

	if(ibmp != mapTexture.end())
	{

		//D3DSURFACE_DESC desc;
		//ibmp->second->GetLevelDesc( 0, &desc );

		CUSTOMVERTEX cvVertices[4];
		LPDIRECT3DVERTEXBUFFER8 g_pVertexBuffer = NULL;

		cvVertices[0].p   = D3DXVECTOR4( x ,	y +height, 0.5f, 1.0f);
		cvVertices[0].col = 0xFFFFFFFF ;
		cvVertices[0].tu  = 0;
		cvVertices[0].tv  = 1;
		cvVertices[1].p   = D3DXVECTOR4( x , y , 0.5f,1.0f);
		cvVertices[1].col = 0xFFFFFFFF ;
		cvVertices[1].tu  = 0;
		cvVertices[1].tv  = 0;
		cvVertices[2].p   = D3DXVECTOR4( x+width, y+height , 0.5f, 1.0f);
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

		/*g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		g_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);*/

		//Turn off lighting because we are specifying that our vertices have textures colour
		//g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_CLAMP );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_CLAMP );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,    FALSE );
		g_pd3dDevice->SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
		g_pd3dDevice->SetRenderState( D3DRS_FILLMODE,     D3DFILL_SOLID );
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE,     D3DCULL_CCW );
		g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_TEX1 );
		
		// Render the image
		g_pd3dDevice->SetStreamSource(0, g_pVertexBuffer,  sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1);


		//Set our background to use our texture buffer
		EnterCriticalSection(&m_criticalSection);
		g_pd3dDevice->SetTexture(0, ibmp->second);
		LeaveCriticalSection(&m_criticalSection);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//g_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );
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

bool D2Xtexture::IsTextureLoaded(const CStdString& strName)
{
	map<CStdString,LPDIRECT3DTEXTURE8>::iterator ibmp;

	ibmp = mapTexture.find(strName.c_str());

	if(ibmp != mapTexture.end())
		return true;
	else
		return false;	
}



