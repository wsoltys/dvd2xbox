#include "d2xsmartxxrgb.h"

static CRITICAL_SECTION m_criticalSection;

static map<CStdString,_RGBVALUES>	map_rgb;
static _RGBVALUES					s_RGBs;
static CStdString					strCurrentStatus;
static CStdString					strLastStatus;
static CStdString					strLastTransition;
static bool							bTransitionDone;
static bool							bRGB1;

D2XSmartXXRGB::D2XSmartXXRGB()
{
	map_rgb.clear();
	InitializeCriticalSection(&m_criticalSection);
	strCurrentStatus = "NULL";
	strLastStatus = "NULL";
	strLastTransition = "NULL";
	s_RGBs.strTransition = "NULL";
	s_RGBs.iFullTransitionTime = 0;
	s_CurrentRGB.red = 0;
	s_CurrentRGB.green = 0;
	s_CurrentRGB.blue = 0;
	dwLastTime = 0;
	iSleepTime = 0;
	bTransitionDone = true;
	bRGB1 = true;
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
	bool bRepeat = false;

	while(!m_bStop && g_d2xSettings.enableSmartXXRGB)
	{
		dwFrameTime = timeGetTime() - dwLastTime;

		if(s_RGBs.strTransition == "none" && strLastTransition != "none")
		{
			strLastTransition = "none";
			SetRGBLed(s_RGBs.red1,s_RGBs.green1,s_RGBs.blue1);
			iSleepTime = 200;
			SetTransitionDone(true);
		}
		else if(s_RGBs.strTransition.Equals("switch") && !strLastTransition.Equals("switch"))
		{
			if(dwFrameTime >= s_RGBs.time )
			{
				s_CurrentRGB.red = s_RGBs.red2;
				s_CurrentRGB.green = s_RGBs.green2;
				s_CurrentRGB.blue = s_RGBs.blue2;	
				strLastTransition = "switch";
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
			}
			else
			{
				s_CurrentRGB.red = s_RGBs.red1;
				s_CurrentRGB.green = s_RGBs.green1;
				s_CurrentRGB.blue = s_RGBs.blue1;
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
				SetTransitionDone(true);
			}	
		}
		else if(s_RGBs.strTransition == "blink")
		{
		
			if(strLastTransition != "blink")
			{
				bRGB1 = true;
				s_CurrentRGB.red = s_RGBs.red1;
				s_CurrentRGB.green = s_RGBs.green1;
				s_CurrentRGB.blue = s_RGBs.blue1;
				strLastTransition = "blink";
			}

			if(dwFrameTime >= s_RGBs.time )
			{
				s_CurrentRGB.red = (bRGB1) ? s_RGBs.red1 : s_RGBs.red2;
				s_CurrentRGB.green = (bRGB1) ? s_RGBs.green1 : s_RGBs.green2;
				s_CurrentRGB.blue = (bRGB1) ? s_RGBs.blue1 : s_RGBs.blue2;	
				dwLastTime = timeGetTime();
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
				bRGB1 = !bRGB1;
				SetTransitionDone(true);
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

				int iMaxSteps = 0;
				if(s_RGBs.iFullTransitionTime > 0)
				{
					iMaxSteps = max(abs(s_RGBs.red2-s_RGBs.red1),abs(s_RGBs.green2-s_RGBs.green1));
					iMaxSteps = max(iMaxSteps,abs(s_RGBs.blue2-s_RGBs.blue1));
					s_RGBs.time = int(s_RGBs.iFullTransitionTime/iMaxSteps);
				}
				iSleepTime = s_RGBs.time - dwFrameTime;
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

			if(s_CurrentRGB.red == s_RGBs.red2 && s_CurrentRGB.green == s_RGBs.green2 && s_CurrentRGB.blue == s_RGBs.blue2)
				SetTransitionDone(true);
			
		}
		else if(s_RGBs.strTransition == "fadeloop")
		{
			if(strLastTransition != "fadeloop")
			{
				s_CurrentRGB.red = s_RGBs.red1;
				s_CurrentRGB.green = s_RGBs.green1;
				s_CurrentRGB.blue = s_RGBs.blue1;
				strLastTransition = "fadeloop";

				int iMaxSteps = 0;
				if(s_RGBs.iFullTransitionTime > 0)
				{
					iMaxSteps = max(abs(s_RGBs.red2-s_RGBs.red1),abs(s_RGBs.green2-s_RGBs.green1));
					iMaxSteps = max(iMaxSteps,abs(s_RGBs.blue2-s_RGBs.blue1));
					s_RGBs.time = int(s_RGBs.iFullTransitionTime/iMaxSteps);
				}
				iSleepTime = s_RGBs.time - dwFrameTime;
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

				if (s_CurrentRGB.red == s_RGBs.red2 && s_CurrentRGB.green == s_RGBs.green2 && s_CurrentRGB.blue == s_RGBs.blue2)
				{ //reset for loop
					strLastTransition.clear();
				}
        
				dwLastTime = timeGetTime();
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
			}
			else
				iSleepTime = s_RGBs.time - dwFrameTime;

			// never done but if accidentally choosen for shutdown
			if(s_CurrentRGB.red == s_RGBs.red2 && s_CurrentRGB.green == s_RGBs.green2 && s_CurrentRGB.blue == s_RGBs.blue2)
				SetTransitionDone(true);
      		
		}
		else if(s_RGBs.strTransition.Equals("faderepeat"))
		{
			if(!strLastTransition.Equals("faderepeat"))
			{
				s_CurrentRGB.red = (bRepeat) ? s_RGBs.red1 : s_RGBs.red2;
				s_CurrentRGB.green = (bRepeat) ? s_RGBs.green1 : s_RGBs.green2;
				s_CurrentRGB.blue = (bRepeat) ? s_RGBs.blue1 : s_RGBs.blue2;
				strLastTransition = "faderepeat";

				int iMaxSteps = 0;
				if(s_RGBs.iFullTransitionTime > 0)
				{
					iMaxSteps = max(abs(s_RGBs.red2-s_RGBs.red1),abs(s_RGBs.green2-s_RGBs.green1));
					iMaxSteps = max(iMaxSteps,abs(s_RGBs.blue2-s_RGBs.blue1));
					s_RGBs.time = int(s_RGBs.iFullTransitionTime/iMaxSteps);
				}
				iSleepTime = s_RGBs.time - dwFrameTime;
			}

			if(dwFrameTime >= s_RGBs.time )
			{
				int i_RGB_R=0,i_RGB_G=0,i_RGB_B=0;
				i_RGB_R = (!bRepeat) ? s_RGBs.red1 : s_RGBs.red2;
				i_RGB_G = (!bRepeat) ? s_RGBs.green1 : s_RGBs.green2;
				i_RGB_B = (!bRepeat) ? s_RGBs.blue1 : s_RGBs.blue2;

				if(s_CurrentRGB.red > i_RGB_R )
					s_CurrentRGB.red--;
				else if(s_CurrentRGB.red < i_RGB_R)
					s_CurrentRGB.red++;
        
				if(s_CurrentRGB.green > i_RGB_G)
					s_CurrentRGB.green--;
				else if(s_CurrentRGB.green < i_RGB_G)
					s_CurrentRGB.green++;
        
				if(s_CurrentRGB.blue > i_RGB_B)
					s_CurrentRGB.blue--;
				else if(s_CurrentRGB.blue < i_RGB_B)
					s_CurrentRGB.blue++;

				if (s_CurrentRGB.red == i_RGB_R && s_CurrentRGB.green == i_RGB_G && s_CurrentRGB.blue == i_RGB_B)
				{ //reset for loop
					strLastTransition.clear();
					bRepeat = !bRepeat;
				}
				dwLastTime = timeGetTime();
				SetRGBLed(s_CurrentRGB.red,s_CurrentRGB.green,s_CurrentRGB.blue);
			}
			// never done but if accidentally choosen for shutdown
			if(s_CurrentRGB.red == s_RGBs.red2 && s_CurrentRGB.green == s_RGBs.green2 && s_CurrentRGB.blue == s_RGBs.blue2)
				SetTransitionDone(true);
		}

		if(iSleepTime < 0)
			iSleepTime = 0;
		Sleep(iSleepTime);
	}
}

void D2XSmartXXRGB::OnExit()
{
	//SetRGBLed(0,0,0);
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
				CStdString	strTemp;
				strTemp = pNode2->FirstChild()->Value();
				if(strTemp[0] == '#')
				{
					strTemp.Remove('#');
					s_rgb.time = atoi(strTemp.c_str());
					s_rgb.iFullTransitionTime = s_rgb.time;
				}
				else
				{
					s_rgb.time = atoi(pNode2->FirstChild()->Value());
					s_rgb.iFullTransitionTime = 0;
				}
				if(strStatus.Equals("shutdown") || strStatus.Equals("startxbe"))
				{
					// we don't want dvd2xbox waiting too long
					if(s_rgb.iFullTransitionTime > 500 || s_rgb.iFullTransitionTime == 0)
						s_rgb.iFullTransitionTime = 500;
				}
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
		SetTransitionDone(false);
		EnterCriticalSection(&m_criticalSection);
		strLastStatus = strCurrentStatus;
		strCurrentStatus = strStatus;
		strLastTransition.clear();
		s_RGBs = ikey->second;
		LeaveCriticalSection(&m_criticalSection);
	}
	return;
}

void D2XSmartXXRGB::SetRGBState(CStdString strRGB1, CStdString strRGB2, CStdString strTransition, CStdString strTranTime)
{

	CStdString	strValue;
	_RGBVALUES	s_rgb;
	map<CStdString,_RGBVALUES>::iterator ikey;

	getRGBValues(strRGB1,strRGB2,&s_rgb);

	if(strTranTime[0] == '#')
	{
		strTranTime.Remove('#');
		s_rgb.time = atoi(strTranTime.c_str());
		s_rgb.iFullTransitionTime = s_rgb.time;
	}
	else
	{
		s_rgb.time = atoi(strTranTime.c_str());
		s_rgb.iFullTransitionTime = 0;
	}

	if (s_rgb.time < 0)
		s_rgb.time = 0;
	if (s_rgb.iFullTransitionTime < 0)
		s_rgb.iFullTransitionTime = 0;
	
	if(!strTransition.Equals("none") || !strTransition.IsEmpty() )
		s_rgb.strTransition = strTransition.c_str();
	else
		s_rgb.strTransition = "none";

	ikey = map_rgb.find(strTransition);
	if(ikey != map_rgb.end())
		ikey->second = s_rgb;
	else
        map_rgb.insert(std::pair<CStdString,_RGBVALUES>(strTransition,s_rgb));

	SetRGBStatus(strTransition);
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

void D2XSmartXXRGB::SetTransitionDone(bool state)
{
	EnterCriticalSection(&m_criticalSection);
	bTransitionDone = state;
	LeaveCriticalSection(&m_criticalSection);
}

bool D2XSmartXXRGB::IsTransitionDone()
{
	bool bState;
	EnterCriticalSection(&m_criticalSection);
	bState = bTransitionDone;
	LeaveCriticalSection(&m_criticalSection);
	return bState;
}