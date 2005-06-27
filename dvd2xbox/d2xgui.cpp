#include "D2Xgui.h"


D2Xgui::D2Xgui()
{
}

D2Xgui::~D2Xgui()
{

}

int D2Xgui::LoadSkin(CStdString strSkinName)
{
	p_ml = new D2Xmedialib();

	CStdString	strValue;
	strValue = "q:\\skins\\"+strSkinName;

	p_ml->LoadMedia(strValue);

	return 1;
}