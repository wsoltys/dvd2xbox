#include <XBApp.h>
#include <xbfontEx.h>
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
#include "dvd2xbox\d2xxbautodetect.h"
#include <network.h>

#include "lib\libdvdread\dvd_reader.h"

//#include "lib\dvdauth\d2xauth.h"
//#include "dvd2xbox\unlock\d2xunlock.h"

//extern "C" uint32_t UDFFindFile2( dvd_reader_t *device, char *filename, uint32_t *size );

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
#else
#pragma comment (lib,"lib/libcdio/libcdio.lib")
#pragma comment (lib,"lib/libsmb/libsmb.lib") 
#pragma comment (lib,"lib/libcdrip/cdrip.lib")
#pragma comment (lib,"lib/libogg/liboggvorbis.lib") 
#pragma comment (lib,"lib/liblame/liblame.lib") 
#pragma comment (lib,"lib/libsndfile/libsndfile.lib") 
#pragma comment (lib,"lib/libftpc/libftpc.lib") 
#pragma comment (lib,"lib/libdvdread/libdvdread.lib") 

#endif
#pragma comment (lib,"lib/libxenium/XeniumSPIg.lib")

 
char *ddumpDirs[]={"e:\\", "e:\\games", NULL};


using namespace std;

//extern int dummy_main(int argc, char *argv[]);


class CXBoxSample : public CXBApplicationEx
{
    
	MEMORYSTATUS memstat;
	int			mx;
	int			my;
	int			type;
	int			copytype;
	int			prevtype;
	//int			iFreeSpace;
	int			current_copy_retries;

	DWORD		dwSTime;
	DWORD		dwcTime;
	DWORD		dwTime;
	DWORD		dwStartCopy;
	DWORD		dwEndCopy;
	DWORD		dwFPStime;
	int			iFPStime;
	WCHAR		driveState[100];
	WCHAR		*message[1024];
	int			mCounter;
	char		mDestPath[1024];
	char		mBrowse1path[1024];
	char		mBrowse2path[1024];
	char		mDestLog[1066];
	map<int,string> drives;
	map<int,string> ddirs;
	map<int,string> ddirsFS;
	map<int,string>	ftpmenu;
	map<int,string>	ftpatt;
	HDDBROWSEINFO	info;
	SWININFO		sinfo;
	CIoSupport		io;
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
	D2Xinput*		p_input;
	D2Xfile*		p_file;
	D2XGM*			p_gm;
	D2Xguiset*		p_gset;
	D2Xgui*			p_gui;
	D2Xmedialib*	p_ml;
	CXBVirtualKeyboard* p_keyboard;
	int				dvdsize;
	int				freespace;
	Xcddb			m_cddb;	
	int				activebrowser;
	bool			b_help;
	bool			copy_retry;
	int				m_Caller;
	int				m_Return;
	int				settings_menu;
	DVD2XBOX_CFG	cfg;
	map<int,string> optionvalue;
	map<int,string> optionvalue2;
	map<int,string> str_mainmenu;
	map<int,string> str_actionmenu;
	map<int,string> str_shutmenu;
	typedef vector <string>::iterator iXBElist;
	map<int,HDDBROWSEINFO>::iterator iselected_item;
	CXBFileZilla*	m_pFileZilla;
	bool			ScreenSaverActive;
	bool			s_prio;
	CStdString		active_skin;
	bool			dialog_active;
	//int				skip_frames;

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
	//void D2Xutils::mapDrives(drives);
	void prepareACL(HDDBROWSEINFO path);
	void StartFTPd();
	void StopFTPd();
	void getlocalIP();
	bool AnyButtonDown();

	void StopApp();
	void Reboot2Remote();
	void StartNetwork();

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
	//p_graph = new D2Xgraphics();
	//p_fcopy = new D2Xfilecopy;

	p_set = D2Xsettings::Instance();
	p_gset = new D2Xguiset;
	p_title = new D2Xtitle;
	p_dstatus = new D2Xdstatus;
	p_swin = new D2Xswin;
	p_swinp = new D2Xswin;
	p_log = new D2Xlogger;
	p_acl = new D2Xacl;
	p_util = new D2Xutils;
	p_keyboard = new CXBVirtualKeyboard();
	p_gui = D2Xgui::Instance();
	p_input = D2Xinput::Instance();
	//p_ml = new D2Xmedialib();
	p_ml = D2Xmedialib::Instance();
	strcpy(mBrowse1path,"e:\\");
	strcpy(mBrowse2path,"e:\\");
	message[0] = NULL;
	copy_retry = false;
	p_file = NULL;
	p_graph = NULL;
	p_gm = NULL;
	m_pFileZilla = NULL;
	p_browser = NULL;
	p_browser2 = NULL;
	p_fcopy = NULL;
	ScreenSaverActive = false;
	wcscpy(g_d2xSettings.localIP,L"no network");
	wcscpy(driveState,L"UNKNOWN");
	dialog_active = false;

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
	//floating point precision to 24 bits (faster performance)
	_controlfp(_PC_24, _MCW_PC);

	p_util->getHomePath(g_d2xSettings.HomePath);
	p_util->RemapHomeDir(g_d2xSettings.HomePath);

	// E: mapped at start to read the stored parameters
	io.Remap("E:,Harddisk0\\Partition1");
	p_set->ReadCFG(&cfg);

	// check if we are remotely called
	if(p_set->OpenRCxml("Q:\\remotecontrol.xml"))
		g_d2xSettings.remoteControlled = true;

	if(!p_set->OpenRGBxml("Q:\\SmartXXRGB.xml"))
		g_d2xSettings.enableSmartXXRGB = false;

	if(p_gui->LoadSkin(g_d2xSettings.strskin)==0)
		return XBAPPERR_MEDIANOTFOUND;

	active_skin = g_d2xSettings.strskin;


	// virtual keyboard
	p_keyboard->Initialize();

	// usb keyboard
	g_Keyboard.Initialize();
	
	p_gui->RenderGUI(GUI_STARTUP);
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);

	// read config files
	WriteText("Loading configs");

	p_gset->LoadConfig();
	 
	if(p_set->readXML("d:\\dvd2xbox.xml"))
	{
		if(g_d2xSettings.autodetectHDD)
		{
			WriteText("Checking partitions");
			OutputDebugString("Checking for available partitions\n");
			p_gset->CheckingPartitions();
		}
		p_set->getDumpDirs(ddirs);
	}
	else
	{
		m_Caller = 0;
		g_d2xSettings.generalError = NO_DVD2XBOX_XMLFILE;
		mCounter = 1000;
	}


 
	mCounter = 0;
	type = 0;
	prevtype = 0; 
	mx = 1;
	my = 1;
	activebrowser = 1;
	b_help = false;
	current_copy_retries = 0;
	copytype = UNDEFINED;


	// create media change detection thread
	p_dstatus->Create();
	if(!g_d2xSettings.detect_media_change)
		wcscpy(driveState,L"Press BACK to detect media");

	// overwrite context if called remotely and close drive
	if(g_d2xSettings.remoteControlled)
	{
		io.CloseTray();
		mCounter = D2X_DISCCOPY_RM;
	}

		

    p_dstatus->GetDriveState(driveState,type);

	dwFPStime = dwTime = dwSTime = timeGetTime();


	// starting network
	if(g_d2xSettings.network_enabled && !g_d2xSettings.remoteControlled)
	{
		//WriteText("Starting network");
		if(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE)
		{
			switch(g_d2xSettings.network_assignment)
			{
				case NETWORK_DASH:
					WriteText("Init network using dash settings...");
					g_network.Initialize(NETWORK_DASH, "","","","");
					break;
				case NETWORK_DHCP:
					WriteText("Init network using DHCP...");
					g_network.Initialize(NETWORK_DHCP, "","","","");
					break;
				default:
					WriteText("Init network using static ip...");
					g_network.Initialize(NETWORK_STATIC,g_d2xSettings.xboxIP,
														g_d2xSettings.netmask,
														g_d2xSettings.gateway,
														g_d2xSettings.DNS);
					break;

			};

			if( g_network.WaitForSetup( 5000 ) )
			{
				DebugOut(" - network fully setup");
				WriteText("Starting network ok"); 
				getlocalIP();

				if(g_d2xSettings.ftpd_enabled)
				{
					WriteText("Starting ftp server");
					StartFTPd();
				}
				if(g_d2xSettings.autodetect_enabled)
						p_gset->StartAutoDetect();
			}
			else
			{
				DebugOut(" - network init timed out");
				WriteText("Failed to start network");
			}


			/*if (!m_cddb.InitializeNetwork(g_d2xSettings.xboxIP,g_d2xSettings.netmask ,g_d2xSettings.gateway ))
			{
				
				D2Xtitle::i_network = 0;
			} else {
				D2Xtitle::i_network = 1;
				WriteText("Starting network ok"); 
				getlocalIP();

				if(g_d2xSettings.ftpd_enabled)
				{
					WriteText("Starting ftp server");
					StartFTPd();
				}
				if(g_d2xSettings.autodetect_enabled)
						p_gset->StartAutoDetect();
				
			}*/
		}
		
	}

	// Remap the CDROM, map E & F Drives
	WriteText("Mapping drives");
	D2Xutils::mapDrives(drives);


	if(!XSetFileCacheSize(8388608))
		XSetFileCacheSize(4194304);

	GlobalMemoryStatus( &memstat );

	p_log->setLogPath(g_d2xSettings.HomePath);

	if(g_d2xSettings.m_bLCDUsed == true)
	{
		CLCDFactory factory;
		g_lcd=factory.Create();
		g_lcd->Initialize();
	}

	// map the cdrom to d
	//io.Remount("D:","Cdrom0");


	// init menus
	ftpmenu.insert(pair<int,string>(0,"Connect"));
	ftpmenu.insert(pair<int,string>(1,"IP:"));
	ftpmenu.insert(pair<int,string>(2,"User:"));
	ftpmenu.insert(pair<int,string>(3,"Password:"));

	ftpatt.insert(pair<int,string>(0,"Connect"));
	ftpatt.insert(pair<int,string>(1,g_d2xSettings.ftpIP));
	ftpatt.insert(pair<int,string>(2,g_d2xSettings.ftpuser));
	ftpatt.insert(pair<int,string>(3,g_d2xSettings.ftppwd));

	str_mainmenu.insert(pair<int,string>(0,"Copy DVD/CD-R to HDD"));
	str_mainmenu.insert(pair<int,string>(1,"Game Manager"));
	str_mainmenu.insert(pair<int,string>(2,"Copy DVD/CD-R to SMB share"));
	str_mainmenu.insert(pair<int,string>(3,"Filemanager"));
	str_mainmenu.insert(pair<int,string>(4,"Settings"));
	str_mainmenu.insert(pair<int,string>(5,"Boot to dash"));

	str_actionmenu.insert(pair<int,string>(0,"Copy file/dir"));
	str_actionmenu.insert(pair<int,string>(1,"Delete file/dir"));
	str_actionmenu.insert(pair<int,string>(2,"Rename file/dir"));
	str_actionmenu.insert(pair<int,string>(3,"Create dir"));
	str_actionmenu.insert(pair<int,string>(4,"Process ACL"));
	str_actionmenu.insert(pair<int,string>(5,"Patch from file"));
	str_actionmenu.insert(pair<int,string>(6,"Edit XBE title"));
	//str_actionmenu.insert(pair<int,string>(7,"Launch XBE"));
	str_actionmenu.insert(pair<int,string>(7,"View textfile"));
	str_actionmenu.insert(pair<int,string>(8,"xbe info"));

	str_shutmenu.insert(pair<int,string>(0,"Shutdown"));
	str_shutmenu.insert(pair<int,string>(1,"Reboot"));
	str_shutmenu.insert(pair<int,string>(2,"Open/Close Tray"));
	str_shutmenu.insert(pair<int,string>(3,"Launch Game Disc"));

	// set led to default color
	if(g_d2xSettings.enableLEDcontrol)
        ILED::CLEDControl(LED_COLOUR_GREEN); 

	// set SmartXX RGB LED
	D2Xutils::SetSmartXXRGB(STAT_GENERAL);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	p_input->update(&m_DefaultGamepad,&m_DefaultIR_Remote);

	switch(mCounter)
	{
		case 0:			
			p_swin->initScrollWindowSTR(10,str_mainmenu);
			mCounter = 11;
			break;
		case 11:
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);

			
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{
				//if(!strcmp(sinfo.item,"Copy DVD/CD-R to HDD")) 
				if(sinfo.item_nr == 0)
				{	
					//ddirsFS.clear();

					//// determine free disk space
					//char temp[40];
		
					//for(int a=0;a<ddirs.size();a++)
					//{
					//	p_util->MakePath(ddirs[a].c_str());
					//	if(!(p_util->getfreeDiskspaceMB((char*)ddirs[a].c_str(),temp)))
					//		strcpy(temp, "");
					//	ddirsFS.insert(pair<int,string>(a,temp));
					//}
					
					mCounter=D2X_DISCCOPY;
					/*p_swin->initScrollWindowSTR(10,ddirsFS);
					p_swinp->initScrollWindowSTR(10,ddirs);*/
				
				} 
				//else if(!strcmp(sinfo.item,"Filemanager")) 
				else if(sinfo.item_nr == 3)
				{
					/*strcpy(mBrowse1path,"e:\\");
					if(g_d2xSettings.useF)
						strcpy(mBrowse2path,"f:\\");
					else
						strcpy(mBrowse2path,"e:\\");*/

					mCounter=D2X_FILEMANAGER;
					/*if(p_browser != NULL)
						delete p_browser;
					if(p_browser2 != NULL)
						delete p_browser2;
					p_browser = new D2Xdbrowser();
					p_browser2 = new D2Xdbrowser();*/
				
				}
				
				//else if(!strcmp(sinfo.item,"Settings")) 
				else if(sinfo.item_nr == 4)
				{
					mCounter=D2X_SETTINGS;
				}
				else if((sinfo.item_nr == 2))
				{
					/*int a=0;
					map<int,string> smbUrls;
					map<CStdString,CStdString>::iterator i;
					for(i = g_d2xSettings.xmlSmbUrls.begin();
						i != g_d2xSettings.xmlSmbUrls.end();
						i++)
					{
						smbUrls.insert(pair<int,string>(a,i->first));
						a++;
					}
					p_swin->initScrollWindowSTR(10,smbUrls);*/
					mCounter = D2X_SMBCOPY;
		
				}

				//else if(!strcmp(sinfo.item,"Game Manager"))
				else if(sinfo.item_nr == 1)
				{
					mCounter = D2X_GAMEMANAGER;
				}
				//else if(!strcmp(sinfo.item,"Boot to dash"))
				else if(sinfo.item_nr == 5)
				{
					StopApp();
					RebootToDash();
				}
	
			}
			else if(p_input->pressed(GP_Y) || p_input->pressed(IR_MENU))
			{
				p_swinp->initScrollWindowSTR(10,str_shutmenu);
				mCounter = 200;
			}
			else if(p_input->pressed(GP_BLACK))
			{
				if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_MEDIA_PRESENT)
				{
					strcpy(mDestPath,"d:\\default.xbe");
					mCounter=710;
					m_Caller=0;
					m_Return=0;
				}
			}
			else if(p_input->pressed(GP_BACK)) 
			{
				if(!g_d2xSettings.detect_media_change)
					p_dstatus->DetectMedia();
			}

#ifdef _DEBUG
			if(p_input->pressed(GP_X))
			{
				//CCdIoSupport cdio;
				//CCdInfo*			m_pCdInfo;
				////	Detect new CD-Information
				//m_pCdInfo = cdio.GetCdInfo();

				//if ( m_pCdInfo != NULL ) 
				//{
				//	delete m_pCdInfo;
				//	m_pCdInfo = NULL;
				//}

				//char *c_line[]={"streamdvd","-i", "\\Device\\Cdrom0", "-t", "1", "-c", "1-2", "-s", "0xe0,0x80"};

				//dummy_main(9,c_line);
				
				
			}
#endif
			if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > 0 )
			{
				if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > 0 )
				{
					if( m_DefaultGamepad.bPressedAnalogButtons[XINPUT_GAMEPAD_Y] )
					{
						strcpy(cfg.skin,"Project_Mayhem_III");
						p_set->WriteCFG(&cfg);
						//p_util->RunXBE(g_d2xSettings.HomePath,"d:\\default.xbe");
						p_util->Reboot();
					}
				}
			}

			// Check if a shortcut was used
			p_gui->getContext(mCounter);

			break;
		case 1:
			{
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
				
				mCounter=13;
				p_swin->initScrollWindowSTR(10,ddirsFS);
				p_swinp->initScrollWindowSTR(10,ddirs);

			}
			break;

		case 13:

			if(!g_d2xSettings.remoteControlled)
			{
				sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
				sinfo = p_swinp->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			}
			if(p_input->pressed(GP_A) || p_input->pressed(GP_START) || p_input->pressed(IR_SELECT) || g_d2xSettings.remoteControlled)
			{
				if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_MEDIA_PRESENT)
				{
				
					if(g_d2xSettings.remoteControlled)
					{
						D2Xdstatus::GetDriveState(driveState,type);
						g_d2xSettings.generalDialog = 0;
						p_input->Unlock();

						if(type == GAME)
						{
							copytype = g_d2xSettings.rm_iGCmode;
							strcpy(mDestPath,g_d2xSettings.rm_strGCdir);
						}
						else if(type == DVD)
						{
							copytype = g_d2xSettings.rm_iVCmode;
							strcpy(mDestPath,g_d2xSettings.rm_strVCdir);
						}
						else if(type == CDDA)
						{
							g_d2xSettings.cdda_encoder = g_d2xSettings.rm_iACmode;
							strcpy(mDestPath,g_d2xSettings.rm_strACdir);
						}
						else
							Reboot2Remote();
					}
					else
                        strcpy(mDestPath,sinfo.item);

					p_util->addSlash(mDestPath);
					p_util->getfreeDiskspaceMB(mDestPath, freespace);
					dvdsize = p_dstatus->countMB("D:\\");
					char temppath[256];
					p_title->GetNextPath(mDestPath,temppath,type);
					strcat(mDestPath,temppath);
					if(g_d2xSettings.generalError != 0)
					{
						m_Caller = 0;
						mCounter = 1000;
					} 
		
					else
						mCounter = 3;

					if(copytype == DVD2ISORIPPER)
					{
						dvdsize = (D2Xutils::QueryVolumeInformation()+EXTRA_SPACE_REQ)/(1024*1024);
					}
					
				}
				else
				{
					if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_NO_MEDIA || D2Xdstatus::getMediaStatus()==DRIVE_OPEN)
					{
						io.CloseTray();
						g_d2xSettings.generalDialog = D2X_DRIVE_NO_DISC;
					}
					if(D2Xdstatus::getMediaStatus()==DRIVE_NOT_READY )
						g_d2xSettings.generalDialog = D2X_DRIVE_NOT_READY;

				}
				if(g_d2xSettings.remoteControlled && D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_NO_MEDIA)
				{
					Reboot2Remote();
				}
			
			}
			if(p_input->pressed(GP_Y))
			{
				switch (copytype)
				{
				case UNDEFINED:
                    copytype = DVD2ISORIPPER;
					break;
				case DVD2ISORIPPER:
					copytype = UNDEFINED;
					break;
				};
			}

			if(p_input->pressed(GP_BACK)) 
			{
				copytype = UNDEFINED;
				mCounter = D2X_MAINMENU;
			}

			// Check if a shortcut was used
			p_gui->getContext(mCounter);

			break;
			
		case 2:
			//wsprintf(mDestPath,"%S",p_keyboard->GetText());
			{
				CStdString	strValue = p_keyboard->GetText();
				strcat(mDestPath,strValue.c_str());
				mCounter=3;
			}
			break;
			
		case 3:
			

			if(p_input->pressed(GP_START) || p_input->pressed(IR_SELECT) || (g_d2xSettings.remoteControlled && !p_set->showKeyboard(type) && (dvdsize < freespace)) )
			{
				if(GetFileAttributes(mDestPath) == -1)
				{
	
					p_util->addSlash(mDestPath);
					mCounter++;
				}
			} 
	
			else if(p_input->pressed(GP_X) || p_input->pressed(IR_MENU)) 
			{
				WCHAR wsFile[1024];
				char* sl = strrchr(mDestPath,'\\');
				sl++;
				swprintf(  wsFile,L"%S", sl );
				*sl = '\0';
				p_keyboard->Reset();
				p_keyboard->SetText(wsFile);
				mCounter=70;
				m_Caller = 3;
				m_Return = 2;
			}
	
			else if(p_input->pressed(GP_BACK)) 
			{
				mCounter=D2X_DISCCOPY;
			} 
			break;
		case 4:

			if(g_d2xSettings.WriteLogfile)
			{
				p_log->enableLog(true);
			}

			dwStartCopy = timeGetTime();

			if(copytype == DVD2ISORIPPER)
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
					p_log->setLogFile("logs\\ISORipper.txt");

				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:\\");
				strcpy(info.name,"\0");
				D2Xutils::SetSmartXXRGB(STAT_COPY_GAME);
			}
			else
			{

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

					D2Xutils::SetSmartXXRGB(STAT_COPY_DVD);

				} else if(type==CDDA)
				{
					p_log->setLogFile("logs\\CDDA.txt");
					CreateDirectory(mDestPath,NULL);
					info.type = BROWSE_DIR;
					strcpy(info.item,"d:\\");

					D2Xutils::SetSmartXXRGB(STAT_COPY_CDDA);
				
				} else if(type==ISO ||type==VCD ||type==SVCD)
				{
					p_log->setLogFile("logs\\ISO.txt");
					CreateDirectory(mDestPath,NULL);
					info.type = BROWSE_DIR;
					strcpy(info.item,"d:\\");
					strcpy(info.name,"\0");
					
					D2Xutils::SetSmartXXRGB(STAT_COPY_ISO);
				
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

					D2Xutils::SetSmartXXRGB(STAT_COPY_GAME);

				}
			}

			// stop autodetection
			p_dstatus->StopThread();
			
			// we start the copy process
			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_ORANGE);

			//iFreeSpace = p_util->getfreeDiskspaceMB(mDestPath);

			p_fcopy = new D2Xfilecopy;
			p_fcopy->Create();
			if(copytype == DVD2ISORIPPER)
				p_fcopy->FileCopy(info,mDestPath,DVD2ISORIPPER);
			else
				p_fcopy->FileCopy(info,mDestPath,type);

			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			//SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter++;
			break;
		case 5:
			//Sleep(250);
			if(D2Xfilecopy::b_finished) 
			{
				//SetThreadPriorityBoost(GetCurrentThread(),false);
				s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				DebugOut("Main thread set priority to normal: %d\n",s_prio?1:0);
				
				copy_retry = false;
				if((D2Xfilecopy::copy_failed > 0) && (type != CDDA) && (!g_d2xSettings.remoteControlled || (g_d2xSettings.remoteControlled && p_set->showCopyRetryDialog(type))))
				{
					if(g_d2xSettings.enableLEDcontrol)
						ILED::CLEDControl(LED_COLOUR_RED);

					D2Xutils::SetSmartXXRGB(STAT_COPY_FAILED);
					mCounter = 8;
				}
				else
				{
					mCounter = 6;

					if(p_fcopy != NULL)
					{
						delete p_fcopy;
						p_fcopy = NULL;
					}
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

			// start autodetection again
			p_dstatus->Create();
			
			if(g_d2xSettings.enableAutoeject)
                io.EjectTray();
			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_GREEN);

			if(D2Xfilecopy::copy_renamed > 0)
				D2Xutils::SetSmartXXRGB(STAT_COPY_RENAMED);
			else
				D2Xutils::SetSmartXXRGB(STAT_COPY_OK);

			p_log->enableLog(false);

			mCounter++; 
			break; 
		case 7:
			//if(p_input->pressed(GP_START) || p_input->pressed(IR_SELECT)) 
			if(AnyButtonDown())
			{
				mCounter=0;
				copytype = UNDEFINED;
				D2Xutils::SetSmartXXRGB(STAT_GENERAL);

				// if remotely called restart to the dash
				if(g_d2xSettings.remoteControlled)
					Reboot2Remote();
			}
			if(p_input->pressed(GP_Y) && g_d2xSettings.WriteLogfile)
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
			if(p_input->pressed(GP_X) || p_input->pressed(IR_BACK)) 
			{
				current_copy_retries = 0;
				mCounter = 6;
				if(p_fcopy != NULL)
				{
					delete p_fcopy;
					p_fcopy = NULL;
				}
			}
			else if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT) || (current_copy_retries <= g_d2xSettings.autoCopyRetries))
			{
				copy_retry = true;
				io.CloseTray();
				io.Remount("D:","Cdrom0");
				
				if(g_d2xSettings.enableLEDcontrol)
					ILED::CLEDControl(LED_COLOUR_ORANGE);			
	
				//p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->CopyFailed(type);
				SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
				mCounter = 5;
				current_copy_retries++;
			}
			break;
		case 10:
			mCounter++;
			break;
		case 20:
			strcpy(mBrowse1path,"e:\\");
			if(g_d2xSettings.useF)
				strcpy(mBrowse2path,"f:\\");
			else
				strcpy(mBrowse2path,"e:\\");

			if(p_browser != NULL)
				delete p_browser;
			if(p_browser2 != NULL)
				delete p_browser2;
			p_browser = new D2Xdbrowser();
			p_browser2 = new D2Xdbrowser();

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

			if(p_input->pressed(C_LEFT))
				activebrowser = 1;
			if(p_input->pressed(C_RIGHT))
				activebrowser = 2;
			if(activebrowser == 1)
			{
				info = p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			} else {
				info = p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}

			// root dir
			/*if(info.type == SELECT_DRIVE)
				mCounter = 48;*/
		
			
			// action menu
			if(p_input->pressed(GP_WHITE) || p_input->pressed(GP_START) || p_input->pressed(IR_MENU))
			{
				p_swin->initScrollWindowSTR(20,str_actionmenu);
				mCounter=25;
			}
			
			// drive window
			if(p_input->pressed(GP_BLACK) || p_input->pressed(IR_TITLE))
			{
				D2Xutils::mapDrives(drives);
				p_swin->initScrollWindowSTR(20,drives);
				mCounter=50;
			}

			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{
				if(info.type == BROWSE_FILE)
				{
					if(p_util->cmpExtension(info.name,"xbe"))
					{
						mCounter = 30;
					}
					else if(p_util->isTextExtension(info.name))
					{
						if(_strnicmp(info.item,"ftp:",4))
						{
							p_view.init(info.item,20,65);
							mCounter = 600;
							m_Caller = 20;
						}
					}

				}
			}
			

			if(p_input->pressed(GP_BACK))
			{
				delete p_browser;
				delete p_browser2;
				p_browser = NULL;
				p_browser2 = NULL;
				mCounter=0;
			}

			// Check if a shortcut was used
			p_gui->getContext(mCounter);

			break;
		case 22:
			if(p_input->pressed(GP_START) || p_input->pressed(IR_SELECT))
			{
				g_d2xSettings.generalNotice = DELETING;
				mCounter = 23;
			}
			if(p_input->pressed(GP_BACK))
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

						CStdString temp = iselected_item->second.item;
						temp = iselected_item->second.type;

						if(iselected_item->second.type == BROWSE_DIR)
						{
							p_file->DeleteDirectory(iselected_item->second.item);

						} else if(iselected_item->second.type == BROWSE_FILE) 
						{
							p_file->DeleteFile(iselected_item->second.item);

						}
						//p_browser->selected_item.erase(iselected_item);
					}
					delete p_file;
					p_file = NULL;
					p_browser->selected_item.clear();
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
						//p_browser2->selected_item.erase(iselected_item);
					}
					delete p_file;
					p_file = NULL;
					p_browser2->selected_item.clear();
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
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			//if(mhelp->pressA(m_DefaultGamepad))

			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{ 
				switch(sinfo.item_nr)
				{
				case 0:
				//if(!strcmp(sinfo.item,"Copy file/dir")) 
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
				break;
				case 1:
				//else if(!strcmp(sinfo.item,"Delete file/dir"))
				{
					if(strcmp(info.name,"..")) 
						mCounter = 22;
				}
				break;
				//else if(!strcmp(sinfo.item,"Patch from file"))
				case 5:
				{
					p_patch = new D2Xpatcher;
					map<int,string> str_files;
					p_patch->getPatchFilesSTR(str_files);
					p_swin->initScrollWindowSTR(20,str_files);
					//p_swin->initScrollWindow(p_patch->getPatchFiles(),20,false);
					mCounter = 45;
				}
				break;
				//else if(!strcmp(sinfo.item,"Launch XBE"))
				/*case 7:
				{
					mCounter = 30;
				}
				break;*/
				//else if(!strcmp(sinfo.item,"Rename file/dir"))
				case 2:
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
				} 
				break;
				//else if(!strcmp(sinfo.item,"Create dir"))
				case 3:
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
				} 
				break;
				//else if(!strcmp(sinfo.item,"Process ACL"))
				case 4:
				{
					mCounter = 100;
				}
				break;
				//else if(!strcmp(sinfo.item,"Edit XBE title"))
				case 6:
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
				break;
				//else if(!strcmp(sinfo.item,"View textfile")) 
				case 7:
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
				break;
				//else if(!strcmp(sinfo.item,"xbe info")) 
				case 8:
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
				break;
				default:
				break;
				}
				
			}
			else if(p_input->pressed(GP_BLACK) || p_input->pressed(IR_TITLE))
			{
				D2Xutils::mapDrives(drives);
				p_swin->initScrollWindowSTR(20,drives);
				mCounter=50;
			}
			else if(p_input->pressed(GP_BACK) || p_input->pressed(GP_WHITE))
			{
				mCounter=21;
			}
			break;
		case 30:
			// launch xbe
			/*if(strstr(info.name,".xbe") || strstr(info.name,".XBE"))
			{*/
				if(p_input->pressed(GP_START) || p_input->pressed(IR_SELECT))
				{
					//char lxbe[50];
					//sprintf(lxbe,"%s\\%s",info.path,info.name);
					StopApp();
					p_util->RunXBE(info.item,NULL);
				}
			/*} else 
				mCounter = 21;*/
			if(p_input->pressed(GP_BACK))
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
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
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
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			if((p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))&& strcmp(sinfo.item,"No files"))
			{
				int i=0;
				while(message[i] != NULL)
				{
					delete message[i];
					message[i] = NULL;
				}
				mCounter = 46;
			}
			if(p_input->pressed(GP_BACK))
			{
				delete p_patch;
				p_swin->initScrollWindowSTR(20,str_actionmenu);
				mCounter=25;
			}
			break;
		case 46:
			p_patch->patchXBEfromFile(info,sinfo.item,message);
			mCounter = 47;
			break;
		case 47: 
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{
				//p_swin->initScrollWindowSTR(p_patch->getPatchFiles(),20,false);
				map<int,string> str_files;
				p_patch->getPatchFilesSTR(str_files);
				p_swin->initScrollWindowSTR(20,str_files);
				mCounter = 45;
				int i=0;
				while(message[i] != NULL)
				{
					delete message[i];
					message[i] = NULL;
				}
				
			}
			break;
		//case 48:
		//	// workaround for drive menu inside the browser window
		//	if(!strncmp(info.item,"ftp:",4))
		//	{
		//		g_d2xSettings.strFTPNick.clear();
  //              mCounter = 699;
		//	}
		//	else if(p_util->getMapValue(g_d2xSettings.autoFTPstr,info.item) != NULL)
		//	{
		//		g_d2xSettings.strFTPNick = info.item;
		//		if(activebrowser == 1)
		//		{
		//			//sprintf(mBrowse1path,"ftp://",sinfo.item);
		//			strcpy(mBrowse1path,"ftp:/");
		//			p_browser->ResetCurrentDir();
		//		}
		//		else 
		//		{
		//			//sprintf(mBrowse2path,"ftp://",sinfo.item);
		//			strcpy(mBrowse2path,"ftp:/");
		//			p_browser2->ResetCurrentDir();
		//		}
		//	
		//		mCounter = 21;

		//	}
		//	else if(p_util->getMapValue(g_d2xSettings.xmlSmbUrls,info.item) != NULL)
		//	{
		//		if(activebrowser == 1)
		//		{
		//			//strcpy(mBrowse1path,p_util->getMapValue(g_d2xSettings.xmlSmbUrls,sinfo.item));
		//			sprintf(mBrowse1path,"smb://%s/",info.item);
		//			p_browser->ResetCurrentDir();
		//		}
		//		else 
		//		{
		//			//strcpy(mBrowse2path,p_util->getMapValue(g_d2xSettings.xmlSmbUrls,sinfo.item));
		//			sprintf(mBrowse2path,"smb://%s/",info.item);
		//			p_browser2->ResetCurrentDir();
		//		}
		//	
		//		mCounter = 21;

		//	}
		//	else
		//	{
		//		if(activebrowser == 1)
		//		{
		//			strcpy(mBrowse1path,info.item);
		//			p_browser->ResetCurrentDir();
		//		}
		//		else 
		//		{
		//			strcpy(mBrowse2path,info.item);
		//			p_browser2->ResetCurrentDir();
		//		}
		//	
		//		mCounter = 21;
		//	}
		//	break;
		case 50:
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			if(p_input->pressed(GP_A) || p_input->pressed(GP_START) || p_input->pressed(IR_SELECT))
			{
				m_Caller = 21;
				if(!strncmp(sinfo.item,"ftp:",4))
				{
					g_d2xSettings.strFTPNick.clear();
                    mCounter = 699;
				}
				else if(p_util->getMapValue(g_d2xSettings.autoFTPstr,sinfo.item) != NULL)
				{
					g_d2xSettings.strFTPNick = sinfo.item;
					if(activebrowser == 1)
					{
						//sprintf(mBrowse1path,"ftp://",sinfo.item);
						strcpy(mBrowse1path,"ftp:/");
						p_browser->ResetCurrentDir();
					}
					else 
					{
						//sprintf(mBrowse2path,"ftp://",sinfo.item);
						strcpy(mBrowse2path,"ftp:/");
						p_browser2->ResetCurrentDir();
					}
				
					mCounter = 21;

				}
				else if(p_util->getMapValue(g_d2xSettings.xmlSmbUrls,sinfo.item) != NULL)
				{
					if(activebrowser == 1)
					{
						//strcpy(mBrowse1path,p_util->getMapValue(g_d2xSettings.xmlSmbUrls,sinfo.item));
						sprintf(mBrowse1path,"smb://%s/",sinfo.item);
						p_browser->ResetCurrentDir();
					}
					else 
					{
						//strcpy(mBrowse2path,p_util->getMapValue(g_d2xSettings.xmlSmbUrls,sinfo.item));
						sprintf(mBrowse2path,"smb://%s/",sinfo.item);
						p_browser2->ResetCurrentDir();
					}
				
					mCounter = 21;

				}
				else
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
             
			}
			else if(p_input->pressed(GP_WHITE) || p_input->pressed(IR_MENU))
			{
				// action menu
				p_swin->initScrollWindowSTR(20,str_actionmenu);
				mCounter=25;
			}
			if(p_input->pressed(GP_BACK) || p_input->pressed(GP_BLACK))
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
			//SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter = 61;
			break;
		case 61: 
			if(D2Xfilecopy::b_finished)
			{
				mCounter = 21;
				//SetThreadPriorityBoost(GetCurrentThread(),false);
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
			//SetThreadPriorityBoost(GetCurrentThread(),true);
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
				//SetThreadPriorityBoost(GetCurrentThread(),false);
				s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				DebugOut("Main thread set priority to normal: %d\n",s_prio?1:0);
			}
		case 70:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
				p_keyboard->OnAction(ACTION_SELECT_ITEM);
			//else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP)
			else if(p_input->pressed(C_UP))
				p_keyboard->OnAction(ACTION_MOVE_UP);
			//else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			else if(p_input->pressed(C_DOWN))
				p_keyboard->OnAction(ACTION_MOVE_DOWN);
			//else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			else if(p_input->pressed(C_LEFT))
				p_keyboard->OnAction(ACTION_MOVE_LEFT);
			//else if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			else if(p_input->pressed(C_RIGHT))
				p_keyboard->OnAction(ACTION_MOVE_RIGHT);
			//else if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK))
			else if(p_input->pressed(GP_B))
				p_keyboard->OnAction(ACTION_PRESS_B);
			else if(p_input->pressed(GP_X))
				p_keyboard->OnAction(ACTION_PRESS_X);
			else if(p_input->pressed(GP_BACK))
				mCounter = m_Caller;

			if(p_input->pressed(GP_TL_LEFT) || p_input->pressed(GP_LTRIGGER) || p_input->pressed(IR_REVERSE))
				p_keyboard->OnAction(ACTION_MOVE_LEFT2);
			else if(p_input->pressed(GP_TL_RIGHT) || p_input->pressed(GP_RTRIGGER) || p_input->pressed(IR_FORWARD))
				p_keyboard->OnAction(ACTION_MOVE_RIGHT2);

			p_keyboard->UpdateUSB();

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

		case 105:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
				mCounter = 21;
			break;
		case 200:
			sinfo = p_swinp->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);

			if(p_input->pressed(GP_A) || p_input->pressed(GP_START) || p_input->pressed(IR_SELECT))
			{
				switch(sinfo.item_nr)
				{
				case 0:
					// Shutdown
					StopApp();
					io.Shutdown();
					break;
				case 1:
					// Reboot
					StopApp();
					//p_util->Reboot();
					XKUtils::XBOXReset();
					break;
				case 2:
					// Open/Close Tray
					if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_MEDIA_PRESENT ||
						D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_NO_MEDIA)
					{
						io.EjectTray();
					}
					else if(D2Xdstatus::getMediaStatus()==DRIVE_OPEN)
						io.CloseTray();
					break;
				case 3:
					// Launch
					if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_MEDIA_PRESENT)
					{
						if(g_d2xSettings.detected_media == GAME)
						{	
							StopApp();
							p_util->RunXBE("d:\\default.xbe",NULL);
						}
					}
					break;
				};
			}
			
			if(p_input->pressed(GP_BACK) || p_input->pressed(GP_Y) || p_input->pressed(IR_BACK)) 
			{
				mCounter=11;
			} 
			
			break;
		case 500:
			{
				int a=0;
				map<int,string> smbUrls;
				map<CStdString,CStdString>::iterator i;
				for(i = g_d2xSettings.xmlSmbUrls.begin();
					i != g_d2xSettings.xmlSmbUrls.end();
					i++)
				{
					smbUrls.insert(pair<int,string>(a,i->first));
					a++;
				}
				p_swin->initScrollWindowSTR(10,smbUrls);
				mCounter = 501;
			}
			break;

		case 501:
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			if((p_input->pressed(GP_A) || p_input->pressed(GP_START) || p_input->pressed(IR_SELECT)))
			{	
				if( !g_d2xSettings.network_enabled)
				{
					g_d2xSettings.generalDialog = D2X_NO_NETWORK;
				}
				else if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_MEDIA_PRESENT)
				{
					char temppath[128];
					sprintf(temppath,"smb://%s/",sinfo.item);
					p_title->GetNextPath(temppath,mDestPath,type,D2X_SMB);
					
					mCounter = 502;
				}
				else
				{
					if(D2Xdstatus::getMediaStatus()==DRIVE_CLOSED_NO_MEDIA || D2Xdstatus::getMediaStatus()==DRIVE_OPEN)
					{
						io.CloseTray();
						g_d2xSettings.generalDialog = D2X_DRIVE_NO_DISC;
					}
					if(D2Xdstatus::getMediaStatus()==DRIVE_NOT_READY )
						g_d2xSettings.generalDialog = D2X_DRIVE_NOT_READY;

				}
			}
			if(p_input->pressed(GP_Y))
			{
				switch (copytype)
				{
				case UNDEFINED:
                    copytype = DVD2ISORIPPER;
					break;
				case DVD2ISORIPPER:
					copytype = UNDEFINED;
					break;
				};
			}
			if(p_input->pressed(GP_BACK)) 
			{
				copytype = UNDEFINED;
				mCounter=D2X_MAINMENU;
			} 

			// Check if a shortcut was used
			p_gui->getContext(mCounter);


			break;
		case 502:
			if(p_input->pressed(GP_START) || p_input->pressed(IR_SELECT)) 
			{
				char temppath[128];
				strncpy(temppath,mDestPath,127);
				sprintf(mDestPath,"smb://%s/%s",sinfo.item,temppath);
				mCounter=510;
			} 
			else if(p_input->pressed(GP_X) || p_input->pressed(IR_MENU)) 
			{
				WCHAR wsFile[1024];
				swprintf(  wsFile,L"%S", mDestPath );
				p_keyboard->Reset();
				p_keyboard->SetText(wsFile);
				mCounter=70;
				m_Caller = 502;
				m_Return = 504;
			}
	
			else if(p_input->pressed(GP_BACK)) 
			{
				mCounter=500;
			} 
			break;
		case 504:
			wsprintf(mDestPath,"%S",p_keyboard->GetText());
			mCounter=502;
			break;
		case 510:
			//dvdsize = 1;
			dwStartCopy = timeGetTime(); 
			char title[128];

			// stop autodetection
			p_dstatus->StopThread();

			if(g_d2xSettings.WriteLogfile)
			{
				p_log->enableLog(true);
			}

			if(g_d2xSettings.enableLEDcontrol)
                ILED::CLEDControl(LED_COLOUR_ORANGE);

			if(copytype == DVD2ISORIPPER)
			{
				dvdsize = D2Xutils::QueryVolumeInformation()/(1024*1024);
				WCHAR game[50];
				if(p_title->getXBETitle("d:\\default.xbe",game))
				{
					sprintf(title,"%S",game);
					getFatxName(title);
					sprintf(mDestLog,"logs\\%s.txt",title);
                    p_log->setLogFile(mDestLog);
				} else
				{
					p_log->setLogFile("logs\\ISOripper.txt");
					strcpy(title,"ISOripper.txt");
				}
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,DVD2ISORIPPER);
			}
			else if(type==DVD)
			{	
				D2Xutils::SetSmartXXRGB(STAT_COPY_DVD);
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
				p_util->addSlash2(mDestPath);
				/*strcat(mDestPath,title);*/
				//sprintf(mDestPath,"smb:/%s",title);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);

			}
			else if(type==CDDA)
			{	
				D2Xutils::SetSmartXXRGB(STAT_COPY_CDDA);

				if(g_d2xSettings.cdda_encoder == WAV)
				{
					m_Caller = 0;
					g_d2xSettings.generalError = SMBTYPE_NOT_SUPPORTED;
					mCounter = 1000;
					p_log->enableLog(false);
				}
				else
				{
					copytype = CDDA;
					p_log->setLogFile("logs\\CDDA.txt");
					p_util->addSlash2(mDestPath);
					info.type = BROWSE_DIR;
					strcpy(info.item,"d:\\");
					strcpy(info.name,"\0");
					p_fcopy = new D2Xfilecopy;
					p_fcopy->Create();
					p_fcopy->FileCopy(info,mDestPath,type);
				}
			}
			else if(type==ISO ||type==VCD ||type==SVCD)
			{
				D2Xutils::SetSmartXXRGB(STAT_COPY_ISO);
				copytype = ISO2SMB;
				p_log->setLogFile("logs\\ISO.txt");
				dvdsize = p_dstatus->countMB("D:\\");
				//sprintf(mDestPath,"%s/%s/",g_d2xSettings.smbShare,"dvd2xbox_iso");
				//sprintf(mDestPath,"smb:/%s","dvd2xbox_iso");
				p_util->addSlash2(mDestPath);
				//strcat(mDestPath,"dvd2xbox_iso");
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
			}
			else 
			{
				D2Xutils::SetSmartXXRGB(STAT_COPY_GAME);
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
				//sprintf(mDestPath,"smb:/%s",title);
				p_util->addSlash2(mDestPath);
				//strcat(mDestPath,title);

				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy = new D2Xfilecopy;
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
				

			}
			s_prio = SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			//SetThreadPriorityBoost(GetCurrentThread(),true);
			DebugOut("Main thread set priority to lowest: %d\n",s_prio?1:0);
			mCounter = 5;
			break;
		case 600:
			p_view.process(m_DefaultGamepad);
			//if(mhelp->pressBACK(m_DefaultGamepad))
			if(p_input->pressed(GP_BACK))
                mCounter = m_Caller;
			break;
		case 699:
			//p_swinp->initScrollWindow(ftpmenu,4,false);
			p_swinp->initScrollWindowSTR(5,ftpmenu);
			p_swin->initScrollWindowSTR(4,ftpatt);
			mCounter = 700;
			break;
		case 700:
			//sinfo = p_swinp->processScrollWindow(&m_DefaultGamepad, &m_DefaultIR_Remote);
			sinfo = p_swinp->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			sinfo = p_swin->processScrollWindowSTR(&m_DefaultGamepad, &m_DefaultIR_Remote);
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
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
			if(p_input->pressed(GP_BACK))
			{
				mCounter = 21;
			}
			break;
		case 701:
			{
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
			}
			break;
		case 710:
			if(p_util->getXBECert(mDestPath))
			{
				strcpy(info.item,mDestPath);
				info.title[0] = '\0';
				mCounter++;
			}
			else
			{
				mCounter = m_Caller;
			}
			break;
		case 711:
			if(p_input->pressed(GP_A) || p_input->pressed(GP_BACK) || p_input->pressed(IR_SELECT)) 
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
				g_d2xSettings.generalNotice = SCANNING;
				mCounter = 757;
			}
			break;
		case 755:
				// full scan
				p_gm->ScanDisk();
				g_d2xSettings.generalNotice = 0;
				p_gm->PrepareList();
				mCounter = 760;
			break;
		case 757:
				// quick scan
				p_gm->PrepareList();
				p_gm->QuickScanDisk();
				g_d2xSettings.generalNotice = 0;
				p_gm->PrepareList();
				mCounter = 760;
			break;
		case 760:
			{
				int ret;
				ret = p_gm->ProcessGameManager(&m_DefaultGamepad, &m_DefaultIR_Remote);
				if(ret == PROCESS_BACK)
				{
					delete p_gm;
					p_gm = NULL;
					mCounter = D2X_MAINMENU;
				}
				else if(ret == PROCESS_RESCAN)
				{
					/*delete p_gm;
					p_gm = NULL;*/
					mCounter = 750;
					g_d2xSettings.generalNotice = SCANNING;
				}

				// Check if a shortcut was used
				p_gui->getContext(mCounter);

			}
			break;
		case 1000:
			//if(mhelp->pressA(m_DefaultGamepad))
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{
				g_d2xSettings.generalError = 0;
				mCounter = m_Caller;
			}
			break;
		case 1010:
			if(p_input->pressed(GP_BACK))
			{
				g_d2xSettings.generalNotice = 0;
				mCounter = m_Caller;
			}
			if(p_input->pressed(GP_A) || p_input->pressed(IR_SELECT))
			{
				//p_util->RunXBE(g_d2xSettings.HomePath,"d:\\default.xbe");
				p_util->Reboot();
			}
			break;
		case 1100:
			switch(p_gset->Process(&m_DefaultGamepad,&m_DefaultIR_Remote))
			{
			case D2X_GUI_BACK:
				mCounter = D2X_MAINMENU;
				break;
			case D2X_GUI_MAPDRIVES:
				D2Xutils::mapDrives(drives);
				p_set->getDumpDirs(ddirs);
				break;
			case D2X_GUI_START_NET:
		
				/*if (!m_cddb.InitializeNetwork(g_d2xSettings.xboxIP,g_d2xSettings.netmask ,g_d2xSettings.gateway ))
				{
					D2Xtitle::i_network = 0;
				} else
				{
					D2Xtitle::i_network = 1;
					getlocalIP();
				}*/
				StartNetwork();
		
				D2Xutils::mapDrives(drives);
				break;
			case D2X_GUI_STOP_NET:
				/*StopFTPd();
				WSACleanup();
				D2Xtitle::i_network = 0;
				wcscpy(g_d2xSettings.localIP,L"no network");
				D2Xutils::mapDrives(drives);*/
				break;
			case D2X_GUI_START_FTPD:
				StartFTPd();
				break;
			case D2X_GUI_STOP_FTPD:
				StopFTPd();
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
				//p_util->RunXBE(g_d2xSettings.HomePath,"d:\\default.xbe");
				p_util->Reboot();
				break;
			case D2X_GUI_START_MEDIAD:
				p_dstatus->Create();
				break;
			default:
				break;
			};

			// Check if a shortcut was used
			p_gui->getContext(mCounter);

			break;
		default:
			break;
		
	}


	if(g_d2xSettings.generalError)
		mCounter = 1000;

	if(g_d2xSettings.generalDialog != 0)
	{
		if(dialog_active == true)
		{
			p_input->Unlock();
			if(p_input->pressed(GP_A) || p_input->pressed(GP_BACK) || p_input->pressed(IR_SELECT)) 
			{
				g_d2xSettings.generalDialog = 0;
				dialog_active = false;
			}
			else
				p_input->Lock();
		}
		else
			dialog_active = true;
	}
	
    dwcTime = timeGetTime();

	if((dwcTime-dwTime) >= 2000)
	{
		dwTime = dwcTime;
		GlobalMemoryStatus( &memstat );

	}

	
	D2Xdstatus::GetDriveState(driveState,type);

	if(p_browser != NULL && p_browser2 != NULL)
	{
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


	if(g_d2xSettings.ScreenSaver != 0)
	{
		if(AnyButtonDown() == true)
		{
			dwSTime = dwcTime;
			ScreenSaverActive = false;
			if(p_graph != NULL)
			{
				delete p_graph;
				p_graph = NULL;
			}
		}	
		if((dwcTime-dwSTime) >= g_d2xSettings.ScreenSaver*60000 )
		{
			if(p_graph == NULL)
				p_graph = new D2Xgraphics();

			ScreenSaverActive = true;
		}
	}

	/*if( m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB )*/
	{
		/*if( m_DefaultGamepad.bAnalogButtons[XINPUT_GAMEPAD_X] )*/
		if(p_input->pressed(GP_TL))
		{
				p_util->TakeScreenshot();
		}
	}

	/*if(AnyButtonDown() == true && g_d2xSettings.generalNotice == 0)
		skip_frames = 2;*/
    
    return S_OK; 
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
	
	// clear screen
	//g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 0.5f, 1.0f );

	CStdString	strlcd1="";
	CStdString	strlcd2="";
	CStdString	strlcd3="";
	CStdString	strlcd4="";

	CStdString mem;
	mem.Format("%d kB",memstat.dwAvailPhys/(1024));
	p_gui->SetKeyValue("freememory",mem);
	p_gui->SetKeyValue("version","0.7.7alpha1");
	p_gui->SetKeyValue("localip",g_d2xSettings.localIP);

	SYSTEMTIME	sltime;
	GetLocalTime(&sltime);
	mem.Format("%2.2d:%2.2d:%2.2d",sltime.wHour,sltime.wMinute,sltime.wSecond);
	p_gui->SetKeyValue("time",mem);
	p_gui->SetKeyValue("statusline",driveState);

	if(ScreenSaverActive)
		p_gui->SetKeyInt("screensaver",1);
		//p_graph->ScreenSaver();

	if(g_d2xSettings.generalDialog != 0)
	{
		switch(g_d2xSettings.generalDialog)
		{
		case D2X_DRIVE_NOT_READY:
			p_gui->SetShowIDs(300);
			break;
		case D2X_DRIVE_NO_DISC:
			p_gui->SetShowIDs(310);
			break;
		case D2X_NO_NETWORK:
			p_gui->SetShowIDs(320);
			break;
		}
	}

	if(g_d2xSettings.generalNotice)
	{
		switch(g_d2xSettings.generalNotice)
		{
			case SCANNING:
				//p_gui->SetGMObject(p_gm);
				p_gui->SetShowIDs(PROCESS_RESCAN);
				//p_gui->RenderGUI(GUI_GAMEMANAGER);
				break;
		};	
	}

	if(mCounter == 11 || mCounter == 200)
	{
		//p_gui->SetKeyValue("statusline",driveState);
		p_gui->SetShowIDs(1);
		p_gui->SetWindowObject(1,p_swin);
		if(mCounter == 200)
		{
			p_gui->SetWindowObject(2,p_swinp);
			p_gui->SetShowIDs(10);
		}
		p_gui->RenderGUI(GUI_MAINMENU);

		if(g_d2xSettings.network_enabled)
			strlcd2.Format("IP: %S - Press A to proceed",g_d2xSettings.localIP);
		else
			strlcd2 = "Press A to proceed";

		strlcd1 = "Welcome to dvd2xbox";
		strlcd3 = driveState;
		strlcd4.Format("Time: %2.2d:%2.2d:%2.2d",sltime.wHour,sltime.wMinute,sltime.wSecond);

		

	}	
	else if(mCounter==13)
	{
		if(!g_d2xSettings.remoteControlled)
		{
			p_gui->SetShowIDs(10);
			p_gui->SetWindowObject(1,p_swin);
			p_gui->SetWindowObject(2,p_swinp);
		}

		if(copytype == UNDEFINED)
			p_gui->SetKeyValue("copytype",L"Normal");
		else if(copytype == DVD2ISORIPPER)
		{
			p_gui->SetShowIDs(400);
			p_gui->SetKeyValue("copytype",L"ISO Ripper");
		}

		//p_gui->SetKeyValue("statusline",driveState);
		p_gui->RenderGUI(GUI_DISKCOPY);

		strlcd1 = "Choose destination:";
		strlcd3 = sinfo.item;
	}
	else if(mCounter==3)
	{
		CStdString text;
		p_gui->SetShowIDs(20);

		if((dvdsize != 0) && (dvdsize > freespace))
		{
			p_gui->SetShowIDs(21);
			text.Format("%d",dvdsize);
			p_gui->SetKeyValue("dvdsize",text);
			text.Format("%d",freespace);
			p_gui->SetKeyValue("freespace",text);
			strlcd1 = "DVDsize > freespace";
		} else if(GetFileAttributes(mDestPath) != -1)
		{
			p_gui->SetShowIDs(22);
			strlcd1 = "path already exists";
		}
		else
			strlcd1 = mDestPath;

		p_gui->SetKeyValue("destination",mDestPath);
		p_gui->RenderGUI(GUI_DISKCOPY);

		strlcd3 = "START to proceed";
		strlcd4 = "BACK to choose again";
	}
	else if(mCounter==4 || mCounter==5)
	{
	
		CStdString text;
		int percent = p_fcopy->GetProgress();
		percent = percent <= 100 ? percent : 100;
		text.Format("%3.0f %%",(float)percent);
		p_gui->SetShowIDs(30);
		p_gui->SetKeyValue("destfile",D2Xfilecopy::c_dest);
		p_gui->SetKeyValue("sourcefile",D2Xfilecopy::c_source);
		p_gui->SetKeyValue("fileprogress",text);
		p_gui->SetKeyInt("fileprogress",percent);

		strlcd1 = "Copy in progress \4"; 
		strlcd2 = D2Xfilecopy::c_source;

		//if((type == DVD || type == GAME || type == UDF) && !copy_retry)
		if(type != CDDA && !copy_retry)
		{
			p_gui->SetShowIDs(31);
			text.Format("%d",dvdsize-p_fcopy->GetMBytes());
			p_gui->SetKeyValue("remainingbytes",text);
			text.Format("%3.0f %%",float(p_fcopy->GetMBytes()*100)/dvdsize);
			p_gui->SetKeyValue("allprogress",text);
			p_gui->SetKeyInt("allprogress",(p_fcopy->GetMBytes()*100)/dvdsize);

			//float res = p_fcopy->GetMBytes()*1000/(timeGetTime()-dwStartCopy);
			text.Format("%1.2f",float(p_fcopy->GetMBytes()*1000)/float(timeGetTime()-dwStartCopy));
			p_gui->SetKeyValue("mbpersecond",text);

			strlcd3.Format("%6d MB to do",dvdsize-p_fcopy->GetMBytes());
		
		}
		//if((copytype == UNDEFINED) && (type != CDDA) )
		if(!D2Xutils::IsSmbPath(mDestPath))
		{
			p_gui->SetShowIDs(32);
			text.Format("%d",p_util->getfreeDiskspaceMB(mDestPath));
			p_gui->SetKeyValue("freespace",text);
			
			strlcd4.Format("%6d MB free",p_util->getfreeDiskspaceMB(mDestPath));
		
		}
		p_gui->RenderGUI(GUI_DISKCOPY);
	}
	else if(mCounter == 6 ||mCounter == 8)
	{
		CStdString text;
		p_gui->SetShowIDs(40);

		if(mCounter == 8)
		{

			p_gui->SetShowIDs(41);
			text.Format("%d",D2Xfilecopy::copy_failed);
			p_gui->SetKeyValue("failed",text);

			strlcd1.Format("Failed: %6d",D2Xfilecopy::copy_failed);
			strlcd3 = "X to cancel";
			strlcd4 = "A to retry";

		}
		else if(g_d2xSettings.enableACL)
		{
		
			p_gui->SetShowIDs(42);

			strlcd1 = "Processing ACL ...";
			strlcd3 = " \4";
		}
		else
		{
           
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


		int hh = (dwEndCopy - dwStartCopy)/3600000;
		int mm = (dwEndCopy - dwStartCopy - hh*3600000)/60000;
		int ss = (dwEndCopy - dwStartCopy - hh*3600000 - mm*60000)/1000;

		text.Format("%2d:%02d:%02d",hh,mm,ss);
		p_gui->SetKeyValue("duration",text);
		
		
		if((type == GAME) && g_d2xSettings.WriteLogfile && g_d2xSettings.enableACL && (copytype != UDF2SMB) && (copytype != DVD2ISORIPPER))
		{
			p_gui->SetShowIDs(51);
		}
		else if((type == GAME) && !g_d2xSettings.WriteLogfile && g_d2xSettings.enableACL && (copytype != UDF2SMB) && (copytype != DVD2ISORIPPER))
		{
			p_gui->SetShowIDs(52);
		}
		else if((copytype == UDF2SMB))
		{
			p_gui->SetShowIDs(53);
		}
		

		strlcd1.Format("Copied: %6d",D2Xfilecopy::copy_ok);
		strlcd2.Format("Failed: %6d",D2Xfilecopy::copy_failed);
		strlcd3.Format("Renamed:%6d",D2Xfilecopy::copy_renamed);
		strlcd4.Format("Duration: %2d:%02d:%02d --- press START to proceed --- ",hh,mm,ss);
		
		p_gui->RenderGUI(GUI_DISKCOPY);
	}
	
	else if(mCounter==21 || mCounter==22 || mCounter == 23 || mCounter == 25 || mCounter == 30 || mCounter == 50 || mCounter == 61 || mCounter == 66 || mCounter == 45 || mCounter == 46 || mCounter == 47 || mCounter == 100 || mCounter == 105 || mCounter == 700 || mCounter == 711)
	{

		CStdString	str_temp;
		
		if(activebrowser == 1)
			p_gui->SetShowIDs(11);
		else
			p_gui->SetShowIDs(22);

		p_gui->SetKeyValue("selectedfilename",info.item);

		if((info.type != BROWSE_DIR) && ((p_util->isdriveD(info.item) && ((type == DVD) || type == GAME)) || !p_util->isdriveD(info.item)))
		{
			str_temp.Format("%d",info.size);
			p_gui->SetKeyValue("selectedfilesize",str_temp);
		}
		else
			p_gui->SetKeyValue("selectedfilesize","#");

		p_gui->SetKeyValue("selectedfiletitle",info.title);
		p_gui->SetShowIDs(1);
		p_gui->SetShowIDs(2);
		p_gui->SetBrowserObject(0, p_browser);
		p_gui->SetBrowserObject(1, p_browser2);
		//p_gui->SetKeyValue("statusline",driveState);


		strlcd1 = "Filemanager";

		

		if(mCounter == 50)
		{
			// drive menu
			p_gui->SetWindowObject(1,p_swin);
			
			if(activebrowser == 1)
			{
				// right
				p_gui->SetShowIDs(102);
			}
			if(activebrowser == 2)
			{
				// left
				p_gui->SetShowIDs(202);
			}
			
		}
		if(mCounter == 61 || mCounter == 66)
		{
			p_gui->SetShowIDs(300);
			CStdString text;
			text.Format("%3.0f %%",(float)p_fcopy->GetProgress());
			p_gui->SetKeyValue("fileprogress",text);
			p_gui->SetKeyInt("fileprogress",p_fcopy->GetProgress());
			p_gui->SetKeyValue("sourcefile",D2Xfilecopy::c_source);
			p_gui->SetKeyValue("destfile",D2Xfilecopy::c_dest);
		}
		if(mCounter == 25 || mCounter == 45)
		{
			p_gui->SetWindowObject(1,p_swin);
			if(activebrowser == 1)
				p_gui->SetShowIDs(104);
			if(activebrowser == 2)
				p_gui->SetShowIDs(204);

		}
		if(mCounter == 100)
		{
			p_gui->SetShowIDs(400);
		}
		if(mCounter == 105)
		{
			p_gui->SetShowIDs(500);
		}
		if(mCounter == 700)
		{
			p_gui->SetShowIDs(600);
			p_gui->SetWindowObject(1,p_swinp);
			p_gui->SetWindowObject(2,p_swin);
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
				text += CStdString("\\\\n") + CStdString(message[i]);
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

	else if(mCounter==70)
	{
		/*p_graph->RenderKeyBoardBG();
		p_keyboard->Render();*/
		p_gui->SetVKObject(p_keyboard);
		p_gui->RenderGUI(GUI_KEYBOARD);
	} 
	
	else if(mCounter == 501)
	{
		p_gui->SetShowIDs(100);
		p_gui->SetWindowObject(1,p_swin);

		if(copytype == UNDEFINED)
			p_gui->SetKeyValue("copytype",L"Normal");
		else if(copytype == DVD2ISORIPPER)
		{
			p_gui->SetShowIDs(400);
			p_gui->SetKeyValue("copytype",L"ISO Ripper");
		}

		//p_gui->SetKeyValue("statusline",driveState);
		p_gui->RenderGUI(GUI_DISKCOPY);

		strlcd1 = "Choose destination:";
		strlcd3 = sinfo.item;
	}
	else if(mCounter == 502)
	{
		p_gui->SetShowIDs(110);

		//p_gui->SetKeyValue("statusline",driveState);
		p_gui->SetKeyValue("destination",mDestPath);
		p_gui->RenderGUI(GUI_DISKCOPY);

		strlcd1 = "Destination Path:";
		strlcd3 = mDestPath;
	}
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


	}
	else if(mCounter == 755 || mCounter== 757 || mCounter == 760)
	{

		INFOitem	info;
	
		p_gm->getInfo(&info);
		p_gui->SetGMObject(p_gm);
		p_gui->SetWindowObject(1,p_gm->getWindowObject());

		p_gui->SetKeyValue("currentdrive",info.cdrive);
		p_gui->SetKeyValue("freedrive",info.isizeMB);
		
		p_gui->SetKeyValue("titlename",info.title);
		p_gui->SetKeyValue("titleid",info.TitleId);
		p_gui->SetKeyValue("titlefiles",info.files);
		p_gui->SetKeyValue("titledirs",info.dirs);
		p_gui->SetKeyValue("titlesize",info.sizeMB);
		p_gui->SetKeyValue("full_path",info.full_path);

		p_gui->SetKeyValue("gmtotalfiles",info.total_files);
		p_gui->SetKeyValue("gmtotaldirs",info.total_dirs);
		p_gui->SetKeyValue("gmtotalsize",info.total_MB);
		p_gui->SetKeyValue("gmtotalitems",info.total_items);

		p_gui->SetShowIDs(1111);
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
		p_gui->SetSGObject(p_gset);
		p_gui->SetShowIDs(p_gset->getShowID());
		p_gui->SetKeyValue("skin",active_skin.c_str());
		p_gui->RenderGUI(GUI_SETTINGS);

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

		switch(g_d2xSettings.generalError)
		{
			case COULD_NOT_AUTH_DVD:
				p_gui->SetShowIDs(200);
				break;
			case SMBTYPE_NOT_SUPPORTED:
				p_gui->SetShowIDs(700);
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
			default:
				break;
		};

		
		
	}


	if(g_d2xSettings.m_bLCDUsed == true)
	{
		g_lcd->SetLine(0,strlcd1);
		g_lcd->SetLine(1,strlcd2);
		g_lcd->SetLine(2,strlcd3);
		g_lcd->SetLine(3,strlcd4);
	}

	// skip some frames to avoid flashing
	/*if(skip_frames > 0)
	{
		skip_frames--;
		return S_OK;
	}*/

	// Let's save some cpu cycles. 25fps should be enough.
	iFPStime = 40-(timeGetTime()-dwFPStime);
	if(iFPStime > 0)
		Sleep(iFPStime);
	dwFPStime = timeGetTime();


	
    // Present the scene
    //m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return S_OK;  
}

//-----------------------------------------------------------------------------


void CXBoxSample::WriteText(char* text)
{
	/*CStdString		strtext;
	strtext = text; */
	p_gui->SetKeyValue("startuptext",CStdString(text));
	p_gui->RenderGUI(GUI_STARTUP);
	/*p_ml->DrawText("D2XDefaultFont",320-strlen(text)/2*11,420,0xffffffff,strtext);
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);*/
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

void CXBoxSample::StartFTPd()
{
	g_d2xSettings.ftpd_enabled = 1;
	if(m_pFileZilla == NULL)
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
	/* filezilla doesn't like to be deleted?  */
  if (m_pFileZilla != NULL)
  {
    
    std::vector<SXFConnection> mConnections;
    std::vector<SXFConnection>::iterator it;

    m_pFileZilla->GetAllConnections(mConnections);

    for(it = mConnections.begin();it != mConnections.end();it++)
    {
      m_pFileZilla->CloseConnection(it->mId);
    }

    m_pFileZilla->Stop();
    delete m_pFileZilla;
    m_pFileZilla = NULL;

  }
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
	wsprintfW(g_d2xSettings.localIP,L"%hs",szIP);
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

void CXBoxSample::StopApp()
{
	/*StopFTPd();
	if(p_dstatus != NULL)
	{
		p_dstatus->StopThread();
		delete p_dstatus;
		p_dstatus = NULL;
	}
	if(p_gset != NULL)
	{
		p_gset->StopAutoDetect();
		delete p_gset;
		p_gset = NULL;
	}*/
	D2Xutils::SetSmartXXRGB(STAT_SHUTDOWN);

	if(g_d2xSettings.m_bLCDUsed == true && g_lcd!=NULL)
	{
		g_lcd->SetLine(0,CStdString(""));
		g_lcd->SetLine(1,CStdString(""));
		g_lcd->SetLine(2,CStdString(""));
		g_lcd->SetLine(3,CStdString(""));
		Sleep(200);
		g_lcd->Stop();
		g_lcd->WaitForThreadExit(INFINITE);
	}
}

void CXBoxSample::Reboot2Remote()
{
	StopApp();
	if(g_d2xSettings.rm_strApp == "-")
		RebootToDash();
	else
		p_util->RunXBE(g_d2xSettings.rm_strApp,NULL);
}

void CXBoxSample::StartNetwork()
{
	if(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE)
	{
		switch(g_d2xSettings.network_assignment)
		{
			case NETWORK_DASH:
				g_network.Initialize(NETWORK_DASH, "","","","");
				break;
			case NETWORK_DHCP:
				g_network.Initialize(NETWORK_DHCP, "","","","");
				break;
			default:
				g_network.Initialize(NETWORK_STATIC,g_d2xSettings.xboxIP,
													g_d2xSettings.netmask,
													g_d2xSettings.gateway,
													g_d2xSettings.DNS);
				break;

		};

		if( g_network.WaitForSetup( 5000 ) )
		{
			DebugOut(" - network fully setup"); 
			getlocalIP();

			if(g_d2xSettings.ftpd_enabled)
			{
				StartFTPd();
			}
			if(g_d2xSettings.autodetect_enabled)
					p_gset->StartAutoDetect();
		}
		else
		{
			DebugOut(" - network init timed out");
		}
	}
}