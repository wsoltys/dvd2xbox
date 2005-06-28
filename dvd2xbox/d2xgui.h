#ifndef D2XGUI
#define D2XGUI

#include <xtl.h>
#include "stdstring.h"
#include "d2xmedialib.h"
#include "d2xgraphics.h"
#include "d2xswindow.h"
#include "..\..\xbox\tinyxml\tinyxml.h"
#include <vector>
#include <map>

#define GUI_MAINMENU	0
#define GUI_GAMEMANAGER	1


class D2Xgui
{
protected:

	D2Xmedialib*	p_ml;
	D2Xgraphics		p_graph;
	D2Xswin*		p_win;
	vector<TiXmlDocument*> vXML;
	map<CStdString,CStdString> strcText;
	TiXmlDocument*	xmlgui;
	CStdString		strSkin;

	int LoadXML(CStdString strSkinName);


public:
	D2Xgui();
	~D2Xgui();

	int LoadSkin(CStdString strSkinName);
	void RenderGUI(int id);
	void SetKeyValue(CStdString key,CStdString value);
	void SetWindowObject(D2Xswin* win);


};

#endif