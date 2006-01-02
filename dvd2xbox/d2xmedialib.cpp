#include "D2Xmedialib.h"


D2Xmedialib::D2Xmedialib()
{
	p_Font = D2Xfont::Instance();
	p_tex  = D2Xtexture::Instance();

	i_xbeIcon  = 256 * 1024;
}

D2Xmedialib::~D2Xmedialib()
{

}

int D2Xmedialib::LoadMedia(CStdString& strSkindir)
{

	if(strSkindir[strSkindir.size()-1] != '\\')
		strSkindir.append("\\");

	if(LoadFonts(strSkindir) == 0)
		return 0;

	if(LoadBitmaps(strSkindir) == 0)
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
		p_tex->LoadFirstTextureFromXPR("T:\\1.xpr", strIconName, 0x00000000);

	delete p_xbe;

	return 1;
}

int D2Xmedialib::UnloadTexture(CStdString strIconName)
{
	return p_tex->UnloadTexture(strIconName);
}


void D2Xmedialib::DrawText( const CStdString name, FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags, FLOAT fMaxPixelWidth )
{
	p_Font->DrawText(  name, fX, fY, dwColor,  strText, dwFlags, fMaxPixelWidth);
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

	
	return 1;
}

void D2Xmedialib::RenderTexture2(CStdString name, FLOAT x, FLOAT y, FLOAT width, FLOAT height)
{
	p_tex->RenderTexture2(name,  x,  y,  width,  height);
}