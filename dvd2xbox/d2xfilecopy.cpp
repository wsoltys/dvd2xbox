#include "d2xfilecopy.h"

int D2Xfilecopy::i_process = 0;
int	D2Xfilecopy::copy_failed = 0;
int	D2Xfilecopy::copy_ok = 0;
int	D2Xfilecopy::copy_renamed = 0;
LONGLONG D2Xfilecopy::llValue = 1;
float D2Xfilecopy::f_ogg_quality = 1.0;
bool D2Xfilecopy::b_finished = false;
WCHAR D2Xfilecopy::c_source[1024];
WCHAR D2Xfilecopy::c_dest[1024];



D2Xfilecopy::D2Xfilecopy()
{
	p_help = new HelperX();
	p_cdripx = new CCDRipX();
	p_title = new D2Xtitle();
	p_log = new D2Xlogger();
	ftype = UNKNOWN;
	writeLog = false;
}

D2Xfilecopy::~D2Xfilecopy()
{
	delete p_help;
	delete p_cdripx;
	delete p_title;
	delete p_log;
}

int D2Xfilecopy::GetProgress()
{
	return D2Xfilecopy::i_process;
}

int D2Xfilecopy::GetMBytes()
{
	return D2Xfilecopy::llValue/1048576;
}

DWORD CALLBACK CopyProgressRoutine(
  LARGE_INTEGER TotalFileSize,          // file size
  LARGE_INTEGER TotalBytesTransferred,  // bytes transferred
  LARGE_INTEGER StreamSize,             // bytes in stream
  LARGE_INTEGER StreamBytesTransferred, // bytes transferred for stream
  DWORD dwStreamNumber,                 // current stream
  DWORD dwCallbackReason,               // callback reason
  HANDLE hSourceFile,                   // handle to source file
  HANDLE hDestinationFile,              // handle to destination file
  LPVOID lpData                         // from CopyFileEx
)
{
	if(TotalFileSize.QuadPart > 0)
		D2Xfilecopy::i_process = (TotalBytesTransferred.QuadPart*100)/TotalFileSize.QuadPart;
	return PROGRESS_CONTINUE;
}


void D2Xfilecopy::FileCopy(HDDBROWSEINFO source,char* dest,int type)
{
	if(p_help->isdriveD(dest))
	{
		StopThread();
		D2Xfilecopy::b_finished = true;
		return;
	}
	/*
	for(int i=0;i<D2Xpatcher::mXBECount;i++)
	{
		delete[] D2Xpatcher::mXBEs[i];
		D2Xpatcher::mXBEs[i] = NULL;
	}
	*/
	llValue = 1;
	D2Xpatcher::reset();
	D2Xfilecopy::i_process = 0;
	D2Xfilecopy::b_finished = false;
	D2Xfilecopy::copy_failed = 0;
	D2Xfilecopy::copy_ok = 0;
	D2Xfilecopy::copy_renamed = 0;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
	fsource = source;
	strcpy(fdest,dest);
	if(!(p_help->isdriveD(source.item)))
		ftype = GAME;
	else
        ftype = type;
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

/////////////////////////////////////////////////////////////
// UDF

int D2Xfilecopy::FileUDF(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_help->getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		if((ftype == DVD) && (strstr(source.item,".vob") || strstr(source.item,".VOB")))
			stat = CopyVOB(source.item,temp);
		else
			stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
			//stat = CopyFile(source.item,temp,false);
		SetFileAttributes(temp,FILE_ATTRIBUTE_NORMAL);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		strcat(temp,"\\");
		sprintf(temp2,"%s%s",dest,source.name);
		p_help->addSlash(temp2);
		stat = DirUDF(temp,temp2);
		p_log->WLog(L"");
		p_log->WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log->WLog(L"");
	}

	return stat;
}

int D2Xfilecopy::DirUDF(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	
	// We must create the dest directory
	if(CreateDirectory(destroot,NULL))
	{

	}

	//DPf_H("copy %s to %s",path,destroot);
	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
		return 0;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);
			
			strcpy(destfile,destroot);
			strcpy(temp,wfd.cFileName);
			p_help->getFatxName(wfd.cFileName);
			strcat(destfile,wfd.cFileName);

			if(strcmp(temp,wfd.cFileName))
			{
				D2Xpatcher::addFATX(wfd.cFileName);
				p_log->WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				copy_renamed++;
			}

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"\\");
				// Recursion
				if(!DirUDF(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				if((ftype == DVD) && (strstr(sourcefile,".vob") || strstr(sourcefile,".VOB")))
				{
					if(!CopyVOB(sourcefile,destfile))
					{
						DPf_H("can't copy %s to %s",sourcefile,destfile);
						p_log->WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						p_log->WLog(L"Copied %hs to %hs",sourcefile,destfile);
						copy_ok++;
						//DPf_H("copydir %s to %s",sourcefile,destfile);
					}
				}
				else 
				{
					if((strstr(wfd.cFileName,".xbe")) || (strstr(wfd.cFileName,".XBE")))
					{
						D2Xpatcher::addXBE(destfile);
					}
		
					//if(!CopyFile(sourcefile,destfile,false))
					if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
					{
						DPf_H("can't copy %s to %s",sourcefile,destfile);
						p_log->WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						p_log->WLog(L"Copied %hs to %hs",sourcefile,destfile);
						copy_ok++;
						//DPf_H("copydir %s to %s",sourcefile,destfile);
					}
					if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
					{
						liSize.LowPart = wfd.nFileSizeLow;
						liSize.HighPart = wfd.nFileSizeHigh;
						D2Xfilecopy::llValue += liSize.QuadPart;
					}
				}
				/*
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					D2Xfilecopy::llValue += liSize.QuadPart;
				}
				*/
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return 1;
}

/////////////////////////////////////////////////////
// ISO


int D2Xfilecopy::FileISO(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_help->getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		stat = CopyISOFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_help->addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		p_help->addSlash(temp2);
		DPf_H("copy iso %s to %s",temp,temp2);
		stat = DirISO(temp,temp2);
		p_log->WLog(L"");
		p_log->WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log->WLog(L"");
	}

	return stat;
}

bool D2Xfilecopy::CopyISOFile(char* lpcszFile,char* destfile)
{
	//WriteLog(L"ISO copy: %s to %s",lpcszFile,destfile);
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
	// attempt open file
	//CIoSupport cdrom;
	//CISO9660 iso9660(cdrom);
	iso9660* mISO;
	mISO = new iso9660();
	HANDLE fh;
	/*
	if (!iso9660.OpenDisc() )
	{
		DPf_H("Couldn't open disc");
		return FALSE;
	}*/

	if ((fh = mISO->OpenFile(lpcszFile)) == INVALID_HANDLE_VALUE)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log->WLog(L"Couldn't open source file %hs",lpcszFile);
		delete mISO;
		mISO = NULL;
		return FALSE;
	}

	//DWORD dwBufferSize  = 2048*16;
	int dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	DWORD dwFileSizeHigh;
	//uint64_t fileSize   = iso9660.GetFileSize();
	uint64_t fileSize   = mISO->GetFileSize((HANDLE)1, &dwFileSizeHigh);
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);

	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		DPf_H("Couldn't create File: %s",destfile);
		mISO->CloseFile(fh);
		delete mISO;
		mISO = NULL;
		delete buffer;
		buffer = NULL;
		return FALSE;
	}
	DPf_H("dest file created: %s",destfile);

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

		//lRead = iso9660.ReadFile(NULL,buffer,dwBufferSize);
		DWORD dwTotalBytesRead;
		lRead = mISO->ReadFile((char*)buffer,&dwBufferSize,&dwTotalBytesRead);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);
		WriteFile(hFile,buffer,(DWORD)lRead,&dwWrote,NULL);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	mISO->CloseFile(fh);
	delete mISO;
	mISO = NULL;
	delete buffer;
	buffer = NULL;

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

bool D2Xfilecopy::DirISO(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	//LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	//CIoSupport cdrom;
	//CISO9660 mISO(cdrom);
	iso9660* mISO;
	mISO = new iso9660();
	//if (!(mISO.OpenDisc() ))
	//	return false;

	// We must create the dest directory
	if(CreateDirectory(destroot,NULL))
	{
		DPf_H("Created Directory: %hs",destroot);
	}


	strcpy(sourcesearch,path);
	//strcpy(sourcesearch,"\\");
	//strcat(sourcesearch,"*");

	// Start the find and check for failure.
	memset(&wfd,0,sizeof(wfd));
	hFind = mISO->FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    DPf_H("SetDirectory ISOFindFirstFile returned invalid HANDLE");
	    return false;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			
			if (wfd.cFileName[0]==0)
				continue;
			/*
			if (!(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				wfd.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			*/
			strcpy(sourcefile,path);
			strcat(sourcefile,wfd.cFileName);
			strcpy(destfile,destroot);
			strcpy(temp,wfd.cFileName);
			p_help->getFatxName(wfd.cFileName);
			strcat(destfile,wfd.cFileName);

			if(strcmp(temp,wfd.cFileName))
			{
				D2Xpatcher::addFATX(wfd.cFileName);
				p_log->WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				copy_renamed++;
			}

			DPf_H("found %hs",wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				CStdString strDir=wfd.cFileName;
				if (strDir != "." && strDir != "..")
				{
					strcat(sourcefile,"\\");
					strcat(destfile,"\\");
					//mDirCount++;
					// Recursion
					if(!DirISO(sourcefile,destfile)) continue;
				}
			}
			else
			{
								
				DPf_H("Copying: %hs",wfd.cFileName);
				if(!CopyISOFile(sourcefile,destfile))
				{
					DPf_H("Failed to copy %hs to %hs.",sourcefile,destfile);
					p_log->WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					copy_failed++;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log->WLog(L"Copied %hs to %hs",sourcefile,destfile);
					copy_ok++;
				}
				/*
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					D2Xfilecopy::llValue += liSize.QuadPart;
				}
				*/
				
			}

	    }
	    while(mISO->FindNextFile( hFind, &wfd ));

	    // Close the find handle.
		hFind = NULL;
	}
	delete mISO;
	mISO = NULL;
	return true;
}

////////////////////////////////////////////////////////////////
// CDDA

int D2Xfilecopy::FileCDDA(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	if(source.type == BROWSE_FILE)
	{
		stat = CopyCDDATrack(source,dest);
	}
	else if(source.type == BROWSE_DIR)
	{
		stat = DirCDDA(dest);
	}

	return stat;
}

int D2Xfilecopy::DirCDDA(char* dest)
{
	int mfilescount;
	char* cFiles[100];
	if(p_cdripx->Init()!=E_FAIL)
	{
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
		for(int i=0;i<mfilescount;i++)
		{
			fsource.track = i+1;
			strcpy(fsource.name,cFiles[i]);
			DPf_H("Calling Filecopy with Track %d,%s,%s",i+1,fsource.name,dest);
			if(CopyCDDATrack(fsource,dest))
				copy_ok++;
			else
				copy_failed++;
		}
				
	} else {	
		DPf_H("Failed to init cdripx");
	}
	return 1;
}

int D2Xfilecopy::CopyCDDATrack(HDDBROWSEINFO source,char* dest)
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	int	nPercent = 0;
	int	nPeakValue;
	int	nJitterErrors;
	int	nJitterPos;
	char file[1024];
	char temp[1024];
	if(p_cdripx->Init()==E_FAIL)
	{
		DPf_H("Failed to init cdripx (FileCDDA)");
		return 0;
	}
	DPf_H("dest %s source %s",dest,source.name);
	//sprintf(file,"%s%s.ogg",dest,source.name);
	strcpy(temp,source.name);
	p_title->getvalidFilename(dest,temp,".ogg");
	DPf_H("file %s",temp);
//	p_help->getFatxName(temp);
	//DPf_H("file %s",temp);
	sprintf(file,"%s%s",dest,temp);
	DPf_H("file %s",file);
	wsprintfW(D2Xfilecopy::c_source,L"%hs",source.name);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",file);
	DPf_H("Rip track %d to %s with quality setting %f",source.track,file,D2Xfilecopy::f_ogg_quality);
	p_cdripx->InitOgg(source.track,file,D2Xfilecopy::f_ogg_quality);
	p_cdripx->AddOggComment("Comment","Ripped with dvd2xbox");
	if(D2Xtitle::i_network)
	{
		// shit
		if((D2Xtitle::track_artist[source.track-1] != NULL) && (strlen(D2Xtitle::track_artist[source.track-1]) > 1))
			p_cdripx->AddOggComment("Artist",D2Xtitle::track_artist[source.track-1]);
		else
			p_cdripx->AddOggComment("Artist",D2Xtitle::disk_artist);
		p_cdripx->AddOggComment("Album",D2Xtitle::disk_title);
		p_cdripx->AddOggComment("Title",D2Xtitle::track_title[source.track-1]);
	}
	
	while(CDRIPX_DONE != p_cdripx->RipToOgg(nPercent,nPeakValue,nJitterErrors,nJitterPos))
	{
		D2Xfilecopy::i_process = nPercent;	
	}
	p_cdripx->DeInit();
	return 1;
}

////////////////////////////////////////////////////////////////
// DVD

/*
int D2Xfilecopy::FileVOB(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("Could not authenticate DVD");
		return 0;
	}
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_help->getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		if(!strstr(source.item,".vob") && !strstr(source.item,".VOB"))
		{
			int stat;
			stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
			DPf_H("Copy %s to %s status %d",source.item,temp,stat);
			SetFileAttributes(temp,FILE_ATTRIBUTE_NORMAL);
		} else
            stat = CopyVOB(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		strcat(temp,"\\");
		//sprintf(temp2,"%s%s\\",dest,source.name);
		//stat = DirISO(temp,dest);
	}
	DVDClose(dvd);
	return stat;
}*/

int D2Xfilecopy::CopyVOB(char* sourcefile,char* destfile)
{
	
	#define rblocks 256
	unsigned char* buffer;
	buffer = new unsigned char[rblocks*2048];
	
	dvd_file_t*		vob;
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("Could not authenticate DVD");
		return 0;
	}
	//
	int set;
	int title;
	int ret = sscanf(sourcefile,"d:\\VIDEO_TS\\VTS_%02i_%i.VOB",&title,&set);
	DPf_H("title %d,set %d,ret %d",title,set,ret);
	//
	vob = DVDOpenSingleFile(dvd,sourcefile);
	if(!vob)
	{
		DPf_H("Could not open file %s code %d",sourcefile,vob);
		return 0;
	}
	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		DPf_H("Couldn't create File: %s",destfile);
		DVDCloseFile(vob);
		return 0;
	}
	
	uint64_t fileSize   = DVDFileSize(vob);
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	uint64_t fileOffset = 0;
	long lRead;
	DWORD dwWrote;

	DPf_H("filesize %d",fileSize*2048);

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

		lRead = DVDReadBlocks(vob,fileOffset,rblocks,buffer);
		if (lRead<=0)
			break;
		
		//DPf_H("read blocks %d",lRead);
		if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);
		WriteFile(hFile,buffer,(DWORD)lRead*2048,&dwWrote,NULL);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	DVDCloseFile(vob);
	delete buffer;

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
	DVDClose(dvd);
	return 1;
}


////////////////////////////////////////////////////////////////
// Thread

//*************************************************************************************
void D2Xfilecopy::OnStartup()
{
}

//*************************************************************************************
void D2Xfilecopy::OnExit()
{
	D2Xfilecopy::b_finished = true;
}

//*************************************************************************************
void D2Xfilecopy::Process()
{
	DPf_H("type %d, %s %s",ftype,fsource.item,fdest);
	switch(ftype)
	{
	case DVD:
		FileUDF(fsource,fdest);
		break;
	case GAME:
		FileUDF(fsource,fdest);
		break;
	case ISO:
		FileISO(fsource,fdest);
		break;
	case VCD:
		FileISO(fsource,fdest);
		break;
	case SVCD:
		FileISO(fsource,fdest);
		break;
	case CDDA:
		FileCDDA(fsource,fdest);
		break;
	default:
		break;
	}
	ftype = UNKNOWN;
	//StopThread();
	//DPf_H("after stopthread");
}

void D2Xfilecopy::CancleThread()
{
	ftype = UNKNOWN;
	D2Xfilecopy::b_finished = true;
	//StopThread();
}

///////////////////////////////////////////////////
// logging

void D2Xfilecopy::setLogFilename(char *file)
{
	strcpy(logFilename,file);
}


void D2Xfilecopy::enableLog(bool value)
{
	writeLog=value;
}

void D2Xfilecopy::WLog(WCHAR *message,...)
{
	//DPf_H("calling WriteLog %s",logFilename);
	if(logFilename == NULL)
		return;
	WCHAR expanded_message[1024];
	va_list tGlop;
	// Expand the varable argument list into the text buffer
	va_start(tGlop,message);
	if(vswprintf(expanded_message,message,tGlop)==-1)
	{
		// Fatal overflow, lets abort
		return;
	}
	va_end(tGlop);
	FILE *stream;
	char mchar[1024];
	if(!(stream = fopen(logFilename,"a+")))
		return;
	wsprintf(mchar,"%S\n",expanded_message);
	fwrite(mchar,sizeof(char),strlen(mchar),stream);
	fclose(stream);
}
