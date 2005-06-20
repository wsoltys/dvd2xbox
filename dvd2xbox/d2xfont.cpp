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
	CXBFont     m_Font; 
	if( FAILED( m_Font.Create( strFilename.c_str() ) ) )
        return 0;
	mapFont.insert(pair<CStdString,CXBFont>(name.c_str(), m_Font));
	return 1;
}

void D2Xfont::SetCursorPosition(FLOAT fX, FLOAT fY )
{
	fCursorX = fX;
	fCursorY = fY;
}

void D2Xfont::DrawText(const CStdString& name, DWORD dwColor, const CStdStringW& strText)
{
	map<CStdString,CXBFont>::iterator ifont;

	ifont = mapFont.find(name.c_str());

	if(ifont != mapFont.end())
	{
		ifont->second.SetCursorPosition( fCursorX, fCursorY);
		ifont->second.DrawText( dwColor, strText );
	}
}