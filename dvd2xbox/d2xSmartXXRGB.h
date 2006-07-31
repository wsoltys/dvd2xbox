#ifndef D2XSMARTXXRGB
#define D2XSMARTXXRGB


#include <xtl.h>
#include "d2xsettings.h"
#include <thread.h>
#include "d2xutils.h"
#include <tinyxml\tinyxml.h>

#define P_RED    0xf70c   //SmartXX V3 port for PWM red output
#define P_GREEN  0xf70d   //SmartXX V3 port for PWM green output
#define P_BLUE   0xf70e   //SmartXX V3 port for PWM blue output
#define P_STATUS 0xf702   //Status LED port

struct _RGBVALUE
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
};

struct _RGBVALUES
{
	CStdString strTransition;
	int time;

	unsigned short red1;
	unsigned short green1;
	unsigned short blue1;

	unsigned short red2;
	unsigned short green2;
	unsigned short blue2;
};

class D2XSmartXXRGB : public CThread
{
protected:

	
	CStdString					strLastTransition;
	_RGBVALUE					s_CurrentRGB;
	DWORD						dwLastTime;
	DWORD						dwFrameTime;

	void getRGBValues(CStdString strValues,CStdString strValues2,_RGBVALUES* s_rgb);
	void SetRGBLed(int red, int green, int blue);
	

public:
	D2XSmartXXRGB();
	~D2XSmartXXRGB();

	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();

	bool LoadXML(CStdString strFilename);
	static void SetRGBStatus(CStdString strStatus);
	static void SetLastRGBStatus();

};

#endif