#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>
#include <stdio.h>
#include <debug.h>

#include <algorithm>
#include <iosupport.h>
//#include <cdio/cdio.h>
#include <undocumented.h>
//#include "libsmb++\src\smb++.h"
//#include <FileSmb.h>
#include <helper.h>
//#include <demuxer.h>
#include <dvd_reader.h>
//#include <dvdnav.h>
#include "Xcddb\cddb.h"
#include "cdripxlib\cdripxlib.h"
#include "background.h"
#include "dvd2xbox\d2xpatcher.h"
#include "dvd2xbox\d2xgraphics.h"
#include "dvd2xbox\d2xdbrowser.h"
#include "dvd2xbox\d2xfilecopy.h"
#include "dvd2xbox\d2xtitle.h"
#include "dvd2xbox\d2xdrivestatus.h"
//#include "ftp\ftp.h"


#define DUMPDIRS	9
char *ddumpDirs[]={"e:\\", "e:\\games", NULL};

class CXBoxSample : public CXBApplicationEx
{
    CXBFont     m_Font;             // Font object
	CXBFont     m_Fontb;  
	CXBoxDebug	*mpDebug;
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
	//DWORD		dwTopColor;
	//DWORD		dwBottomColor;
	WCHAR		driveState[100];
	WCHAR		*m_Errormsg;
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
	CIoSupport		io;
	HelperX*		mhelp;
	D2Xpatcher*		p_patch;
	D2Xgraphics*	p_graph;
	D2Xdbrowser*	p_browser;
	D2Xdbrowser*	p_browser2;
	D2Xfilecopy*	p_fcopy;
	D2Xtitle*		p_title;
	D2Xdstatus*		p_dstatus;
	dvd_reader_t*	dvd;
	dvd_file_t*		vob;
	int				dvdsize;
	int				freespace;
	ULONGLONG		currentdumped;
	float			ogg_quality;
	CCDRipX			*cdripx;
	int				cdripxok;
	Xcddb			m_cddb;	
	int				network;
	int				cddbquery;
	int				autopatch;
	int				wlogfile;
	//int				copyretries;
	bool			useF;
	bool			useG;
	int				activebrowser;
	bool			b_help;
	char			ftp_ip[40];
	char			ftp_user[10];
	char			ftp_pwd[10];


public:
    virtual HRESULT Initialize();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
	//virtual int gameTitle();
	//virtual char* GetNextPath(char *drive);
	virtual bool CreateDirs(char *path);
	//virtual bool DumpDVDtoHD(char *path,char *destroot);
	//virtual bool DumpISOtoHD(char *path,char *destroot);
	//virtual bool CopyISOFile(char* sourcefile,char* destfile);
	//virtual bool DumpVobs(char* dest);
	//virtual bool DumpCDDA(char* dest);
	//virtual bool queryCDDB(char* title);
	//virtual bool getDVDTitle(char* title);
	//virtual bool DumpTitle(char* dest,int chap,dvd_read_domain_t what);
	virtual void MLog(WCHAR *message,...);
	//BOOL InitializeNetwork();


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

	m_Errormsg = new WCHAR[40];
	m_GameTitle = new WCHAR[40];
	mhelp = new HelperX;
	p_patch = new D2Xpatcher;
	p_graph = new D2Xgraphics(&m_Fontb);
	p_browser = new D2Xdbrowser;
	p_browser2 = new D2Xdbrowser;
	p_fcopy = new D2Xfilecopy;
	p_title = new D2Xtitle;
	p_dstatus = new D2Xdstatus;
	strcpy(mBrowse1path,"e:\\");
	strcpy(mBrowse2path,"f:\\");
	useF = false;
	useG = false;

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
				dumpDirs[x] = new char[strlen(mhelp->getIniValue("main",temp)+1)];
				dumpDirs[x] = (char *)mhelp->getIniValue("main",temp);
				x++;
			}
		} 
		//dumpDirs[x] = NULL;
	}

	ogg_quality = (float)atof(mhelp->getIniValue("main","oggquality"));
	D2Xfilecopy::f_ogg_quality = ogg_quality;
	autopatch = atoi(mhelp->getIniValue("main","autopatch"));
	network = atoi(mhelp->getIniValue("network","enabled"));
	wlogfile = atoi(mhelp->getIniValue("main","logfile"));
	strcpy(D2Xtitle::c_cddbip,mhelp->getIniValue("network","cddbip"));
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
	cdripxok = 1;
	mx = 1;
	activebrowser = 1;
	b_help = false;
	

	p_dstatus->GetDriveState(driveState,type);
	dwTime = timeGetTime();
	cdripx = new CCDRipX();

	if(network)
	{
		
		if (!m_cddb.InitializeNetwork((char*)mhelp->getIniValue("network","xboxip"), (char*)mhelp->getIniValue("network","netmask"), (char*)mhelp->getIniValue("network","gateway")))
		//if(!InitializeNetwork())
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

				// determine free disk space
				int i=0;
				char temp[20];
				while(dumpDirs[i]!=NULL)
				{
					CreateDirs(dumpDirs[i]);
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
				// determine free disk space
				int i=0;
				char temp[20];
				while(hdds[i]!=NULL)
				{
					if(!(mhelp->getfreeDiskspace(hdds[i],temp)))
						strcpy(temp, "");
					dumpDirsFS[i] = new char[strlen(temp)+1];
					strcpy(dumpDirsFS[i],temp);
					i++;
				}
				dumpDirsFS[i]=NULL;
				strcpy(mBrowse1path,"e:\\");
				//strcpy(mBrowse1path,"smb://wiso:Warp99@wiso3");
				strcpy(mBrowse2path,"f:\\");
				mCounter=21;
			}
			if(mhelp->pressX(m_DefaultGamepad))
			{
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
				mCounter=3;
				sprintf(mDestTitle,"%sdefault.xbe",mDestPath);
				sprintf(mDestLog,"%sdvd2xbox.log",mDestPath);	
				//mhelp->setLogFilename(mDestLog);
				p_fcopy->setLogFilename(mDestLog);
			
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter--;
			}

			break;
			/*
		case 2:
			mpDebug->delhistory();
			if((dvdsize != 0) && (dvdsize > freespace))
			{
				mpDebug->Message(L"Warning:");
				mpDebug->Message(L"DVD size: %d MB > free space: %d MB",dvdsize,freespace);
				mpDebug->Message(L"");
			}

			mpDebug->Message(L"Destination path:");
			mpDebug->Message(L"%hs",mDestPath);
			mpDebug->Message(L"");
			mpDebug->Message(L"Press START to procceed");
			mpDebug->Message(L"Press BACK to choose again");
			mpDebug->Message(L"Press LEFT+RIGHT+BLACK for dashboard");
			mCounter++;
			break;
			*/
		case 3:
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_START))
			{
				mCounter++;
			} else if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter-=2;
			}
			break;
		case 4:

			if(wlogfile)
				p_fcopy->enableLog(true);
			
			if(type==DVD)
			{	
				//DumpDVDtoHD("D:\\",mDestPath);
				//DumpVobs(mDestPath);
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

				//HelperX::writeLog2 = true;
				//DumpDVDtoHD("d:\\",mDestPath);
				//mpDebug->delhistory();
/*
				if(autopatch)
				{
					for(int i=0;i<mXBECount;i++)
					{
						MLog(L"checking %hs",mXBEs[i]);
						ULONG mt;
						if(p_patch->SetMediatype(mXBEs[i],mt,"FF010040"))
						{
							MLog(L"Setting media type from 0x%08x to 0x400001FF",mt);
						} else {
							MLog(L"Error while setting media type");
						}

						p_patch->PatchMediaStd(mXBEs[i],message);
						int n=0;
						while(message[n]!=NULL)
						{
							MLog(message[n]);
							delete[] message[n];
							n++;
						}
						MLog(L"");
					}
				}
				*/
			}
			/*
			MLog(L"");

			if(mLongFilesCount > 0)
			{
				MLog(L"%d file(s) are longer than fatx can handle:",mLongFilesCount);
				for(int i=0;i<mLongFilesCount;i++)
				{
					MLog(L"%hs",mLongPath[i]);
					MLog(L"    %hs",mLongFile[i]);
				}
				MLog(L"");
				MLog(L"This game may not run from HDD. Please transfer the");
				MLog(L"files directly from the DVD to your PC to burn a");
				MLog(L"working copy.");
			}

			MLog(L"");
			*/
			//mhelp->writeLog = false;
			mCounter++;
			break;
		case 5:
			if(D2Xfilecopy::b_finished)
				mCounter = 6;
			/*
			if(mhelp->pressX(m_DefaultGamepad))
			{
				p_fcopy->CancleThread();
				mCounter = 7;
			}
			*/
			break;

		case 6:
			if(autopatch && (type == GAME))
			{
				for(int i=0;i<D2Xpatcher::mXBECount;i++)
				{
					MLog(L"checking %hs",D2Xpatcher::mXBEs[i]);
					ULONG mt;
					if(p_patch->SetMediatype(D2Xpatcher::mXBEs[i],mt,"FF010040"))
					{
						MLog(L"Setting media type from 0x%08x to 0x400001FF",mt);
					} else {
						MLog(L"Error while setting media type");
					}
					DPf_H("Patching");
					int ret;
					for(int n=0;n<2;n++)
					{
                        ret = p_patch->PatchMediaStd(D2Xpatcher::mXBEs[i],n);
						switch(ret)
						{
						case FOUND_OK:
							MLog(L"%hs removed",D2Xpatcher::p_hexsearch[n]);
							break;
						case FOUND_ERROR:
							MLog(L"%hs found but couldn't be removed",D2Xpatcher::p_hexsearch[n]);
							break;
						case NOT_FOUND:
							MLog(L"%hs not found",D2Xpatcher::p_hexsearch[n]);
							break;
						default:
							break;
						}
					}

					MLog(L"");
					
				}
			}
			/*
			for(int i=0;i<mLongFilesCount;i++)
			{
				delete[] mLongPath[i];
				delete[] mLongFile[i];
			}*/
			//for(int i=0;i<mXBECount;i++)
			{
			//	delete[] mXBEs[i];
			}
			//mLongFilesCount = 0;
			//mXBECount = 0;
			/*
			mpDebug->MessageInstant(L"Finished.");
			mpDebug->MessageInstant(L"You can now");
			mpDebug->MessageInstant(L"- switch off");
			mpDebug->MessageInstant(L"- press LEFT+RIGHT+BLACK for dashboard");
			mpDebug->MessageInstant(L"- press START to rip next DVD");
			mpDebug->MessageInstant(L"");
			mpDebug->MessageInstant(L"");
			*/
			if(atoi(mhelp->getIniValue("main","autoeject")))
                io.EjectTray();
			p_fcopy->enableLog(false);
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
			mpDebug->Message(m_Errormsg);
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
		
			if(info.button == BUTTON_X)
				mCounter=22;
			if(info.button == BUTTON_Y)
			{
				mCounter=30;
			}
			if(info.button == BUTTON_START)
			{
				mCounter=40;
			}
			if(info.button == BUTTON_RTRIGGER)
			{
				mCounter=50;
			}
			if(info.button == BUTTON_WHITE)
			{
				mCounter=60;
			}
			if(info.button == BUTTON_B)
			{
				/*
				char temp[1024];
				sprintf(temp,"%s%s",info.item,"default.xbe");
				if(mhelp->getXBETitle(temp,m_GameTitle))
					wsprintf(mvDirs[0],"%S",m_GameTitle);
				else
					//strcpy(mvDirs[0],GetNextPath(mDestPath));
				mCounter=70;
				*/
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
				// determine free disk space
				int i=0;
				char temp[20];
				while(hdds[i]!=NULL)
				{
					if(!(mhelp->getfreeDiskspace(hdds[i],temp)))
						strcpy(temp, "");
					dumpDirsFS[i] = new char[strlen(temp)+1];
					strcpy(dumpDirsFS[i],temp);
					i++;
				}
				dumpDirsFS[i]=NULL;
				mCounter = 21;
				p_browser->Renew();
				p_browser2->Renew();
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter--;
			}
			break;
		case 30:
			// launch xbe
			if(mhelp->pressSTART(m_DefaultGamepad))
			{
				mhelp->LaunchXbe(info.item,"d:\\default.xbe");
			}
			if((m_DefaultGamepad.wPressedButtons & XINPUT_GAMEPAD_BACK)) {
				mCounter=20;
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
			mCounter = 61;
			break;
		case 61:
			if(D2Xfilecopy::b_finished)
			{
				mCounter = 21;
				//activebrowser = 0;
				D2Xdbrowser::renewAll = true;
				//p_browser->Renew();
				//p_browser2->Renew();
			}
			break;
		case 70:
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
		m_Font.DrawText( 80, 30, 0xffffffff, L"Welcome to DVD2Xbox 0.5.0" );
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
		} 
		wsprintfW(temp2,L"%hs",mDestPath);
		m_Fontb.DrawText( 60, 210, 0xffffffff, temp2 );
		m_FontButtons.DrawText( 60, 260, 0xffffffff, L"G");
		m_Font.DrawText( 110, 260, 0xffffffff, L" proceed" );
		m_FontButtons.DrawText( 60, 300, 0xffffffff, L"H");
		m_Font.DrawText( 110, 300, 0xffffffff, L"  choose drive again" );
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
		if(type && GAME)
            m_Font.DrawText(55, 160, 0xffffffff, L"Patching xbe's ..." );
	}
	else if(mCounter == 7)
	{
		WCHAR copy[50];
		WCHAR renamed[50];
		WCHAR failed[50];
		WCHAR mcrem1[50];
		WCHAR mcremL[50];
		WCHAR mcremS[50];
		   wsprintfW(copy,L"Files copied:   %6d",D2Xfilecopy::copy_ok);
		 wsprintfW(failed,L"Failed to copy: %6d",D2Xfilecopy::copy_failed);
		wsprintfW(renamed,L"Files renamed:  %6d",D2Xfilecopy::copy_renamed);

		wsprintfW(mcrem1,L"Files with MediaCheck 1:                %2d",D2Xpatcher::mXBECount);
		wsprintfW(mcremL,L"Files with MediaCheck 2 (long string):  %2d",D2Xpatcher::mcheck[0]);
		wsprintfW(mcremS,L"Files with MediaCheck 2 (short string): %2d",D2Xpatcher::mcheck[1]);
			
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Copy report:" );
		m_Fontb.DrawText( 60, 140, 0xffffffff, copy );
		m_Fontb.DrawText( 60, 170, 0xffffffff, failed );
		m_Fontb.DrawText( 60, 200, 0xffffffff, renamed );

		if(type == GAME)
		{
			m_Fontb.DrawText( 60, 250, 0xffffffff, mcrem1 );
			m_Fontb.DrawText( 60, 280, 0xffffffff, mcremL );
			m_Fontb.DrawText( 60, 310, 0xffffffff, mcremS );
		}
		
		m_Font.DrawText( 60, 435, 0xffffffff, L"press START to proceed" );
	
	}
	/*
	else if(mCounter==20)
	{
		p_graph->RenderMainFrames();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Helpscreen, press A to proceed" );
		m_FontButtons.DrawText( 60, 140, 0xffffffff, L"A");
		m_Font.DrawText( 90, 140, 0xffffffff, L" select" );
		m_FontButtons.DrawText( 60, 170, 0xffffffff, L"B");
		m_Font.DrawText( 90, 170, 0xffffffff, L" N/A" );
		m_FontButtons.DrawText( 300, 140, 0xffffffff, L"C");
		m_Font.DrawText( 330, 140, 0xffffffff, L" delete file/directory" );
		m_FontButtons.DrawText( 300, 170, 0xffffffff, L"D");
		m_Font.DrawText( 330, 170, 0xffffffff, L" launch default.xbe" );
		m_FontButtons.DrawText( 60, 210, 0xffffffff, L"F");
		m_Font.DrawText( 90, 210, 0xffffffff, L" select drive" );
		m_FontButtons.DrawText( 300, 210, 0xffffffff, L"I");
		m_Font.DrawText( 330, 210, 0xffffffff, L" copy file/directory" );
		m_FontButtons.DrawText( 60, 250, 0xffffffff, L"G");
		m_Font.DrawText( 110, 250, 0xffffffff, L" remove media check from .xbe" );
		m_FontButtons.DrawText( 60, 290, 0xffffffff, L"H");
		m_Font.DrawText( 110, 290, 0xffffffff, L" come back to this screen" );
	}*/
	else if(mCounter==21 || mCounter == 50 || mCounter == 61)
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
		wsprintfW(temp,L"%hs\\default.xbe",info.item);
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

	if(b_help)
	{
		p_graph->RenderHelpFrame();
		m_Font.DrawText( 80, 30, 0xffffffff, L"Helpscreen, press BLACK to proceed" );
		m_FontButtons.DrawText( 60, 140, 0xffffffff, L"A");
		m_Font.DrawText( 90, 140, 0xffffffff, L" select" );
		m_FontButtons.DrawText( 60, 170, 0xffffffff, L"B");
		m_Font.DrawText( 90, 170, 0xffffffff, L" N/A" );
		m_FontButtons.DrawText( 300, 140, 0xffffffff, L"C");
		m_Font.DrawText( 330, 140, 0xffffffff, L" delete file/directory" );
		m_FontButtons.DrawText( 300, 170, 0xffffffff, L"D");
		m_Font.DrawText( 330, 170, 0xffffffff, L" launch default.xbe" );
		m_FontButtons.DrawText( 60, 210, 0xffffffff, L"F");
		m_Font.DrawText( 90, 210, 0xffffffff, L" select drive" );
		m_FontButtons.DrawText( 300, 210, 0xffffffff, L"I");
		m_Font.DrawText( 330, 210, 0xffffffff, L" copy file/directory" );
		m_FontButtons.DrawText( 60, 250, 0xffffffff, L"G");
		m_Font.DrawText( 110, 250, 0xffffffff, L" remove media check from .xbe" );
		m_FontButtons.DrawText( 60, 290, 0xffffffff, L"H");
		m_Font.DrawText( 110, 290, 0xffffffff, L" come back to this screen" );
	}


    // Present the scene
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
    return S_OK;  
}

//-----------------------------------------------------------------------------
BOOL IsEthernetConnected()
{
	if (!(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE))
		return FALSE;

	return TRUE;
}

/*
BOOL CXBoxSample::InitializeNetwork()
{


	if (!IsEthernetConnected())
		return FALSE;

	// if local address is specified
	if ( network )

	{
		// Thanks and credits to Team Evox for the description of the 
		// XNetConfigParams structure.

		TXNetConfigParams configParams;   

		DPf_H("Loading network configuration...");
		XNetLoadConfigParams( (LPBYTE) &configParams );
		DPf_H("Ready.");

		BOOL bXboxVersion2 = (configParams.V2_Tag == 0x58425632 );	// "XBV2"
		BOOL bDirty = FALSE;

		DPf_H("User local address: ");
	
		if (bXboxVersion2)
		{
			if (configParams.V2_IP != inet_addr(mhelp->getIniValue("network","xboxip")))
			{
				configParams.V2_IP = inet_addr(mhelp->getIniValue("network","xboxip"));
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_IP != inet_addr(mhelp->getIniValue("network","xboxip")))
			{
				configParams.V1_IP = inet_addr(mhelp->getIniValue("network","xboxip"));
				bDirty = TRUE;
			}
		}

		if (bXboxVersion2)
		{
			if (configParams.V2_Subnetmask != inet_addr(mhelp->getIniValue("network","netmask")))
			{
				configParams.V2_Subnetmask = inet_addr(mhelp->getIniValue("network","netmask"));
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_Subnetmask != inet_addr(mhelp->getIniValue("network","netmask")))
			{
				configParams.V1_Subnetmask = inet_addr(mhelp->getIniValue("network","netmask"));
				bDirty = TRUE;
			}
		}

		if (bXboxVersion2)
		{
			if (configParams.V2_Defaultgateway != inet_addr(mhelp->getIniValue("network","gateway")))
			{
				configParams.V2_Defaultgateway = inet_addr(mhelp->getIniValue("network","gateway"));
				bDirty = TRUE;
			}
		}
		else
		{
			if (configParams.V1_Defaultgateway != inet_addr(mhelp->getIniValue("network","gateway")))
			{
				configParams.V1_Defaultgateway = inet_addr(mhelp->getIniValue("network","gateway"));
				bDirty = TRUE;
			}
		}

		if (configParams.Flag != (0x04|0x08) )
		{
			configParams.Flag = 0x04 | 0x08;
			bDirty = TRUE;
		}

		if (bDirty)
		{
			DPf_H("Updating network configuration...\n");
			XNetSaveConfigParams( (LPBYTE) &configParams );
			DPf_H("Ready.\n");
		}
	}

	XNetStartupParams xnsp;
	memset(&xnsp, 0, sizeof(xnsp));
	xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);

	// Bypass security so that we may connect to 'untrusted' hosts
	xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
  // create more memory for networking
  xnsp.cfgPrivatePoolSizeInPages = 64; // == 256kb, default = 12 (48kb)
  xnsp.cfgEnetReceiveQueueLength = 16; // == 32kb, default = 8 (16kb)
  xnsp.cfgIpFragMaxSimultaneous = 16; // default = 4
  xnsp.cfgIpFragMaxPacketDiv256 = 32; // == 8kb, default = 8 (2kb)
  xnsp.cfgSockMaxSockets = 64; // default = 64
  xnsp.cfgSockDefaultRecvBufsizeInK = 128; // default = 16
  xnsp.cfgSockDefaultSendBufsizeInK = 128; // default = 16
	INT err = XNetStartup(&xnsp);

	XNADDR xna;
	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(500);
	} while (dwState==XNET_GET_XNADDR_PENDING);


	WSADATA WsaData;
	err = WSAStartup( MAKEWORD(2,2), &WsaData );
	if (err == NO_ERROR)
	{
		//gFileZillaXBMP = new CXBFileZilla("Q:\\");
		//gFileZillaXBMP->Start();

		//if (g_playerSettings.bWebServer)
		{
			//gXBWebServer = new CWebServer;
			//int status = gXBWebServer->Start( this, "Q:\\");
		}
	}
	return ( err == NO_ERROR );
}
*/
/*
int CXBoxSample::gameTitle()
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	//wchar_t name[40];
	stream  = fopen( "d:\\default.xbe", "rb" );
	if(stream == NULL) {
		m_Errormsg = L"Could not open file";
		//mCounter=10;
	} else {
	
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,1,sizeof(HC),stream);
		fclose(stream);
		wcscpy(m_GameTitle, HC.TitleName);
		return 1;
	}	
	m_GameTitle=NULL;
    return 0;
}
*/
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
        p_fcopy->WLog(expanded_message);

}

/*
char* CXBoxSample::GetNextPath(char *drive)
{
	static char testname[1024];
	static char title[50];
	char drivepath[1024];
	int count=1;
	int ret = 0;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	boolean GoodOne=true;

	strcpy(drivepath,drive);
	strcat(drivepath,"*");

	strcpy(title,"dvd2xbox");

	if(type == GAME)
	{
		if( gameTitle() ) 
		{
			wsprintf(title,"%S",m_GameTitle);	
			mhelp->getFatxName(title);
			count = 0;
		}
	} else if(type == DVD)
	{	
		if(getDVDTitle(title))
		{
			mhelp->getFatxName(title);
			count = 0;
		}
	} 
	else if((type == CDDA) && network)
	{	
		if(queryCDDB(title))
		{
			//mhelp->getFatxName(title);
			count = 0;
		}
	} 
	while(count<=999)
	{
		if(count)
		{
			if(strlen(title) > 38)
			{
				strncpy(testname,title,38);
				testname[39] = '\0';
				sprintf(testname,"%hs%03d",testname,count);
			} else {
                sprintf(testname,"%hs%03d",title,count);
			}
		} else 
		{
			sprintf(testname,"%hs",title);
		}

		GoodOne=true;

		// Start the find and check for failure.
		hFind = FindFirstFile(drivepath, &wfd );

		// See if this exists in the directory
	    do
	    {
			// Only do directories
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY && strcmp(wfd.cFileName,testname)==0)
			{
				GoodOne=false;
				break;
			}
	    }
	    while(FindNextFile( hFind, &wfd ));
	    // Close the find handle.
	    FindClose( hFind );

		if(GoodOne) break;
		count++;
	}

	strcat(testname,"\\");

	return testname;

}
*/
bool CXBoxSample::CreateDirs(char *path)
{
	char temp[255];
	int i=3;
	strncpy(temp,path,3);
	while(path[i] != NULL)
	{
		sprintf(temp,"%s%c",temp,path[i]);
		if(path[i] == '\\')
		{
			if(GetFileAttributes(temp) == -1)
			{
				if(!CreateDirectory(temp,NULL))
					return false;
			}
		}
		i++;
	}
	return true;
}

/*
bool CXBoxSample::DumpDVDtoHD(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	// Draw a gradient filled background
	//RenderGradientBackground( dwTopColor, dwBottomColor );
	

	// We must create the dest directory
	if(CreateDirectory(destroot,NULL))
	{
		MLog(L"Created Directory: %hs",destroot);
	}
	



	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    if(mpDebug) mpDebug->MessageInstant(L"SetDirectory FindFirstFile returned invalid HANDLE");
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);
			if(((strstr(wfd.cFileName,".vob") || strstr(wfd.cFileName,".VOB"))) && type==DVD)
				continue;

			strcpy(destfile,destroot);
			strcat(destfile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"\\");
				mDirCount++;
				// Recursion
				if(!DumpDVDtoHD(sourcefile,destfile)) continue;
			}
			else
			{
				if(strlen(wfd.cFileName) > 42)
				{
					mLongPath[mLongFilesCount] = new char[strlen(path)+1];
					mLongFile[mLongFilesCount] = new char[strlen(wfd.cFileName)+1];
					strcpy(mLongPath[mLongFilesCount],path);
					strcpy(mLongFile[mLongFilesCount],wfd.cFileName);
					mLongFilesCount++;
				}
				if((strstr(wfd.cFileName,".xbe")) || (strstr(wfd.cFileName,".XBE")))
				{
					mXBEs[mXBECount] = new char[strlen(destfile)+1];
					strcpy(mXBEs[mXBECount],destfile);
					mXBECount++;
				}
				MLog(L"Copying: %hs",wfd.cFileName);

				for(int i=0;i<copyretries;i++)
				{
					if(!CopyFile(sourcefile,destfile,0))
					{
						MLog(L"Failed to copy %hs to %hs. Try %d",sourcefile,destfile,i);
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						mFileCount++;
						break;
					}
				}
				
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return true;
}

*/

///////////////////////////////////////////////////////////////////////
// DVD part
/*
bool CXBoxSample::DumpVobs(char* dest)
{
	int i=0;
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		mpDebug->MessageInstant(L"Could not authenticate DVD");
		return false;
	}
	//if(!(mhelp->getusedDSul("D:\\",dvdsize)))
	//	dvdsize = 0;

	currentdumped = 0;

	// dumping menus
	while(DumpTitle(dest,i++,DVD_READ_MENU_VOBS)){};

	i=1;
	// dumping titles
	while(DumpTitle(dest,i++,DVD_READ_TITLE_VOBS)){};

    DVDClose(dvd);
	return true;

}

bool CXBoxSample::getDVDTitle(char* title)
{
	char titleid[32];
	unsigned char setid[128];
	int ret = 0;

	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		//mpDebug->MessageInstant(L"Could not authenticate DVD");
		//sprintf(title,"No DVDOpen");
		return false;
	}

	ret = DVDUDFVolumeInfo(dvd,titleid,sizeof(titleid),setid,sizeof(setid));
	if((ret != -1) && (ret != 0))
	{
		sprintf(title,"%hs",titleid);
	} else if(ret == 0)
	{
		if(DVDISOVolumeInfo(dvd,titleid,sizeof(titleid),setid,sizeof(setid)) != 1)
			sprintf(title,"%hs",titleid);
	} 
	
    DVDClose(dvd);
	return true;
}


bool CXBoxSample::DumpTitle(char* dest,int chap,dvd_read_domain_t what)
{
	#define rblocks 256
	FILE* fh;
	int read=0;
	int second=1;
	int off=0;
	bool run = true;
	unsigned char* buffer;
	buffer = new unsigned char[rblocks*2048];
	char temp[255];
	char tempm[255];
	WCHAR text[20];
	
	vob = DVDOpenFile(dvd,chap,what);
	if(!vob)
	{
		//mpDebug->MessageInstant(L"Could not open title set %d",chap);
		return false;
	}

	while(run)
	{
		if((DVD_READ_MENU_VOBS == what) && (chap == 0))
		{
			sprintf(temp,"%sVIDEO_TS\\VIDEO_TS.VOB",dest);
		} 
		else if((DVD_READ_MENU_VOBS == what) && (chap != 0))
		{
			sprintf(temp,"%sVIDEO_TS\\VTS_%02d_0.VOB",dest,chap);
		}
		else if(DVD_READ_TITLE_VOBS == what)
		{
			sprintf(temp,"%sVIDEO_TS\\VTS_%02d_%d.VOB",dest,chap,second);
		} else {
			mpDebug->MessageInstant(L"title wrong");
			DVDCloseFile(vob);
			return false;
		}

		strcpy(tempm,temp);
	
		if(!(fh = fopen(temp,"w+b")))
		{
			mpDebug->MessageInstant(L"Could not open dest file: %hs",temp);
			DVDCloseFile(vob);
			return false;
		}


		while((read = DVDReadBlocks(vob,off,rblocks,buffer)) > 0)
		{
		
			if(off+read >= DVDFileSize(vob))
			{
				read = read-((off+read)-DVDFileSize(vob));
				if(fwrite(buffer,DVD_VIDEO_LB_LEN,read,fh) != read)
					mpDebug->MessageInstant(L"Error during writing");
				currentdumped+=read;
				run = false;
				break;
			} else if(off+read > 524287*second)
			{
				read = read-((off+read)-524287*second);
				second++;
				if(fwrite(buffer,DVD_VIDEO_LB_LEN,read,fh) != read)
					mpDebug->MessageInstant(L"Error during writing");
				off+=read;
				currentdumped+=read;
				break;
			} else {
		
				if(fwrite(buffer,DVD_VIDEO_LB_LEN,read,fh) != read)
					mpDebug->MessageInstant(L"Error during writing");
				off+=read;
				currentdumped+=read;
			}

			dwcTime = timeGetTime();
			if((dwcTime-dwTime) >= 4000)
			{
				dwTime = dwcTime;
				// Draw a gradient filled background
				//RenderGradientBackground( dwTopColor, dwBottomColor );
				m_BackGround.Render( &m_Font, 0, 0 );
		
				m_Font.DrawText( 30, 100, 0xffffffff, L"Copying:" );
				wsprintfW(text,L"%hs",tempm);
				m_Font.DrawText( 30, 130, 0xffffffff, text );
				if(dvdsize)
				{
					wsprintfW(text,L"ca. %6u MB left",dvdsize-(DVD_VIDEO_LB_LEN*currentdumped/1048576));
					m_Font.DrawText( 60, 180, 0xffffffff, text );
				}
				// Present the scene
				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			}
		}
		fclose(fh);
		if(read<=0)
			run = false;
	}
	DVDCloseFile(vob);
	if(buffer)
		delete[] buffer;
	return true;
}

*/

///////////////////////////////////////////////////////////////////////////
// ISO part

/*
bool CXBoxSample::CopyISOFile(char* lpcszFile,char* destfile)
{
	// attempt open file
	CIoSupport cdrom;
	CISO9660 iso9660(cdrom);
	int fh;
	if (!iso9660.OpenDisc() )
	{
		return FALSE;
	}
	if ((fh = iso9660.OpenFile(lpcszFile)) < 0)
	{		
		return FALSE;
	}

	DWORD dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	uint64_t fileSize   = iso9660.GetFileSize();
	uint64_t fileOffset = 0;

	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		delete buffer;
		return FALSE;
	}

	//CHAR szText[128];
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	long lRead;
	DWORD dwWrote;

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			//sprintf(szText, STRING(403) ,nNewPercentage);
			nOldPercentage = nNewPercentage;
		}

		lRead = iso9660.ReadFile(NULL,buffer,dwBufferSize);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);
		WriteFile(hFile,buffer,(DWORD)lRead,&dwWrote,NULL);
		fileOffset+=lRead;

		nNewPercentage = ((fileOffset*100)/fileSize);

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	iso9660.CloseFile(fh);
	delete buffer;

	return TRUE;
}


bool CXBoxSample::DumpISOtoHD(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	CIoSupport cdrom;
	CISO9660 mISO(cdrom);
	if (!(mISO.OpenDisc() ))
		return false;

	// We must create the dest directory
	if(CreateDirectory(destroot,NULL))
	{
		MLog(L"Created Directory: %hs",destroot);
	}


	strcpy(sourcesearch,path);
	//strcpy(sourcesearch,"\\");
	//strcat(sourcesearch,"*");

	// Start the find and check for failure.
	memset(&wfd,0,sizeof(wfd));
	hFind = mISO.FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    if(mpDebug) mpDebug->MessageInstant(L"SetDirectory ISOFindFirstFile returned invalid HANDLE");
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			
			if (wfd.cFileName[0]==0)
				continue;
			
			//if (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			//	wfd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);
			strcpy(destfile,destroot);
			mhelp->getFatxName(wfd.cFileName);
			strcat(destfile,wfd.cFileName);

			//mpDebug->MessageInstant(L"found %hs, depth %d",wfd.cFileName,mDirCount);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"\\");
				mDirCount++;
				// Recursion
				if(!DumpISOtoHD(sourcefile,destfile)) continue;
			}
			else
			{
								
				MLog(L"Copying: %hs",wfd.cFileName);

				for(int i=0;i<atoi(mhelp->getIniValue("main","copyretries"));i++)
				{
					if(!CopyISOFile(sourcefile,destfile))
					{
						MLog(L"Failed to copy %hs to %hs. Try %d",sourcefile,destfile,i);
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						mFileCount++;
						break;
					}
				}
			}

	    }
	    while(mISO.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
		hFind = NULL;
	}
	return true;
}
*/

//////////////////////////////////////////////////////////////
// CDDA part
/*
bool CXBoxSample::DumpCDDA(char* dest)
{
	char file[1024];
	WCHAR temp[1024];
	WCHAR temp2[1024];
	

	int			m_nNumTracks;

	if(cdripx->Init()==E_FAIL)
		return false;

	//ogg
	int			nPercent = 0;
	int			nPeakValue;
	int			nJitterErrors;
	int			nJitterPos;

	if(CreateDirectory(dest,NULL))
	{
		MLog(L"Created Directory: %hs",dest);
	}

	m_nNumTracks =cdripx->GetNumTocEntries();

	MLog(L"Found %d Audio Tracks",m_nNumTracks);
	for(int ntrack=1;ntrack<=m_nNumTracks;ntrack++)
	{
		char buffer[1024];
		char dartist[1024];
		char artist[1024];
		char titel[1024];

		if(cddbquery && network)
		{
			buffer[0] = '\0';
			
			m_cddb.getDiskArtist(dartist);
			if((m_cddb.getTrackArtist(ntrack) != NULL) && (strlen(m_cddb.getTrackArtist(ntrack)) > 1))
			{
				strcpy(artist,m_cddb.getTrackArtist(ntrack));
				mhelp->getFatxName(artist);
				strcat(buffer,artist);
			}
			else if(dartist != NULL)
			{
				mhelp->getFatxName(dartist);
				strcat(buffer,dartist);
			}
			if(strlen(buffer) > 1)
				strcat(buffer," - ");
			if(m_cddb.getTrackTitle(ntrack) != NULL)
			{
				strcpy(titel,m_cddb.getTrackTitle(ntrack));
				mhelp->getFatxName(titel);
				strcat(buffer,titel);
			}
				
			mhelp->getFatxName(buffer);
			sprintf(file,"%s%s.ogg",dest,buffer);

		} else {
            sprintf(file,"%strack%02d.ogg",dest,ntrack);
		}

		cdripx->InitOgg(ntrack,file,ogg_quality);

	
		if(cddbquery && network)
		{
			if(titel != NULL)
                cdripx->AddOggComment("Title",titel);
			if((artist != NULL) && (strlen(artist) > 1))
				cdripx->AddOggComment("Artist",artist);
			else if(dartist != NULL)
				cdripx->AddOggComment("Artist",dartist);
			char t_disk_title[1024];
			m_cddb.getDiskTitle(t_disk_title);
			if(t_disk_title != NULL)
			{
				mhelp->getFatxName(t_disk_title);
				cdripx->AddOggComment("Album",t_disk_title);
			}
	
		}
		cdripx->AddOggComment("Comment","Ripped with dvd2xbox");
		while(CDRIPX_DONE != cdripx->RipToOgg(nPercent,nPeakValue,nJitterErrors,nJitterPos))
		{
			dwcTime = timeGetTime();
			if((dwcTime-dwTime) >= 2000)
			{
				dwTime = dwcTime;
				// Draw a gradient filled background
				//RenderGradientBackground( dwTopColor, dwBottomColor );
				m_BackGround.Render( &m_Font, 0, 0 );
				wsprintfW(temp,L"Track%2d from %d %3d %%",ntrack,m_nNumTracks,nPercent);
				m_Font.DrawText( 30, 100, 0xffffffff, L"CDDA ripping in progress ... " );
				m_Font.DrawText( 30, 130, 0xffffffff, temp);
				if(network)
				{
					wsprintfW(temp2,L"%hs",buffer);
					m_Font.DrawText( 30, 170, 0xffffffff, temp2);
				}
				m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
			}
		}
	}
	cdripx->DeInit();
	return true;
}


bool CXBoxSample::queryCDDB(char* title)
{
	io.Remount("D:","Cdrom0");
	if(cdripx->Init()==E_FAIL)
	{
		cddbquery = 0;
		return false;
	}
	int tocentries = cdripx->GetNumTocEntries();
	m_cddb.setCDDBIpAdress((char*)mhelp->getIniValue("network","cddbip"));
	DPf_H("found %d tracks",tocentries);
	toc cdtoc[100];
	for (int i=0;i<=tocentries;i++)
	{
		// stupid but it works
		cdtoc[i].min=cdripx->oCDCon[i].min;
		cdtoc[i].sec=cdripx->oCDCon[i].sec;
		cdtoc[i].frame=cdripx->oCDCon[i].frame;
	}
	
	cdripx->DeInit();

	if ( m_cddb.queryCDinfo(tocentries, cdtoc) == E_FAILED)
	{

		DPf_H("Query failed");
		int lasterror=m_cddb.getLastError();
		
		if (lasterror == E_WAIT_FOR_INPUT)
		{
		

			int i=1;
			
			if((m_cddb.getInexactArtist(i)!=NULL) && (m_cddb.getInexactTitle(i)!=NULL))
			{
		
				char buffer[2048];
				char t_disk_artist[1024];

				if ( m_cddb.queryCDinfo(i) == E_FAILED)
				{
					cddbquery = 0;
					return false;
				}

				strcpy(buffer,"\0");
				strcpy(t_disk_artist,m_cddb.getInexactArtist(i));
				char t_disk_title[1024];
				strcpy(t_disk_title,m_cddb.getInexactTitle(i));
				if(t_disk_artist!=NULL)
					strcat(buffer,t_disk_artist);
				if(t_disk_artist!=NULL && t_disk_title!=NULL )
					strcat(buffer," - ");
				if(t_disk_title!=NULL)
					strcat(buffer,t_disk_title);
				mhelp->getFatxName(buffer);
				strcpy(title,buffer);
				cddbquery = 1;
				return true;
			} else {
				cddbquery = 0;
				return false;
			}
		} else {
			cddbquery = 0;
			return false;
		}
		
	}
	else
	{

		// Jep, tracks are received
		char buffer[2048];
		char t_disk_artist[1024];
		strcpy(buffer,"\0");
		m_cddb.getDiskArtist(t_disk_artist);
		char t_disk_title[1024];
		m_cddb.getDiskTitle(t_disk_title);
		if(t_disk_artist!=NULL)
			strcat(buffer,t_disk_artist);
		if(t_disk_artist!=NULL && t_disk_title!=NULL )
			strcat(buffer," - ");
		if(t_disk_title!=NULL)
			strcat(buffer,t_disk_title);
		mhelp->getFatxName(buffer);
		strcpy(title,buffer);
		cddbquery = 1;
	}

	return true;
}

*/