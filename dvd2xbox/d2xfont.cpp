#include "D2Xfont.h"

std::auto_ptr<D2Xfont> D2Xfont::sm_inst;

D2Xfont::D2Xfont()
{
	fCursorX = 0.0;
	fCursorY = 0.0;
}


D2Xfont* D2Xfont::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xfont> tmp(new D2Xfont);
        sm_inst = tmp;
    }
    return sm_inst.get();
}

int	D2Xfont::LoadFont(const CStdString& strFilename,const CStdString& name)
{
	CXBFont*     m_Font=0; 
	m_Font = new CXBFont();
	if( FAILED( m_Font->Create( strFilename.c_str())  ))
        return 0;
	mapFont.insert(pair<CStdString,CXBFont*>(name.c_str(), m_Font));
	return 1;
}

void D2Xfont::SetCursorPosition(FLOAT fX, FLOAT fY )
{
	fCursorX = fX;
	fCursorY = fY;
}

void D2Xfont::DrawText(const CStdString& name, DWORD dwColor, const CStdStringW& strText)
{
	map<CStdString,CXBFont*>::iterator ifont;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{
		ifont->second->SetCursorPosition( fCursorX, fCursorY);
		ifont->second->DrawText( dwColor, strText );
	}
}

void D2Xfont::DrawText( const CStdString& name, FLOAT fX, FLOAT fY, DWORD dwColor, const CStdStringW& strText, DWORD dwFlags,DWORD dwFlags2, FLOAT fMaxPixelWidth, bool scroll )
{
	map<CStdString,CXBFont*>::iterator ifont;
	CStdStringW	strText2=strText;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{

		FLOAT fwidth = getFontWidth(name,strText2);

		if(scroll == true && fwidth > fMaxPixelWidth && fMaxPixelWidth != 0)
		{
			if(dwFlags2 & (D2XFONT_VERTICAL))
				getScrollText(name,strText,strText2,fMaxPixelWidth, &fY);
			else
				getScrollText(name,strText,strText2,fMaxPixelWidth, &fX);
			fMaxPixelWidth = 0.0;
		}
		else if(dwFlags2 & (D2XFONT_RIGHT|D2XFONT_CENTER))
		{
			if(fwidth < fMaxPixelWidth)
			{
				if(dwFlags2 & D2XFONT_RIGHT)
				{
					if(dwFlags2 & (D2XFONT_VERTICAL))
						fY = fY + (fMaxPixelWidth - fwidth);
					else
                        fX = fX + (fMaxPixelWidth - fwidth);
				}
				else if(dwFlags2 & D2XFONT_CENTER)
				{
					if(dwFlags2 & (D2XFONT_VERTICAL))
						fY = fY + (fMaxPixelWidth - fwidth)/2;
					else
						fX = fX + (fMaxPixelWidth - fwidth)/2;
				}
			}

		}
		// adjust scaling
		fX += g_d2xSettings.ScreenX1;
		fY += g_d2xSettings.ScreenY1;
		fX *= g_d2xSettings.ScreenScaleX;
		fY *= g_d2xSettings.ScreenScaleY;

		ifont->second->SetScaleFactors( g_d2xSettings.ScreenScaleX, g_d2xSettings.ScreenScaleY);

		if(dwFlags2 & (D2XFONT_VERTICAL))
			ifont->second->DrawTextVertical( fX, fY, dwColor, strText2, dwFlags, fMaxPixelWidth );
		else
        	ifont->second->DrawText( fX, fY, dwColor, strText2, dwFlags, fMaxPixelWidth );
	}
}

void D2Xfont::getScrollText(const CStdString& name,const CStdStringW& strText,CStdStringW& strText2, FLOAT fMaxPixelWidth, FLOAT* pixel_x)
{
	map<CStdStringW,_SCROLL>::iterator iscroll;
	DWORD dwTime = timeGetTime();
	strText2 = strText;
	FLOAT f_pixel=0;

	iscroll = mapScroll.find(strText.c_str());

	if(iscroll != mapScroll.end())
	{
		if(iscroll->second.last_access + D2XFONT_SCROLL_TIME <= dwTime)
		{
			
			if(iscroll->second.pixel_x == 0)
			{
				CStdStringW tmpText = iscroll->second.strScrolled;
				strText2 = tmpText.substr(1);
				strText2 += tmpText[0];
				iscroll->second.strScrolled = strText2;
				iscroll->second.pixel_x = getFontWidth(name,tmpText.substr(0,1))-1;
				f_pixel = iscroll->second.pixel_x;
			}
			else
			{
				strText2 = iscroll->second.strScrolled;
				--iscroll->second.pixel_x;
				f_pixel = iscroll->second.pixel_x;
			}
			iscroll->second.last_access = dwTime;
		}
		else
		{
			strText2 = iscroll->second.strScrolled;
			f_pixel = iscroll->second.pixel_x;
		}
	}
	else
	{
		_SCROLL tscroll;
		tscroll.last_access = dwTime+1000;
		tscroll.strScrolled = strText + "  -  ";
		tscroll.pixel_x = 0;
		mapScroll.insert(pair<CStdStringW,_SCROLL>(strText,tscroll));
	}
	while(getFontWidth(name,strText2) > fMaxPixelWidth-f_pixel)
	{
		strText2.erase(strText2.end()-1,strText2.end());
	}
	*pixel_x += f_pixel;
}

void D2Xfont::clearScrollCache()
{
	map<CStdStringW,_SCROLL>::iterator iscroll;
	map<CStdStringW,_SCROLL> tmpMap;
	DWORD dwTime = timeGetTime();
	int dwDiff;

	for(iscroll = mapScroll.begin(); iscroll != mapScroll.end(); iscroll++)
	{
		dwDiff = dwTime - iscroll->second.last_access;
		if(dwDiff <= D2XFONT_CACHE_TIME || dwDiff < 0)
		{
			tmpMap.insert(pair<CStdStringW,_SCROLL>(iscroll->first,iscroll->second));
		}
	}
	mapScroll = tmpMap;
}

float D2Xfont::getFontHeight( const CStdString& name)
{
	map<CStdString,CXBFont*>::iterator ifont;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{
		return ifont->second->GetFontHeight();
	}
	return 0.00;
}

float D2Xfont::getFontWidth( const CStdString& name, const CStdStringW& text)
{
	map<CStdString,CXBFont*>::iterator ifont;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{
		return ifont->second->GetTextWidth(text.c_str());
	}
	return 0.00;
}

CXBFont* D2Xfont::getFontObj( const CStdString& name)
{
	map<CStdString,CXBFont*>::iterator ifont;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{
		return ifont->second;
	}
	return NULL;
}

int	D2Xfont::LoadResource(const CStdString& strFilename,const CStdString& name)
{
	CXBPackedResource*     m_Res=0; 
	m_Res = new CXBPackedResource();
	if( FAILED( m_Res->Create( strFilename.c_str() ) ) )
        return 0;
	mapRes.insert(pair<CStdString,CXBPackedResource*>(name.c_str(), m_Res));
	return 1;
}

LPDIRECT3DTEXTURE8 D2Xfont::GetTexture(const CStdString& name, DWORD dwOffset)
{
	map<CStdString,CXBPackedResource*>::iterator ires;

	ires = mapRes.find(name.c_str());

	if(ires != mapRes.end())
	{
		return ires->second->GetTexture( dwOffset );
	}
	return 0;
}