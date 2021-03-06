#include "D2Xmedialib.h"

std::auto_ptr<D2Xmedialib> D2Xmedialib::sm_inst;

D2Xmedialib::D2Xmedialib()
{
	p_Font = D2Xfont::Instance();
	p_tex  = D2Xtexture::Instance();
	p_wav  = D2Xguisounds::Instance();
}

D2Xmedialib* D2Xmedialib::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xmedialib> tmp(new D2Xmedialib);
        sm_inst = tmp;
    }
    return sm_inst.get();
}

int D2Xmedialib::LoadMedia(CStdString& strSkindir)
{

	if(strSkindir[strSkindir.size()-1] != '\\')
		strSkindir.append("\\");

	if(LoadFonts(strSkindir) == 0)
		return 0;

	if(LoadBitmaps(strSkindir) == 0)
		return 0;

	if(LoadSounds(strSkindir) == 0)
		return 0;

	if(LoadSoundMap(strSkindir) == 0)
		return 0;
	
	return 1;
}

int D2Xmedialib::LoadFonts(CStdString& strSkindir)
{
	TiXmlElement*		itemElement;
	TiXmlNode*			fontNode;
	CStdString			strValue;

	strValue = strSkindir;
	strValue.append("fonts.xml");

	// Load default font
	if(p_Font->LoadFont("Q:\\media\\Font.xpr","D2XDefaultFont") == 0)
		return 0;

	TiXmlDocument xmldoc( strValue );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;

	itemElement = xmldoc.RootElement();
	if( !itemElement )
		return 0;

	strValue = itemElement->Value();
	if (strValue != CStdString("fonts"))
		return 0;

	for( fontNode = itemElement->FirstChild( "font" );
	fontNode;
	fontNode = fontNode->NextSibling( "font" ) )
	{
		const TiXmlNode *pNode = fontNode->FirstChild("name");
		if (pNode)
		{
			CStdString strFontName = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = fontNode->FirstChild("filename");
			if (pNode)
			{
				CStdString strFontFileName = strSkindir;
				strFontFileName.append("media\\");
				strFontFileName.append( pNode->FirstChild()->Value() );
				if (strstr(strFontFileName, ".xpr") != NULL)
				{
					p_Font->LoadFont(strFontFileName,strFontName);
				}
			}
		}
	}

	for( fontNode = itemElement->FirstChild( "res" );
	fontNode;
	fontNode = fontNode->NextSibling( "res" ) )
	{
		const TiXmlNode *pNode = fontNode->FirstChild("name");
		if (pNode)
		{
			CStdString strResName = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = fontNode->FirstChild("filename");
			if (pNode)
			{
				CStdString strResFileName = strSkindir;
				strResFileName.append("media\\");
				strResFileName.append( pNode->FirstChild()->Value() );
				if (strstr(strResFileName, ".xpr") != NULL)
				{
					if(strResName == "keyboard")
						p_Font->LoadResource(strResFileName,strResName);
				}
			}
		}
	}

	
	return 1;
}

int D2Xmedialib::LoadXBEIcon(CStdString strXBEPath, CStdString strIconName)
{
	CXBE* p_xbe=NULL;

	p_xbe = new CXBE();
	if(p_xbe == NULL)
	{
		return 0;
	}
	::DeleteFile("T:\\1.xpr");
	if(p_xbe->ExtractIcon(strXBEPath,"T:\\1.xpr") == true)
	{
		delete p_xbe;
		return p_tex->LoadFirstTextureFromXPR("T:\\1.xpr", strIconName, 0x00000000);
	}
	else
	{
		delete p_xbe;
		return 0;
	}
}

bool D2Xmedialib::getTextureStatus(const CStdString& strTextureName)
{
	map<CStdString,bool>::iterator ibmp;
	ibmp = mapTextureStatus.find(strTextureName);
	if(ibmp != mapTextureStatus.end())
		return ibmp->second;
	else 
		return true;
}

void D2Xmedialib::SetTextureStatus(const CStdString& strTextureName,bool status)
{
	map<CStdString,bool>::iterator ibmp;
	ibmp = mapTextureStatus.find(strTextureName);
	if(ibmp != mapTextureStatus.end())
		ibmp->second = status;
	else 
		mapTextureStatus.insert(pair<CStdString,bool>(strTextureName,status));
}

int D2Xmedialib::LoadTextureFromTitleID(const CStdString& titleid, const CStdString& alt_FileName, const CStdString& name,DWORD dwColorKey)
{
	CStdString strFileName = "e:\\UDATA\\"+titleid+"\\TitleImage.xbx";
	int ret = p_tex->LoadFirstTextureFromXPR(strFileName, name, 0x00000000);
	if(!ret)
		ret = LoadXBEIcon(alt_FileName,titleid);
	if(!ret)
		SetTextureStatus(titleid,false);
	else
		SetTextureStatus(titleid,true);

	return ret;
}

void D2Xmedialib::UnloadGameIcons()
{
	map<CStdString,bool>::iterator ibmp;
	for(ibmp = mapTextureStatus.begin(); ibmp != mapTextureStatus.end(); ibmp++)
	{
		p_tex->UnloadTexture(ibmp->first);
	}
	mapTextureStatus.clear();
}

int D2Xmedialib::UnloadTexture(CStdString strIconName)
{
	return p_tex->UnloadTexture(strIconName);
}


void D2Xmedialib::DrawText( const CStdString name, FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags, DWORD dwFlags2, FLOAT fMaxPixelWidth, bool scroll )
{
	p_Font->DrawText(  name, fX, fY, dwColor,  strText, dwFlags, dwFlags2, fMaxPixelWidth, scroll);
}

float D2Xmedialib::getFontHeight( const CStdString& name)
{
	return p_Font->getFontHeight(name);
}

float D2Xmedialib::getFontWidth( const CStdString& name, const CStdStringW& text)
{
	return p_Font->getFontWidth(name,text);
}

CXBFont* D2Xmedialib::getFontObj( const CStdString& name)
{
	return p_Font->getFontObj(name);
}

LPDIRECT3DTEXTURE8 D2Xmedialib::GetTexture(const CStdString& name, DWORD dwOffset)
{
	return p_Font->GetTexture(name, dwOffset);
}

// Bitmaps

int D2Xmedialib::LoadBitmaps(CStdString& strSkindir)
{
	TiXmlElement*		itemElement;
	TiXmlNode*			bmpNode;
	CStdString			strValue;

	strValue = strSkindir;
	strValue.append("textures.xml");

	TiXmlDocument xmldoc( strValue );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;

	itemElement = xmldoc.RootElement();
	if( !itemElement )
		return 0;

	strValue = itemElement->Value();
	if (strValue != CStdString("textures"))
		return 0;

	for( bmpNode = itemElement->FirstChild( "texture" );
	bmpNode;
	bmpNode = bmpNode->NextSibling( "texture" ) )
	{
		const TiXmlNode *pNode = bmpNode->FirstChild("name");
		if (pNode)
		{
			CStdString strBMPName = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = bmpNode->FirstChild("filename");
			if (pNode)
			{
				CStdString strBMPFileName = strSkindir;
				strBMPFileName.append("media\\");
				strBMPFileName.append( pNode->FirstChild()->Value() );

				const TiXmlNode *pNode = bmpNode->FirstChild("colorkey");
				
				DWORD ck = 0;
				if(pNode)
				{
					CStdString colorkey = pNode->FirstChild()->Value();
					sscanf( colorkey.c_str(),"%X",&ck);
				}

				
				p_tex->LoadTexture2(strBMPFileName,strBMPName, ck);
		
			}
		}
	}

	for( bmpNode = itemElement->FirstChild( "res" );
	bmpNode;
	bmpNode = bmpNode->NextSibling( "res" ) )
	{
		const TiXmlNode *pNode = bmpNode->FirstChild("name");
		if (pNode)
		{
			CStdString strResName = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = bmpNode->FirstChild("filename");
			if (pNode)
			{
				CStdString strResFileName = strSkindir;
				strResFileName.append("media\\");
				strResFileName.append( pNode->FirstChild()->Value() );
				if (strstr(strResFileName, ".xpr") != NULL)
				{
						p_tex->LoadTexturesFromXPR(strResFileName);
				}
			}
		}
	}

	if(!IsTextureLoaded("calibration_background"))
	{
		CStdString strFileName = strSkindir + "media\\calibration_background.png";
		if(!p_tex->LoadTexture2(strFileName,"calibration_background", 0))
		{
			strFileName = "Q:\\media\\calibration_background.png";
			p_tex->LoadTexture2(strFileName,"calibration_background", 0);
		}
	}
	return 1;
}

void D2Xmedialib::RenderTexture2(CStdString name, FLOAT x, FLOAT y, FLOAT width, FLOAT height, bool scaled)
{
	p_tex->RenderTexture2(name,  x,  y,  width,  height, scaled);
}

bool D2Xmedialib::IsTextureLoaded(const CStdString strName)
{
	return p_tex->IsTextureLoaded(strName);
}

void D2Xmedialib::clearScrollCache()
{
	p_Font->clearScrollCache();
}

// Sounds

int D2Xmedialib::LoadSounds(CStdString& strSkindir)
{

	if(!p_wav->InitDirectMusic())
		return 0;

	TiXmlElement*		itemElement;
	TiXmlNode*			sndNode;
	CStdString			strValue;

	strValue = strSkindir;
	strValue.append("sounds.xml");

	TiXmlDocument xmldoc( strValue );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;

	itemElement = xmldoc.RootElement();
	if( !itemElement )
		return 0;

	strValue = itemElement->Value();
	if (strValue != CStdString("sounds"))
		return 0;

	for( sndNode = itemElement->FirstChild( "sound" );
	sndNode;
	sndNode = sndNode->NextSibling( "sound" ) )
	{
		const TiXmlNode *pNode = sndNode->FirstChild("name");
		if (pNode)
		{
			CStdString strSNDName = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = sndNode->FirstChild("filename");
			if (pNode)
			{
				CStdString strSNDFileName = strSkindir;
				strSNDFileName.append("sound\\");
				strSNDFileName.append( pNode->FirstChild()->Value() );

					
				p_wav->LoadSound(strSNDFileName,strSNDName);
		
			}
		}
	}
	
	p_wav->ReleaseLoader();
	return 1;
}

void D2Xmedialib::DoSoundWork()
{
	p_wav->DoSoundWork();
}

// Key Sounds

int D2Xmedialib::LoadSoundMap(CStdString& strSkindir)
{

	TiXmlElement*		itemElement;
	TiXmlNode*			sndNode;
	CStdString			strValue;

	strValue = strSkindir;
	strValue.append("guisounds.xml");

	TiXmlDocument xmldoc( strValue );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;

	itemElement = xmldoc.RootElement();
	if( !itemElement )
		return 0;

	strValue = itemElement->Value();
	if (strValue != CStdString("sounds"))
		return 0;

	for( sndNode = itemElement->FirstChild( "action" );
	sndNode;
	sndNode = sndNode->NextSibling( "action" ) )
	{
		const TiXmlNode *pNode = sndNode->FirstChild("name");
		if (pNode)
		{
			CStdString strSNDAction = pNode->FirstChild()->Value();
			const TiXmlNode *pNode = sndNode->FirstChild("soundname");
			if (pNode)
			{
				CStdString strSNDName = pNode->FirstChild()->Value();				
				mapSoundKeyMap.insert(pair<CStdString,CStdString>(strSNDAction,strSNDName));
			}
		}
	}
	
	return 1;
}


void D2Xmedialib::PlayKeySound(CStdString strAction)
{
	map<CStdString,CStdString>::iterator iwav;
	iwav = mapSoundKeyMap.find(strAction);
	if(iwav != mapSoundKeyMap.end())
	{
		p_wav->PlaySound(iwav->second);
	}
}

void D2Xmedialib::PlaySound(CStdString strName)
{
	p_wav->PlaySound(strName);
}

void D2Xmedialib::PlaySoundOnce(CStdString strName)
{
	map<CStdString,DWORD>::iterator iwav;
	iwav = mapSoundPlaying.find(strName);
	DWORD dwTime = timeGetTime();
	if(iwav == mapSoundPlaying.end())
	{
		map<CStdString,DWORD> tmpMap;
		p_wav->PlaySound(strName);
		mapSoundPlaying.insert(pair<CStdString,DWORD>(strName,dwTime));
	}
	else
	{
		iwav->second = dwTime;
	}
}

void D2Xmedialib::clearSoundCache(DWORD dwCacheTime)
{
	map<CStdString,DWORD>::iterator icache;
	map<CStdString,DWORD> tmpMap;
	DWORD dwTime = timeGetTime();
	DWORD dwDiff;

	for(icache = mapSoundPlaying.begin(); icache != mapSoundPlaying.end(); icache++)
	{
		dwDiff = dwTime - icache->second;
		if(dwDiff <= dwCacheTime)
		{
			tmpMap.insert(pair<CStdString,DWORD>(icache->first,icache->second));
		}
	}
	mapSoundPlaying = tmpMap;
}