#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <stdio.h>
#include <debug.h>

#include <algorithm>
#include <iosupport.h>
#include <undocumented.h>
#include <helper.h>
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
#include "keyboard\virtualkeyboard.h"
//#include "ftp\ftp.h"


#define DUMPDIRS	9
char *ddumpDirs[]={"e:\\", "e:\\games", NULL};
char *actionmenu[]={"Copy file/dir","Delete file/dir","Rename file/dir","Create dir","Patch Media check 1/2","Process ACL","Patch from file","Launch XBE",NULL};

class CXBoxSample : public CXBApplicationEx
{
    CXBFont     m_Font;             // Font object
	CXBFont     m_Fontb;  
	//CXBoxDebug	*mpDebug;
    CXBHelp     m_Help;             // Help object
	CXBHelp		m_BackGround;
	CXBFont	    m_FontButtons;      // Xbox Button font
	int			mx;
	int			my;
	int			type;
	int			prevtype;
	int			ini;
	DWORD		dwcTime;
	DWORD		dwTime;
	DWORD		dwStartCopy;
	DWORD		dwEndCopy;
	WCHAR		driveState[100];
	WCHAR		*m_GameTitle;
	WCHAR		*message[1024];
	WCHAR		messagefix[20][200];
	int			mCounter;
	int			mFileCount;
	int			mDirCount;
	int			mLongFilesCount;
	int			mXBECount;
	char		mDestPath[1024];
	char		mBrowse1path[1024];
	char		mBrowse2path[1024];
	char		mDestTitle[1066];
	char		mDestLog[1066];
	char*		mLongPath[100];
	char*		mLongFile[100];
	char*		mXBEs[100];
	char*		dumpDirs[DUMPDIRS+1];
	char*		dumpDirsFS[DUMPDIRS+1];
	char*		hdds[8];
	char*		disks[8];
	char		inidump[255];
	char		mvDirs[5][43];
	HDDBROWSEINFO	info;
	SWININFO		sinfo;
	CIoSupport		io;
	HelperX*		mhelp;
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
	CXBVirtualKeyboard* p_keyboard;
	dvd_reader_t*	dvd;
	dvd_file_t*		vob;
	int				dvdsize;
	int				freespace;
	ULONGLONG		currentdumped;
	float			ogg_quality;
	Xcddb			m_cddb;	
	int				network;
	int				autopatch;
	int				enableACL;
	int				wlogfile;
	bool			useF;
	bool			useG;
	int				activebrowser;
	bool			b_help;
	int				m_Caller;
	int				m_Return;
	char			ftp_ip[40];
	char			ftp_user[10];
	char			ftp_pwd[10];


public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
	virtual bool CreateDirs(char *path);

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

	m_GameTitle = new WCHAR[40];
	mhelp = new HelperX;
	p_patch = new D2Xpatcher;
	p_graph = new D2Xgraphics(&m_Fontb);
	p_browser = new D2Xdbrowser;
	p_browser2 = new D2Xdbrowser;
	p_fcopy = new D2Xfilecopy;
	p_title = new D2Xtitle;
	p_dstatus = new D2Xdstatus;
	p_swin = new D2Xswin;
	p_swinp = new D2Xswin;
	p_log = new D2Xlogger;
	p_acl = new D2Xacl;
	p_util = new D2Xutils;
	p_keyboard = new CXBVirtualKeyboard();
	strcpy(mBrowse1path,"e:\\");
	strcpy(mBrowse2path,"f:\\");
	useF = false;
	useG = false;
	dumpDirsFS[0] = NULL;

}

//-----------------------------------------------------------------------------
// Name: Initialize()
// Desc: Performs initialization
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Initialize()
{

	// Create a font
	//mpDebug = new CXBoxDebug(0, 0,40.0,80.0);
    if( FAILED( m_Font.Create( "Font.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
	if( FAILED( m_Fontb.Create( "debugfont.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;
	if( FAILED( m_BackGround.Create( "background.xpr" ) ) )
	{
        return XBAPPERR_MEDIANOTFOUND;
	}
	// Xbox dingbats (buttons) 24
    if( FAILED( m_FontButtons.Create( "Xboxdings_24.xpr" ) ) )
        return XBAPPERR_MEDIANOTFOUND;

	if( FAILED(p_keyboard->Initialize()))
		return XBAPPERR_MEDIANOTFOUND;
	// Draw a gradient filled background
    //RenderGradientBackground( dwTopColor, dwBottomColor );
	m_BackGround.Render( &m_Font, 0, 0 );
	m_pd3dDevice->Present(NULL,NULL,NULL,NULL);

	
	// read config file
	if(mhelp->readIni("d:\\dvd2xbox.xml"))
	{
		ini = 0;
		int i=0;
		do
		{
			dumpDirs[i] = ddumpDirs[i];
		}
		while(ddumpDirs[i++]!=NULL);
		
	} else {
		char temp[20];
		ini = 1;
		useF = atoi(mhelp->getIniValue("main","enableFdrive")) ? true : false;
		useG = atoi(mhelp->getIniValue("main","enableGdrive")) ? true : false;
		
		int x = 0;
		for(int i=0;i<DUMPDIRS;i++)
		{
			sprintf(temp,"dumpdir%d",i+1);
			if(mhelp->getIniValue("main",temp)==NULL)
			{
				dumpDirs[x] = NULL;
				break;
			}
			if(!_strnicmp((char *)mhelp->getIniValue("main",temp),"e:",2) || (!_strnicmp((char *)mhelp->getIniValue("main",temp),"f:",2) && useF) || (!_strnicmp((char *)mhelp->getIniValue("main",temp),"g:",2) && useG))
			{
				// one extra char for the backslash if omitted
				dumpDirs[x] = new char[strlen(mhelp->getIniValue("main",temp)+2)];
				dumpDirs[x] = (char *)mhelp->getIniValue("main",temp);
				x++;
			}
		} 
		//dumpDirs[x] = NULL;
	}

	ogg_quality = (float)atof(mhelp->getIniValue("CDDA","oggquality"));
	D2Xfilecopy::f_ogg_quality = ogg_quality;
	autopatch = atoi(mhelp->getIniValue("UDF","autopatch"));
	enableACL = atoi(mhelp->getIniValue("UDF","enableACL"));
	network = atoi(mhelp->getIniValue("network","enabled"));
	wlogfile = atoi(mhelp->getIniValue("main","logfile"));
	strcpy(D2Xtitle::c_cddbip,mhelp->getIniValue("network","cddbip"));

	p_fcopy->setExcludePatterns(mhelp->getIniValue("UDF","excludeFiles"),mhelp->getIniValue("UDF","excludeDirs"));
	//strcpy(ftp_ip,mhelp->getIniValue("network","ftpip"));
	//strcpy(ftp_user,mhelp->getIniValue("network","ftpuser"));
	//strcpy(ftp_pwd,mhelp->getIniValue("network","ftppwd"));
	
	// Remap the CDROM, map E & F Drives

	io.Remap("C:,Harddisk0\\Partition2");
	io.Remap("E:,Harddisk0\\Partition1");
	io.Remount("D:","Cdrom0");
	int x = 0;
	int y = 0;
	hdds[x++] = "e:\\";
	//hdds[x++] = "f:\\";
	hdds[x] = NULL;
	disks[y++] = "d:\\";
	disks[y++] = "e:\\";
	//disks[y++] = "f:\\";
	//disks[y++] = "ftp:";
	disks[y] = NULL;

	
	if(useF)
	{
		io.Remap("F:,Harddisk0\\Partition6");
		hdds[x++] = "f:\\";
		hdds[x] = NULL;
		disks[y++] = "f:\\";
		disks[y] = NULL;
	}

	if(useG)
	{
		io.Remap("G:,Harddisk0\\Partition7");
		hdds[x++] = "g:\\";
		hdds[x] = NULL;
		disks[y++] = "g:\\";
		disks[y] = NULL;
	}
		 
	mCounter = 0;
	mLongFilesCount = 0;
	mXBECount = 0;
	currentdumped = 0;
	type = 0;
	mx = 1;
	activebrowser = 1;
	b_help = false;
	

	p_dstatus->GetDriveState(driveState,type);
	dwTime = timeGetTime();

	char* ip=(char*)mhelp->getIniValue("network","xboxip");
	char* netm=(char*)mhelp->getIniValue("network","netmask");
	char* gatew=(char*)mhelp->getIniValue("network","gateway");
	if(ip==NULL)
		ip="\0";
	if(netm==NULL)
		netm="\0";
	if(gatew==NULL)
		gatew="\0";

	if(network)
	{
		
		if (!m_cddb.InitializeNetwork(ip,netm ,gatew ))
		{
			network = 0;
			D2Xtitle::i_network = 0;
			DPf_H("Could not init network");
		} else {
			DPf_H("network initialized");
			D2Xtitle::i_network = 1;
		}
		
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::FrameMove()
{
	switch(mCounter)
	{
		case 0:
			if(mhelp->pressSTART(m_DefaultGamepad) || mhelp->pressA(m_DefaultGamepad))
			{	
				io.CloseTray();
				io.Remount("D:","Cdrom0");
				
				int a=0;
				while(dumpDirsFS[a] != NULL)
				{
					delete dumpDirsFS[a];
					dumpDirsFS[a] = NULL;
					a++;
				}

				// determine free disk space
				int i=0;
				char temp[40];
				while(dumpDirs[i]!=NULL)
				{
					p_util->MakePath(dumpDirs[i]);
					if(!(mhelp->getfreeDiskspace(dumpDirs[i],temp)))
						strcpy(temp, "");
					dumpDirsFS[i] = new char[strlen(temp)+1];
					strcpy(dumpDirsFS[i],temp);
					i++;
				}
				dumpDirsFS[i]=NULL;
				mCounter++;
			} 
			if(mhelp->pressB(m_DefaultGamepad))
			{
				io.CloseTray();
				io.Remount("D:","Cdrom0");
				strcpy(mBrowse1path,"e:\\");
				//strcpy(mBrowse1path,"smb://wiso:Warp99@wiso3");
				if(useF)
                    strcpy(mBrowse2path,"f:\\");
				else
					strcpy(mBrowse2path,"e:\\");

				mCounter=21;
				D2Xdbrowser::renewAll = true;
			}
			if(mhelp->pressX(m_DefaultGamepad))
			{
				
				
				/*
				p_log->setLogFilename("f:\\test\\dvd2xbox.log");
				p_log->enableLog(true);
				p_acl->processACL("f:\\test");
				*/

				//mCounter = 70;
				/*
				ftp theFtpConnection;
				theFtpConnection.DoOpen("192.168.1.30");
				theFtpConnection.DoLogin("user wiso:Warp99");
				theFtpConnection.DoList("dir");
				theFtpConnection.DoClose();
				*/


				/*
				CdIo *cdio = cdio_open ("f:\\videocd.nrg", DRIVER_UNKNOWN);
				if(cdio != NULL)
				{
					dwTime = timeGetTime();
					wsprintfW(driveState,L"cdio opened");
				} else {
					dwTime = timeGetTime();
					wsprintfW(driveState,L"could not open cdio");
					break;
				}
				track_t first_track_num = cdio_get_first_track_num(cdio);
				track_t num_tracks      = cdio_get_num_tracks(cdio);
				int j, i=first_track_num;
				DPf_H("CD-ROM Track List (%i - %i)\n", first_track_num, num_tracks);

				DPf_H("  #:  LSN\n");
  
				for (j = 0; j < num_tracks; i++, j++) {
					lsn_t lsn = cdio_get_track_lsn(cdio, i);
					if (CDIO_INVALID_LSN != lsn)
						DPf_H("%3d: %06d\n", (int) i, lsn);
				}
				DPf_H("%3X: %06d  leadout\n", CDIO_CDROM_LEADOUT_TRACK, 
				cdio_get_track_lsn(cdio, CDIO_CDROM_LEADOUT_TRACK));
				cdio_destroy(cdio);
				*/

				/*
				dvdnav_t*	dvdnav;
				int			dvdtitles=0;
				int			dvdprogs=0;
				if(DVDNAV_STATUS_OK == dvdnav_open(&dvdnav, "\\Device\\Cdrom0"))
				{
					dvdnav_get_number_of_titles(dvdnav,&dvdtitles);
					dvdnav_close(dvdnav);
					dwcTime = timeGetTime();
					dwTime = dwcTime;
					wsprintfW(driveState,L"Found %d titles",dvdtitles);
				}
				*/
				//char *test[] = {"appname.exe","test.vob","0xc0","f:\\audio.ac3","0x80"};
				//char *test[] = {"appname.exe","test.vob"};
				//char *test[] = {"appname.exe","test.vob","0xE0","f:\\video.m2v"};
				//bbDemux(5,test);

				/*
				CFileSmb* m_pSMB = new CFileSmb;
				if(m_pSMB->Open("pups","pups","Pentium/192.168.1.20/MP3","test.txt",445,false))
				{
					m_pSMB->Close();
					dwTime = timeGetTime();
					wsprintfW(driveState,L"File opened");
				} else {
					dwTime = timeGetTime();
					wsprintfW(driveState,L"could not open file");
				}

				*/

			}
			break;
		case 1:
			mhelp->processList(m_DefaultGamepad ,m_DefaultIR_Remote,mx ,my);
			if(mx <= 0) { mx = mhelp->getnList(dumpDirs);}
			if(mx > mhelp->getnList(dumpDirs)) { mx = 1;}
			if(mhelp->pressA(m_DefaultGamepad) || mhelp->pressSTART(m_DefaultGamepad) || mhelp->IRpressSELECT(m_DefaultIR_Remote))
			{
				strcpy(mDestPath,dumpDirs[mx-1]);
				mhelp->addSlash(mDestPath);
				mhelp->getfreeDS(mDestPath, freespace);
				dvdsize = mhelp->getusedDSul("D:\\");
				strcat(mDestPath,p_title->GetNextPath(mDestPath,type));
				mCounter = 3;
				/*
				WCHAR wsFile[1024];
				swprintf(  wsFile,L"%S", mDestPath );
				p_keyboard->Reset();
				p_keyboard->SetText(wsFile);
				mCounter=70;
				m_Caller = 1;
				m_Return = 2;*/
				//sprintf(mDestTitle,"%sdefault.xbe",mDestPath);
				//sprintf(mDestLog,"%sdvd2xbox.log",mDestPath);	
				//p_log->setLogFilename(mDestLog);
			
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter--;
			}

			break;
			
		case 2:
			wsprintf(mDestPath,"%S",p_keyboard->GetText());
			mCounter=3;
			break;
			
		case 3:
			
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START))
			{
				if(GetFileAttributes(mDestPath) == -1)
				{
					mhelp->addSlash(mDestPath);
					//sprintf(mDestTitle,"%sdefault.xbe",mDestPath);
					sprintf(mDestLog,"%sdvd2xbox.log",mDestPath);	
					p_log->setLogFilename(mDestLog);
					mCounter++;
				}
			} else if(mhelp->pressX(m_DefaultGamepad)) 
			{
				WCHAR wsFile[1024];
				swprintf(  wsFile,L"%S", mDestPath );
				p_keyboard->Reset();
				p_keyboard->SetText(wsFile);
				mCounter=70;
				m_Caller = 3;
				m_Return = 2;
			}else if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter=1;
			} 
			break;
		case 4:

			if(wlogfile)
			{
				p_log->enableLog(true);
			}

			dwStartCopy = timeGetTime();
			
			if(type==DVD)
			{	
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);

			} else if(type==CDDA)
			{
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:\\");
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
			
			} else if(type==ISO ||type==VCD ||type==SVCD)
			{
				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:\\");
				strcpy(info.name,"\0");
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);
			
			} else //if(type==GAME)
			{

				CreateDirectory(mDestPath,NULL);
				info.type = BROWSE_DIR;
				strcpy(info.item,"d:");
				strcpy(info.name,"\0");
				p_fcopy->Create();
				p_fcopy->FileCopy(info,mDestPath,type);

			}
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			mCounter++;
			break;
		case 5:
			if(D2Xfilecopy::b_finished)
			{
				SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				mCounter = 6;
			}
			/*
			if(mhelp->pressX(m_DefaultGamepad))
			{
				p_fcopy->StopThread();
				mCounter = 7;
			}*/
			
			break;

		case 6:
			dwEndCopy = timeGetTime();

			if(enableACL && (type == GAME))
			{
				p_acl->processACL(mDestPath);
			} 
			else if(autopatch && (type == GAME))
			{
				for(int i=0;i<D2Xpatcher::mXBECount;i++)
				{
					p_log->WLog(L"checking %hs",D2Xpatcher::mXBEs[i]);
					ULONG mt;
					if(p_patch->SetMediatype(D2Xpatcher::mXBEs[i],mt,"FF010040"))
					{
						p_log->WLog(L"Setting media type from 0x%08x to 0x400001FF",mt);
					} else {
						p_log->WLog(L"Error while setting media type");
					}
					DPf_H("Patching");
					int ret;
					for(int n=0;n<2;n++)
					{
                        ret = p_patch->PatchMediaStd(D2Xpatcher::mXBEs[i],n);
						switch(ret)
						{
						case FOUND_OK:
							p_log->WLog(L"%hs removed",D2Xpatcher::p_hexsearch[n]);
							break;
						case FOUND_ERROR:
							p_log->WLog(L"%hs found but couldn't be removed",D2Xpatcher::p_hexsearch[n]);
							break;
						case NOT_FOUND:
							p_log->WLog(L"%hs not found",D2Xpatcher::p_hexsearch[n]);
							break;
						default:
							break;
						}
					}

					p_log->WLog(L"");
					
				}
			}
			
			if(atoi(mhelp->getIniValue("main","autoeject")))
                io.EjectTray();
			p_log->enableLog(false);
			mCounter++;
			break;
		case 7:
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START))
			{
				mCounter=0;
				type=0;
			}
			break;
		
		case 10:
			mCounter++;
			break;
		case 20:
			//if(mhelp->pressA(m_DefaultGamepad) || mhelp->pressSTART(m_DefaultGamepad) || mhelp->IRpressSELECT(m_DefaultIR_Remote))
			{
				mCounter++;
				strcpy(mBrowse1path,"e:\\");
				strcpy(mBrowse2path,"f:\\");
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				p_browser->resetDirBrowser();
				p_browser2->resetDirBrowser();
				mCounter=0;
			}
			break;
		case 21:
			if(!activebrowser)
			{
				activebrowser = 1;
				info = p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}
			if(!strncmp(mBrowse1path,"ftp:",4))
			{
				if(!(p_browser->FTPisConnected()))
				{
					if(!(p_browser->FTPconnect(ftp_ip,ftp_user,ftp_pwd)))
						strcpy(mBrowse1path,"e:\\");
				}
			} else if(!strncmp(mBrowse2path,"ftp:",4))
			{
				if(!(p_browser2->FTPisConnected()))
				{
					if(!(p_browser2->FTPconnect(ftp_ip,ftp_user,ftp_pwd)))
						strcpy(mBrowse2path,"f:\\");
				}
			} else 
			{
				if(strncmp(mBrowse2path,"ftp:",4) && (p_browser2->FTPisConnected()))
					p_browser2->FTPclose();
				if(strncmp(mBrowse1path,"ftp:",4) && (p_browser->FTPisConnected()))
					p_browser->FTPclose();
			}

			if(D2Xdbrowser::renewAll == true)
			{
				p_browser->processDirBrowser(20,mBrowse1path,m_DefaultGamepad,m_DefaultIR_Remote,type);
				D2Xdbrowser::renewAll = true;
				p_browser2->processDirBrowser(20,mBrowse2path,m_DefaultGamepad,m_DefaultIR_Remote,type);
			}

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
				mCounter=50;
			}
			
			
		
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				//p_browser->resetDirBrowser();
				//p_browser2->resetDirBrowser();
				//activebrowser = 1;
				mCounter=0;
			}
			break;
		case 22:
			// Delete file/directory
			if(mhelp->pressSTART(m_DefaultGamepad))
			{
				if(info.type == BROWSE_DIR)
				{
                    mhelp->DelTree(info.item);
				} else if(info.type == BROWSE_FILE) {
					DeleteFile(info.item);
				}
				mCounter = 21;
				p_browser->Renew();
				p_browser2->Renew();
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter--;
			}
			break;
		case 25:
			sinfo = p_swin->processScrollWindow(m_DefaultGamepad);
			if(mhelp->pressA(m_DefaultGamepad))
			{
				if(!strcmp(sinfo.item,"Copy file/dir"))
					mCounter = 60;
				else if(!strcmp(sinfo.item,"Delete file/dir"))
				{
					if(strcmp(info.name,".."))
						mCounter = 22;
				}
				else if(!strcmp(sinfo.item,"Patch Media check 1/2"))
					mCounter = 40;
				else if(!strcmp(sinfo.item,"Patch from file"))
				{
					p_swin->initScrollWindow(p_patch->getPatchFiles(),20,false);
					mCounter = 45;
				}
				else if(!strcmp(sinfo.item,"Launch XBE"))
					mCounter = 30;
				else if(!strcmp(sinfo.item,"Rename file/dir"))
				{
					WCHAR wsFile[1024];
					WCHAR title[50];
					char temp[1024];
					
					if(info.type == BROWSE_DIR)
					{
						swprintf(  wsFile,L"%S", info.item );
						strcpy(temp,info.item);
						mhelp->addSlash(temp);
						strcat(temp,"default.xbe");
						if(_access(temp,00)!=-1)
						{
							p_title->getXBETitle(temp,title);
							strcpy(temp,info.item);
							char* p_xbe = strrchr(temp,'\\');
							p_xbe[0] = 0;
							swprintf(  wsFile,L"%S\\", temp );
							wcscat(wsFile,title);
						}
					} else if(info.type == BROWSE_FILE)
					{
						swprintf(  wsFile,L"%S", info.item );
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
					swprintf(  wsFile,L"%S", info.item );
					p_keyboard->SetText(wsFile);
					mCounter = 70;
					m_Caller = 25;
					m_Return = 90;
				} else if(!strcmp(sinfo.item,"Process ACL"))
				{
					mCounter = 100;
				}
			}
			if(m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK) {
				mCounter=21;
			}
			break;
		case 30:
			// launch xbe
			if(strstr(info.name,".xbe") || strstr(info.name,".XBE"))
			{
				if(mhelp->pressSTART(m_DefaultGamepad))
				{
					char lxbe[50];
					sprintf(lxbe,"d:\\%s",info.name);
					mhelp->LaunchXbe(info.path,lxbe);
				}
			} else 
				mCounter = 21;
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter=21;
			}
			break;
		case 40:
			{
			char *reverse = new char[strlen(info.item)+1];
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
			delete[] reverse;
			}
			break;
		case 41:
			if(mhelp->pressA(m_DefaultGamepad))
			{
				
				int n=0;
				while(message[n]!=NULL)
				{
					delete[] message[n];
					n++;
				}
				mCounter = 21;
			
			}
			break;
		case 45:
			sinfo = p_swin->processScrollWindow(m_DefaultGamepad);
			if(mhelp->pressA(m_DefaultGamepad) && strcmp(sinfo.item,"No files"))
			{
				int i=0;
				while(message[i] != NULL)
				{
					delete message[i];
					message[i] = NULL;
				}
				mCounter = 46;
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				p_swin->initScrollWindow(actionmenu,20,false);
				mCounter=25;
			}
			break;
		case 46:
			p_patch->patchXBEfromFile(info,sinfo.item,message);
			mCounter = 47;
			break;
		case 47:
			if(mhelp->pressA(m_DefaultGamepad)) {
				//p_swin->initScrollWindow(actionmenu,20,false);
				//mCounter=25;
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
			mhelp->processList(m_DefaultGamepad ,m_DefaultIR_Remote,mx ,my);
			if(mx <= 0) { mx = mhelp->getnList(disks);}
			if(mx > mhelp->getnList(disks)) { mx = 1;}
			if(mhelp->pressA(m_DefaultGamepad) || mhelp->pressSTART(m_DefaultGamepad) || mhelp->IRpressSELECT(m_DefaultIR_Remote))
			{
				if(activebrowser == 1)
				{
					strcpy(mBrowse1path,disks[mx-1]);
					p_browser->Renew();
				}
				else 
				{
					strcpy(mBrowse2path,disks[mx-1]);
					p_browser2->Renew();
				}
				mCounter = 21;
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
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_LOWEST);
			mCounter = 61;
			break;
		case 61:
			if(D2Xfilecopy::b_finished)
			{
				mCounter = 21;
				SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
				//activebrowser = 0;
				D2Xdbrowser::renewAll = true;
				//p_browser->Renew();
				//p_browser2->Renew();
			}
			break;
		case 70:
			if(mhelp->pressA(m_DefaultGamepad))
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
			if(p_keyboard->IsConfirmed())
				mCounter = m_Return;
			break;
		case 80:
			{
			char newitem[1024];

			wsprintf(newitem,"%S",p_keyboard->GetText());
			if(_access(newitem,00) == -1)
			{
				MoveFileEx(info.item,newitem,MOVEFILE_COPY_ALLOWED);
			}
			mCounter = 21;
			D2Xdbrowser::renewAll = true;
			}
			break;
		case 90:
			{
			char newitem[1024];
			wsprintf(newitem,"%S",p_keyboard->GetText());
			//if((newitem[1] == ':') && !(mhelp->isdriveD(newitem)))
			DPf_H("Item %c , newitem %s",newitem[1],newitem);
            //CreateDirs(newitem);
			p_util->MakePath(newitem);
			mCounter = 21;
			D2Xdbrowser::renewAll = true;
			}
			break;
		case 100:
			mCounter = 105;
			if(info.type == BROWSE_DIR)
			{
				char		acl_dest[1024];
				strcpy(acl_dest,info.item);
				mhelp->addSlash(acl_dest);
				strcat(acl_dest,"default.xbe");
				if(GetFileAttributes(acl_dest) != -1)
				{
					if(wlogfile)
					{
						strcpy(acl_dest,info.item);
						mhelp->addSlash(acl_dest);
						strcat(acl_dest,"dvd2xbox.log");	
						p_log->setLogFilename(acl_dest);
						p_log->enableLog(true);
						DPf_H("logfile: %s",acl_dest);
					}
					p_acl->processACL(info.item);
					p_log->enableLog(false);
					D2Xdbrowser::renewAll = true;
					mCounter = 21;
				} 
			} 
			break;
		case 105:
			if(mhelp->pressA(m_DefaultGamepad))
				mCounter = 21;
			break;
		default:
			break;
		
	}

	if((mhelp->pressBLACK(m_DefaultGamepad)) && (mCounter > 20))
	{
		if(b_help)
			b_help = false;
		else
			b_help = true;
	}
	if((mCounter <=20 ) && (b_help == true))
	{
		b_help = false;
	}
	
    dwcTime = timeGetTime();
	if((mCounter<4 || mCounter == 21) && ((dwcTime-dwTime) >= 2000))
	{
	
		dwTime = dwcTime;
		p_dstatus->GetDriveState(driveState,type);
		if((type != prevtype) && (type != 0) )
		{
			D2Xdbrowser::renewAll = true;
			if(mhelp->isdriveD(mBrowse1path))
				p_browser->resetDirBrowser();
			else if(mhelp->isdriveD(mBrowse2path))
				p_browser2->resetDirBrowser();
			prevtype = type;
		}
	
	}
    
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the scene
//-----------------------------------------------------------------------------
HRESULT CXBoxSample::Render()
{
    // Draw a gradient filled background
    //RenderGradientBackground(  0xff0000ff, 0xff000000);
	m_BackGround.Render( &m_Font, 0, 0 );
	if(mCounter==0)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Welcome to DVD2Xbox 0.5.2tc1" );
		m_FontButtons.DrawText( 80, 160, 0xffffffff, L"A");
		m_Font.DrawText( 240, 160, 0xffffffff, L" Copy DVD/CD-R to HDD" );
		m_FontButtons.DrawText( 80, 200, 0xffffffff, L"B");
		m_Font.DrawText( 240, 200, 0xffffffff, L" DVD/CD-R/HDD browser" );
		m_FontButtons.DrawText( 80, 240, 0xffffffff, L"E8F8J");
		m_Font.DrawText( 240, 240, 0xffffffff, L" back to dashboard" );
		if(!ini)
		{
			m_Font.DrawText(80,300,0xffffffff,L"Could not process config file dvd2xbox.xml" );
			m_Font.DrawText( 80, 320, 0xffffffff, L"Using defaults" );
		}
		m_Font.DrawText( 60, 435, 0xffffffff, driveState );
		
	}
	else if(mCounter==1)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Choose dump directory:" );
		mhelp->showList(60,120,mx,m_Font,dumpDirsFS);
		mhelp->showList(240,120,mx,m_Font,dumpDirs);
		m_Font.DrawText( 60, 435, 0xffffffff, driveState );
	}
	else if(mCounter==3)
	{
		WCHAR temp[60];
		WCHAR temp2[1024];
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Destination path:" );
		if((dvdsize != 0) && (dvdsize > freespace))
		{
			m_Font.DrawText( 60, 140, 0xffffffff, L"Warning:" );
			wsprintfW(temp,L"DVD size: %d MB > free space: %d MB",dvdsize,freespace);
			m_Font.DrawText( 60, 170, 0xffffffff, temp );
		} else if(GetFileAttributes(mDestPath) != -1)
		{
			m_Font.DrawText( 60, 140, 0xffffffff, L"Warning:" );
			m_Font.DrawText( 60, 170, 0xffffffff, L"The path you specified already exists." );
		}
		wsprintfW(temp2,L"%hs",mDestPath);
		m_Fontb.DrawText( 60, 210, 0xffffffff, temp2 );
		m_FontButtons.DrawText( 60, 260, 0xffffffff, L"G");
		m_Font.DrawText( 110, 260, 0xffffffff, L" proceed" );
		m_FontButtons.DrawText( 60, 300, 0xffffffff, L"C");
		m_Font.DrawText( 110, 300, 0xffffffff, L"  change dir" );
		m_FontButtons.DrawText( 60, 340, 0xffffffff, L"H");
		m_Font.DrawText( 110, 340, 0xffffffff, L"  choose drive again" );
	}
	else if(mCounter==4 || mCounter==5)
	{
		WCHAR dest[70];
		WCHAR remain[50];
		WCHAR free[50];
		if(wcslen(D2Xfilecopy::c_dest) > 66)
		{
			wcsncpy(dest,D2Xfilecopy::c_dest,66);
			dest[66] = L'\0';
		} else {
			wcscpy(dest,D2Xfilecopy::c_dest);
		}
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Main copy module" );
		p_graph->RenderPopup();
		m_Font.DrawText(55, 160, 0xffffffff, L"Copy:" );
		m_Fontb.DrawText(55, 205, 0xffffffff, D2Xfilecopy::c_source);
		m_Fontb.DrawText(55, 220, 0xffffffff, dest);
		p_graph->RenderProgressBar(240,float(p_fcopy->GetProgress()));
		if(type == DVD || type == GAME)
		{
			p_graph->RenderProgressBar(265,float(((p_fcopy->GetMBytes())*100)/dvdsize));
			wsprintfW(remain,L"Remaining MBytes to copy:  %6d MB",dvdsize-p_fcopy->GetMBytes());
			m_Fontb.DrawText( 60, 320, 0xffffffff, remain);
		}
		wsprintfW(free,L"Remaining free space:      %6d MB",mhelp->getfreeDSMB(mDestPath));
		m_Fontb.DrawText( 60, 350, 0xffffffff, free );
	}
	else if(mCounter == 6)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Main copy module" );
		p_graph->RenderPopup();
		if(enableACL)
		{
			m_Font.DrawText(55, 160, 0xffffffff, L"Processing ACL ..." );
		}
		else
		{
            m_Font.DrawText(55, 160, 0xffffffff, L"Patching xbe's ..." );
		}
	}
	else if(mCounter == 7)
	{
		WCHAR copy[50];
		WCHAR renamed[50];
		WCHAR failed[50];
		WCHAR mcrem1[50];
		WCHAR mcremL[50];
		WCHAR mcremS[50];
		WCHAR duration[50];
		wsprintfW(copy,    L"Files copied:   %6d",D2Xfilecopy::copy_ok);
		wsprintfW(failed,  L"Failed to copy: %6d",D2Xfilecopy::copy_failed);
		wsprintfW(renamed, L"Files renamed:  %6d",D2Xfilecopy::copy_renamed);
		int hh = (dwEndCopy - dwStartCopy)/3600000;
		int mm = (dwEndCopy - dwStartCopy - hh*3600000)/60000;
		int ss = (dwEndCopy - dwStartCopy - hh*3600000 - mm*60000)/1000;

		wsprintfW(duration,L"Copy duration (HH:MM:SS): %2d:%2d:%2d",hh,mm,ss);

			
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Copy report:" );
		m_Fontb.DrawText( 60, 140, 0xffffffff, copy );
		m_Fontb.DrawText( 60, 170, 0xffffffff, failed );
		m_Fontb.DrawText( 60, 200, 0xffffffff, renamed );
		m_Fontb.DrawText( 60, 230, 0xffffffff, duration );
		
		if((type == GAME) && autopatch && !enableACL)
		{
			wsprintfW(mcrem1,L"Files with MediaCheck 1:                %2d",D2Xpatcher::mXBECount);
			wsprintfW(mcremL,L"Files with MediaCheck 2 (long string):  %2d",D2Xpatcher::mcheck[0]);
			wsprintfW(mcremS,L"Files with MediaCheck 2 (short string): %2d",D2Xpatcher::mcheck[1]);
		
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );
			m_Fontb.DrawText( 60, 310, 0xffffffff, mcremL );
			m_Fontb.DrawText( 60, 340, 0xffffffff, mcremS );
		}
		else if((type == GAME) && wlogfile && enableACL)
		{
			wsprintfW(mcrem1,L"ACL processed. Read the logfile to get more informations.");
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );
		}
		else if((type == GAME) && !wlogfile && enableACL)
		{
			wsprintfW(mcrem1,L"ACL processed. Enable logfile writing to get more informations.");
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcrem1 );
		}
		
		m_Font.DrawText( 60, 435, 0xffffffff, L"press START to proceed" );
	
	}
	
	else if(mCounter==21 || mCounter == 25 || mCounter == 50 || mCounter == 61 || mCounter == 45 || mCounter == 100 || mCounter == 105)
	{
		p_graph->RenderBrowserFrames(activebrowser);
		WCHAR temp[1024];
		WCHAR temp2[30];
		wsprintfW(temp,L"%hs",info.item);
		wsprintfW(temp2,L"Filesize: %d KB",info.size);
		m_Fontb.DrawText( 55, 30, 0xffffffff, temp );
		if((info.type != BROWSE_DIR) && ((mhelp->isdriveD(info.item) && ((type == DVD) || type == GAME)) || !mhelp->isdriveD(info.item)))
			m_Fontb.DrawText( 55, 45, 0xffffffff, temp2 );
		m_Fontb.DrawText( 55, 60, 0xffffffff, info.title);
		p_browser->showDirBrowser(20,55,95,0xffffffff,0xffffff00,m_Fontb);
		p_browser2->showDirBrowser(20,330,95,0xffffffff,0xffffff00,m_Fontb);
		m_Font.DrawText( 60, 435, 0xffffffff, driveState );
		if(mCounter == 50)
		{
			p_graph->RenderPopup();
			m_Font.DrawText(250, 155, 0xffffffff, L"Choose drive:" );
			mhelp->showList(250,180,mx,m_Font,disks);
		}
		if(mCounter == 61)
		{
			p_graph->RenderPopup();
			WCHAR dest[70];
			if(wcslen(D2Xfilecopy::c_dest) > 66)
			{
				wcsncpy(dest,D2Xfilecopy::c_dest,66);
				dest[66] = L'\0';
			} else {
				wcscpy(dest,D2Xfilecopy::c_dest);
			}
			m_Font.DrawText(55, 160, 0xffffffff, L"Copy:" );
			m_Fontb.DrawText(55, 205, 0xffffffff, D2Xfilecopy::c_source);
			m_Fontb.DrawText(55, 220, 0xffffffff, dest);
			p_graph->RenderProgressBar(240,float(p_fcopy->GetProgress()));
		}
		if(mCounter == 25 || mCounter == 45)
		{
			p_graph->RenderBrowserPopup(activebrowser);
			if(activebrowser == 1)
                p_swin->showScrollWindow(330,100,32,0xffffffff,0xffffff00,m_Fontb);
			if(activebrowser == 2)
                p_swin->showScrollWindow(55,100,32,0xffffffff,0xffffff00,m_Fontb);

		}
		if(mCounter == 100)
		{
			p_graph->RenderPopup();
			m_Font.DrawText(55, 160, 0xffffffff, L"Processing ACL ..." );
		}
		if(mCounter == 105)
		{
			p_graph->RenderPopup();
			m_Font.DrawText(55, 160, 0xffffffff, L"Failed to process ACL list");
			m_Font.DrawText(55, 200, 0xffffffff, L"Tried on file or dir without default.xbe ?");
		}
	}
	else if(mCounter==22)
	{
		p_graph->RenderMainFrames();
		WCHAR temp[1024];
		wsprintfW(temp,L"%hs",info.item);
		m_Font.DrawText( 80, 30, 0xffffffff, L"Please confirm deletion of:" );
		m_Font.DrawText( 60, 140, 0xffffffff, temp );
		m_FontButtons.DrawText( 80, 200, 0xffffffff, L"G");
		m_Font.DrawText( 130, 200, 0xffffffff, L"  delete" );
		m_FontButtons.DrawText( 80, 240, 0xffffffff, L"H");
		m_Font.DrawText( 130, 240, 0xffffffff, L"  cancel" );
	}
	else if(mCounter==30)
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
	}
	else if(mCounter==41)
	{
		p_graph->RenderMainFrames();
		WCHAR temp[1024];
		wsprintfW(temp,L"checking: %hs",info.item);
		m_Font.DrawText( 80, 30, 0xffffffff, L"Searching for media checks:" );
		m_Fontb.DrawText( 60, 140, 0xffffffff, temp );
		m_Fontb.DrawText( 60, 170, 0xffffffff, messagefix[0]);
		int n=0;
		while(message[n]!=NULL)
		{
			m_Fontb.DrawText( 60, 170+m_Fontb.GetFontHeight()*(n+1), 0xffffffff, message[n]);
			n++;
		}
		m_Fontb.DrawText( 60, 170+m_Fontb.GetFontHeight()*(n+1), 0xffffffff, L"Press A to proceed");
	}
	else if(mCounter == 46)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Searching for media checks:" );
		p_graph->RenderPopup();
		m_Font.DrawText(55, 160, 0xffffffff, L"Patching ..." );
	}
	else if(mCounter==47)
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
	} else if(mCounter==70)
	{
		p_graph->RenderKeyBoardBG();
		p_keyboard->Render();
	}

	if(b_help)
	{
		p_graph->RenderHelpFrame();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Helpscreen, press BLACK to proceed" );
		m_FontButtons.DrawText( 60, 140, 0xffffffff, L"A");
		m_Font.DrawText( 90, 140, 0xffffffff, L" select" );
		m_FontButtons.DrawText( 60, 170, 0xffffffff, L"E");
		m_Font.DrawText( 90, 170, 0xffffffff, L" action menu" );
		//m_FontButtons.DrawText( 300, 140, 0xffffffff, L"C");
		//m_Font.DrawText( 330, 140, 0xffffffff, L" delete file/directory" );
		//m_FontButtons.DrawText( 300, 170, 0xffffffff, L"D");
		//m_Font.DrawText( 330, 170, 0xffffffff, L" launch default.xbe" );
		m_FontButtons.DrawText( 60, 210, 0xffffffff, L"F");
		m_Font.DrawText( 90, 210, 0xffffffff, L" select drive" );
		//m_FontButtons.DrawText( 300, 210, 0xffffffff, L"I");
		//m_Font.DrawText( 330, 210, 0xffffffff, L" copy file/directory" );
		//m_FontButtons.DrawText( 60, 250, 0xffffffff, L"G");
		//m_Font.DrawText( 110, 250, 0xffffffff, L" remove media check from .xbe" );
		m_FontButtons.DrawText( 60, 290, 0xffffffff, L"H");
		m_Font.DrawText( 110, 290, 0xffffffff, L" back or one action menu level up" );
	}


    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return S_OK;  
}

//-----------------------------------------------------------------------------


/*
void CXBoxSample::MLog(WCHAR *message,...)
{
	WCHAR expanded_message[VARARG_BUFFSIZE];
	va_list tGlop;
	// Expand the varable argument list into the text buffer
	va_start(tGlop,message);
	if(vswprintf(expanded_message,message,tGlop)==-1)
	{
		// Fatal overflow, lets abort
		return;
	}
	va_end(tGlop);
	
	//mpDebug->MessageInstant(expanded_message);
	if(wlogfile)
	{
        //p_fcopy->WLog(expanded_message);
		p_log->WLog(expanded_message);
	}
}*/

bool CXBoxSample::CreateDirs(char *path)
{
	char temp[255];
	int i=3;
	mhelp->addSlash(path);
	DPf_H("CreateDirs path %s",path);
	strncpy(temp,path,3);
	while(path[i] != NULL)
	{
		sprintf(temp,"%s%c",temp,path[i]);
		if(path[i] == '\\')
		{
			DPf_H("CreateDirs: %s",temp);
			if(GetFileAttributes(temp) == -1)
			{
				DPf_H("CreateDirs2: %s",temp);
				if(!CreateDirectory(temp,NULL))
				{
					DPf_H("Error CreateDirs");
					return false;
				}
			}
		}
		i++;
	}
	return true;
}






