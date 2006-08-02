#include "d2xsmartxxrgb.h"

static CRITICAL_SECTION m_criticalSection;

static map<CStdString,_RGBVALUES>	map_rgb;
static _RGBVALUES					s_RGBs;
static CStdString					strCurrentStatus;
static CStdString					strLastStatus;

D2XSmartXXRGB::D2XSmartXXRGB()
{
	map_rgb.clear();
	InitializeCriticalSection(&m_criticalSection);
	strCurrentStatus = "NULL";
	strLastStatus = "NULL";
	s_RGBs.strTransition = "NULL";
	s_CurrentRGB.red = 0;
	s_CurrentRGB.green = 0;
	s_CurrentRGB.blue = 0;
	dwLastTime = 0;
	iSleepTime = 0;
}

D2XSmartXXRGB::~D2XSmartXXRGB()
{
	map_rgb.clear();
	DeleteCriticalSection(&m_criticalSection);
}

static void outb(unsigned short port, unsigned char data)
{
  __asm
  {
    nop
    mov dx, port
    nop
    mov al, data
    nop
    out dx,al
    nop
    nop
  }
}

void D2XSmartXXRGB::OnStartup()
{
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
	DebugOut("Starting SmartXX RGB LED thread\n");

	/*DebugOut("RGB: Loading XML\n");
	if(!LoadXML("Q:\\SmartXXRGB.xml"))
	{
		DebugOut("RGB: Error loading XML\n");
		StopThread();
	}*/

	outb(P_STATUS, 0x0);  // Status LED OFF
	SetRGBStatus("general");
}

void D2XSmartXXRGB::Process()
{
	while(!m_bStop && g_d2xSettings.enableSmartXXRGB)
	{
		dwFrameTime = timeGetTime() - dwLastTime;

		if(s_RGBs.strTransition == "none" && strLastTransition != "none")
		{
			strLastTransition = "none";
			SetRGBLed(s_RGBs.red1,s_RGBs.green1,s_RGBs.blue1);
			iSleepTime = 200;
		}
		else if(s_RGBs.strTransition == "blink")
		{
			strLastTransition = "blink";
			if(dwFrameTime >= s_RGBs.time )
			{
				s_CurrentRGB.red = (s_CurrentRGB.red != s_RGBs.red1) ? s_RGBs.red1 : s_RGBs.red2;
				s_CurrentRGB.green = (s_CurrentRGB.green != s_RGBs.green1) ? s_RGBs.green1 : s_RGBs.green2;
				s_CurrentRGB.blue = (s_CurrentRGB.blue != s_RGBs.blue1) ? s_RGBs.blue1 : s_RGBs.blue2;	
				dwLastTime = timeGetTime();
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
			}
			else
				iSleepTime = s_RGBs.time - dwFrameTime;

		}
		else if(s_RGBs.strTransition == "fade")
		{
			if(strLastTransition != "fade")
			{
				s_CurrentRGB.red = s_RGBs.red1;
				s_CurrentRGB.green = s_RGBs.green1;
				s_CurrentRGB.blue = s_RGBs.blue1;
				strLastTransition = "fade";
			}

			if(dwFrameTime >= s_RGBs.time )
			{
				if(s_CurrentRGB.red > s_RGBs.red2)
					s_CurrentRGB.red--;
				else if(s_CurrentRGB.red < s_RGBs.red2)
					s_CurrentRGB.red++;

				if(s_CurrentRGB.green > s_RGBs.green2)
					s_CurrentRGB.green--;
				else if(s_CurrentRGB.green < s_RGBs.green2)
					s_CurrentRGB.green++;

				if(s_CurrentRGB.blue > s_RGBs.blue2)
					s_CurrentRGB.blue--;
				else if(s_CurrentRGB.blue < s_RGBs.blue2)
					s_CurrentRGB.blue++;

				dwLastTime = timeGetTime();
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
			}
			else
				iSleepTime = s_RGBs.time - dwFrameTime;
			
		}
		if(iSleepTime < 0)
			iSleepTime = 0;
		Sleep(iSleepTime);
	}
}

void D2XSmartXXRGB::OnExit()
{
	SetRGBLed(0,0,0);
	DebugOut("Stopping SmartXX RGB LED thread\n");
}

bool D2XSmartXXRGB::LoadXML(CStdString strFilename)
{
	TiXmlElement*		itemElement;
	TiXmlNode*			pNode;
	TiXmlNode*			pNode2;
	CStdString			strValue;

	TiXmlDocument rm_xmldoc( strFilename );
	bool loadOkay = rm_xmldoc.LoadFile();
	if ( !loadOkay )
		return false;

	itemElement = rm_xmldoc.RootElement();
	if( !itemElement )
		return false;

	strValue = itemElement->Value();
	if (strValue != CStdString("smartxxrgb"))
		return false;

	for( pNode = itemElement->FirstChild( "item" );
	pNode;
	pNode = pNode->NextSibling( "item" ) )
	{
		pNode2 = pNode->FirstChild("status");
		if(pNode2)
		{
			CStdString	strStatus,strValue1,strValue2;
			_RGBVALUES	s_rgb;

			strStatus = pNode2->FirstChild()->Value();
			pNode2 = pNode->FirstChild("rgb1");
			if(pNode2)
			{
				strValue1 = pNode2->FirstChild()->Value();
			}
			pNode2 = pNode->FirstChild("rgb2");
			if(pNode2)
			{
				strValue2 = pNode2->FirstChild()->Value();
			}

			getRGBValues(strValue1,strValue2,&s_rgb);
				
			pNode2 = pNode->FirstChild("transition");
			if(pNode2)
			{
				s_rgb.strTransition = pNode2->FirstChild()->Value();
			}
			else
				s_rgb.strTransition = "none";

			pNode2 = pNode->FirstChild("time");
			if(pNode2)
			{
				s_rgb.time = atoi(pNode2->FirstChild()->Value());
			}
			else
				s_rgb.time = 0;

			map_rgb.insert(std::pair<CStdString,_RGBVALUES>(strStatus,s_rgb)); 
		}
	}
	return true;
}

void D2XSmartXXRGB::getRGBValues(CStdString strValues1,CStdString strValues2,_RGBVALUES* s_rgb)
{
	DWORD red=0,green=0,blue=0;
	int ret = sscanf(strValues1.c_str(),"#%2X%2X%2X",&red,&green,&blue); 
	if(ret == 3)
	{
		s_rgb->red1 = int(red/2);
		s_rgb->green1 = int(green/2);
		s_rgb->blue1 = int(blue/2);
	}
	else
	{
		s_rgb->red1 = 0;
		s_rgb->green1 = 0;
		s_rgb->blue1 = 0;
	}

	ret = sscanf(strValues2.c_str(),"#%2X%2X%2X",&red,&green,&blue);
	if(ret == 3)
	{
		s_rgb->red2 = int(red/2);
		s_rgb->green2 = int(green/2);
		s_rgb->blue2 = int(blue/2);
	}
	else
	{
		s_rgb->red2 = 0;
		s_rgb->green2 = 0;
		s_rgb->blue2 = 0;
	}
}

void D2XSmartXXRGB::SetRGBStatus(CStdString strStatus)
{
	map<CStdString,_RGBVALUES>::iterator ikey;

	ikey = map_rgb.find(strStatus);
	if(ikey != map_rgb.end())
	{
		EnterCriticalSection(&m_criticalSection);
		strLastStatus = strCurrentStatus;
		strCurrentStatus = strStatus;
		s_RGBs = ikey->second;
		LeaveCriticalSection(&m_criticalSection);
	}
	return;
}

void D2XSmartXXRGB::SetLastRGBStatus()
{
	SetRGBStatus(strLastStatus);
}


void D2XSmartXXRGB::SetRGBLed(int red, int green, int blue)
{
	DebugOut("Setting SmartXX RGB LED to %s (%d,%d,%d) %d",strCurrentStatus.c_str(),red,green,blue,timeGetTime() );

	outb(P_RED,red);
	outb(P_GREEN,green); 
	outb(P_BLUE,blue);
}