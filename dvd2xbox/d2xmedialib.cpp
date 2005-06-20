#include "D2Xmedialib.h"


D2Xmedialib::D2Xmedialib()
{

}

D2Xmedialib::~D2Xmedialib()
{

}

int D2Xmedialib::LoadMedia(CStdString& strFilename)
{
	TiXmlElement*		itemElement;

	TiXmlDocument xmldoc( strFilename );
	bool loadOkay = xmldoc.LoadFile();
	if ( !loadOkay )
		return 0;

	
	return 1;
}