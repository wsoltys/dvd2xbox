#include "d2xdbrowser.h"

bool D2Xdbrowser::renewAll = true;


////////////////////////////////////////////////
// SMB
/*
char szPassWd[128];
char szUserName[128];
class MyCallback : public SmbAnswerCallback
{
protected:
	// Warning: don't use a fixed size buffer in a real application.
	// This is a security hazard.
	char buf[200];
public:
	char *getAnswer(int type, const char *optmessage) {
		switch (type) {
			case ANSWER_USER_NAME:
				strcpy(buf, szUserName);
				break;
			case ANSWER_USER_PASSWORD:
//				cout<<"Password for user "<<optmessage<<": ";
//				cin>>buf;
				strcpy(buf, szPassWd);
				break;
			case ANSWER_SERVICE_PASSWORD:
				strcpy(buf, szPassWd);
				break;
		}
		return buf;
	}
} cb;
*/
////////////////////////////////////////////////////

D2Xdbrowser::D2Xdbrowser()
{
	p_help = new HelperX();
	p_cdripx = new CCDRipX();
	p_title = new D2Xtitle();
	p_ftp = new D2Xftp();
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
	mdirscount = 0;
	mfilescount = 0;
	level = 0;
	renew = true;
}

D2Xdbrowser::~D2Xdbrowser()
{
	delete p_help;
	delete p_cdripx;
	delete p_title;
	delete p_ftp;
	delete m_pIsoReader;
}

void D2Xdbrowser::Renew()
{
	renew = true;
}

int compare( const void *arg1, const void *arg2 )
{
   /* Compare all of both strings: */
   return _stricmp( * ( char** ) arg1, * ( char** ) arg2 );
}

HDDBROWSEINFO D2Xdbrowser::processDirBrowser(int lines,char* path,XBGAMEPAD gp, XBIR_REMOTE ir,int type)
{
	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	HDDBROWSEINFO info;

	//if(strncmp(path,"d:",2))
	if(!(p_help->isdriveD(path)) && (type != SMBDIR))
		type = GAME;
	if(!strncmp(path,"ftp:",4))
		type = FTP;

	info.button = NO_PRESSED;

	if(renew || renewAll)
	{
		renew = false;
		D2Xdbrowser::renewAll = false;
		for(int i=0;i<mdirscount;i++)
		{
			if(cDirs[i])
			{
				delete[] cDirs[i];
				cDirs[i]=NULL;
			}
		}
	
		for(int i=0;i<mfilescount;i++)
		{
			if(cFiles[i])
			{
				delete[] cFiles[i];
				cFiles[i]=NULL;
			}
		}
		mdirscount = 0;
		mfilescount = 0;
		cbrowse = 1;
		crelbrowse = 1;
		coffset = 0;

		DPf_H("browser type %d",type);

		if(type != CDDA)
		{
			if(level > 0)
			{
				mdirscount = 1;
				cDirs[0] = new char[3];
				strcpy(cDirs[0],"..");
			}
			strcpy(sourcesearch,path);
			if((type != SMBDIR) && (type != FTP))
				strcat(sourcesearch,"*");

			strcpy(currentdir,path);

			// Start the find and check for failure.
			hFind = D2XFindFirstFile( sourcesearch, &wfd ,type);

			if( INVALID_HANDLE_VALUE == hFind )
			{
				return info;
			}
	
			do
			{
				if (wfd.cFileName[0]!=0)
		        {
				//DPf_H("found %hs",wfd.cFileName);
				// Only do files
				if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
					CStdString strDir=wfd.cFileName;
					if (strDir != "." && strDir != "..")
					{
						cDirs[mdirscount] = new char[strlen(wfd.cFileName)+1];
						strcpy(cDirs[mdirscount],wfd.cFileName);
						mdirscount++;
					}
				}
				else
				{
				
					cFiles[mfilescount] = new char[strlen(wfd.cFileName)+1];
					strcpy(cFiles[mfilescount],wfd.cFileName);
					mfilescount++;

				}
				}

			}
			while(D2XFindNextFile( hFind, &wfd ,type));

			// Close the find handle.
			D2XFindClose( hFind,type );
			//DPf_H("findclose. found %d dirs and %d files",mdirscount,mfilescount);
			qsort( (void *)cDirs, (size_t)mdirscount, sizeof( char * ), compare );
			qsort( (void *)cFiles, (size_t)mfilescount, sizeof( char * ), compare );
		} else {
			if(p_cdripx->Init()==E_FAIL)
			{
				DPf_H("Failed to init cdripx");
				return info;
			}
			mfilescount = p_cdripx->GetNumTocEntries();
			DPf_H("Found %d Tracks",mfilescount);
			p_cdripx->DeInit();
			if(D2Xtitle::i_network)
			{
				char temp[100];
				if(p_title->getCDDADiskTitle(temp))
				{
					
					for(int i=1;i<=mfilescount;i++)
					{
						strcpy(temp,"\0");
						p_title->getCDDATrackTitle(temp,i);
						cFiles[i-1] = new char[strlen(temp)+1];
						strcpy(cFiles[i-1],temp);
					}
					
				} 
				else
				{
					DPf_H("error during getCDDADiskTitle");
					for(int i=0;i<mfilescount;i++)
					{
						cFiles[i] = new char[8];
						sprintf(cFiles[i],"Track%02d",i+1);
					}
				}
			}
			else
			{
				for(int i=0;i<mfilescount;i++)
				{
					cFiles[i] = new char[8];
					sprintf(cFiles[i],"Track%02d",i+1);
				}
			}
		}
	}

	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP) || (ir.wPressedButtons == XINPUT_IR_REMOTE_UP)) {
		if(cbrowse > 1)
            cbrowse--;
		if(crelbrowse>1)
		{
            crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
	}
	if((gp.fY1 > 0.5)) {
		Sleep(100);
		if(cbrowse > 1)
            cbrowse--;
		if(crelbrowse>1)
		{
            crelbrowse--;
		} else {
			if(coffset > 0)
				coffset--;
		}
	}
	if((gp.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN) || (ir.wPressedButtons == XINPUT_IR_REMOTE_DOWN)) {
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-lines))
				coffset++;
		}
	}
	if(gp.fY1 < -0.5) {
		Sleep(100);
		if(cbrowse < (mdirscount+mfilescount))
            cbrowse++;
		if(crelbrowse<lines)
		{
            crelbrowse++;
		} else {
			if(coffset < (mdirscount+mfilescount-lines))
				coffset++;
		}
	}

	if(p_help->pressA(gp) || p_help->IRpressSELECT(ir))
	{
		if(cbrowse <= mdirscount)
		{
			renew = true;
			if(!strncmp(cDirs[cbrowse-1],"..",2))
			{
				level--;
				strcpy(path,cprevdir[level]);
			} 
			else
			{
				strcat(currentdir,cDirs[cbrowse-1]);
				cbrowse = 1;
				crelbrowse = 1;
				coffset = 0;
				strcpy(cprevdir[level],path);
				level++;
				strcpy(path,currentdir);
				strcat(path,"\\");
				DPf_H("new path: %hs, old path: %hs",path,cprevdir[level-1]);
			}
		} else {
			//strcat(currentdir,cFiles[cbrowse-mdirscount]);
		}
		
	}

	if(cbrowse <= mdirscount)
	{
		// Directory
		sprintf(info.item,"%s%s",path,cDirs[cbrowse-1]);
		strcpy(info.name,cDirs[cbrowse-1]);
		info.size = 0;
		wcscpy(info.title,L"");
		info.type=BROWSE_DIR;
	} else {
		if(cFiles[cbrowse-mdirscount-1] != NULL)
		{	
			sprintf(info.item,"%s%s",path,cFiles[cbrowse-mdirscount-1]);
			strcpy(info.name,cFiles[cbrowse-mdirscount-1]);
			info.size = p_help->getFilesize(info.item);	
			p_help->getXBETitle(info.item,info.title);
			info.track = cbrowse-mdirscount;
		}
		else 
		{
			sprintf(info.item,"%s",path);
			info.size = 0;
			wcscpy(info.title,L"");
		}
		////DPf_H("Delete file %hs",info.item);
		info.type=BROWSE_FILE;
	}
	
	if(p_help->pressX(gp) || p_help->pressSTART(gp))
	{
		if(mdirscount || mfilescount)
		{
			info.button = p_help->pressX(gp) ? BUTTON_X : BUTTON_START;
		}
		return info;	
	}
	
	if(p_help->pressY(gp))
	{
		if(cbrowse > mdirscount && !strcmp(cFiles[cbrowse-mdirscount-1],"default.xbe"))
		{
			info.button = BUTTON_Y;
			strcpy(info.item,path);
			return info;
		}
	}

	if(p_help->pressB(gp))
	{
		info.button = BUTTON_B;
	}

	if(p_help->pressRTRIGGER(gp))
	{
		info.button = BUTTON_RTRIGGER;
	}
	if(p_help->pressBLACK(gp))
	{
		info.button = BUTTON_BLACK;
	}
	if(p_help->pressWHITE(gp))
	{
		info.button = BUTTON_WHITE;
	}

	
	return info;
}

bool D2Xdbrowser::resetDirBrowser()
{

	for(int i=0;i<mdirscount;i++)
	{
		delete[] cDirs[i];
		cDirs[i]=NULL;
	}
	
	for(int i=0;i<mfilescount;i++)
	{
		delete[] cFiles[i];
		cFiles[i]=NULL;
	}
	cbrowse = 1;
	crelbrowse = 1;
	coffset = 0;
	mdirscount = 0;
	mfilescount = 0;
	level = 0;
	renew = true;
	return true;
}

bool D2Xdbrowser::showDirBrowser(int lines,float x,float y,DWORD fc,DWORD hlfc, CXBFont &font)
{
	WCHAR text[50];
	float tmpy=0;
	int c=0;
	char path[1024];

	strcpy(path,currentdir);
	strcat(path,"\\");

	for(int i=0;i<lines;i++)
	{
		c = i+coffset;
		tmpy = i*font.m_fFontHeight;
		if(c >= (mdirscount+mfilescount))
			break;
		char tname[34];
		if(c < mdirscount)
		{		
			strncpy(tname,cDirs[c],30);
			if(30 <= strlen(cDirs[c]))
				tname[30] = '\0';
			wsprintfW(text,L"<%hs>",tname);
		} else {
			strncpy(tname,cFiles[c-mdirscount],32);
			if(32 <= strlen(cFiles[c-mdirscount]))
				tname[32] = '\0';
			wsprintfW(text,L"%hs",tname);
		}
		if((i+coffset) == (cbrowse-1))
		{
            font.DrawText( x, y+tmpy, hlfc, text );
		} else {
			font.DrawText( x, y+tmpy, fc, text );
		}

	}


	
	return true;
}

HANDLE D2Xdbrowser::D2XFindFirstFile(char* lpFileName,LPWIN32_FIND_DATA lpFindFileData,int type)
{
	switch(type)
	{
	case DVD:
		dvd = DVDOpen("\\Device\\Cdrom0");
		return FindFirstFile(lpFileName,lpFindFileData);
		break;
	case GAME:
		return FindFirstFile(lpFileName,lpFindFileData);
		break;
	case ISO:
		{
		//m_pIsoReader = new CISO9660(m_cdrom);
			/*
		if (!m_pIsoReader->OpenDisc() )
		{
			delete m_pIsoReader;
			m_pIsoReader=NULL;
			return INVALID_HANDLE_VALUE;
		}*/
		m_pIsoReader = new iso9660();
		return m_pIsoReader->FindFirstFile(lpFileName,lpFindFileData);
		}
		break;
	case VCD:
		{
		/*m_pIsoReader = new CISO9660(m_cdrom);
		if (!m_pIsoReader->OpenDisc() )
		{
			delete m_pIsoReader;
			m_pIsoReader=NULL;
			return INVALID_HANDLE_VALUE;
		}*/
		m_pIsoReader = new iso9660();
		return m_pIsoReader->FindFirstFile(lpFileName,lpFindFileData);
		}
		break;
	case SVCD:
		{
		/*m_pIsoReader = new CISO9660(m_cdrom);
		if (!m_pIsoReader->OpenDisc() )
		{
			delete m_pIsoReader;
			m_pIsoReader=NULL;
			return INVALID_HANDLE_VALUE;
		}*/
		m_pIsoReader = new iso9660();
		return m_pIsoReader->FindFirstFile(lpFileName,lpFindFileData);
		}
		break;
	case FTP:
		return p_ftp->FindFirstFile(lpFileName,lpFindFileData);
		break;
		/*
	case SMBDIR:
		{
		
		char* pPasswd=strstr(&lpFileName[4],":");
		if (pPasswd)
		{
			int i=0;
			pPasswd++;
			while (pPasswd[i] && pPasswd[i] != '@') i++;
			strncpy(szPassWd,pPasswd,i);
			szPassWd[i]=0;
		}
		else strcpy(szPassWd,"");
		
		smb.setNBNSAddress("192.168.1.1");
		smb.setPasswordCallback(&cb);
		smbfd = smb.opendir(lpFileName);
		if (smbfd < 0) 
		{
			DPf_H("Could not open smb dir %s",lpFileName);
			return INVALID_HANDLE_VALUE;
		}
		DPf_H("open smb dir");
		dirEnt = smb.readdir(smbfd);
		if (dirEnt->st_mode & S_IFDIR) lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
		else lpFindFileData->dwFileAttributes =  FILE_ATTRIBUTE_NORMAL;
		if (dirEnt->d_name && strcmp(dirEnt->d_name,".") && strcmp(dirEnt->d_name,".."))
		{
			DPf_H("found first entry %s",dirEnt->d_name);
			strcpy(lpFindFileData->cFileName,dirEnt->d_name);
			return NULL;
		} else 
		{
			DPf_H("No entry found");
			return INVALID_HANDLE_VALUE;
		}
		}
		break;
		*/
	default:
		return FindFirstFile(lpFileName,lpFindFileData);
		break;
	}
}

BOOL D2Xdbrowser::D2XFindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData,int type)
{
	switch(type)
	{
	case DVD:
		return FindNextFile(hFindFile,lpFindFileData);
		break;
	case GAME:
		return FindNextFile(hFindFile,lpFindFileData);
		break;
	case ISO:
		return m_pIsoReader->FindNextFile(hFindFile,lpFindFileData);
		break;
	case VCD:
		return m_pIsoReader->FindNextFile(hFindFile,lpFindFileData);
		break;
	case SVCD:
		return m_pIsoReader->FindNextFile(hFindFile,lpFindFileData);
		break;
	case FTP:
		return p_ftp->FindNextFile(hFindFile,lpFindFileData);
		break;
		/*
	case SMBDIR:
		dirEnt = smb.readdir(smbfd);
		if (dirEnt->st_mode & S_IFDIR) lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY;
		else lpFindFileData->dwFileAttributes =  FILE_ATTRIBUTE_NORMAL;
		if (dirEnt->d_name && strcmp(dirEnt->d_name,".") && strcmp(dirEnt->d_name,".."))
		{
			DPf_H("Found entry %s",dirEnt->d_name);
			strcpy(lpFindFileData->cFileName,dirEnt->d_name);
			return true;
		} else
		{
			DPf_H("No entry found");
			return false;
		}
		break;
		*/
	default:
		return FindNextFile(hFindFile,lpFindFileData);
		break;
	}
}

BOOL D2Xdbrowser::D2XFindClose(HANDLE hFindFile,int type)
{
	bool status;
	switch(type)
	{
	case DVD:
		DVDClose(dvd);
		return FindClose(hFindFile);
		break;
	case GAME:
		return FindClose(hFindFile);
		break;
	case ISO:
		//hFindFile = NULL;
		//if (!m_pIsoReader) return true;
		//delete m_pIsoReader;
		//m_pIsoReader=NULL;
		//return true;
		status = m_pIsoReader->FindClose(hFindFile);
		delete m_pIsoReader;
		m_pIsoReader=NULL;
		return status;
		break;
	case VCD:
		//hFindFile = NULL;
		//if (!m_pIsoReader) return true;
		//delete m_pIsoReader;
		//m_pIsoReader=NULL;
		//return true;*/
		status = m_pIsoReader->FindClose(hFindFile);
		delete m_pIsoReader;
		m_pIsoReader=NULL;
		return status;
		break;
	case SVCD:
		//hFindFile = NULL;
		//if (!m_pIsoReader) return true;
		//delete m_pIsoReader;
		//m_pIsoReader=NULL;
		//return true;*/
		status = m_pIsoReader->FindClose(hFindFile);
		delete m_pIsoReader;
		m_pIsoReader=NULL;
		return status;
		break;
	case FTP:
		return p_ftp->FindClose(hFindFile);
		break;
		/*
	case SMBDIR:
		hFindFile = NULL;
		smb.closedir(smbfd);
		return true;
		break;
		*/
	default:
		return FindClose(hFindFile);
		break;
	}
}

int D2Xdbrowser::getFilesize(char* filename,int type)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	LARGE_INTEGER liSize;
	LONGLONG llValue = 0;
	hFind = D2XFindFirstFile( filename, &wfd,type );
	if( INVALID_HANDLE_VALUE == hFind )
	{
		llValue = 0;
	}
	else
	{
		liSize.LowPart = wfd.nFileSizeLow;
		liSize.HighPart = wfd.nFileSizeHigh;
		llValue = liSize.QuadPart/1024;
	}
	D2XFindClose( hFind,type );
	return llValue;
}

int D2Xdbrowser::FTPconnect(char* ip,char* user,char* pwd)
{
	return p_ftp->connect(ip,user,pwd);
}

int D2Xdbrowser::FTPclose()
{
	return p_ftp->close();
}

bool D2Xdbrowser::FTPisConnected()
{
	return p_ftp->isConnected();
}
