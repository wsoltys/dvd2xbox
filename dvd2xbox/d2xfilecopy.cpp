#include "d2xfilecopy.h"

int D2Xfilecopy::i_process = 0;
int	D2Xfilecopy::copy_failed = 0;
int	D2Xfilecopy::copy_ok = 0;
int	D2Xfilecopy::copy_renamed = 0;
LONGLONG D2Xfilecopy::llValue = 1;
float D2Xfilecopy::f_ogg_quality = 1.0;
bool D2Xfilecopy::b_finished = false;
WCHAR D2Xfilecopy::c_source[1024]={0};
WCHAR D2Xfilecopy::c_dest[1024]={0};
//char* D2Xfilecopy::excludeDirs = NULL;
//char* D2Xfilecopy::excludeFiles = NULL;

vector<string> D2Xfilecopy::excludeList;
vector<string> D2Xfilecopy::XBElist;
map<string,string> D2Xfilecopy::RENlist;


D2Xfilecopy::D2Xfilecopy()
{
	//p_help = new HelperX();
	//p_title = new D2Xtitle();
	//p_log = D2Xlogger();
	ftype = UNKNOWN;
	m_bStop = false;
}

D2Xfilecopy::~D2Xfilecopy()
{
	//delete p_help;
	//delete p_title;
	//delete p_log;
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
	{
		D2Xfilecopy::i_process = (TotalBytesTransferred.QuadPart*100)/TotalFileSize.QuadPart;
		/*D2Xfilecopy::i_process = TotalBytesTransferred.QuadPart;
		D2Xfilecopy::i_process *= 100;
		D2Xfilecopy::i_process /= TotalFileSize.QuadPart;*/
	}
	return PROGRESS_CONTINUE;
}


void D2Xfilecopy::FileCopy(HDDBROWSEINFO source,char* dest,int type)
{
	if(p_utils.isdriveD(dest))
	{
		StopThread();
		D2Xfilecopy::b_finished = true;
		return;
	}
	llValue = 1;
	D2Xpatcher::reset();
	XBElist.clear();
	RENlist.clear();
	D2Xfilecopy::i_process = 0;
	D2Xfilecopy::b_finished = false;
	D2Xfilecopy::copy_failed = 0;
	D2Xfilecopy::copy_ok = 0;
	D2Xfilecopy::copy_renamed = 0;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
	fsource = source;
	strcpy(fdest,dest);
	if(!(p_utils.isdriveD(source.item)))
		ftype = GAME;
	else
        ftype = type;

	if(!strncmp(dest,"ftp:",4) && (ftype == GAME))
		ftype = UDF2FTP;
	else if(!strncmp(source.item,"ftp:",4))
		ftype = FTP2UDF;

	SetPriority(THREAD_PRIORITY_HIGHEST);
	DPf_H("Left FileCopy. ftype: %d",ftype);
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
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
		//stat = CopyUDFFile(source.item,temp);
		SetFileAttributes(temp,FILE_ATTRIBUTE_NORMAL);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		strcat(temp,"\\");
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		stat = DirUDF(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
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
	D2Xtitle p_title;

	
	// We must create the dest directory
	CreateDirectory(destroot,NULL);

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
			p_utils.getFatxName(wfd.cFileName);
			

			if(!strcmp(temp,wfd.cFileName))
				strcat(destfile,wfd.cFileName);
			else
			{
				p_title.getvalidFilename(destroot,wfd.cFileName,"");
				strcat(destfile,wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				RENlist.insert(pair<string,string>(sourcefile,destfile));
				++copy_renamed;
			} 
				


			if(!excludeList.empty())
			{
				iexcludeList it;
				it = excludeList.begin();
				bool cont = true;
				while (it != excludeList.end() )
				{
					string& item = *it;
					//DPf_H("Checking exclude item %s with %s",item.c_str(),sourcefile);
					if(!_stricmp(item.c_str(),sourcefile))
					{
						p_log.WLog(L"excluded %hs due to rule.",sourcefile);
						cont = false;
					}
					++it;
				}
				if(!cont)
					continue;
			}

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"\\");
				if(!DirUDF(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				
				if(strstr(wfd.cFileName,".xbe") || strstr(wfd.cFileName,".XBE"))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
	
				if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
				//if(!CopyUDFFile(sourcefile,destfile))
				{
					DPf_H("can't copy %s to %s",sourcefile,destfile);
					p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					++copy_failed;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
					++copy_ok;
				}
				if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				{
					liSize.LowPart = wfd.nFileSizeLow;
					liSize.HighPart = wfd.nFileSizeHigh;
					D2Xfilecopy::llValue += liSize.QuadPart;
				}
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return 1;
}

bool D2Xfilecopy::CopyUDFFile(char* lpcszFile,char* destfile)
{
#define UDFBUFFERSIZE	32768	// 2048*16
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	HANDLE fh = CreateFile( lpcszFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

	if (fh  == INVALID_HANDLE_VALUE)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	//int dwBufferSize  = UDFBUFFERSIZE;
	BYTE buffer[UDFBUFFERSIZE];

	uint64_t fileSize   = GetFileSize(fh,NULL);
	uint64_t fileOffset = 0;
	bool bResult		= false;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);


	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}
	
	DPf_H("dest file created: %s",destfile);

	//CHAR szText[128];
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	DWORD lRead;
	DWORD dwWrote;

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			//sprintf(szText, STRING(403) ,nNewPercentage);
			nOldPercentage = nNewPercentage;
		}

	
		bResult = ReadFile(fh,buffer,UDFBUFFERSIZE,&lRead,NULL);
		//if (lRead<=0)
		//	break;
		if (bResult &&  lRead == 0 ) 
		{ 
		    // We're at the end of the file. 
			break;
		} 

		//if((fileOffset+lRead) > fileSize)
		//	lRead = long(fileSize - fileOffset);
		WriteFile(hFile,buffer,lRead,&dwWrote,NULL);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	//} while ( fileOffset<fileSize );
	} while ( true );

	CloseHandle(hFile);
	CloseHandle(fh);

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

/////////////////////////////////////////////////////////////
// UDF DVD

int D2Xfilecopy::FileDVD(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];

	/*dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("Could not authenticate DVD");
		g_d2xSettings.generalError = COULD_NOT_AUTH_DVD;
		return 0;
	}*/
	
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		if(strstr(source.item,".vob") || strstr(source.item,".VOB"))
			stat = CopyVOB(source.item,temp);
		else
			stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
		SetFileAttributes(temp,FILE_ATTRIBUTE_NORMAL);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		strcat(temp,"\\");
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		stat = DirDVD(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}
	return stat;

	//DVDClose(dvd);
}

int D2Xfilecopy::DirDVD(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	D2Xtitle p_title;

	
	// We must create the dest directory
	if(CreateDirectory(destroot,NULL) == 0)
	{
		// do nothing
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
			p_utils.getFatxName(wfd.cFileName);
			

			if(!strcmp(temp,wfd.cFileName))
				strcat(destfile,wfd.cFileName);
			else
			{
				p_title.getvalidFilename(destroot,wfd.cFileName,"");
				strcat(destfile,wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				RENlist.insert(pair<string,string>(sourcefile,destfile));
				copy_renamed++;
			} 
				


			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"\\");
				if(!DirDVD(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				if(strstr(sourcefile,".vob") || strstr(sourcefile,".VOB"))
				{
					if(!CopyVOB(sourcefile,destfile))
					{
						DPf_H("can't copy %s to %s",sourcefile,destfile);
						p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
						copy_ok++;
						//DPf_H("copydir %s to %s",sourcefile,destfile);
					}
				}
				else 
				{
					
					if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
					{
						DPf_H("can't copy %s to %s",sourcefile,destfile);
						p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
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
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return 1;
}


int D2Xfilecopy::CopyVOB(char* sourcefile,char* destfile)
{
	
	#define rblocks 256
	unsigned char* buffer;
	buffer = new unsigned char[rblocks*2048];
	
	dvd_file_t*		vob;
	dvd_reader_t*	dvd;
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("Could not authenticate DVD");
		g_d2xSettings.generalError = COULD_NOT_AUTH_DVD;
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
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		stat = CopyISOFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		DPf_H("copy iso %s to %s",temp,temp2);
		stat = DirISO(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}

	return stat;
}

bool D2Xfilecopy::CopyISOFile(char* lpcszFile,char* destfile)
{
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
	// attempt open file
	iso9660 mISO;
	//mISO = new iso9660();
	HANDLE fh;

	if ((fh = mISO.OpenFile(lpcszFile)) == INVALID_HANDLE_VALUE)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		//delete mISO;
		//mISO = NULL;
		return FALSE;
	}

	//DWORD dwBufferSize  = 2048*16;
	int dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	//DWORD dwFileSizeHigh;
	//uint64_t fileSize   = mISO->GetFileSize((HANDLE)1, &dwFileSizeHigh);
	uint64_t fileSize   = mISO.GetFileSize();
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);


	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		DPf_H("Couldn't create File: %s",destfile);
		//mISO->CloseFile(fh);
		mISO.CloseFile();
		//delete mISO;
		//mISO = NULL;
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

		//DWORD dwTotalBytesRead;
		//lRead = mISO->ReadFile((char*)buffer,&dwBufferSize,&dwTotalBytesRead);
		lRead = mISO.ReadFile(1,buffer,dwBufferSize);
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
	mISO.CloseFile();
	//delete mISO;
	//mISO = NULL;
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

	D2Xtitle p_title;
	iso9660 mISO;
	//mISO = new iso9660();

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
	hFind = mISO.FindFirstFile( sourcesearch, &wfd );

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
			p_utils.getFatxName(wfd.cFileName);
			

			if(!strcmp(temp,wfd.cFileName))
				strcat(destfile,wfd.cFileName);
			else
			{
				p_title.getvalidFilename(destroot,wfd.cFileName,"");
				strcat(destfile,wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
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
					p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					copy_failed++;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
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
	    while(mISO.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
		hFind = NULL;
	}
	//delete mISO;
	//mISO = NULL;
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
	CCDRipX	p_cdripx;
	D2Xtitle p_title;
	if(p_cdripx.Init()!=E_FAIL)
	{
		mfilescount = p_cdripx.GetNumTocEntries();
		DPf_H("Found %d Tracks",mfilescount);
		p_cdripx.DeInit();
		if(D2Xtitle::i_network)
		{
			char temp[100];
			if(p_title.getCDDADiskTitle(temp))
			{
				
				for(int i=1;i<=mfilescount;i++)
				{	
					strcpy(temp,"\0");
					p_title.getCDDATrackTitle(temp,i);
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
	if(g_d2xSettings.cdda_encoder == OGGVORBIS)
		return CopyCDDATrackOgg(source,dest);
	else if(g_d2xSettings.cdda_encoder == WAV)
		return CopyCDDATrackWav(source,dest);
	else
		return CopyCDDATrackLame(source,dest);
}

int D2Xfilecopy::CopyCDDATrackOgg(HDDBROWSEINFO source,char* dest)
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	int	nPercent = 0;
	int	nPeakValue;
	int	nJitterErrors;
	int	nJitterPos;
	char file[1024];
	char temp[1024];
	CCDRipX	p_cdripx;
	D2Xtitle p_title;
	if(p_cdripx.Init()==E_FAIL)
	{
		DPf_H("Failed to init cdripx (FileCDDA)");
		return 0;
	}
	DPf_H("dest %s source %s",dest,source.name);
	//sprintf(file,"%s%s.ogg",dest,source.name);
	strcpy(temp,source.name);
	p_title.getvalidFilename(dest,temp,".ogg");
	DPf_H("file %s",temp);
//	p_utils.getFatxName(temp);
	//DPf_H("file %s",temp);
	sprintf(file,"%s%s",dest,temp);
	DPf_H("file %s",file);
	wsprintfW(D2Xfilecopy::c_source,L"%hs",source.name);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",file);
	DPf_H("Rip track %d to %s with quality setting %f",source.track,file,D2Xfilecopy::f_ogg_quality);
	p_cdripx.InitOgg(source.track,file,D2Xfilecopy::f_ogg_quality);
	p_cdripx.AddOggComment("Comment","Ripped with dvd2xbox");
	if(D2Xtitle::i_network)
	{
		// shit
		if((D2Xtitle::track_artist[source.track-1] != NULL) && (strlen(D2Xtitle::track_artist[source.track-1]) > 1))
			p_cdripx.AddOggComment("Artist",D2Xtitle::track_artist[source.track-1]);
		else
			p_cdripx.AddOggComment("Artist",D2Xtitle::disk_artist);
		p_cdripx.AddOggComment("Album",D2Xtitle::disk_title);
		p_cdripx.AddOggComment("Title",D2Xtitle::track_title[source.track-1]);
	}
	
	while(CDRIPX_DONE != p_cdripx.RipToOgg(nPercent,nPeakValue,nJitterErrors,nJitterPos))
	{
		D2Xfilecopy::i_process = nPercent;	
	}
	p_cdripx.DeInit();
	return 1;
}

int D2Xfilecopy::CopyCDDATrackLame(HDDBROWSEINFO source,char* dest)
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	int	nPercent = 0;
	int	nPeakValue;
	int	nJitterErrors;
	int	nJitterPos;
	char file[1024];
	char temp[1024];
	CCDRipX	p_cdripx;
	D2Xtitle p_title;
	if(p_cdripx.Init()==E_FAIL)
	{
		DPf_H("Failed to init cdripx (FileCDDA)");
		return 0;
	}
	DPf_H("dest %s source %s",dest,source.name);
	//sprintf(file,"%s%s.ogg",dest,source.name);
	strcpy(temp,source.name);
	p_title.getvalidFilename(dest,temp,".mp3"); 
	DPf_H("file %s",temp);
//	p_utils.getFatxName(temp);
	//DPf_H("file %s",temp);
	sprintf(file,"%s%s",dest,temp);
	DPf_H("file %s",file);
	wsprintfW(D2Xfilecopy::c_source,L"%hs",source.name);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",file);
	DPf_H("Rip track %d to %s with Lame",source.track,file);
	p_cdripx.InitLame(source.track,file);
	p_cdripx.AddLameTag(lame_comment,"Ripped with dvd2xbox");
	if(D2Xtitle::i_network)
	{
		// shit
		if((D2Xtitle::track_artist[source.track-1] != NULL) && (strlen(D2Xtitle::track_artist[source.track-1]) > 1))
			p_cdripx.AddLameTag(lame_artist,D2Xtitle::track_artist[source.track-1]);
		else
			p_cdripx.AddLameTag(lame_artist,D2Xtitle::disk_artist);
		p_cdripx.AddLameTag(lame_album,D2Xtitle::disk_title);
		p_cdripx.AddLameTag(lame_title,D2Xtitle::track_title[source.track-1]);
	}
	
	while(CDRIPX_DONE != p_cdripx.RipToLame(nPercent,nPeakValue,nJitterErrors,nJitterPos))
	{
		D2Xfilecopy::i_process = nPercent;	
	}
	p_cdripx.DeInit();
	return 1;
}

int D2Xfilecopy::CopyCDDATrackWav(HDDBROWSEINFO source,char* dest)
{
	//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	int	nPercent = 0;
	int	nPeakValue;
	int	nJitterErrors;
	int	nJitterPos;
	char file[1024];
	char temp[1024];
	CCDRipX	p_cdripx;
	D2Xtitle p_title;
	if(p_cdripx.Init()==E_FAIL)
	{
		DPf_H("Failed to init cdripx (FileCDDA)");
		return 0;
	}
	DPf_H("dest %s source %s",dest,source.name);
	strcpy(temp,source.name);
	p_title.getvalidFilename(dest,temp,".wav"); 
	DPf_H("file %s",temp);
	sprintf(file,"%s%s",dest,temp);
	DPf_H("file %s",file);
	wsprintfW(D2Xfilecopy::c_source,L"%hs",source.name);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",file);
	DPf_H("Rip track %d to %s with WAV",source.track,file);
	p_cdripx.InitWav(source.track,file);
	
	while(CDRIPX_DONE != p_cdripx.RipToWav(nPercent,nPeakValue,nJitterErrors,nJitterPos))
	{
		D2Xfilecopy::i_process = nPercent;	
	}
	p_cdripx.DeInit();
	return 1;
}



/////////////////////////////////////////////////////
// UDF2SMB


int D2Xfilecopy::FileUDF2SMB(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	/*if(ftype == DVD2SMB)
	{
		dvd = DVDOpen("\\Device\\Cdrom0");
		if(!dvd)
		{
			DPf_H("Could not authenticate DVD");
			return 0;
		}
	}
*/	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		if((ftype == DVD2SMB) && (strstr(source.item,".vob") || strstr(source.item,".VOB")))
			stat = CopyVOB2SMB(source.item,temp);
		else
            stat = CopyUDF2SMBFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		//p_utils.addSlash(temp2);
		DPf_H("copy iso %s to %s",temp,temp2);
		stat = DirUDF2SMB(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}
	//if(ftype == DVD2SMB)
	//	DVDClose(dvd);

	return stat;
}

bool D2Xfilecopy::CopyUDF2SMBFile(char* lpcszFile,char* destfile)
{
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	CFileSMB	p_smb;

	DPf_H("Calling FileSMB with %s %s",lpcszFile,destfile);

	if ((p_smb.Create(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,destfile,445,true)) == false)
	{		
		DPf_H("Couldn't open file: %s",destfile);
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		//delete p_smb;
		//p_smb = NULL;
		return FALSE;
	}

	HANDLE hFile = CreateFile( lpcszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==NULL)
	{
		DPf_H("Couldn't open File: %s",lpcszFile);
		//delete p_smb;
		//p_smb = NULL;
		return false;
	}

	int dwBufferSize  = 2048*16;
	LARGE_INTEGER l_filesize;
	GetFileSizeEx(hFile,&l_filesize);
	LPBYTE buffer		= new BYTE[dwBufferSize];
	uint64_t fileSize   = l_filesize.QuadPart;
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);

	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	DWORD lRead;
	DWORD dwWrote;

	

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

		ReadFile(hFile,buffer,dwBufferSize,&lRead,NULL);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = DWORD(fileSize - fileOffset);
		dwWrote = p_smb.Write(buffer,lRead);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	p_smb.Close();
	//delete p_smb;
	//p_smb = NULL;
	delete buffer;
	buffer = NULL;
	return TRUE;
}

bool D2Xfilecopy::DirUDF2SMB(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	CFileSMB	p_smb;

	DPf_H("Calling DIRSMB with %s %s",path,destroot);
	// We must create the dest directory
	if(p_smb.CreateDirectory(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,destroot,445,true) == 0)
	{
		DPf_H("Created Directory: %hs",destroot);
	} else
		DPf_H("Can't create Dir: %s",destroot);

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
			strcat(destfile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"/");
				// Recursion
				if(!DirUDF2SMB(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				{
					if((strstr(wfd.cFileName,".xbe")) || (strstr(wfd.cFileName,".XBE")))
					{
						//D2Xpatcher::addXBE(destfile);
						string xbe(destfile);
						XBElist.push_back(xbe);
						D2Xpatcher::mXBECount++;
					}
					if((ftype == DVD2SMB) && (strstr(sourcefile,".vob") || strstr(sourcefile,".VOB")))
					{
						if(!CopyVOB2SMB(sourcefile,destfile))
						{
							DPf_H("can't copy %s to %s",sourcefile,destfile);
							p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
							copy_failed++;
							continue;
						} else {
							p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
							copy_ok++;
							//DPf_H("copydir %s to %s",sourcefile,destfile);
						}
					}
					else 
					{
		
						if(!CopyUDF2SMBFile(sourcefile,destfile))
						{
							DPf_H("can't copy %s to %s",sourcefile,destfile);
							p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
							copy_failed++;
							continue;
						} else {
							p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
							copy_ok++;
						}
					}
				}
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}

	//delete p_smb;
	//p_smb = NULL;
	return 1;
}

///////////////////////////////////////////////////////
// VOB2SMB

int D2Xfilecopy::CopyVOB2SMB(char* sourcefile,char* destfile)
{
	
	#define rblocks 256
	//#define rblocks 16
	unsigned char* buffer;
	buffer = new unsigned char[rblocks*2048];
	
	dvd_reader_t*	dvd;
	dvd_file_t*		vob;
	CFileSMB	p_smb;
	dvd = DVDOpen("\\Device\\Cdrom0");
	if(!dvd)
	{
		DPf_H("DVD not authenticated");
		g_d2xSettings.generalError = COULD_NOT_AUTH_DVD;
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
	

	DPf_H("Calling FileSMB with %s %s",sourcefile,destfile);

	if ((p_smb.Create(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,destfile,445,true)) == false)
	{	
	//HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	//if (hFile==NULL)
	//{
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
		//WriteFile(hFile,buffer,(DWORD)lRead*2048,&dwWrote,NULL);
		dwWrote = p_smb.Write(buffer,(DWORD)lRead*2048);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	//CloseHandle(hFile);
	DVDCloseFile(vob);
	p_smb.Close();
	delete buffer;

	//SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
	DVDClose(dvd);
	return 1;
}

/////////////////////////////////////////////////////
// ISO2SMB


int D2Xfilecopy::FileISO2SMB(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		stat = CopyISO2SMBFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		//p_utils.addSlash(temp2);
		DPf_H("copy iso %s to %s",temp,temp2);
		stat = DirISO2SMB(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}

	return stat;
}

bool D2Xfilecopy::CopyISO2SMBFile(char* lpcszFile,char* destfile)
{
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
	// attempt open file
	iso9660 mISO;
	CFileSMB	p_smb;
	//mISO = new iso9660();
	HANDLE fh;

	if ((fh = mISO.OpenFile(lpcszFile)) == INVALID_HANDLE_VALUE)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		//delete mISO;
		//mISO = NULL;
		return FALSE;
	}

	//DWORD dwBufferSize  = 2048*16;
	int dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	//DWORD dwFileSizeHigh;
	//uint64_t fileSize   = mISO->GetFileSize((HANDLE)1, &dwFileSizeHigh);
	uint64_t fileSize   = mISO.GetFileSize();
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);


	//HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	//if (hFile==NULL)
	if ((p_smb.Create(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,destfile,445,true)) == false)
	{
		DPf_H("Couldn't create File: %s",destfile);
		//mISO->CloseFile(fh);
		mISO.CloseFile();
		//delete mISO;
		//mISO = NULL;
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

		//DWORD dwTotalBytesRead;
		//lRead = mISO->ReadFile((char*)buffer,&dwBufferSize,&dwTotalBytesRead);
		lRead = mISO.ReadFile(1,buffer,dwBufferSize);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);
		//WriteFile(hFile,buffer,(DWORD)lRead,&dwWrote,NULL);
		dwWrote = p_smb.Write(buffer,(DWORD)lRead);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

//	CloseHandle(hFile);
	mISO.CloseFile();
	p_smb.Close();
	//delete mISO;
	//mISO = NULL;
	delete buffer;
	buffer = NULL;

	//SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

bool D2Xfilecopy::DirISO2SMB(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	//LARGE_INTEGER liSize;
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	iso9660 mISO;
	CFileSMB	p_smb;
	//mISO = new iso9660();

	// We must create the dest directory
	if(p_smb.CreateDirectory(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,destroot,445,true) == 0)
	{
		DPf_H("Created Directory: %hs",destroot);
	}


	strcpy(sourcesearch,path);
	//strcpy(sourcesearch,"\\");
	//strcat(sourcesearch,"*");

	// Start the find and check for failure.
	memset(&wfd,0,sizeof(wfd));
	hFind = mISO.FindFirstFile( sourcesearch, &wfd );

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
			//strcpy(temp,wfd.cFileName);
			//p_utils.getFatxName(wfd.cFileName);
			strcat(destfile,wfd.cFileName);
		
			/*
			if(strcmp(temp,wfd.cFileName))
			{
				D2Xpatcher::addFATX(wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				copy_renamed++;
			}
			*/

			DPf_H("found %hs",wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				CStdString strDir=wfd.cFileName;
				if (strDir != "." && strDir != "..")
				{
					strcat(sourcefile,"\\");
					strcat(destfile,"/");
					//mDirCount++;
					// Recursion
					if(!DirISO2SMB(sourcefile,destfile)) continue;
				}
			}
			else
			{
								
				DPf_H("Copying: %hs",wfd.cFileName);
				if(!CopyISO2SMBFile(sourcefile,destfile))
				{
					DPf_H("Failed to copy %hs to %hs.",sourcefile,destfile);
					p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					copy_failed++;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
					copy_ok++;
				}
				
			}

	    }
	    while(mISO.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
		hFind = NULL;
	}
	//delete mISO;
	//mISO = NULL;
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
// UDF2FTP

int D2Xfilecopy::FileUDF2FTP(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	char* ch = strchr(dest,'\\');
	ch++;
	dest = ch;

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		/*if((ftype == DVD2SMB) && (strstr(source.item,".vob") || strstr(source.item,".VOB")))
			stat = CopyVOB2SMB(source.item,temp);
		else*/
        stat = CopyUDF2FTPFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		//p_utils.addSlash(temp2);
		strcat(temp2,"/");
		DPf_H("copy ftp %s to %s",temp,temp2);
		stat = DirUDF2FTP(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}

	return stat;
}

bool D2Xfilecopy::CopyUDF2FTPFile(char* lpcszFile,char* destfile)
{
	#define UDF2FTP_BUFFERSIZE	512
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	D2Xftp	p_ftp;

	DPf_H("Calling FileFTP with %s %s",lpcszFile,destfile);

	if ((p_ftp.CreateFile(destfile)) != 1)
	{		
		DPf_H("Couldn't open file: %s",destfile);
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}

	HANDLE hFile = CreateFile( lpcszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==NULL)
	{
		DPf_H("Couldn't open File: %s",lpcszFile);
		return false;
	}

	int dwBufferSize  = UDF2FTP_BUFFERSIZE;
	LARGE_INTEGER l_filesize;
	GetFileSizeEx(hFile,&l_filesize);
	BYTE buffer[UDF2FTP_BUFFERSIZE];
	uint64_t fileSize   = l_filesize.QuadPart;
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);

	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	DWORD lRead;
	DWORD dwWrote;

	

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

		ReadFile(hFile,buffer,dwBufferSize,&lRead,NULL);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = DWORD(fileSize - fileOffset);
		dwWrote = p_ftp.Write(buffer,lRead);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	p_ftp.CloseFile();
	/*delete buffer;
	buffer = NULL;*/
	return TRUE;
}

bool D2Xfilecopy::DirUDF2FTP(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	D2Xftp	p_ftp;
	D2Xtitle p_title;

	DPf_H("Calling DIRFTP with %s %s",path,destroot);
	// We must create the dest directory
	if(p_ftp.CreateDir(destroot))
	{
		DPf_H("Created Directory: %hs",destroot);
	} else
		DPf_H("Can't create Dir: %s",destroot);

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
			p_utils.getFatxName(wfd.cFileName);
			

			if(!strcmp(temp,wfd.cFileName))
				strcat(destfile,wfd.cFileName);
			else
			{
				//p_title.getvalidFilename(destroot,wfd.cFileName,"");
				strcat(destfile,wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				RENlist.insert(pair<string,string>(sourcefile,destfile));
				++copy_renamed;
			} 

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"\\");
				strcat(destfile,"/");
				// Recursion
				if(!DirUDF2FTP(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				{
					if((strstr(wfd.cFileName,".xbe")) || (strstr(wfd.cFileName,".XBE")))
					{
						//D2Xpatcher::addXBE(destfile);
						string xbe(destfile);
						XBElist.push_back(xbe);
						D2Xpatcher::mXBECount++;
					}
					//if((ftype == DVD2SMB) && (strstr(sourcefile,".vob") || strstr(sourcefile,".VOB")))
					//{
					//	if(!CopyVOB2SMB(sourcefile,destfile))
					//	{
					//		DPf_H("can't copy %s to %s",sourcefile,destfile);
					//		p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					//		copy_failed++;
					//		continue;
					//	} else {
					//		p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
					//		copy_ok++;
					//		//DPf_H("copydir %s to %s",sourcefile,destfile);
					//	}
					//}
					//else 
					{
		
						if(!CopyUDF2FTPFile(sourcefile,destfile))
						{
							DPf_H("can't copy %s to %s",sourcefile,destfile);
							p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
							copy_failed++;
							continue;
						} else {
							p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
							copy_ok++;
						}
					}
				}
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////
// FTP2UDF

int D2Xfilecopy::FileFTP2UDF(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	char* ch = strchr(source.item,'\\');
	ch++;

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		/*if((ftype == DVD2SMB) && (strstr(source.item,".vob") || strstr(source.item,".VOB")))
			stat = CopyVOB2SMB(source.item,temp);
		else*/
		DPf_H("copy ftp %s to %s",ch,temp);
        stat = CopyFTP2UDFFile(ch,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		//p_utils.addSlash(temp);
		strcat(temp,"/");
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		DPf_H("copy ftp %s to %s",temp,temp2);
		stat = DirFTP2UDF(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}

	return stat;
}

bool D2Xfilecopy::CopyFTP2UDFFile(char* lpcszFile,char* destfile)
{
	#define FTP2UDF_BUFFERSIZE 1024
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	D2Xftp	p_ftp;

	DPf_H("Calling FileFTP with %s %s",lpcszFile,destfile);

	HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
	{
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}

	int fileSize;
	if(!p_ftp.GetFileSize(lpcszFile,&fileSize))
	{
		DPf_H("Couldn't determine source file size: %hs",lpcszFile);
		p_log.WLog(L"Couldn't determine source file size: %hs",lpcszFile);
		return false;
	}

	if ((p_ftp.ReadFile(lpcszFile)) != 1)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	int dwBufferSize  = FTP2UDF_BUFFERSIZE;
	//int i_filesize;
	/*GetFileSizeEx(hFile,&l_filesize);*/
	/*LPBYTE buffer		= new BYTE[dwBufferSize];*/
	BYTE buffer[FTP2UDF_BUFFERSIZE];
	/*uint64_t fileSize   = l_filesize.QuadPart;*/
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);

	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	DWORD lRead;
	DWORD dwWrote;

	

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

		lRead = p_ftp.Read(buffer,dwBufferSize);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = DWORD(fileSize - fileOffset);
		WriteFile(hFile,buffer,lRead,&dwWrote,NULL);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	CloseHandle(hFile);
	p_ftp.CloseFile();
	/*delete buffer;
	buffer = NULL;*/
	return TRUE;
}

bool D2Xfilecopy::DirFTP2UDF(char *path,char *destroot)
{
	char sourcesearch[1024]="";
	char sourcefile[1024]="";
	char destfile[1024]="";
	char temp[100]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	D2Xftp	p_ftp;
	D2Xtitle p_title;

	DPf_H("Calling DIRFTP with %s %s",path,destroot);
	// We must create the dest directory
	CreateDirectory(destroot,NULL);

	strcpy(sourcesearch,p_ftp.DelFTP(path));

	/*strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");*/

	// Start the find and check for failure.
	hFind = p_ftp.FindFirstFile( sourcesearch, &wfd );

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
			p_utils.getFatxName(wfd.cFileName);
			

			if(!strcmp(temp,wfd.cFileName))
				strcat(destfile,wfd.cFileName);
			else
			{
				p_title.getvalidFilename(destroot,wfd.cFileName,"");
				strcat(destfile,wfd.cFileName);
				p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
				RENlist.insert(pair<string,string>(sourcefile,destfile));
				++copy_renamed;
			} 

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				strcat(sourcefile,"/");
				strcat(destfile,"\\");
				// Recursion
				if(!DirFTP2UDF(sourcefile,destfile)) continue;
			}
			else
			{
				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				{
					if((strstr(wfd.cFileName,".xbe")) || (strstr(wfd.cFileName,".XBE")))
					{
						//D2Xpatcher::addXBE(destfile);
						string xbe(destfile);
						XBElist.push_back(xbe);
						D2Xpatcher::mXBECount++;
					}
					
		
					if(!CopyFTP2UDFFile(sourcefile,destfile))
					{
						DPf_H("can't copy %s to %s",sourcefile,destfile);
						p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
						copy_ok++;
					}
				
				}
			}
	    }while(p_ftp.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    p_ftp.FindClose( hFind );
	}
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
	//D2Xfilecopy::excludeDirs = 0;
	//D2Xfilecopy::excludeFiles = 0;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
}

//*************************************************************************************
void D2Xfilecopy::Process()
{
	DPf_H("type %d, %s %s",ftype,fsource.item,fdest);
	switch(ftype)
	{
	case DVD:
		FileDVD(fsource,fdest);
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
	case UDF2SMB:
		FileUDF2SMB(fsource,fdest);
		break;
	case DVD2SMB:
		FileUDF2SMB(fsource,fdest);
		break;
	case ISO2SMB:
		FileISO2SMB(fsource,fdest);
		break;
	case VCD2SMB:
		FileISO2SMB(fsource,fdest);
		break;
	case SVCD2SMB:
		FileISO2SMB(fsource,fdest);
		break;
	case UDF2FTP:
		FileUDF2FTP(fsource,fdest);
		break;
	case FTP2UDF:
		FileFTP2UDF(fsource,fdest);
		break;
	default:
		break;
	}
	ftype = UNKNOWN;
	//StopThread();
	//DPf_H("after stopthread");
}
/*
void D2Xfilecopy::CancleThread()
{
	ftype = UNKNOWN;
	D2Xfilecopy::b_finished = true;
	ExitThread(3);
}*/

