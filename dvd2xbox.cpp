#include <XBApp.h>
#include <XBFont.h>
//#include <XBHelp.h>
#include <xgraphics.h>
#include <stdio.h>
//#include <debug.h>
#include <map>

#include <algorithm>
#include <iosupport.h>
#include <undocumented.h>
#include "Xcddb\cddb.h" 
#include "background.h"
#include "dvd2xbox\d2xpatcher.h"
#include "dvd2xbox\d2xgraphics.h"
#include "dvd2xbox\d2xdbrowser.h"
#include "dvd2xbox\d2xfilecopy.h"
#include "dvd2xbox\d2xtitle.h"
#include "dvd2xbox\d2xswindow.h"
#include "dvd2xbox\d2xdrivestatus.h"
#include "dvd2xbox\d2xlogger.h"
#include "dvd2xbox\d2xacl.h"
#include "dvd2xbox\d2xutils.h"
#include "dvd2xbox\d2xsettings.h"
#include "dvd2xbox\d2xviewer.h"
#include "dvd2xbox\d2xinput.h"
#include "dvd2xbox\d2xguisettings.h"
#include "keyboard\virtualkeyboard.h"
#include "xbox\LCDFactory.h"
#include "xbox\led.h"
#include "dvd2xbox\d2xfilefactory.h"
#include "dvd2xbox\d2xgamemanager.h"
#include "lib/libfilezilla/xbfilezilla.h"
#include <xkhdd.h>
#include "dvd2xbox\d2xgui.h"
#include "dvd2xbox\d2xmedialib.h"


/*
extern "C" 
{
	int VampsPlayTitle( char* device,char* titlenr,char* chapternr,char* anglenr );
}*/


 
#ifdef _DEBUG
#pragma comment (lib,"lib/libcdio/libcdiod.lib") 
#pragma comment (lib,"lib/libsmb/libsmbd.lib") 
#pragma comment (lib,"lib/libcdrip/cdripd.lib")
#pragma comment (lib,"lib/libogg/liboggvorbisd.lib") 
#pragma comment (lib,"lib/liblame/liblamed.lib") 
#pragma comment (lib,"lib/libsndfile/libsndfiled.lib")  
#pragma comment (lib,"lib/libftpc/libftpcd.lib") 
#pragma comment (lib,"lib/libdvdread/libdvdreadd.lib") 
//#pragma comment (lib,"lib/libfilezilla/debug/xbfilezillad.lib") 
#else
#pragma comment (lib,"lib/libcdio/libcdio.lib")
#pragma comment (lib,"lib/libsmb/libsmb.lib") 
#pragma comment (lib,"lib/libcdrip/cdrip.lib")
#pragma comment (lib,"lib/libogg/liboggvorbis.lib") 
#pragma comment (lib,"lib/liblame/liblame.lib") 
#pragma comment (lib,"lib/libsndfile/libsndfile.lib") 
#pragma comment (lib,"lib/libftpc/libftpc.lib") 
#pragma comment (lib,"lib/libdvdread/libdvdread.lib") 
//#pragma comment (lib,"lib/libfilezilla/release/xbfilezilla.lib") 
#endif
#pragma comment (lib,"lib/libxenium/XeniumSPIg.lib")

 
char *mainmenu[]={"Copy DVD/CD-R to HDD","Game Manager","Copy DVD/CD-R to SMB share","Filemanager","Settings","Boot to dash",NULL};
char *ddumpDirs[]={"e:\\", "e:\\games", NULL};
char *actionmenu[]={"Copy file/dir","Delete file/dir","Rename file/dir","Create dir"/*,"Patch Media check 1/2"*/,"Process ACL",
					"Patch from file","Edit XBE title","Launch XBE","View textfile","xbe info",NULL};
char *optionmenu[]={"Enable F: drive",
					"Enable G: drive",
					"Enable logfile writing", 
					"Enable ACL processing",
					"Enable RM (deletion) in ACL", 
					"Enable auto eject",
					"Enable LED control",
					"Enable network",
					"Modchip LCD",
					"Enable media change detection",
					"Enable ftp server",
					NULL};

char *optionmenu2[]={"Encoder",
					 "Ogg quality",
					 "MP3 mode",
					 "MP3 bitrate",
					 NULL};

char *ftpmenu[]={"",
				 "IP: ",
				 "User: ",
				 "Password:",
				 NULL};

using namespace std;


class CXBoxSample : public CXBApplicationEx
{
    //CXBFont     m_Font;             // Font object
	//CXBFont     m_Fontb;  
	//CXBFont     m_Font12; 
    //CXBHelp     m_Help;             // Help object
	//CXBHelp		m_BackGround;
	//CXBFont	    m_FontButtons;      // Xbox Button font
	MEMORYSTATUS memstat;
	int			mx;
	int			my;
	int			type;
	int			copytype;
	int			prevtype;
	int			iFreeSpace;

	DWORD		dwSTime;
	DWORD		dwcTime;
	DWORD		dwTime;
	DWORD		dwStartCopy;
	DWORD		dwEndCopy;
	WCHAR		driveState[100];
	WCHAR		*message[1024];
	WCHAR		localIP[32];
	int			mCounter;
	char		mDestPath[1024];
	char		mBrowse1path[1024];
	char		mBrowse2path[1024];
	char		mDestLog[1066];
	map<int,string> drives;
	map<int,string> ddirs;
	map<int,string> ddirsFS;
	map<int,string>	ftpatt;
	HDDBROWSEINFO	info;
	SWININFO		sinfo;
	CIoSupport		io;
	//HelperX*		mhelp;
	D2Xpatcher*		p_patch;
	D2Xgraphics*	p_graph;
	D2Xdbrowser*	p_browser;
	D2Xdbrowser*	p_browser2;
	D2Xfilecopy*	p_fcopy;
	D2Xtitle*		p_title;
	D2Xdstatus*		p_dstatus;
	D2Xswin*		p_swin;
	D2Xswin*		p_swinp;
	D2Xlogger*		p_log;
	D2Xacl*			p_acl;
	D2Xutils*		p_util;
	D2Xsettings*	p_set;
	D2Xviewer		p_view;
	D2Xinput		p_input;
	D2Xfile*		p_file;
	D2XGM*			p_gm;
	D2Xguiset		p_gset;
	D2Xgui*			p_gui;
	D2Xmedialib*	p_ml;
	CXBVirtualKeyboard* p_keyboard;
	int				dvdsize;
	int				freespace;
	Xcddb			m_cddb;	
	//bool			useF;
	//bool			useG;
	int				activebrowser;
	bool			b_help;
	bool			copy_retry;
	int				m_Caller;
	int				m_Return;
	int				settings_menu;
	//char			ftp_ip[40];
	//char			ftp_user[10];
	//char			ftp_pwd[10];
	DVD2XBOX_CFG	cfg;
	map<int,string> optionvalue;
	map<int,string> optionvalue2;
	typedef vector <string>::iterator iXBElist;
	map<int,HDDBROWSEINFO>::iterator iselected_item;
	CXBFileZilla*	m_pFileZilla;
	bool			ScreenSaverActive;
	bool			s_prio;
	CStdString		active_skin;

#if defined(_DEBUG)
	bool	showmem;
#endif


public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
	//bool CreateDirs(char *path);
	//void getDumpdirs();
	void WriteText(char* text);
	void mapDrives();
	void prepareACL(HDDBROWSEINFO path);
	void StartFTPd();
	void StopFTPd();
	void getlocalIP();
	bool AnyButtonDown();

    CXBoxSample();
};

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    CXBoxSample xbApp;
    if( FAILED( xbApp.Create() ) )
        return;
    xbApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CXBoxSample()
// Desc: Constructor for CXBoxSample class
//-----------------------------------------------------------------------------
CXBoxSample::CXBoxSample() 
            :CXBApplicationEx()
{

	//mhelp = new HelperX; 
	//p_patch = new D2Xpatcher;
	p_graph = new D2Xgraphics();
	//p_fcopy = new D2Xfilecopy;
	p_title = new D2Xtitle;
	p_dstatus = new D2Xdstatus;
	p_swin = new D2Xswin;
	p_swinp = new D2Xswin;
	p_log = new D2Xlogger;
	p_acl = new D2Xacl;
	p_util = new D2Xutils;
	p_keyboard = new CXBVirtualKeyboard();
	p_set = D2Xsettings::Instance();
	p_gui = D2Xgui::Instance();
	p_ml = new D2Xmedialib();
	strcpy(mBrowse1path,"e:\\");
	strcpy(mBrowse2path,"e:\\");
	//useF = false;
	//useG = false;
	message[0] = NULL;
	copy_retry = false;
	p_file = NULL;
	p_gm = NULL;
	m_pFileZilla = NULL;
	p_browser = NULL;
	p_browser2 = NULL;
	p_fcopy = NULL;
	//ini = 0;
	ScreenSaverActive = false;
	wcscpy(localIP,L"no network");

#if defined(_DEBUG)
	showmem = false;
#endif
	
}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{
	p_util->getHomePath(g_d2xSettings.HomePath);
	p_util->RemapHomeDir(g_d2xSettings.HomePath);

	// E: mapped at start to read the stored parameters
	io.Remap("E:,Harddisk0\\Partition1");
	p_set->ReadCFG(&cfg);

	if(p_gui->LoadSkin(g_d2xSettings.strskin)==0)
		return XBAPPERR_MEDIANOTFOUND;

	active_skin = g_d2xSettings.strskin;

	// Create a font
	//mpDebug = new CXBoxDebug(0, 0,40.0,80.0);
    /*if( FAILED( m_Font.Create( "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;*/
	
	/*if( FAILED( m_BackGround.Create( "background.xpr" ) ) )
	{
        return XBAPPERR_MEDIANOTFOUND;
	}*/

	/*if(!p_graph->LoadTextures())
		return XBAPPERR_MEDIANOTFOUND;*/

	/*if( FAILED( m_Fontb.Create( "debugfont.xpr" ) ) )
	{
		WriteText("Mediafile not found");
		Sleep(2000);
        return XBAPPERR_MEDIANOTFOUND;
	}*/

	/*if( FAILED( m_Font12.Create( "Font12.xpr" ) ) )
	{
		WriteText("Mediafile not found");
		Sleep(2000);
        return XBAPPERR_MEDIANOTFOUND;
	}*/

	// Xbox dingbats (buttons) 24
   /* if( FAILED( m_FontButtons.Create( "Xboxdings_24.xpr" ) ) )
	{
		WriteText("Mediafile not found");
		Sleep(2000);
        return XBAPPERR_MEDIANOTFOUND;
	}*/

	p_keyboard->Initialize();
	/*
	{
		WriteText("Mediafile not found");
		Sleep(2000);
		return XBAPPERR_MEDIANOTFOUND;
	}*/

	p_graph->RenderBackground();
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);

	// read config files
	WriteText("Loading configs");
	//p_set->ReadCFG(&cfg);
	p_gset.LoadConfig();
	 
	if(p_set->readXML("d:\\dvd2xbox.xml"))
	{
		//ini = 1;
		if(g_d2xSettings.autodetectHDD)
		{
			WriteText("Checking partitions");
			OutputDebugString("Checking for available partitions");
			/*cfg.EnableF = g_d2xSettings.useF = p_util->IsDrivePresent("F:\\");
			cfg.EnableG = g_d2xSettings.useG = p_util->IsDrivePresent("G:\\");*/
			p_gset.CheckingPartitions();
		}
		/*else
		{
			g_d2xSettings.useF = cfg.EnableF;
			g_d2xSettings.useG = cfg.EnableG;
		}*/
		p_set->getDumpDirs(ddirs);
	}
	else
	{
		m_Caller = 0;
		g_d2xSettings.generalError = NO_DVD2XBOX_XMLFILE;
		mCounter = 1000;
	}


	
//	D2Xfilecopy::f_ogg_quality = cfg.OggQuality;


 
	mCounter = 0;
	type = 0;
	prevtype = 0; 
	mx = 1;
	my = 1;
	activebrowser = 1;
	b_help = false;
	
	WriteText("Checking dvd drive status");

	if(!g_d2xSettings.detect_media)
		wcscpy(driveState,L"Press BACK to detect media");

    p_dstatus->GetDriveState(driveState,type);

	dwTime = dwSTime = timeGetTime();

	//if(cfg.EnableNetwork)
	if(g_d2xSettings.network_enabled)
	{
		WriteText("Starting network");
		if (!m_cddb.InitializeNetwork(g_d2xSettings.xboxIP,g_d2xSettings.netmask ,g_d2xSettings.gateway ))
		{
			WriteText("Starting network failed");
			D2Xtitle::i_network = 0;
		} else {
			D2Xtitle::i_network = 1;
			WriteText("Starting network ok"); 
			if(g_d2xSettings.ftpd_enabled)
			{
				WriteText("Starting ftp server");
				StartFTPd();
			}
			getlocalIP();
			
		}
		
	}

	// Remap the CDROM, map E & F Drives
	WriteText("Mapping drives");
	mapDrives();


	if(!XSetFileCacheSize(8388608))
		XSetFileCacheSize(4194304);

	p_log->setLogPath(g_d2xSettings.HomePath);

	if(g_d2xSettings.m_bLCDUsed == true)
	{
		CLCDFactory factory;
		g_lcd=factory.Create();
		g_lcd->Initialize();
	}

	ftpatt.insert(pair<int,string>(0,"Connect"));
	ftpatt.insert(pair<int,string>(1,g_d2xSettings.ftpIP));
	ftpatt.insert(pair<int,string>(2,g_d2xSettings.ftpuser));
	ftpatt.insert(pair<int,string>(3,g_d2xSettings.ftppwd));

	// set led to default color
	if(g_d2xSettings.enableLEDcontrol)
        ILED::CLEDControl(LED_COLOUR_GREEN);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	p_input.update(m_DefaultGamepad,m_DefaultIR_Remote);
	GlobalMemoryStatus( &memstat );

	switch(mCounter)
	{
		case 0:
			p_swin->initScrollWindow(mainmenu,10,false);
			mCounter = 11;
			break;
		case 11:
			sinfo = p_swin->processScrollWindow(m_DefaultGamepad);

			switch(sinfo.item_nr)
			{
			case 0:
				p_graph->SetIcon(BM_DUMP_TO_HDD);
				break;
			case 1:
				p_graph->SetIcon(BM_GAMEMANAGER);
				break;
			case 2:
				p_graph->SetIcon(BM_DUMP_TO_SMB);
				break;
			case 3:
				p_graph->SetIcon(BM_FILEMANAGER);
				break;
			case 4:
				p_graph->SetIcon(BM_SETTINGS);
				break;
			case 5:
				p_graph->SetIcon(BM_BOOT_TO_DASH);
				break;
			}

			//if(p_input.pressed(GP_START) || p_input.pressed(GP_A))
			if(p_input.pressed(GP_A))
			{
				if(!strcmp(sinfo.item,"Copy DVD/CD-R to HDD")) 
				{	
					io.CloseTray();
					io.Remount("D:","Cdrom0");
					ddirsFS.clear();

					// determine free disk space
					char temp[40];
		
					for(int a=0;a<ddirs.size();a++)
					{
						p_util->MakePath(ddirs[a].c_str());
						if(!(p_util->getfreeDiskspaceMB((char*)ddirs[a].c_str(),temp)))
							strcpy(temp, "");
						ddirsFS.insert(pair<int,string>(a,temp));
					}
					
					mCounter=1;
					p_swin->initScrollWindowSTR(10,ddirsFS);
					p_swinp->initScrollWindowSTR(10,ddirs);
				
				} 
				//if(p_input.pressed(GP_B))
				else if(!strcmp(sinfo.item,"Filemanager")) 
				{
					//
					io.CloseTray();
					io.Remount("D:","Cdrom0");
					strcpy(mBrowse1path,"e:\\");
					if(g_d2xSettings.useF)
						strcpy(mBrowse2path,"f:\\");
					else
						strcpy(mBrowse2path,"e:\\");

					mCounter=20;
					/*p_browser->resetDirBrowser();
					p_browser2->resetDirBrowser();*/
					if(p_browser != NULL)
						delete p_browser;
					if(p_browser2 != NULL)
						delete p_browser2;
					p_browser = new D2Xdbrowser();
					p_browser2 = new D2Xdbrowser();
				
				}
				
				//if(p_input.pressed(GP_WHITE))
				else if(!strcmp(sinfo.item,"Settings")) 
				{
					mCounter=1100;
				}

				else if(p_input.pressed(GP_BLACK))
				{
					io.CloseTray();
					io.Remount("D:","Cdrom0");
					strcpy(mDestPath,"d:\\default.xbe");
					mCounter=710;
					m_Caller=0;
					m_Return=0;
				
				}
				//if(p_input.pressed(GP_X) && cfg.EnableNetwork)
				else if(!strcmp(sinfo.item,"Copy DVD/CD-R to SMB share") && g_d2xSettings.network_enabled)
				{
					io.CloseTray();
					io.Remount("D:","Cdrom0");
					mCounter = 500;
		
				}
				//if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) 
				else if(p_input.pressed(GP_BACK)) 
				{
					g_d2xSettings.detect_media = 1;
				}

				//else if(p_input.pressed(GP_Y)) 
				else if(!strcmp(sinfo.item,"Game Manager"))
				{
					mCounter = 750;
				}
				else if(!strcmp(sinfo.item,"Boot to dash"))
				{
					if(g_d2xSettings.m_bLCDUsed == true)
					{
						g_lcd->SetLine(0,"");
						g_lcd->SetLine(1,"");
						g_lcd->SetLine(2,"");
						g_lcd->SetLine(3,"");
						Sleep(200);
						g_lcd->Stop();
						g_lcd->WaitForThreadExit(INFINITE);
						// we should exit all running threads here. later :-)
					}
					RebootToDash();
				}
			}


			if(p_input.pressed(GP_X))
			{
				//mCounter = 1100;
			}
			if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
			{
				if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
				{
					if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
					{
						strcpy(cfg.skin,"default");
						p_set->WriteCFG(&cfg);
						p_util->LaunchXbe(g_d2xSettings.HomePath,"d:\\default.xbe");
					}
				}
			}

			break;
		case 1:
			sinfo = p_swin->processScrollWindowSTR(m_DefaultGamepad);
			sinfo = p_swinp->processScrollWindowSTR(m_DefaultGamepad);
			if(p_input.pressed(GP_A) || p_input.pressed(GP_START))
			{
				strcpy(mDestPath,sinfo.item);
				p_util->addSlash(mDestPath);
				p_util->getfreeDiskspaceMB(mDestPath, freespace);
				dvdsize = p_dstatus->countMB("D:\\");
				strcat(mDestPath,p_title->GetNextPath(mDestPath,type));
				if(g_d2xSettings.generalError != 0)
				{
					m_Caller = 0;
					mCounter = 1000;
				} else
					mCounter = 3;
			
			}

			if(p_input.pressed(GP_BACK)) 
			{
				mCounter--;
			}

			break;
			
		case 2:
			wsprintf(mDestPath,"%S",p_keyboard->GetText());
			mCounter=3;
			break;
			
		case 3:
			

			if(p_input.pressed(GP_START)) 
			{
				if(GetFileAttributes(mDestPath) == -1)
				{
	
					p_util->addSlash(mDestPath);
					mCounter++;
				}
			} 
	
			else if(p_input.pressed(GP_X)) 
			{
				WCHAR wsFile[1024];
				swprintf(  wsFile,L"%S", mDestPath );
				p_keyboard->Reset();
				p_keyboard->SetText(wsFile);
				mCounter=70;
				m_Caller = 3;
				m_Return = 2;
			}
	
			else if(p_input.pressed(GP_BACK)) 
			{
				mCounter=1;
			} 
			break;
		case 4:

			if(g_d2xSettings.WriteLogfile)
			{
				p_log->enableLog(true);
			}

			dwStartCopy = timeGetTime();
			copytype = UNDEFINED;
			
			if(type==DVD)
			{	
				char dvdtitle[128];
				if(p_title->getDVDTitle(dvdtitle))
				{
					sprintf(mDestLog,"logs\\%s.txt",dvdtitle);
                    p_log->setLogFile(mDestLog);
				}
				else
					p_log->setLogFile("logs\\DVD.txt");
			

				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");

			} else if(type==CDDA)
			{
				p_log->setLogFile("logs\\CDDA.txt");
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:\\");
			
			} else if(type==ISO ||type==VCD ||type==SVCD)
			{
				p_log->setLogFile("logs\\ISO.txt");
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:\\");
				strcpy(info.name,"\0");
				
			
			} else //if(type==GAME)
			{
			
				WCHAR xbeTitle[44];
				char temptitle[45];
				if(p_title->getXBETitle("d:\\default.xbe",xbeTitle))
				{
					wsprintf(temptitle,"%S.txt",xbeTitle);	
					getFatxName(temptitle);
					sprintf(mDestLog,"logs\\%s",temptitle);
                    p_log->setLogFile(mDestLog);
				}
				else
					p_log->setLogFile("logs\\UDF.txt");
			
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				if(g_d2xSettings.enableACL && (type == GAME))
				{
					p_acl->processACL("d:\\",ACL_PREPROCESS); 
				}

			}
			
			// we start the copy process
			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_ORANGE);

			iFreeSpace = p_util->getfreeDiskspaceMB(mDestPath);

			p_fcopy = new D2Xfilecopy;
			p_fcopy->Create();
			p_fcopy->FileCopy(info,mDestPath,type);
			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter++;
			break;
		case 5:
			Sleep(250);
			if(D2Xfilecopy::b_finished) 
			{
				SetThreadPriorityBoost(GetCurrentThread(),false);
				s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				DebugOut("Main thread set priority to normal: %d\n",s_prio?1:0);
				
				copy_retry = false;
				if((D2Xfilecopy::copy_failed > 0) && (type != CDDA))
				{
					if(g_d2xSettings.enableLEDcontrol)
						ILED::CLEDControl(LED_COLOUR_RED);
					mCounter = 8;
				}
				else
					mCounter = 6;

				if(p_fcopy != NULL)
				{
					delete p_fcopy;
					p_fcopy = NULL;
				}
			}
			break;

		case 6:
			dwEndCopy = timeGetTime();

			if(g_d2xSettings.enableACL && (type == GAME) && (copytype == UNDEFINED))
			{
				p_acl->processACL(mDestPath,ACL_POSTPROCESS);
			} 

			if(type == GAME)
			{
				p_gm = new D2XGM();
				if(p_gm != NULL)
				{
					p_gm->AddGameToList(mDestPath);
					delete p_gm;
					p_gm = NULL;
				}
			}
			
			// we should clear the cached xbe files to prevent it from beeing used again
			D2Xfilecopy::XBElist.clear();
			
			if(g_d2xSettings.enableAutoeject)
                io.EjectTray();
			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_GREEN);

			p_log->enableLog(false);
			mCounter++; 
			break; 
		case 7:
			if(p_input.pressed(GP_START)) 
			{
				mCounter=0;
				copytype = UNDEFINED;
			}
			if(p_input.pressed(GP_Y) && g_d2xSettings.WriteLogfile)
			{
				if(GetFileAttributes(D2Xlogger::logFilename) != -1)
				{
					p_view.init(D2Xlogger::logFilename,20,65);
					mCounter = 600;
					m_Caller = 7;
				}
			}
			break;
		case 8:
			if(p_input.pressed(GP_X)) 
			{
				mCounter = 6;
			}
			if(p_input.pressed(GP_A))
			{
				copy_retry = true;
				io.CloseTray();
				io.Remount("D:","Cdrom0");
				
				if(g_d2xSettings.enableLEDcontrol)
					ILED::CLEDControl(LED_COLOUR_ORANGE);
	
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->CopyFailed(type);
				SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
				mCounter = 5;
			}
			break;
		case 10:
			mCounter++;
			break;
		case 20:
			if(!activebrowser)
				activebrowser = 1;
			info = p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			D2Xdbrowser::renewAll = true;
			info = p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			mCounter = 21;
			break;
		case 21:
		
			if(p_fcopy != NULL)
			{
				delete p_fcopy;
				p_fcopy = NULL;
			}

			if(!activebrowser)
			{
				activebrowser = 1;
				info = p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}

			if(D2Xdbrowser::renewAll == true)
			{
				p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
				D2Xdbrowser::renewAll = true;
				p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}
			if(p_browser->RenewStatus())
				p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			if(p_browser2->RenewStatus())
				p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);

			if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
				activebrowser = 1;
			if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
				activebrowser = 2;
			if(activebrowser == 1)
			{
				info = p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			} else {
				info = p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}
		
			
			if(info.button == BUTTON_LTRIGGER)
			{
				// Action menu
				p_swin->initScrollWindow(actionmenu,20,false);
				mCounter=25;
			}
			
			if(info.button == BUTTON_RTRIGGER)
			{
				p_swin->initScrollWindowSTR(20,drives);
				mCounter=50;
			}
			
			
		

			if(p_input.pressed(GP_BACK))
			{
				delete p_browser;
				delete p_browser2;
				p_browser = NULL;
				p_browser2 = NULL;
				mCounter=0;
			}
			break;
		case 22:
			if(p_input.pressed(GP_START))
			{
				g_d2xSettings.generalNotice = DELETING;
				mCounter = 23;
			}
			if(p_input.pressed(GP_BACK))
			{
				mCounter = 21;
			}
			break;
		case 23:
			// Delete file/directory
			
				if((activebrowser == 1) && !(p_browser->selected_item.empty()))
				{
					D2Xff factory;
										
					for(iselected_item = p_browser->selected_item.begin();
						iselected_item != p_browser->selected_item.end();
						iselected_item++)
					{
						if(p_file == NULL)
							p_file = factory.Create(iselected_item->second.item);

						if(iselected_item->second.type == BROWSE_DIR)
						{
							p_file->DeleteDirectory(iselected_item->second.item);

						} else if(iselected_item->second.type == BROWSE_FILE) 
						{
							p_file->DeleteFile(iselected_item->second.item);

						}
						p_browser->selected_item.erase(iselected_item);
					}
					delete p_file;
					p_file = NULL;
					p_browser->ResetCurrentDir();
				}
				else if((activebrowser == 2) && !(p_browser2->selected_item.empty()))
				{
					D2Xff factory;

					for(iselected_item = p_browser2->selected_item.begin();
						iselected_item != p_browser2->selected_item.end(); 
						iselected_item++)
					{
						if(p_file == NULL)
							p_file = factory.Create(iselected_item->second.item);

						if(iselected_item->second.type == BROWSE_DIR) 
						{
							p_file->DeleteDirectory(iselected_item->second.item);

						} else if(iselected_item->second.type == BROWSE_FILE) 
						{
							p_file->DeleteFile(iselected_item->second.item);

						}
						p_browser2->selected_item.erase(iselected_item);
					}
					delete p_file;
					p_file = NULL;
					p_browser2->ResetCurrentDir();
				}
				else
				{
					D2Xff factory;
					p_file = factory.Create(info.item);

					if(info.type == BROWSE_DIR)
					{
						p_file->DeleteDirectory(info.item);

					} else if(info.type == BROWSE_FILE) 
					{
						p_file->DeleteFile(info.item);
			
					}
					delete p_file;
					p_file = NULL;
				}
				g_d2xSettings.generalNotice = 0;
				mCounter = 21;
				p_browser->ResetCurrentDir();
				p_browser2->ResetCurrentDir();
		
			
			break;
		case 25:
			sinfo = p_swin->processScrollWindow(m_DefaultGamepad);
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input.pressed(GP_A))
			{ 
				if(!strcmp(sinfo.item,"Copy file/dir")) 
				{ 
					p_fcopy = new D2Xfilecopy;

					if((activebrowser == 1) && !(p_browser->selected_item.empty())) 
						mCounter = 65;
					else if((activebrowser == 2) && !(p_browser2->selected_item.empty()))
						mCounter = 65;
					else
                        mCounter = 60;
					
					m_Caller = 21;
				}
				else if(!strcmp(sinfo.item,"Delete file/dir"))
				{
					if(strcmp(info.name,"..")) 
						mCounter = 22;
				}
				/*else if(!strcmp(sinfo.item,"Patch Media check 1/2"))
				{
					mCounter = 40;
				}*/
				else if(!strcmp(sinfo.item,"Patch from file"))
				{
					p_patch = new D2Xpatcher;
					p_swin->initScrollWindow(p_patch->getPatchFiles(),20,false);
					mCounter = 45;
				}
				else if(!strcmp(sinfo.item,"Launch XBE"))
				{
					mCounter = 30;
				}
				else if(!strcmp(sinfo.item,"Rename file/dir"))
				{
					WCHAR wsFile[1024];
					//WCHAR title[50];
					char temp[1024];
					
					if(info.type == BROWSE_DIR)
					{
						swprintf(  wsFile,L"%S", info.item );
						strcpy(temp,info.item);
						p_util->addSlash(temp);
						strcat(temp,"default.xbe");
						if(_access(temp,00)!=-1)
						{
							p_title->getXBETitle(temp,wsFile);
							p_util->getFatxNameW(wsFile);
							if(wcslen(wsFile) <= 0)
								swprintf(  wsFile,L"%S", info.name );
						}
						else
							swprintf(  wsFile,L"%S", info.name );

					} else if(info.type == BROWSE_FILE)
					{						
						swprintf(  wsFile,L"%S", info.name );
					} 
					
					p_keyboard->Reset();
					p_keyboard->SetText(wsFile);
					mCounter = 70;
					m_Caller = 25;
					m_Return = 80;
				} else if(!strcmp(sinfo.item,"Create dir"))
				{
					WCHAR wsFile[1024];
					p_keyboard->Reset();
					//if(info.mode != FTP)
						//swprintf(  wsFile,L"%S",info.name);
					//else
					wcscpy(  wsFile,L"" );
					p_keyboard->SetText(wsFile);
					mCounter = 70;
					m_Caller = 25;
					m_Return = 90;
				} else if(!strcmp(sinfo.item,"Process ACL"))
				{
					mCounter = 100;
				}
				else if(!strcmp(sinfo.item,"Edit XBE title"))
				{
					WCHAR wsFile[1024];
					if(p_title->getXBETitle(info.item,wsFile))
					{
						p_keyboard->Reset();
						p_keyboard->SetText(wsFile);
						mCounter = 70;
						m_Caller = 25;
						m_Return = 95;
					}

				}
				else if(!strcmp(sinfo.item,"View textfile")) 
				{
					if(_strnicmp(info.item,"ftp:",4))
					{
						p_view.init(info.item,20,65);
						mCounter = 600;
						m_Caller = 21;
					}
					else
						mCounter = 21;

				}
				else if(!strcmp(sinfo.item,"xbe info")) 
				{
					if(strstr(info.item,".xbe") || strstr(info.item,".XBE"))
					{
						strcpy(mDestPath,info.item);
						mCounter = 710;
						m_Return = 21;
						m_Caller = 21;

					}
					else
					{
						mCounter = 21;
					}

				}
				
				/*if((info.mode == FTP) && strcmp(sinfo.item,"Create dir") &&
										 strcmp(sinfo.item,"Delete file/dir") &&
										 strcmp(sinfo.item,"Copy file/dir")) 
						mCounter = 21;*/
				
			}
			if(p_input.pressed(GP_BACK))
			{
				mCounter=21;
			}
			break;
		case 30:
			// launch xbe
			if(strstr(info.name,".xbe") || strstr(info.name,".XBE"))
			{
				if(p_input.pressed(GP_START))
				{
					char lxbe[50];
					sprintf(lxbe,"d:\\%s",info.name);
					p_util->LaunchXbe(info.path,lxbe);
				}
			} else 
				mCounter = 21;
			if(p_input.pressed(GP_BACK))
			{
				mCounter=21;
			}
			break;
		case 40:
			{
			/*char *reverse = new char[strlen(info.item)+1];
			strcpy(reverse,info.item);
			if((info.type == BROWSE_FILE) && !_strnicmp(_strrev(reverse),"ebx.",4))
			{
				ULONG mt;
				if(p_patch->SetMediatype(info.item,mt,"FF010040"))
				{
					wsprintfW(messagefix[0],L"Setting media type from 0x%08x to 0x400001FF",mt);
				} else {
					wsprintfW(messagefix[0],L"Error while setting media type");
				}
				int ret;
				for(int n=0;n<2;n++)
				{
                    ret = p_patch->PatchMediaStd(info.item,n);
					message[n] = new WCHAR[100];
					switch(ret)
					{
					case FOUND_OK:
						wsprintfW(message[n],L"%hs removed",D2Xpatcher::p_hexsearch[n]);
						break;
					case FOUND_ERROR:
						wsprintfW(message[n],L"%hs found but couldn't be removed",D2Xpatcher::p_hexsearch[n]);
						break;
					case NOT_FOUND:
						wsprintfW(message[n],L"%hs not found",D2Xpatcher::p_hexsearch[n]);
						break;
					default:
						break;
					}
				}
				message[n] = NULL;
				mCounter = 41;
			} else {
				mCounter = 21;
			}
			delete[] reverse;*/
			}
			break;
		case 41:
			if(p_input.pressed(GP_A))
			{
				
				/*int n=0;
				while(message[n]!=NULL)
				{
					delete[] message[n];
					n++;
				}
				mCounter = 21;*/
			
			}
			break;
		case 45:
			sinfo = p_swin->processScrollWindow(m_DefaultGamepad);
			if(p_input.pressed(GP_A) && strcmp(sinfo.item,"No files"))
			{
				int i=0;
				while(message[i] != NULL)
				{
					delete message[i];
					message[i] = NULL;
				}
				mCounter = 46;
			}
			if(p_input.pressed(GP_BACK))
			{
				delete p_patch;
				p_swin->initScrollWindow(actionmenu,20,false);
				mCounter=25;
			}
			break;
		case 46:
			p_patch->patchXBEfromFile(info,sinfo.item,message);
			mCounter = 47;
			break;
		case 47: 
			if(p_input.pressed(GP_A))
			{
				p_swin->initScrollWindow(p_patch->getPatchFiles(),20,false);
				mCounter = 45;
				int i=0;
				while(message[i] != NULL)
				{
					delete message[i];
					message[i] = NULL;
				}
				
			}
			break;
		case 50:
			sinfo = p_swin->processScrollWindowSTR(m_DefaultGamepad);
			if(p_input.pressed(GP_A) || p_input.pressed(GP_START))
			{
				if(strncmp(sinfo.item,"ftp:",4))
				{
					if(activebrowser == 1)
					{
						strcpy(mBrowse1path,sinfo.item);
						p_browser->ResetCurrentDir();
					}
					else 
					{
						strcpy(mBrowse2path,sinfo.item);
						p_browser2->ResetCurrentDir();
					}
				
					mCounter = 21;
				}
				else
				{
                    mCounter = 699;
				}
                	
			}
			//if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK) 
			if(p_input.pressed(GP_BACK))
			{
				mCounter=21;
			}
			break;
		case 60:
			p_fcopy->Create();
			if(activebrowser == 1)
			{
				p_fcopy->FileCopy(info,mBrowse2path,type);
			} 
			else 
			{
				p_fcopy->FileCopy(info,mBrowse1path,type);
			}
			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter = 61;
			break;
		case 61: 
			if(D2Xfilecopy::b_finished)
			{
				mCounter = 21;
				SetThreadPriorityBoost(GetCurrentThread(),false);
				s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				DebugOut("Main thread set priority to normal: %d\n",s_prio?1:0);
				if(activebrowser == 1)
				{
					p_browser2->ResetCurrentDir();
				} 
				else 
				{
					p_browser->ResetCurrentDir();
				}
			}
			break;
		case 65:
			p_fcopy->Create();
			if(activebrowser == 1)
			{
				iselected_item = p_browser->selected_item.begin();
				p_fcopy->FileCopy(iselected_item->second,mBrowse2path,type);
			} 
			else 
			{
				iselected_item = p_browser2->selected_item.begin();
				p_fcopy->FileCopy(iselected_item->second,mBrowse1path,type);
			}
			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter = 66; 
			break;
		case 66: 
			if(D2Xfilecopy::b_finished)
			{
				if(activebrowser == 1)
				{
					p_browser->selected_item.erase(iselected_item);
					if(p_browser->selected_item.empty())
						mCounter = m_Caller;
					else
						mCounter = 65;

					p_browser2->ResetCurrentDir();
				} 
				else 
				{
					p_browser2->selected_item.erase(iselected_item);
					if(p_browser2->selected_item.empty())
						mCounter = m_Caller;
					else
						mCounter = 65;

					p_browser->ResetCurrentDir();
				}
				SetThreadPriorityBoost(GetCurrentThread(),false);
				s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				DebugOut("Main thread set priority to normal: %d\n",s_prio?1:0);
			}
		case 70:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input.pressed(GP_A))
				p_keyboard->OnAction(ACTION_SELECT_ITEM);
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
				p_keyboard->OnAction(ACTION_MOVE_UP);
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
				p_keyboard->OnAction(ACTION_MOVE_DOWN);
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
				p_keyboard->OnAction(ACTION_MOVE_LEFT);
			else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
				p_keyboard->OnAction(ACTION_MOVE_RIGHT);
			else if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK))
				mCounter = m_Caller;

			if(p_input.pressed(GP_TL_LEFT) || p_input.pressed(GP_LTRIGGER))
				p_keyboard->OnAction(ACTION_MOVE_LEFT2);
			else if(p_input.pressed(GP_TL_RIGHT) || p_input.pressed(GP_RTRIGGER))
				p_keyboard->OnAction(ACTION_MOVE_RIGHT2);

			if(p_keyboard->IsConfirmed())
				mCounter = m_Return;
			break; 
		case 80:
			{
			char newitem[1024];
			wsprintf(newitem,"%hs%S",info.path,p_keyboard->GetText());
			
			D2Xff factory;
			p_file = factory.Create(newitem);
			//if(_access(newitem,00) == -1)
			{
				//MoveFileEx(info.item,newitem,MOVEFILE_COPY_ALLOWED);
				p_file->MoveItem(info.item,newitem);
			}
			delete p_file;
			p_file = NULL;
			
			mCounter = 21;
			D2Xdbrowser::renewAll = true;
			}
			break;
		case 90:
			{
			char newitem[1024];

			wsprintf(newitem,"%hs%S",info.path,p_keyboard->GetText());
			
			D2Xff factory;
			p_file = factory.Create(newitem);

			p_file->CreateDirectory(newitem);
			
			/*if(info.mode != FTP)
			{
				wsprintf(newitem,"%S",p_keyboard->GetText());
				DPf_H("UDF: newitem %s",newitem);
				p_util->MakePath(newitem);
			}
			else
			{
				wsprintf(newitem,"%s/%S",info.path,p_keyboard->GetText());
				DPf_H("FTP: newitem %s",newitem);
				D2Xftp	p_ftp;
				p_ftp.CreateDir(newitem);
			}*/

			delete p_file;
			p_file = NULL;

			mCounter = 21;
			D2Xdbrowser::renewAll = true;
			}
			break;
		case 95:
			p_util->writeTitleName(info.item,p_keyboard->GetText());
			mCounter = 21;
			break;

		// discbrowser: process ACL //
		case 100:
			mCounter = 105;
			{
				if((activebrowser == 1) && !(p_browser->selected_item.empty()))
				{
					for(iselected_item = p_browser->selected_item.begin();
						iselected_item != p_browser->selected_item.end();
						iselected_item++)
					{
						if(iselected_item->second.type == BROWSE_DIR)
						{
							prepareACL(iselected_item->second);
							mCounter = 21;
						} 
						p_browser->selected_item.erase(iselected_item);
					}
					p_browser->ResetCurrentDir();
				}
				else if((activebrowser == 2) && !(p_browser2->selected_item.empty()))
				{
					for(iselected_item = p_browser2->selected_item.begin();
						iselected_item != p_browser2->selected_item.end(); 
						iselected_item++)
					{
						if(iselected_item->second.type == BROWSE_DIR) 
						{
							prepareACL(iselected_item->second);
							mCounter = 21;
						} 
						p_browser2->selected_item.erase(iselected_item);
					}
					p_browser2->ResetCurrentDir();
				}
				else
				{
					if(info.type == BROWSE_DIR)
					{
						prepareACL(info);
						mCounter = 21;
					} 
				}

				p_browser->ResetCurrentDir();
				p_browser2->ResetCurrentDir();
			}
			break;
		/*case 100:
			mCounter = 105;
			if(info.type == BROWSE_DIR)
			{
				char		acl_dest[1024];
				strcpy(acl_dest,info.item);
				p_util->addSlash(acl_dest);
				strcat(acl_dest,"default.xbe");
				if(GetFileAttributes(acl_dest) != -1)
				{
					if(cfg.WriteLogfile)
					{
						sprintf(acl_dest,"logs\\%s.txt",info.name);
						p_log->setLogFile(acl_dest);
						p_log->enableLog(true);
						DPf_H("logfile: %s",acl_dest);
					}
					p_acl->processACL(info.item,ACL_POSTPROCESS);
					p_log->enableLog(false);
					D2Xdbrowser::renewAll = true;
					mCounter = 21;
				} 
			} 
			break;*/
		case 105:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input.pressed(GP_A))
				mCounter = 21;
			break;
		case 200:
			if(settings_menu == 0)
			{
				p_swin->initScrollWindow(optionmenu,20,false);
				p_swinp->initScrollWindowSTR(20);
			}
			else {
				p_swin->initScrollWindow(optionmenu2,20,false);
				p_swinp->initScrollWindowSTR(20);
			}
			mCounter = 201;
			break;
		case 500:
			dvdsize = 1;
			dwStartCopy = timeGetTime(); 
			char title[128];

			if(g_d2xSettings.WriteLogfile)
			{
				p_log->enableLog(true);
			}

			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_ORANGE);
			
			if(type==DVD)
			{	
				copytype = DVD2SMB;
				//dvdsize = mhelp->getusedDSul("D:\\");
				dvdsize = p_dstatus->countMB("D:\\");
				if((p_title->getDVDTitle(title)))
				{
					getFatxName(title);
					sprintf(mDestLog,"logs\\%s.txt",title);
                    p_log->setLogFile(mDestLog);
				} else
				{
					strcpy(title,"dvd2xbox");
					p_log->setLogFile("logs\\DVD.txt");
				}

				if(g_d2xSettings.generalError != 0)
				{
					m_Caller = 0;
					mCounter = 1000;
					break;
				} 
				getFatxName(title);
				//sprintf(mDestPath,"%s/%s/",g_d2xSettings.smbShare,title);
				sprintf(mDestPath,"smb:/%s",title);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);

			}
			else if(type==CDDA)
			{	
				m_Caller = 0;
				g_d2xSettings.generalError = SMBTYPE_NOT_SUPPORTED;
				mCounter = 1000;
				p_log->enableLog(false);
				break;
			}
			else if(type==ISO ||type==VCD ||type==SVCD)
			{
				copytype = ISO2SMB;
				p_log->setLogFile("logs\\ISO.txt");
				//sprintf(mDestPath,"%s/%s/",g_d2xSettings.smbShare,"dvd2xbox_iso");
				sprintf(mDestPath,"smb:/%s","dvd2xbox_iso");
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
			}
			else 
			{
				copytype = UDF2SMB;
				dvdsize = p_dstatus->countMB("D:\\");
				WCHAR game[50];
				if(p_title->getXBETitle("d:\\default.xbe",game))
				{
					sprintf(title,"%S",game);
					getFatxName(title);
					sprintf(mDestLog,"logs\\%s.txt",title);
                    p_log->setLogFile(mDestLog);
				} else
				{
					p_log->setLogFile("logs\\UDF.txt");
					strcpy(title,"UDF.txt");
				}


				//sprintf(mDestPath,"%s/%s/",g_d2xSettings.smbShare,title);
				sprintf(mDestPath,"smb:/%s",title);

				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
				

			}
			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter = 5;
			break;
		case 600:
			p_view.process(m_DefaultGamepad);
			//if(mhelp->pressBACK(m_DefaultGamepad))
			if(p_input.pressed(GP_BACK))
                mCounter = m_Caller;
			break;
		case 699:
			p_swinp->initScrollWindow(ftpmenu,4,false);
			p_swin->initScrollWindowSTR(4,ftpatt);
			mCounter = 700;
			break;
		case 700:
			sinfo = p_swinp->processScrollWindow(m_DefaultGamepad);
			sinfo = p_swin->processScrollWindowSTR(m_DefaultGamepad);
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input.pressed(GP_A))
			{
				if(sinfo.item_nr == 0)
				{
					if(activebrowser == 1)
					{
						strcpy(mBrowse1path,"ftp:/");
						p_browser->resetDirBrowser();
					}
					else 
					{
						strcpy(mBrowse2path,"ftp:/");
						p_browser2->resetDirBrowser();
					}
					mCounter = 21;
					m_Caller = 699;
					g_d2xSettings.generalNotice = FTP_CONNECT;
				}
				else
				{
					WCHAR wsFile[1024];
					p_keyboard->Reset();
					swprintf(  wsFile,L"%S", ftpatt[sinfo.item_nr].c_str() );
					p_keyboard->SetText(wsFile);
					mCounter = 70;
					m_Caller = 699;
					m_Return = 701;
				}
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter = 21;
			}
			break;
		case 701:
			char temp[128];
			wsprintf(temp,"%S",p_keyboard->GetText());
			ftpatt[sinfo.item_nr] = string(temp);
			switch(sinfo.item_nr)
			{
				case 1:
					strcpy(cfg.ftpIP,temp);
					strcpy(g_d2xSettings.ftpIP, cfg.ftpIP);
					break;
				case 2:
					strcpy(cfg.ftpuser,temp);
					strcpy(g_d2xSettings.ftpuser, cfg.ftpuser);
					break;
				case 3:
					strcpy(cfg.ftppwd,temp);
					strcpy(g_d2xSettings.ftppwd, cfg.ftppwd);
					break;
			}
															
			p_set->WriteCFG(&cfg);
			mCounter = 699;
			break;
		case 710:
			if(p_util->getXBECert(mDestPath))
			{
				mCounter++;
			}
			else
			{
				mCounter = m_Caller;
			}
			break;
		case 711:
			if(p_input.pressed(GP_A) || p_input.pressed(GP_BACK)) 
			{
				mCounter = m_Return;
			}
			break;

			// game manager
		case 750:
			if(p_gm != NULL)
			{
				delete p_gm;
				p_gm = NULL;
			}
			p_gm = new D2XGM();
			if(GetFileAttributes(g_d2xSettings.disk_statsPath) == -1)
			{
				g_d2xSettings.generalNotice = SCANNING;
				mCounter = 755;
			}
			else
			{
				mCounter = 760;
				if(p_gm->PrepareList() == 0)
				{
					p_gm->DeleteStats();
					mCounter = 755;
				}
			}
			break;
		case 755:
				p_gm->ScanDisk();
				g_d2xSettings.generalNotice = 0;
				p_gm->PrepareList();
				mCounter = 760;
			break;
		case 760:
			{
				int ret;
				ret = p_gm->ProcessGameManager(m_DefaultGamepad);
				if(ret == PROCESS_BACK)
				{
					delete p_gm;
					p_gm = NULL;
					mCounter = 0;
				}
				else if(ret == PROCESS_RESCAN)
				{
					delete p_gm;
					p_gm = NULL;
					mCounter = 750;
				}
			}
			break;
		case 1000:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input.pressed(GP_A))
			{
				g_d2xSettings.generalError = 0;
				mCounter = m_Caller;
			}
			break;
		case 1010:
			if(p_input.pressed(GP_BACK))
			{
				g_d2xSettings.generalNotice = 0;
				mCounter = m_Caller;
			}
			if(p_input.pressed(GP_A))
			{
				p_util->LaunchXbe(g_d2xSettings.HomePath,"d:\\default.xbe");
			}
			break;
		case 1100:
			switch(p_gset.Process(m_DefaultGamepad))
			{
			case D2X_GUI_BACK:
				mCounter = 0;
				break;
			case D2X_GUI_MAPDRIVES:
				mapDrives();
				p_set->getDumpDirs(ddirs);
				break;
			case D2X_GUI_START_NET:
		
				if (!m_cddb.InitializeNetwork(g_d2xSettings.xboxIP,g_d2xSettings.netmask ,g_d2xSettings.gateway ))
				{
					D2Xtitle::i_network = 0;
				} else
				{
					D2Xtitle::i_network = 1;
					getlocalIP();
				}
		
				mapDrives();
				break;
			case D2X_GUI_STOP_NET:
				m_pFileZilla->Stop();
				WSACleanup();
				D2Xtitle::i_network = 0;
				wcscpy(localIP,L"no network");
				mapDrives();
				break;
			case D2X_GUI_START_FTPD:
				StartFTPd();
				break;
			case D2X_GUI_STOP_FTPD:
				m_pFileZilla->Stop();
				break;
			case D2X_GUI_START_LCD:
				{
					CLCDFactory factory;
					g_lcd=factory.Create();
					g_lcd->Initialize();
				}
				break;
			case D2X_GUI_RESTART_LCD:
				{
					g_lcd->Stop();
					CLCDFactory factory;
					g_lcd=factory.Create();
					g_lcd->Initialize();
				}
				break;
			case D2X_GUI_STOP_LCD:
				{
					g_lcd->Stop();
				}
				break;
			case D2X_GUI_SET_LCD:
				g_lcd->SetBackLight(g_d2xSettings.m_iLCDBackLight);
				g_lcd->SetContrast(g_d2xSettings.m_iContrast);
				break;
			case D2X_GUI_SAVE_SKIN:
				strcpy(cfg.skin,g_d2xSettings.strskin);
				p_set->WriteCFG(&cfg);
				break;
			case D2X_GUI_RESTART:
				strcpy(cfg.skin,g_d2xSettings.strskin);
				p_set->WriteCFG(&cfg);
				p_util->LaunchXbe(g_d2xSettings.HomePath,"d:\\default.xbe");
				break;
			default:
				break;
			};
			break;
		default:
			break;
		
	}


	if(g_d2xSettings.generalError)
		mCounter = 1000;
	
    dwcTime = timeGetTime();
#if defined(_DEBUG)
	if(showmem && ((dwcTime-dwTime) >= 1000)) 
	{
		GlobalMemoryStatus( &memstat );
	}
#endif
	if((dwcTime-dwTime) >= 2000)
	{
		dwTime = dwcTime;
		if((mCounter<4 || mCounter == 21 || mCounter == 7) || (mCounter == 11))
		{
			p_dstatus->GetDriveState(driveState,type);
			if((type != prevtype) && (type != 0) )
			{
				D2Xdbrowser::renewAll = true;
				if(p_util->isdriveD(mBrowse1path))
					p_browser->resetDirBrowser();
				else if(p_util->isdriveD(mBrowse2path)) 
					p_browser2->resetDirBrowser();
				prevtype = type;
			}
		}
	}
	if(g_d2xSettings.ScreenSaver != 0)
	{
		if(AnyButtonDown() == true)
		{
			dwSTime = dwcTime;
			ScreenSaverActive = false;
		}
		if((dwcTime-dwSTime) >= g_d2xSettings.ScreenSaver*60000 )
			ScreenSaverActive = true;
	}
    
    return S_OK; 
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{

	//p_graph->RenderBackground();
	
	// clear screen
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.5f, 1.0f );

	CStdString	strlcd1="";
	CStdString	strlcd2="";
	CStdString	strlcd3="";
	CStdString	strlcd4="";

	CStdString mem;
	mem.Format("%d kB",memstat.dwAvailPhys/(1024));
	p_gui->SetKeyValue("freememory",mem);

	if(mCounter == 11)
	{
		p_gui->SetKeyValue("version","0.6.8");
		p_gui->SetKeyValue("localip",localIP);
		p_gui->SetKeyValue("statusline",driveState);
		p_gui->SetWindowObject(1,p_swin);
		p_gui->RenderGUI(GUI_MAINMENU);

		if(g_d2xSettings.network_enabled)
			strlcd2.Format("IP: %S - Press A to proceed",localIP);
		else
			strlcd2 = "Press A to proceed";

		strlcd1 = "Welcome to dvd2xbox";
		strlcd3 = driveState;
		SYSTEMTIME	sltime;
		GetLocalTime(&sltime);
		strlcd4.Format("Time: %2.2d:%2.2d:%2.2d",sltime.wHour,sltime.wMinute,sltime.wSecond);

	}	
	/*else if(mCounter==11)
	{
		p_graph->RenderMainMenuIcons();
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Welcome to DVD2Xbox 0.6.7" );
		if(g_d2xSettings.network_enabled)
		{
			m_Fontb.DrawText(80,70, 0xffffffff,L"IP: ");
            m_Fontb.DrawText(110,70, 0xffffffff,localIP);

			WCHAR wlocalIP[20];
			wsprintfW(wlocalIP,L"IP:   %s",localIP);
			strlcd3 = wlocalIP;
		}

		p_swin->showMainScrollWindow(80,160,100,COLOUR_WHITE,COLOUR_WHITE,m_Font);

		m_Font.DrawText( 60, 435, COLOUR_WHITE, driveState ); 

		strlcd1 = "Welcome to dvd2xbox";
		strlcd2 = "Press A to proceed";
		strlcd3 = driveState;
		SYSTEMTIME	sltime;
		GetLocalTime(&sltime);
		strlcd4.Format("Time: %2.2d:%2.2d:%2.2d",sltime.wHour,sltime.wMinute,sltime.wSecond);

	}*/
	else if(mCounter==1)
	{
		/*p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Choose dump directory:" );
		p_swin->showScrollWindowSTR(60,120,100,0xffffffff,0xffffff00,m_Font);
		p_swinp->showScrollWindowSTR(240,120,100,0xffffffff,0xffffff00,m_Font);
		m_Font.DrawText( 60, 435, 0xffffffff, driveState );*/

		p_gui->SetShowIDs(10);
		p_gui->SetWindowObject(1,p_swin);
		p_gui->SetWindowObject(2,p_swinp);

		p_gui->SetKeyValue("statusline",driveState);
		p_gui->RenderGUI(GUI_DISKCOPY);

		strlcd1 = "Choose destination:";
		strlcd3 = sinfo.item;
	}
	else if(mCounter==3)
	{
		CStdString text;
		p_gui->SetShowIDs(20);

		/*WCHAR temp[60];
		WCHAR temp2[1024];
		p_graph->RenderMainFrames();
		int i=0;
		m_Font.DrawText( 80, 30, 0xffffffff, L"Destination path:" );*/
		if((dvdsize != 0) && (dvdsize > freespace))
		{
			p_gui->SetShowIDs(21);
			text.Format("%d",dvdsize);
			p_gui->SetKeyValue("dvdsize",text);
			text.Format("%d",freespace);
			p_gui->SetKeyValue("freespace",text);
			/*m_Font.DrawText( 60, 140, 0xffffffff, L"Warning:" );
			wsprintfW(temp,L"DVD size: %d MB > free space: %d MB",dvdsize,freespace);
			m_Font.DrawText( 60, 170, 0xffffffff, temp );*/
			strlcd1 = "DVDsize > freespace";
		} else if(GetFileAttributes(mDestPath) != -1)
		{
			p_gui->SetShowIDs(22);
			/*m_Font.DrawText( 60, 140, 0xffffffff, L"Warning:" );
			m_Font.DrawText( 60, 170, 0xffffffff, L"The path you specified already exists." );*/
			strlcd1 = "path already exists";
		}
		else
			strlcd1 = mDestPath;

		p_gui->SetKeyValue("destination",mDestPath);
		p_gui->RenderGUI(GUI_DISKCOPY);

		/*wsprintfW(temp2,L"%hs",mDestPath);
		m_Fontb.DrawText( 60, 210, 0xffffffff, temp2 );
		m_FontButtons.DrawText( 60, 260, 0xffffffff, L"G");
		m_Font.DrawText( 110, 260, 0xffffffff, L" proceed" );
		m_FontButtons.DrawText( 60, 300, 0xffffffff, L"C");
		m_Font.DrawText( 110, 300, 0xffffffff, L"  change dir" );
		m_FontButtons.DrawText( 60, 340, 0xffffffff, L"H");
		m_Font.DrawText( 110, 340, 0xffffffff, L"  choose drive again" );*/

		strlcd3 = "START to proceed";
		strlcd4 = "BACK to choose again";
	}
	else if(mCounter==4 || mCounter==5)
	{
		/*WCHAR dest[70];
		WCHAR remain[50];
		WCHAR free[50];*/
		CStdString text;
		p_gui->SetShowIDs(30);
		p_gui->SetKeyValue("destfile",D2Xfilecopy::c_dest);
		p_gui->SetKeyValue("sourcefile",D2Xfilecopy::c_source);
		p_gui->SetKeyInt("fileprogress",p_fcopy->GetProgress());

		/*if(wcslen(D2Xfilecopy::c_dest) > 66)
		{
			wcsncpy(dest,D2Xfilecopy::c_dest,66);
			dest[66] = L'\0';
		} else {
			wcscpy(dest,D2Xfilecopy::c_dest);
		}*/
		/*p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Main copy module" );
		p_graph->RenderPopup();
		m_Font.DrawText(55, 160, 0xffffffff, L"Copy:" );
		m_Fontb.DrawText(55, 205, 0xffffffff, D2Xfilecopy::c_source);
		m_Fontb.DrawText(55, 220, 0xffffffff, dest);
		p_graph->RenderProgressBar(240,float(p_fcopy->GetProgress()));*/

		strlcd1 = "Copy in progress \4"; 
		strlcd2 = D2Xfilecopy::c_source;

		if((type == DVD || type == GAME || type == UDF) && !copy_retry)
		{
			p_gui->SetShowIDs(31);
			text.Format("%d",dvdsize-p_fcopy->GetMBytes());
			p_gui->SetKeyValue("remainingbytes",text);
			p_gui->SetKeyInt("allprogress",(p_fcopy->GetMBytes()*100)/dvdsize);
			

			/*p_graph->RenderProgressBar(265,float(((p_fcopy->GetMBytes())*100)/dvdsize));
			wsprintfW(remain,L"Remaining MBytes to copy:  %6d MB",dvdsize-p_fcopy->GetMBytes());
			m_Fontb.DrawText( 60, 320, 0xffffffff, remain);*/
			
			strlcd3.Format("%6d MB to do",dvdsize-p_fcopy->GetMBytes());
		
		}
		if((copytype == UNDEFINED) && (type != CDDA) )
		{
			p_gui->SetShowIDs(32);
			text.Format("%d",iFreeSpace-p_fcopy->GetMBytes());
			p_gui->SetKeyValue("freespace",text);

			/*wsprintfW(free,L"Remaining free space:      %6d MB",iFreeSpace-p_fcopy->GetMBytes());
			m_Fontb.DrawText( 60, 350, 0xffffffff, free );*/
			
			strlcd4.Format("%6d MB free",iFreeSpace-p_fcopy->GetMBytes());
		
		}
		p_gui->RenderGUI(GUI_DISKCOPY);
	}
	else if(mCounter == 6 ||mCounter == 8)
	{
		CStdString text;
		p_gui->SetShowIDs(40);


		/*p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Main copy module" );
		p_graph->RenderPopup();*/
		if(mCounter == 8)
		{
			/*WCHAR temp[56];
			p_graph->RenderPopup();
			wsprintfW(temp,L"%d file(s) failed to copy.",D2Xfilecopy::copy_failed);
			m_Font.DrawText(55, 160, 0xffff0000, temp );
			m_Font.DrawText(55, 190, 0xffffffff, L"You may want to clean the DVD and try again." );
			m_Font.DrawText(55, 240, 0xffffffff, L"X to cancel - A to retry" );*/

			p_gui->SetShowIDs(41);
			text.Format("%d",D2Xfilecopy::copy_failed);
			p_gui->SetKeyValue("failed",text);

			strlcd1.Format("Failed: %6d",D2Xfilecopy::copy_failed);
			strlcd3 = "X to cancel";
			strlcd4 = "A to retry";

		}
		else if(g_d2xSettings.enableACL)
		{
			/*m_Font.DrawText(55, 160, 0xffffffff, L"Processing ACL ..." );*/
		
			p_gui->SetShowIDs(42);

			strlcd1 = "Processing ACL ...";
			strlcd3 = " \4";
		}
		else
		{
            /*m_Font.DrawText(55, 160, 0xffffffff, L"Patching xbe's ..." );*/

			p_gui->SetShowIDs(43);

			strlcd1 = "Patching xbe's ...";
			strlcd3 = " \4";
		}
		p_gui->RenderGUI(GUI_DISKCOPY);
	}
	else if(mCounter == 7)
	{
		CStdString text;
		p_gui->SetShowIDs(50);
		text.Format("%d",D2Xfilecopy::copy_ok);
		p_gui->SetKeyValue("copyok",text);
		text.Format("%d",D2Xfilecopy::copy_failed);
		p_gui->SetKeyValue("copyfailed",text);
		text.Format("%d",D2Xfilecopy::copy_renamed);
		p_gui->SetKeyValue("copyrenamed",text);


		/*WCHAR copy[50];
		WCHAR renamed[50];
		WCHAR failed[50];
		WCHAR mcrem1[70];
		WCHAR duration[50];
		wsprintfW(copy,    L"Files copied:   %6d",D2Xfilecopy::copy_ok);
		wsprintfW(failed,  L"Failed to copy: %6d",D2Xfilecopy::copy_failed);
		wsprintfW(renamed, L"Files renamed:  %6d",D2Xfilecopy::copy_renamed);*/
		int hh = (dwEndCopy - dwStartCopy)/3600000;
		int mm = (dwEndCopy - dwStartCopy - hh*3600000)/60000;
		int ss = (dwEndCopy - dwStartCopy - hh*3600000 - mm*60000)/1000;

		text.Format("%2d:%02d:%02d",hh,mm,ss);
		p_gui->SetKeyValue("duration",text);
		/*wsprintfW(duration,L"Copy duration (HH:MM:SS): %2d:%02d:%02d",hh,mm,ss);*/

			
		/*p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Copy report:" );
		m_Fontb.DrawText( 60, 140, 0xffffffff, copy );
		m_Fontb.DrawText( 60, 170, 0xffffffff, failed );
		m_Fontb.DrawText( 60, 200, 0xffffffff, renamed );
		m_Fontb.DrawText( 60, 230, 0xffffffff, duration );*/
		
		
		if((type == GAME) && g_d2xSettings.WriteLogfile && g_d2xSettings.enableACL && (copytype != UDF2SMB))
		{
			p_gui->SetShowIDs(51);
			/*wsprintfW(mcrem1,L"ACL processed. Read the logfile (press Y) for more information.");
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );*/
		}
		else if((type == GAME) && !g_d2xSettings.WriteLogfile && g_d2xSettings.enableACL && (copytype != UDF2SMB))
		{
			p_gui->SetShowIDs(52);
			/*wsprintfW(mcrem1,L"ACL processed. Enable logfile writing for more information.");
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );*/
		}
		else if((copytype == UDF2SMB) || (copytype == DVD2SMB) || (copytype == ISO2SMB))
		{
			p_gui->SetShowIDs(53);
			/*wsprintfW(mcrem1,L"ACL processing and media check patching is not supported via smb.");
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );*/
		}
		

		strlcd1.Format("Copied: %6d",D2Xfilecopy::copy_ok);
		strlcd2.Format("Failed: %6d",D2Xfilecopy::copy_failed);
		strlcd3.Format("Renamed:%6d",D2Xfilecopy::copy_renamed);
		strlcd4.Format("Duration: %2d:%02d:%02d --- press START to proceed --- ",hh,mm,ss);
		
		/*m_Font.DrawText( 60, 435, 0xffffffff, L"press START to proceed" );*/
		
		p_gui->RenderGUI(GUI_DISKCOPY);
	}
	
	else if(mCounter==21 || mCounter==22 || mCounter == 23 || mCounter == 25 || mCounter == 30 || mCounter == 50 || mCounter == 61 || mCounter == 66 || mCounter == 45 || mCounter == 46 || mCounter == 47 || mCounter == 100 || mCounter == 105 || mCounter == 700 || mCounter == 711)
	{
		//p_graph->RenderBrowserFrames(activebrowser);
		
		/*WCHAR temp[1024];
		WCHAR temp2[30];*/
		CStdString	str_temp;
		
		if(activebrowser == 1)
			p_gui->SetShowIDs(11);
		else
			p_gui->SetShowIDs(22);
		//wsprintfW(temp,L"%hs",info.item);
		p_gui->SetKeyValue("selectedfilename",info.item);
		//wsprintfW(temp2,L"Filesize: %d KB",info.size);
		//m_Fontb.DrawText( 55, 30, 0xffffffff, temp );
		if((info.type != BROWSE_DIR) && ((p_util->isdriveD(info.item) && ((type == DVD) || type == GAME)) || !p_util->isdriveD(info.item)))
		{
			//m_Fontb.DrawText( 55, 45, 0xffffffff, temp2 );
			str_temp.Format("%d",info.size);
			p_gui->SetKeyValue("selectedfilesize",str_temp);
		}
		else
			p_gui->SetKeyValue("selectedfilesize","#");

		//m_Fontb.DrawText( 55, 60, 0xffffffff, info.title);
		p_gui->SetKeyValue("selectedfiletitle",info.title);
		p_gui->SetShowIDs(1);
		p_gui->SetShowIDs(2);
		p_gui->SetBrowserObject(0, p_browser);
		p_gui->SetBrowserObject(1, p_browser2);
		p_gui->SetKeyValue("statusline",driveState);
		//p_browser->showDirBrowser(20,55,95,0xffffffff,0xff000000,m_Fontb);
		//p_browser2->showDirBrowser(20,330,95,0xffffffff,0xff000000,m_Fontb);
		//m_Font.DrawText( 60, 435, 0xffffffff, driveState );
		strlcd1 = "Filemanager";

		if(mCounter == 50)
		{

			//p_graph->RenderBrowserPopup(activebrowser);
			p_gui->SetWindowObject(1,p_swin);
			if(activebrowser == 1)
			{
				p_gui->SetShowIDs(102);
				/*m_Font.DrawText(330, 100, 0xffffffff, L"Choose drive:" );
                p_swin->showScrollWindowSTR(340,125,10,0xffffffff,0xffffff00,m_Font);*/
			}
			if(activebrowser == 2)
			{
				p_gui->SetShowIDs(202);
				/*m_Font.DrawText(55, 100, 0xffffffff, L"Choose drive:" );
                p_swin->showScrollWindowSTR(65,125,10,0xffffffff,0xffffff00,m_Font);*/
			}
		}
		if(mCounter == 61 || mCounter == 66)
		{
			p_gui->SetShowIDs(300);
			p_gui->SetKeyInt("fileprogress",p_fcopy->GetProgress());
			p_gui->SetKeyValue("sourcefile",D2Xfilecopy::c_source);
			p_gui->SetKeyValue("destfile",D2Xfilecopy::c_dest);
			/*p_graph->RenderPopup();
			WCHAR dest[70];
			WCHAR source[70];
			if(wcslen(D2Xfilecopy::c_dest) > 66)
			{
				wcsncpy(dest,D2Xfilecopy::c_dest,66);
				dest[66] = L'\0';
			} else {
				wcscpy(dest,D2Xfilecopy::c_dest);
			}
			if(wcslen(D2Xfilecopy::c_source) > 66)
			{
				wcsncpy(source,D2Xfilecopy::c_source,66);
				source[66] = L'\0';
			} else {
				wcscpy(source,D2Xfilecopy::c_source);
			}
			m_Font.DrawText(55, 160, 0xffffffff, L"Copy:" );
			m_Fontb.DrawText(55, 205, 0xffffffff, source);
			m_Fontb.DrawText(55, 220, 0xffffffff, dest);
			p_graph->RenderProgressBar(240,float(p_fcopy->GetProgress()));*/
		}
		if(mCounter == 25 || mCounter == 45)
		{
			p_gui->SetWindowObject(1,p_swin);
			if(activebrowser == 1)
				p_gui->SetShowIDs(104);
			if(activebrowser == 2)
				p_gui->SetShowIDs(204);
			/*p_graph->RenderBrowserPopup(activebrowser);
			if(activebrowser == 1)
                p_swin->showScrollWindow(330,100,32,0xffffffff,0xffffff00,m_Fontb);
			if(activebrowser == 2)
                p_swin->showScrollWindow(55,100,32,0xffffffff,0xffffff00,m_Fontb);*/

		}
		if(mCounter == 100)
		{
			p_gui->SetShowIDs(400);
			/*p_graph->RenderPopup();
			m_Font.DrawText(55, 160, 0xffffffff, L"Processing ACL ..." );*/
		}
		if(mCounter == 105)
		{
			p_gui->SetShowIDs(500);
			/*p_graph->RenderPopup();
			m_Font.DrawText(55, 160, 0xffffffff, L"Failed to process ACL list");
			m_Font.DrawText(55, 200, 0xffffffff, L"Tried on file or dir without default.xbe ?");*/
		}
		if(mCounter == 700)
		{
			p_gui->SetShowIDs(600);
			p_gui->SetWindowObject(1,p_swinp);
			p_gui->SetWindowObject(2,p_swin);
			/*p_graph->RenderPopup();
			p_swinp->showScrollWindow(180,160,20,0xffff0000,0xffff0000,m_Font);
			p_swin->showScrollWindowSTR(340,160,20,0xffffffff,0xffffff00,m_Font);*/
		}
		if(mCounter==22 || mCounter == 23)
		{
			p_gui->SetShowIDs(700);
			if((activebrowser == 1) && !(p_browser->selected_item.empty()))
				p_gui->SetKeyValue("todelete","selected items in left window");
			else if((activebrowser == 2) && !(p_browser2->selected_item.empty()))
				p_gui->SetKeyValue("todelete","selected items in right window");
			else
				p_gui->SetKeyValue("todelete",info.item);
		}
		if(mCounter==30)
		{
			CStdString	text;
			text.Format("%s%s",info.path,info.name);
			p_gui->SetShowIDs(710);
			p_gui->SetKeyValue("tolaunch",text);
		}
		if(mCounter == 46)
		{
			p_gui->SetShowIDs(720);
		}
		if(mCounter == 47)
		{
			p_gui->SetShowIDs(730);
			CStdString	text;
			text.Format("used patch file: %s",sinfo.item);
			int i=0;
			while(message[i] != NULL)
			{
				text += CStdString("\\n") + CStdString(message[i]);
				i++;
			}
			p_gui->SetKeyValue("patchtext",text);
		}
		if(mCounter == 711)
		{
			CStdString	text,temp_text;
			p_gui->SetShowIDs(740);
			p_gui->SetKeyValue("titlename",p_util->xbecert.TitleName);
			text.Format("%08X",p_util->xbecert.TitleId);
			p_gui->SetKeyValue("titleid",text);
			text.Format("%08X",p_util->xbecert.Timestamp);
			p_gui->SetKeyValue("timestamp",text);
			text.Format("%08X",p_util->xbecert.MediaTypes);
			p_gui->SetKeyValue("mediatypes",text);
			text.Format("%08X",p_util->xbecert.GameRegion);
			p_gui->SetKeyValue("gameregion",text);
			text.Format("%08X",p_util->xbecert.GameRating);
			p_gui->SetKeyValue("gamerating",text);
			text.Format("%08X",p_util->xbecert.DiskNumber);
			p_gui->SetKeyValue("disknumber",text);
			text.Format("%08X",p_util->xbecert.Version);
			p_gui->SetKeyValue("version",text);
			
			text.clear();
			for(int i=0;i<=15;i++)
			{
				temp_text.Format("%02X",p_util->xbecert.LanKey[i]);
				text += temp_text;
			}
			p_gui->SetKeyValue("lankey",text);

			text.clear();
			for(int i=0;i<=15;i++)
			{
				temp_text.Format("%02X",p_util->xbecert.SignatureKey[i]);
				text += temp_text;
			}
			p_gui->SetKeyValue("signkey",text);

		}
		CStdString	text;
		switch(g_d2xSettings.generalNotice)
		{
		case FTP_CONNECT:
			text.Format("ftp://%hs",g_d2xSettings.ftpIP);
			p_gui->SetKeyValue("ftpip",text);
			p_gui->SetShowIDs(610);
			break;
		case DELETING:
			p_gui->SetShowIDs(702);
			break;
		default:
			break;
		}
		

		p_gui->RenderGUI(GUI_FILEMANAGER);
	}
	/*else if(mCounter==22 || mCounter == 23)
	{
		p_graph->RenderMainFrames();
		WCHAR temp[1024];
		wsprintfW(temp,L"%hs",info.item);
		m_Font.DrawText( 80, 30, 0xffffffff, L"Please confirm deletion of:" );
		if((activebrowser == 1) && !(p_browser->selected_item.empty()))
			m_Font.DrawText( 60, 140, 0xffffffff, L"selected items in left window" );
		else if((activebrowser == 2) && !(p_browser2->selected_item.empty()))
			m_Font.DrawText( 60, 140, 0xffffffff, L"selected items in right window" );
		else
			m_Font.DrawText( 60, 140, 0xffffffff, temp );
		m_FontButtons.DrawText( 80, 200, 0xffffffff, L"G");
		m_Font.DrawText( 130, 200, 0xffffffff, L"  delete" );
		m_FontButtons.DrawText( 80, 240, 0xffffffff, L"H");
		m_Font.DrawText( 130, 240, 0xffffffff, L"  cancel" );
	}*/
	/*else if(mCounter==30)
	{
		p_graph->RenderMainFrames();
		WCHAR temp[1024];
		wsprintfW(temp,L"%hs%hs",info.path,info.name);
		m_Font.DrawText( 80, 30, 0xffffffff, L"Please confirm launch of:" );
		m_Font.DrawText( 60, 140, 0xffffffff, temp );
		m_FontButtons.DrawText( 80, 200, 0xffffffff, L"G");
		m_Font.DrawText( 130, 200, 0xffffffff, L"  launch" );
		m_FontButtons.DrawText( 80, 240, 0xffffffff, L"H");
		m_Font.DrawText( 130, 240, 0xffffffff, L"  cancel" );
	}*/
	/*else if(mCounter == 46)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Searching for media checks:" );
		p_graph->RenderPopup();
		m_Font.DrawText(55, 160, 0xffffffff, L"Patching ..." );
	}*/
	/*else if(mCounter==47)
	{
		p_graph->RenderMainFrames();
		WCHAR temp[1024];
		wsprintfW(temp,L"used patch file: %hs",sinfo.item);
		m_Font.DrawText( 80, 30, 0xffffffff, L"Searching for media checks:" );
		m_Fontb.DrawText( 60, 140, 0xffffffff, temp );
		int i=0;
		while(message[i] != NULL)
		{
			m_Fontb.DrawText( 60, 160+i*m_Fontb.GetFontHeight(), 0xffffffff, message[i] );
			i++;
		}
		i++;
		m_Fontb.DrawText( 60, 160+i*m_Fontb.GetFontHeight(), 0xffffffff, L"Press A to proceed." );
	}*/
	else if(mCounter==70)
	{
		/*p_graph->RenderKeyBoardBG();
		p_keyboard->Render();*/
		p_gui->SetVKObject(p_keyboard);
		p_gui->RenderGUI(GUI_KEYBOARD);
	} 
	/*else if(mCounter==200 || mCounter==201 || mCounter==205)
	{
		p_graph->RenderBigFrame();
		if(settings_menu == 0)
		{
			m_Font.DrawText( 100,40 , 0xffffffff, L"<1/2> Main Settings:" );
			p_swin->showScrollWindow(100,100,100,0xffffffff,0xffffff00,m_Fontb);
			p_swinp->showScrollWindowSTR(400,100,100,0xffffffff,0xffffff00,m_Fontb);
		} else if(settings_menu == 1)
		{
			m_Font.DrawText( 100,40 , 0xffffffff, L"<2/2> Audio Settings:" );
			p_swin->showScrollWindow(100,100,100,0xffffffff,0xffffff00,m_Fontb);
			p_swinp->showScrollWindowSTR(400,100,100,0xffffffff,0xffffff00,m_Fontb);
		}
		WCHAR mem1[40];
		WCHAR mem2[40];
		WCHAR mem3[40];
		WCHAR mem4[40];
		m_Fontb.DrawText( 100, 350, 0xffffffff, L"Memory Status:" );
		wsprintfW(mem1,L"%4d total MB of physical memory.",memstat.dwTotalPhys/(1024*1024));
		wsprintfW(mem2,L"%4d free  MB of physical memory.",memstat.dwAvailPhys/(1024*1024));
		wsprintfW(mem3,L"%4d total MB of virtual memory.",memstat.dwTotalVirtual/(1024*1024));
		wsprintfW(mem4,L"%4d free  MB of virtual memory.",memstat.dwAvailVirtual/(1024*1024));
		m_Fontb.DrawText( 100, 350+m_Fontb.GetFontHeight(), 0xffffffff, mem1 );
		m_Fontb.DrawText( 100, 350+2*m_Fontb.GetFontHeight(), 0xffffffff, mem2 );
		m_Fontb.DrawText( 100, 350+3*m_Fontb.GetFontHeight(), 0xffffffff, mem3 );
		m_Fontb.DrawText( 100, 350+4*m_Fontb.GetFontHeight(), 0xffffffff, mem4 );

		if(g_d2xSettings.m_bLCDUsed == true)
		{
			CStdString strtext;
			strlcd1 = "1: dvd2xbox settings";
			p_util->GetDVDModel(strtext);
			strlcd2.Format("2: DVD: %s",strtext.c_str());
			p_util->GetHDDModel(strtext);
			strlcd3.Format("3: HDD: %s",strtext.c_str());
			strlcd4.Format("4: %2d MB RAM free ",memstat.dwAvailPhys/(1024*1024));
		}
	
	}*/
	else if(mCounter == 600)
	{
		CStdString	value;

		p_gui->SetViewObject(&p_view);

		value.Format("%i",p_view.getRow());
		p_gui->SetKeyValue("row",value);
		value.Format("%i",p_view.getAllRows());
		p_gui->SetKeyValue("allrows",value);
		value.Format("%i",p_view.getCol());
		p_gui->SetKeyValue("col",value);
		value.Format("%i",p_view.getAllCols());
		p_gui->SetKeyValue("allcols",value);
		p_gui->SetKeyValue("filename",p_view.getFileName());

		p_gui->SetShowIDs(1);
		p_gui->RenderGUI(GUI_VIEWER);


		//p_graph->RenderBrowserBig();
		//p_view.show(55,95,0xffffffff,0xff000000,m_Fontb);
	}
	/*else if(mCounter == 711)
	{

		WCHAR text[100];
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"xbe information:" );

		m_Fontb.DrawText( 60, 140, 0xffffffff, L"TitleName:" );
		m_Fontb.DrawText( 160, 140, 0xffffffff, p_util->xbecert.TitleName );

		m_Fontb.DrawText( 60, 160, 0xffffffff, L"TitleID:" );
		wsprintfW(text,L"%08X",p_util->xbecert.TitleId);
		m_Fontb.DrawText( 160, 160, 0xffffffff, text );

		m_Fontb.DrawText( 60, 180, 0xffffffff, L"TimeDate:" );
		wsprintfW(text,L"%08X",p_util->xbecert.Timestamp);
		m_Fontb.DrawText( 160, 180, 0xffffffff, text );

		m_Fontb.DrawText( 60, 200, 0xffffffff, L"MediaFlags:" );
		wsprintfW(text,L"%08X",p_util->xbecert.MediaTypes);
		m_Fontb.DrawText( 160, 200, 0xffffffff, text );

		m_Fontb.DrawText( 60, 220, 0xffffffff, L"GameRegion:" );
		wsprintfW(text,L"%08X",p_util->xbecert.GameRegion);
		m_Fontb.DrawText( 160, 220, 0xffffffff, text );

		m_Fontb.DrawText( 60, 240, 0xffffffff, L"GameRating:" );
		wsprintfW(text,L"%08X",p_util->xbecert.GameRating);
		m_Fontb.DrawText( 160, 240, 0xffffffff, text );

		m_Fontb.DrawText( 60, 260, 0xffffffff, L"DiscNumber:" );
		wsprintfW(text,L"%08X",p_util->xbecert.DiskNumber);
		m_Fontb.DrawText( 160, 260, 0xffffffff, text );

		m_Fontb.DrawText( 60, 280, 0xffffffff, L"Version:" );
		wsprintfW(text,L"%08X",p_util->xbecert.Version);
		m_Fontb.DrawText( 160, 280, 0xffffffff, text );

		m_Fontb.DrawText( 60, 300, 0xffffffff, L"LANKey:" );
		for(int i=0;i<=15;i++)
		{
			wsprintfW(text+2*i,L"%02X",p_util->xbecert.LanKey[i]);
		}
		m_Fontb.DrawText( 160, 300, 0xffffffff, text );

		m_Fontb.DrawText( 60, 320, 0xffffffff, L"SignKey:" );
		for(int i=0;i<=15;i++)
		{
			wsprintfW(text+2*i,L"%02X",p_util->xbecert.SignatureKey[i]);
		}
		m_Fontb.DrawText( 160, 320, 0xffffffff, text );
	

	}*/
	else if(mCounter == 760)
	{
		//p_gm->ShowGameManager(m_Font12);

		INFOitem	info;
		D2Xswin		gm_swin;
		p_gui->SetGMObject(p_gm);
	
		p_gm->getInfo(&info);
		p_gm->getWindowObject(&gm_swin);
		p_gui->SetWindowObject(1,&gm_swin);

		p_gui->SetKeyValue("currentdrive",info.cdrive);
		p_gui->SetKeyValue("freedrive",info.isizeMB);
		
		p_gui->SetKeyValue("titlename",info.title);
		p_gui->SetKeyValue("titleid",info.TitleId);
		p_gui->SetKeyValue("titlefiles",info.files);
		p_gui->SetKeyValue("titledirs",info.dirs);
		p_gui->SetKeyValue("titlesize",info.sizeMB);

		p_gui->SetKeyValue("gmtotalfiles",info.total_files);
		p_gui->SetKeyValue("gmtotaldirs",info.total_dirs);
		p_gui->SetKeyValue("gmtotalsize",info.total_MB);
		p_gui->SetKeyValue("gmtotalitems",info.total_items);

		p_gui->SetShowIDs(info.gm_mode);
		p_gui->RenderGUI(GUI_GAMEMANAGER);

		if(g_d2xSettings.m_bLCDUsed == true)
		{
			CStdString strtext;
			strlcd1 = "Game Manager:";
			strlcd3 = info.title;
		}

	}
	else if(mCounter == 1100)
	{
		p_gui->SetSGObject(&p_gset);
		p_gui->SetShowIDs(p_gset.getShowID());
		p_gui->SetKeyValue("skin",active_skin.c_str());
		p_gui->RenderGUI(GUI_SETTINGS);
		//p_gset.ShowGUISettings(m_Font,m_Fontb);
		if(g_d2xSettings.m_bLCDUsed == true)
		{
			CStdString strtext;
			strlcd1 = "1: dvd2xbox settings";
			p_util->GetDVDModel(strtext);
			strlcd2.Format("2: DVD: %s",strtext.c_str());
			p_util->GetHDDModel(strtext);
			strlcd3.Format("3: HDD: %s",strtext.c_str());
			strlcd4.Format("4: %2d MB RAM free ",memstat.dwAvailPhys/(1024*1024));
		}
	}
	
	
	if(mCounter == 1000)
	{
		//p_graph->RenderPopup();

		switch(g_d2xSettings.generalError)
		{
			case COULD_NOT_AUTH_DVD:
				p_gui->SetShowIDs(200);
				/*m_Font.DrawText(55, 160, 0xffffffff, L"Can't authenticate DVD.");
				m_Font.DrawText(55, 210, 0xffffffff, L"Open and close drive then try again.");*/
				break;
			case SMBTYPE_NOT_SUPPORTED:
				p_gui->SetShowIDs(700);
				//m_Font.DrawText(55, 160, 0xffffffff, L"Disk type not supported by smb copy.");
				break;
			case FTPTYPE_NOT_SUPPORTED:
				p_gui->SetShowIDs(620);
				break;
			case FTP_COULD_NOT_CONNECT:
				p_gui->SetShowIDs(630);
				break;
			case FTP_COULD_NOT_LOGIN:
				p_gui->SetShowIDs(640);
				break;
			case NO_DVD2XBOX_XMLFILE:
				p_gui->SetShowIDs(100);
				/*m_Font.DrawText(55, 160, 0xffffffff, L"Couldn't find dvd2xbox.xml.");
				m_Font.DrawText(55, 200, 0xffffffff, L"Please copy it in your dvd2xbox directory");
				m_Font.DrawText(55, 220, 0xffffffff, L"and reboot.");*/
				strlcd1 = "Couldn't find dvd2xbox.xml.";
				strlcd2 = "Please copy it in your dvd2xbox directory";
				strlcd3 = "and reboot.";
				break;
			default:
				break;
		};

		p_gui->RenderGUI(GUI_ERROR);
		
	}
	if(g_d2xSettings.generalNotice)
	{
		//WCHAR temp[128];
		//p_graph->RenderPopup();
		switch(g_d2xSettings.generalNotice)
		{
			case FTP_CONNECT:
				{
					/*wsprintfW(temp,L"ftp://%hs",g_d2xSettings.ftpIP);
					m_Font.DrawText(55, 160, 0xffffffff, L"Connecting to ftp host:");
					m_Font.DrawText(55, 210, 0xffffffff, temp);*/
				}
				break;
			case DELETING:
				/*m_Font.DrawText(55, 160, 0xffffffff, L"Deleting ...");*/
				break;
			case SCANNING:
				//m_Font.DrawText(55, 160, 0xffffffff, L"Scanning HDD for Games ...");
				p_gui->SetGMObject(p_gm);
				p_gui->SetShowIDs(PROCESS_RESCAN);
				p_gui->RenderGUI(GUI_GAMEMANAGER);
				break;
			/*case REBOOTING:
				m_Font.DrawText(55, 160, 0xffff0000, L"Restart dvd2xbox to enable changes ?");
				m_Font.DrawText(55, 210, 0xffffffff, L"press A to reboot");
				m_Font.DrawText(55, 230, 0xffffffff, L"press BACK to cancel");
				break;*/
			default:
				break;
		};

		
		
	}

//#if defined(_DEBUG)
//	if(showmem)
//	{
//		WCHAR mem1[40];
//		wsprintfW(mem1,L"%d KB free",memstat.dwAvailPhys/1024);
//		m_Font.DrawText( 400, 435, 0xffffffff, mem1 );
//	}
//#endif


	if(g_d2xSettings.m_bLCDUsed == true)
	{
		g_lcd->SetLine(0,strlcd1);
		g_lcd->SetLine(1,strlcd2);
		g_lcd->SetLine(2,strlcd3);
		g_lcd->SetLine(3,strlcd4);
	}

	if(ScreenSaverActive)
		p_graph->ScreenSaver();


    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return S_OK;  
}

//-----------------------------------------------------------------------------


void CXBoxSample::WriteText(char* text)
{
	/*WCHAR wText[64];
	wsprintfW(wText,L"%hs",text);*/
	//p_tex->RenderTexture(0,0,0);
	//m_BackGround.Render( &m_Font, 0, 0 );
	CStdString		strtext;
	strtext = text; 
	p_graph->RenderBackground();
	p_ml->DrawText("D2XDefaultFont",320-strlen(text)/2*11,420,0xffffffff,strtext);
	//m_Font.DrawText(320-strlen(text)/2*11,420,0xffffffff,wText);
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);
}



void CXBoxSample::prepareACL(HDDBROWSEINFO path)
{
	char acl_dest[1024];
	strcpy(acl_dest,path.item);
	p_util->addSlash(acl_dest);
	strcat(acl_dest,"default.xbe");
	if(GetFileAttributes(acl_dest) != -1)
	{
		if(g_d2xSettings.WriteLogfile)
		{
			sprintf(acl_dest,"logs\\%s.txt",path.name);
			p_log->enableLog(true);
			p_log->setLogFile(acl_dest);
		}
		p_acl->processACL(path.item,ACL_POSTPROCESS);
		p_log->enableLog(false);
	} 
}

void CXBoxSample::mapDrives()
{
	io.Remap("C:,Harddisk0\\Partition2");
	// E: mapped at start to read the stored parameters
	io.Remap("X:,Harddisk0\\Partition3");
	io.Remap("Y:,Harddisk0\\Partition4");
	io.Remap("Z:,Harddisk0\\Partition5");

	io.Remount("D:","Cdrom0"); 
	int x = 0;
	int y = 0;
	drives.clear();
	drives.insert(pair<int,string>(y++,"c:\\"));
	drives.insert(pair<int,string>(y++,"d:\\"));
	drives.insert(pair<int,string>(y++,"e:\\"));

	if(g_d2xSettings.useF)
	{
		io.Remap("F:,Harddisk0\\Partition6");
		drives.insert(pair<int,string>(y++,"f:\\"));
	} else
		io.Unmount("f:\\");

	if(g_d2xSettings.useG)
	{
		io.Remap("G:,Harddisk0\\Partition7");
		drives.insert(pair<int,string>(y++,"g:\\"));
	} else 
		io.Unmount("g:\\");

	drives.insert(pair<int,string>(y++,"x:\\"));
	drives.insert(pair<int,string>(y++,"y:\\"));
	drives.insert(pair<int,string>(y++,"z:\\"));

	if(g_d2xSettings.network_enabled)
	{
		drives.insert(pair<int,string>(y++,"ftp:/"));
		if(strlen(g_d2xSettings.smbUrl) >= 2)
			drives.insert(pair<int,string>(y++,"smb:/"));
	}

}

void CXBoxSample::StartFTPd()
{
	g_d2xSettings.ftpd_enabled = 1;
	m_pFileZilla = new CXBFileZilla(NULL);
	m_pFileZilla->Start();
	m_pFileZilla->mSettings.SetMaxUsers(g_d2xSettings.ftpd_max_user);
	m_pFileZilla->mSettings.SetThreadNum(2);
	m_pFileZilla->mSettings.SetInFxp(0);
	m_pFileZilla->mSettings.SetOutFxp(0);
	m_pFileZilla->mSettings.SetNoInFxpStrict(1);
	m_pFileZilla->mSettings.SetNoOutFxpStrict(1);
	/*m_pFileZilla->mSettings.SetCustomPasvIpType(0);
	m_pFileZilla->mSettings.SetCustomPasvIP("192.168.1.1");
	m_pFileZilla->mSettings.SetCustomPasvMinPort(1);
	m_pFileZilla->mSettings.SetCustomPasvMaxPort(65535);*/
	m_pFileZilla->mSettings.SetWelcomeMessage("Welcome to the dvd2xbox ftp server.");
	CXFUser* pUser;
	m_pFileZilla->AddUser(g_d2xSettings.ftpduser, pUser);
	pUser->SetPassword(g_d2xSettings.ftpd_pwd);
	pUser->SetShortcutsEnabled(false);
	pUser->SetUseRelativePaths(false);
	pUser->SetBypassUserLimit(false);
	pUser->SetUserLimit(0);
	pUser->SetIPLimit(0);
	pUser->AddDirectory("/", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS|XBDIR_HOME);
	pUser->AddDirectory("C:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("D:\\", XBFILE_READ|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("E:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("X:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("Y:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("Z:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("F:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->AddDirectory("G:\\", XBFILE_READ|XBFILE_WRITE|XBFILE_DELETE|XBFILE_APPEND|XBDIR_DELETE|XBDIR_CREATE|XBDIR_LIST|XBDIR_SUBDIRS);
	pUser->CommitChanges();
}


void CXBoxSample::StopFTPd()
{
	/*if (m_pFileZilla) 
	{
		m_pFileZilla->Stop();
		delete m_pFileZilla;
		m_pFileZilla = NULL;
	}*/
}

void CXBoxSample::getlocalIP()
{
	XNADDR xna;
	char szIP[33];
	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(1000);
	} while (dwState==XNET_GET_XNADDR_PENDING);
	XNetInAddrToString(xna.ina,szIP,32);
	wsprintfW(localIP,L"%hs",szIP);
}

bool CXBoxSample::AnyButtonDown()
{
	if (m_DefaultGamepad.wPressedButtons || m_DefaultIR_Remote.wButtons)
		return true;

	for (int i = 0; i < 6; ++i)
	{
		if (m_DefaultGamepad.bPressedAnalogButtons[i])
			return true;
	}
	return false;
}