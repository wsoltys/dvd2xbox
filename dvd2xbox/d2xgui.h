#ifndef D2XGUI
#define D2XGUI

#include <xtl.h>
#include "stdstring.h"
#include "d2xmedialib.h"
#include "d2xgraphics.h"
#include "d2xswindow.h"
#include "d2xgamemanager.h"
#include "d2xdbrowser.h"
#include "d2xguisettings.h"
#include "d2xviewer.h"
#include "d2xinput.h"
#include "..\keyboard\virtualkeyboard.h"
#include "..\..\xbox\tinyxml\tinyxml.h"
#include <vector>
#include <map>

#define X				0
#define Y				1

#define GUI_MAINMENU	0
#define GUI_GAMEMANAGER	1
#define GUI_FILEMANAGER	2
#define GUI_KEYBOARD	3
#define GUI_SETTINGS	4
#define GUI_VIEWER		5
#define GUI_DISKCOPY	6
#define	GUI_ERROR		7
#define GUI_STARTUP		8
#define GUI_CALIBRATION 9

#define CAL_TOP			100
#define CAL_BOTTOM		200


class D2Xgui
{
protected:

	D2Xgui();
	D2Xmedialib*				p_ml;
	D2Xgraphics					p_graph;
	D2XGM*						p_gm;
	CXBVirtualKeyboard* 		p_vk;
	D2Xguiset*					p_sg;
	D2Xviewer*					p_v;
	D2Xinput*					p_input;
	vector<TiXmlDocument*>		vXML;
	map<CStdString,CStdString>	strcText;
	map<CStdString,int>			strcInt;
	map<int,D2Xswin*>			map_swin;
	vector<int>					v_showids;
	D2Xdbrowser*	a_browser[2];
	TiXmlDocument*	xmlgui;
	CStdString		strSkin;
	int				prev_id;
	int				skip_frame;
	int				context_counter;

	typedef struct _CALIBRATION
	{
		int		focus;
		FLOAT	x1;
		FLOAT	y1;
		FLOAT	x2;
		FLOAT	y2;
	} CALIBRATION;

	CALIBRATION		s_cal;

	static std::auto_ptr<D2Xgui> sm_inst;

	int LoadXML(CStdString strSkinName);
	void DoClean();
	bool IsShowIDinList(int showid);
	float getMenuPosXY(int XY, int id, int showID);
	float getMenuOrigin(int XY, int id, int showID);
	int getMenuItems( int id, int showID, int* vspace=NULL);
	int getContextCounter(CStdString str_context);
	void ProcessXML(TiXmlElement* itemElement, int id);


public:
	

	static D2Xgui* Instance();

	int LoadSkin(CStdString strSkinName);
	void RenderGUI(int id);
	void SetKeyValue(CStdString key,CStdString value);
	void SetKeyInt(CStdString key, int value);
	int	 getKeyInt(CStdString key);
	CStdString getKeyValue(CStdString key);
	void SetWindowObject(int id, D2Xswin* win);
	void SetGMObject(D2XGM* gm);
	void SetVKObject(CXBVirtualKeyboard* vk);
	void SetBrowserObject(int id, D2Xdbrowser* b);
	void SetSGObject(D2Xguiset* sg);
	void SetViewObject(D2Xviewer* v);
	void SetShowIDs(int showid);
	void getContext(int& mCounter);

	void SetCalibrationFocus(int focus);
	int GetCalibrationFocus();
	void SetCalibrationCoordinates(FLOAT x, FLOAT y);
	void processCalibration();
	void SetNewScreen();
	void GetScreen();
	void ResetCalibration();
	void GetCalibrationValues(FLOAT& x, FLOAT& y, FLOAT& xf, FLOAT& yf);


};

#endif