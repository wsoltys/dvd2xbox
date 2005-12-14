#include "d2xfilecopy.h"

int D2Xfilecopy::i_process = 0;
int	D2Xfilecopy::copy_failed = 0;
int	D2Xfilecopy::copy_ok = 0;
int	D2Xfilecopy::copy_renamed = 0;
LONGLONG D2Xfilecopy::llValue = 0;
//float D2Xfilecopy::f_ogg_quality = 1.0;
bool D2Xfilecopy::b_finished = false;
WCHAR D2Xfilecopy::c_source[1024]={0};
WCHAR D2Xfilecopy::c_dest[1024]={0};
//char* D2Xfilecopy::excludeDirs = NULL;
//char* D2Xfilecopy::excludeFiles = NULL;

vector<string> D2Xfilecopy::excludeList;
vector<string> D2Xfilecopy::XBElist;
map<string,string> D2Xfilecopy::RENlist;
map<string,string> D2Xfilecopy::FAILlist;


D2Xfilecopy::D2Xfilecopy()
{
	p_source = NULL;
	p_dest = NULL;
	gBuffer = NULL;
	ftype = UNKNOWN_;
	m_bStop = false;
	D2Xfilecopy::llValue = 0;
	D2Xfilecopy::i_process = 0;
	DebugOut("Filecopy thread %d constructor\n",ThreadId());
}

D2Xfilecopy::~D2Xfilecopy()
{
	if(p_source != NULL)
	{
		delete p_source;
		p_source = NULL;
	}
	if(p_dest != NULL)
	{
		delete p_dest;
		p_dest = NULL;
	}
	if(gBuffer != NULL)
	{
		delete gBuffer;
		gBuffer = NULL;
	}
	D2Xfilecopy::llValue = 0;
	D2Xfilecopy::i_process = 0;

	DebugOut("Filecopy thread %d destructor\n",ThreadId());
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
	D2Xfilecopy::llValue = 0;
	D2Xpatcher::reset();
	XBElist.clear();
	RENlist.clear();
	FAILlist.clear();
	D2Xfilecopy::i_process = 0;
	D2Xfilecopy::b_finished = false;
	D2Xfilecopy::copy_failed = 0;
	D2Xfilecopy::copy_ok = 0;
	D2Xfilecopy::copy_renamed = 0;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
	fsource = source;
	strcpy(fdest,dest);
	source_type = type;

	if(gBuffer != NULL)
	{
		delete gBuffer;
		gBuffer = NULL;
	}

	gBuffersize = GENERIC_BUFFER_SIZE;

	if(!(p_utils.isdriveD(source.item)))
		ftype = GAME;
	else
        ftype = type;

	/*if(!strncmp(dest,"ftp:",4) && (ftype == GAME))
		ftype = UDF2FTP;
	else if(!strncmp(source.item,"ftp:",4))
		ftype = FTP2UDF;*/

	if(!strncmp(source.item,"smb:",4) && strncmp(source.item,"ftp:",4) != 0)
	{
		ftype = SMB2UDF;
	}
	if(!strncmp(source.item,"ftp:",4) && strncmp(source.item,"smb:",4) != 0)
	{
		ftype = FTP2UDF;
	}
	
	if(!strncmp(dest,"ftp:",4))
	{
		ftype = X2FTP;
		gBuffersize = UDF2FTP_BUFFER_SIZE;
		if(!_strnicmp(source.item,"smb:",4))
			source_type = D2X_SMB;
		else if(!(p_utils.isdriveD(source.item)))
			source_type = UDF;	
	}
	if(!strncmp(dest,"smb:",4))
	{
		ftype = X2SMB;
		if(!_strnicmp(source.item,"ftp:",4))
		{
			source_type = FTP;
			gBuffersize = UDF2FTP_BUFFER_SIZE;
		}
		else if(!(p_utils.isdriveD(source.item)))
			source_type = UDF;
	}
	
	gBuffer = new BYTE[gBuffersize];

	bool s_prio = SetPriority(THREAD_PRIORITY_HIGHEST);
	DebugOut("Filecopy set thread priority to highest: %d",s_prio?1:0);
}

void D2Xfilecopy::CopyFailed(int type)
{
	ftype = COPYFAILED;
	fail_type = type;
	D2Xfilecopy::llValue = 0;
	D2Xpatcher::reset();
	D2Xfilecopy::i_process = 0;
	D2Xfilecopy::b_finished = false;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
	
	SetPriority(THREAD_PRIORITY_HIGHEST);

}

/////////////////////////////////////////////////////////////
// Generic

int D2Xfilecopy::CopyGeneric(HDDBROWSEINFO source,char* dest,int sourcetype, int desttype)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	D2Xff factory;
	p_source = factory.Create(sourcetype);
	p_dest = factory.Create(desttype);
	
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		if(desttype == UDF)
            getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		stat = CopyFileGeneric(source.item,temp);
		SetFileAttributes(temp,FILE_ATTRIBUTE_NORMAL);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.name);
		if(desttype == UDF)
            getFatxName(temp);
		sprintf(temp2,"%s%s",dest,temp);
		//p_utils.addSlash(temp2);
		strcpy(temp,source.item);
		strcat(temp,"\\");		
		stat = CopyDirectoryGeneric(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}
	delete p_source;
	p_source = NULL;
	delete p_dest;
	p_dest = NULL;
	return stat;
}

int D2Xfilecopy::CopyDirectoryGeneric(char* source, char* dest)
{
	char sourcefile[1024]="";
	char destfile[1024]="";
	char file[100]="";
	char temp[100]="";

	VECFILEITEMS item;
	D2Xtitle p_title;

	// We must create the dest directory
	if(!p_dest->CreateDirectory(dest))
		return 0;

	if(!p_source->GetDirectory(source, &item))
		return 0;

	// Display each file and ask for the next.
	for(int i=0;i<item.size();i++)
	{
		strcpy(file,item[i].name.c_str());
		strcpy(sourcefile,source);
		strcat(sourcefile,file);
		
		strcpy(destfile,dest);
		p_utils.addSlash(destfile);
		strcpy(temp,file);
		if(p_dest->GetType() == UDF)
            getFatxName(file);
		

		if(!strcmp(temp,file))
			strcat(destfile,file);
		else
		{
			p_title.getvalidFilename(dest,file,"");
			strcat(destfile,file);
			p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
			RENlist.insert(pair<string,string>(sourcefile,destfile));
			++copy_renamed;
		} 

		// Only do files
		if(item[i].isDirectory)
		{
			strcat(sourcefile,"\\");
			strcat(destfile,"\\");
			// Recursion
			if(!CopyDirectoryGeneric(sourcefile,destfile)) continue;
		}
		else
		{
			wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
			wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
			{
				//if((strstr(file,".xbe")) || (strstr(file,".XBE")))
				if(!_strnicmp(file+strlen(file)-4,".xbe",4))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
				
	
				if(!CopyFileGeneric(sourcefile,destfile))
				{
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
	return 1;
}


bool D2Xfilecopy::CopyFileGeneric(char* source, char* dest)
{

	wsprintfW(D2Xfilecopy::c_source,L"%hs",source);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",dest);

	if (p_source->FileOpenRead(source, OPEN_MODE_SEQ) == 0)
	{		
		p_log.WLog(L"Couldn't open source file %hs",source);
		return FALSE;
	}


	gFileSize   = p_source->GetFileSize();
	gFileOffset = 0;
	gbResult	= false;

	if (p_dest->FileOpenWrite(dest, OPEN_MODE_SEQ, gFileSize) == 0)
	{
		p_log.WLog(L"Couldn't open destination file %hs",dest);
		p_source->FileClose();
		return FALSE;
	}

	gnOldPercentage = 1;
	gnNewPercentage = 0;
	glRead = 0;
	gdwWrote = 0;
	int retry;

	do
	{
		if (gnNewPercentage!=gnOldPercentage)
		{
			gnOldPercentage = gnNewPercentage;
		}

		//gbResult = p_source->FileRead(gBuffer,gBuffersize,&glRead);

		//if (gbResult &&  glRead == 0 ) 
		//{ 
		//    // We're at the end of the file. 
		//	break;
		//} else if(gbResult == 0)
		//{
		//	p_log.WLog(L"Read error %hs at position %d",source,gFileOffset);
		//	p_source->FileClose();
		//	p_dest->FileClose();
		//	return false;
		//}

		retry = 0;

		while(true)
		{
			gbResult = p_source->FileRead(gBuffer,gBuffersize,&glRead);

			if (gbResult > 0 &&  glRead == 0 ) 
			{ 
				// We're at the end of the file. 
				break;
			} else if(gbResult == 0)
			{
				retry++;
				p_log.WLog(L"Read error %d;File: %hs;Offset: %d;Error: %d",retry,source,gFileOffset,GetLastError());
				if(retry >= g_d2xSettings.autoReadRetries)
				{
					p_source->FileClose();
					p_dest->FileClose();
					return false;
				}
			}
			else
			{
				// we got what we want
				break;
			}
		}

		p_dest->FileWrite(gBuffer,glRead,&gdwWrote);
		gFileOffset+=glRead;
		D2Xfilecopy::llValue += gdwWrote;

		if(gFileSize > 0)
			gnNewPercentage = ((gFileOffset*100)/gFileSize);
		D2Xfilecopy::i_process = gnNewPercentage;


	} while ( gFileOffset < gFileSize  );

	p_source->FileClose();
	p_dest->FileClose();

	SetFileAttributes(dest,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

/////////////////////////////////////////////////////////////
// UDF

int D2Xfilecopy::FileUDF(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	D2Xff factory;
	p_source = factory.Create(UDF);
	p_dest = factory.Create(UDF);
	
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		//stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
		stat = CopyUDFFile(source.item,temp);
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
	delete p_source;
	p_source = NULL;
	delete p_dest;
	p_dest = NULL;
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

	//DebugOut("copy %s to %s",path,destroot);
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
			getFatxName(wfd.cFileName);
			

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
					//DebugOut("Checking exclude item %s with %s",item.c_str(),sourcefile);
					if(!_stricmp(item.c_str(),sourcefile))
					{
						p_log.WLog(L"excluded %hs due to ACL rule.",sourcefile);
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
				if(g_d2xSettings.replaceVideo)
				{
					switch(D2Xutils::IsVideoExt(wfd.cFileName))
					{
					case D2X_BIK:
						if(GetFileAttributes("Q:\\dummyfiles\\blank_video.bik") != -1)
                            strcpy(sourcefile,"Q:\\dummyfiles\\blank_video.bik");
						break;
					case D2X_SFD:
						if(GetFileAttributes("Q:\\dummyfiles\\blank_video.sfd") != -1)
							strcpy(sourcefile,"Q:\\dummyfiles\\blank_video.sfd");
						break;
					case D2X_WMV:
						if(GetFileAttributes("Q:\\dummyfiles\\blank_video.wmv") != -1)
							strcpy(sourcefile,"Q:\\dummyfiles\\blank_video.wmv");
						break;
					case D2X_XMV:
						if(GetFileAttributes("Q:\\dummyfiles\\blank_video.xmv") != -1)
							strcpy(sourcefile,"Q:\\dummyfiles\\blank_video.xmv");
						break;
					default:
						break;
					}
				}

				wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
				wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
				
				if(strstr(wfd.cFileName,".xbe") || strstr(wfd.cFileName,".XBE"))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
	
				
				if(!CopyUDFFile(sourcefile,destfile))
				{
					DebugOut("can't copy %s to %s",sourcefile,destfile);
					p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					FAILlist.insert(pair<string,string>(sourcefile,destfile));
					++copy_failed;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
					++copy_ok;
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


	if (p_source->FileOpenRead(lpcszFile, OPEN_MODE_SEQ) == 0)
	{		
		DebugOut("Couldn't open file: %s\n",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	BYTE buffer[UDFBUFFERSIZE];

	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t fileOffset = 0;
	int iResult			= 0;


	if (p_dest->FileOpenWrite(destfile, OPEN_MODE_SEQ, fileSize) == 0)
	{
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}
	
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	DWORD lRead;
	DWORD dwWrote;
	int	retry;
	bool loop = true;

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

		retry = 0;

		while(true)
		{
			iResult = p_source->FileRead(buffer,UDFBUFFERSIZE,&lRead);

			if (iResult > 0 &&  lRead == 0 ) 
			{ 
				// We're at the end of the file. 
				loop = false;
				break;
			} else if(iResult == 0)
			{
				retry++;
				p_log.WLog(L"Read error %d;File: %hs;Error: %d",retry,lpcszFile,GetLastError());
				if(retry >= g_d2xSettings.autoReadRetries)
				{
					p_source->FileClose();
					p_dest->FileClose();
					return false;
				}
			}
			else
			{
				// we got what we want
				break;
			}
		}

		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( loop );


	p_source->FileClose();
	p_dest->FileClose();

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

void D2Xfilecopy::CopyFailedGeneric()
{
	D2Xff factory;
	p_source = factory.Create(fail_type);
	p_dest = factory.Create(UDF);
	
	if(gBuffer != NULL)
	{
		delete gBuffer;
		gBuffer = NULL;
	}

	gBuffersize = GENERIC_BUFFER_SIZE;
	gBuffer = new BYTE[gBuffersize];

	map<string,string> temp_FAILlist;

	map<string,string>::iterator it;
	for(it = FAILlist.begin();it != FAILlist.end();it++)
	{
		if(D2Xfilecopy::CopyFileGeneric((char*)it->first.c_str(),(char*)it->second.c_str()))
		{
			p_log.WLog(L"Copied %hs to %hs",it->first.c_str(),it->second.c_str());
			copy_ok++;
			//SetFileAttributes(it->second.c_str(),FILE_ATTRIBUTE_NORMAL);
			//FAILlist.erase(it);
		}
		else
		{
			temp_FAILlist.insert(pair <string,string> (it->first.c_str(),it->second.c_str()));
			p_log.WLog(L"Failed to copy %hs to %hs",it->first.c_str(),it->second.c_str());
		}
	} 
	delete p_source;
	p_source = NULL;
	
	delete p_dest;
	p_dest = NULL;
	
	if(gBuffer != NULL)
	{
		delete gBuffer;
		gBuffer = NULL;
	}

	FAILlist.clear();
	FAILlist = temp_FAILlist;
	copy_failed = FAILlist.size();
}

/////////////////////////////////////////////////////////////
// UDF DVD

int D2Xfilecopy::FileDVD(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	D2Xff factory;
	p_source = factory.Create(DVD);
	p_dest = factory.Create(UDF);
	
	
	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		wsprintfW(D2Xfilecopy::c_source,L"%hs",source.item);
		wsprintfW(D2Xfilecopy::c_dest,L"%hs",temp);
		//if(strstr(source.item,".vob") || strstr(source.item,".VOB"))
			stat = CopyVOB(source.item,temp);
		//else
		//	stat = CopyFileEx(source.item,temp,&CopyProgressRoutine,NULL,NULL,NULL);
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
	
	delete p_source;
	p_source = NULL;
	
	delete p_dest;
	p_dest = NULL;
	
	return stat;
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

	//DebugOut("copy %s to %s",path,destroot);
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
			getFatxName(wfd.cFileName);
			

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
				//if(strstr(sourcefile,".vob") || strstr(sourcefile,".VOB"))
				//{
					if(!CopyVOB(sourcefile,destfile))
					{
						p_log.WLog(L"Failed to copy %hs to %hs.",sourcefile,destfile);
						copy_failed++;
						continue;
					} else {
						SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
						p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
						copy_ok++;
						//DebugOut("copydir %s to %s",sourcefile,destfile);
					}
				//}
				//else 
				//{
				//	
				//	if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
				//	{
				//		DebugOut("can't copy %s to %s",sourcefile,destfile);
				//		p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
				//		copy_failed++;
				//		continue;
				//	} else {
				//		SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
				//		p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
				//		copy_ok++;
				//		//DebugOut("copydir %s to %s",sourcefile,destfile);
				//	}
				//	if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
				//	{
				//		liSize.LowPart = wfd.nFileSizeLow;
				//		liSize.HighPart = wfd.nFileSizeHigh;
				//		D2Xfilecopy::llValue += liSize.QuadPart;
				//	}
				//}
					/*if ( wfd.nFileSizeLow || wfd.nFileSizeHigh )
					{
						liSize.LowPart = wfd.nFileSizeLow;
						liSize.HighPart = wfd.nFileSizeHigh;
						D2Xfilecopy::llValue += liSize.QuadPart;
					}*/
			}
	    }while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	return 1;
}

int D2Xfilecopy::CopyVOB(char* sourcefile,char* destfile)
{
	
	//#define BUFFERSIZE 524288 // 256 blocks a 2048 bytes on DVD
	#define BUFFERSIZE 32768	// 2048*16
	unsigned char buffer[BUFFERSIZE];
	
	if(!(p_source->FileOpenRead(sourcefile)))
	{
		WIN32_FILE_ATTRIBUTE_DATA FileAttributeData;
		GetFileAttributesEx(sourcefile, GetFileExInfoStandard, &FileAttributeData);
		if(FileAttributeData.nFileSizeLow != 0)
		{
			p_log.WLog(L"Could not open file %hs.",sourcefile);
			p_dest->FileClose();
			return 0;
		}
		else
		{
			p_dest->FileOpenWrite(destfile);
			p_dest->FileClose();
			return 1;
		}
	}
	
	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	uint64_t fileOffset = 0;
	DWORD lRead;
	DWORD dwWrote;

	if(!(p_dest->FileOpenWrite(destfile, OPEN_MODE_SEQ, fileSize)))
	{
		p_log.WLog(L"Couldn't create File: %hs",destfile);
		return 0;
	}

	do
	{
		if (nNewPercentage!=nOldPercentage)
		{
			nOldPercentage = nNewPercentage;
		}

	
		p_source->FileRead(buffer,BUFFERSIZE,&lRead);
		if (lRead<=0)
			break;
		
	
		/*if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);*/
	
		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	/*} while ( fileOffset<fileSize );*/
	} while ( true);

	p_source->FileClose();
	p_dest->FileClose();

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
	return 1;
}




/////////////////////////////////////////////////////
// ISO


int D2Xfilecopy::FileISO(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	D2Xff factory;
	p_source = factory.Create(ISO);
	p_dest = factory.Create(UDF);

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
		stat = CopyISOFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		stat = DirISO(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}
	delete p_source;
	p_source = NULL;
	delete p_dest;
	p_dest = NULL;
	return stat;
}

bool D2Xfilecopy::CopyISOFile(char* lpcszFile,char* destfile)
{
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	if (!(p_source->FileOpenRead(lpcszFile)))
	{		
		DebugOut("Couldn't open file: %s\n",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	int dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	//uint64_t fileSize   = mISO.GetFileSize();
	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t fileOffset = 0;

	DebugOut("Filesize: %s %d",lpcszFile,fileSize);


	if(!(p_dest->FileOpenWrite(destfile,OPEN_MODE_SEQ,fileSize)))
	{
		DebugOut("Couldn't create File: %s\n",destfile);
		//mISO.CloseFile();
		p_source->FileClose();
		delete buffer;
		buffer = NULL;
		return FALSE;
	}
	

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

		//lRead = mISO.ReadFile(1,buffer,dwBufferSize);
		p_source->FileRead(buffer,dwBufferSize,&lRead);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = long(fileSize - fileOffset);
		//WriteFile(hFile,buffer,(DWORD)lRead,&dwWrote,NULL);
		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	//CloseHandle(hFile);
	//mISO.CloseFile();
	p_source->FileClose();
	p_dest->FileClose();
	delete buffer;
	buffer = NULL;

	SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);

	return TRUE;
}

bool D2Xfilecopy::DirISO(char *path,char *destroot)
{
	char sourcefile[1024]="";
	char destfile[1024]="";
	char file[100]="";
	char temp[100]="";

	VECFILEITEMS item;
	D2Xtitle p_title;

	// We must create the dest directory
	if(!p_dest->CreateDirectory(destroot))
		return 0;

	//strcpy(sourcesearch,p_ftp.DelFTP(path));

	if(!p_source->GetDirectory(path, &item))
		return 0;

	// Display each file and ask for the next.
	for(int i=0;i<item.size();i++)
	{
		strcpy(file,item[i].name.c_str());
		strcpy(sourcefile,path);
		strcat(sourcefile,file);
		
		strcpy(destfile,destroot);
		strcpy(temp,file);
		getFatxName(file);
		

		if(!strcmp(temp,file))
			strcat(destfile,file);
		else
		{
			p_title.getvalidFilename(destroot,file,"");
			strcat(destfile,file);
			p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
			RENlist.insert(pair<string,string>(sourcefile,destfile));
			++copy_renamed;
		} 

		// Only do files
		if(item[i].isDirectory)
		{
			strcat(sourcefile,"\\");
			strcat(destfile,"\\");
			// Recursion
			if(!DirISO(sourcefile,destfile)) continue;
		}
		else
		{
	
			wsprintfW(D2Xfilecopy::c_source,L"%hs",sourcefile);
			wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);
			{
				if((strstr(file,".xbe")) || (strstr(file,".XBE")))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
				
	
				if(!CopyISOFile(sourcefile,destfile))
				{
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
	return 1;
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
	D2Xcdrip p_cdripx;
	D2Xtitle p_title;
	mfilescount = p_cdripx.GetNumTocEntries();
	p_log.WLog(L"Found CDDA with %d Tracks",mfilescount);
	//if(p_cdripx.Init()!=E_FAIL)
	if(mfilescount > 0)
	{
		
		DebugOut("Found %d Tracks",mfilescount);
		p_cdripx.DeInit();
		if(D2Xtitle::i_network)
		{
			char temp[100];
			if(p_title.getCDDADiskTitle(temp))
			{
				p_log.WLog(L"Ripping %hs",temp);
				for(int i=1;i<=mfilescount;i++)
				{	
					strcpy(temp,"\0");
					p_title.getCDDATrackTitle(temp,i);
					cFiles[i-1] = new char[strlen(temp)+1];
					strcpy(cFiles[i-1],temp);
					p_log.WLog(L"Found Track%02d %hs",i,temp);
				}
			
			} 
			else
			{
				p_log.WLog(L"Error getting CDDA DiskTitle");
				for(int i=0;i<mfilescount;i++)
					{
					cFiles[i] = new char[8];
					sprintf(cFiles[i],"Track%02d",i+1);
					p_log.WLog(L"Found Track%02d",i+1);
				}
			}
		}
		else
		{
			for(int i=0;i<mfilescount;i++)
			{
				cFiles[i] = new char[8];
				sprintf(cFiles[i],"Track%02d",i+1);
				p_log.WLog(L"Found Track%02d",i+1);
			}
		}
		for(int i=0;i<mfilescount;i++)
		{
			fsource.track = i+1;
			strcpy(fsource.name,cFiles[i]);
			if(CopyCDDATrack(fsource,dest))
			{
				copy_ok++;
				p_log.WLog(L"Ok: copied Track%02d from %hs to %hs",i+1,fsource.name,dest);
			}
			else
			{
				copy_failed++;
				p_log.WLog(L"Error: failed to copy Track%02d from %hs to %hs",i+1,fsource.name,dest);
			}
		}
				
	} else {	
		p_log.WLog(L"Error: couldn't init CdRipx library");
	}
	return 1;
}

int D2Xfilecopy::CopyCDDATrack(HDDBROWSEINFO source,char* dest)
{
	BYTE* pbuffer = NULL;
	LONG numBytes;
	char file[1024];
	char temp[1024];
	D2Xcdrip p_cdrip;
	D2Xtitle p_title;

	if(p_cdrip.Init(source.track) == 0)
	{
		p_log.WLog(L"Error: couldn't init CdRipx library");
		return 0;
	}
	
	strcpy(temp,source.name);

	if(g_d2xSettings.cdda_encoder == OGGVORBIS)
	{
		p_title.getvalidFilename(dest,temp,".ogg"); 
		sprintf(file,"%s%s",dest,temp);
		p_cdrip.InitEnc(file,D2XAENC_OGG);
		p_log.WLog(L"Track%02d Ogg Vorbis Encoding",source.track);
	}
	else if(g_d2xSettings.cdda_encoder == WAV)
	{
		p_title.getvalidFilename(dest,temp,".wav"); 
		sprintf(file,"%s%s",dest,temp);
		p_cdrip.InitEnc(file,D2XAENC_WAV);
		p_log.WLog(L"Track%02d WAVE Encoding",source.track);
	}
	else
	{
		p_title.getvalidFilename(dest,temp,".mp3"); 
		sprintf(file,"%s%s",dest,temp);
		p_cdrip.InitEnc(file,D2XAENC_MP3);
		p_log.WLog(L"Track%02d MP3 Encoding",source.track);
	}
	wsprintfW(D2Xfilecopy::c_source,L"%hs",source.name);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",file);
	p_cdrip.AddTag(ENC_COMMENT,"Ripped with dvd2xbox"); 
	if(D2Xtitle::i_network)
	{
		// shit
		if((D2Xtitle::track_artist[source.track-1] != NULL) && (strlen(D2Xtitle::track_artist[source.track-1]) > 1))
			p_cdrip.AddTag(ENC_ARTIST,D2Xtitle::track_artist[source.track-1]);
		else
			p_cdrip.AddTag(ENC_ARTIST,D2Xtitle::disk_artist);
		p_cdrip.AddTag(ENC_ALBUM,D2Xtitle::disk_title);
		p_cdrip.AddTag(ENC_TITLE,D2Xtitle::track_title[source.track-1]);
	}
	int ret;
	while(1)
	{
		ret = p_cdrip.RipChunk();
		if(ret == CD_DONE)
			break;
		if(ret == CD_ERROR)
			break;
		D2Xfilecopy::i_process = p_cdrip.GetPercentCompleted();	
	}
	p_cdrip.Close();
	p_cdrip.DeInit();
	return 1;
}



//////////////////////////////////////////////////////////////////////////////////////
// UDF2FTP

int D2Xfilecopy::FileUDF2FTP(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	/*char* ch = strchr(dest,'\\');
	ch++;
	dest = ch;*/
	D2Xff factory;
	p_source = factory.Create(UDF);
	p_dest = factory.Create(FTP);

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
        stat = CopyUDF2FTPFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		p_utils.addSlash(temp);
		sprintf(temp2,"%s%s",dest,source.name);
		strcat(temp2,"/");
		stat = DirUDF2FTP(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}
	
	delete p_source;
	p_source = NULL;
	delete p_dest;
	p_dest = NULL;
	return stat;
}

bool D2Xfilecopy::CopyUDF2FTPFile(char* lpcszFile,char* destfile)
{
	#define UDF2FTP_BUFFERSIZE	512
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);


	if(p_source->FileOpenRead(lpcszFile) == 0)
	{
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return false;
	}

	if(p_dest->FileOpenWrite(destfile) == 0)
	{		
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		p_source->FileClose();
		return false;
	}

	int dwBufferSize  = UDF2FTP_BUFFERSIZE;
	LARGE_INTEGER l_filesize;
	//GetFileSizeEx(hFile,&l_filesize);
	uint64_t fileSize   = p_source->GetFileSize();

	BYTE buffer[UDF2FTP_BUFFERSIZE];
	//uint64_t fileSize   = l_filesize.QuadPart;
	uint64_t fileOffset = 0;


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

		//ReadFile(hFile,buffer,dwBufferSize,&lRead,NULL);
		p_source->FileRead(buffer,dwBufferSize,&lRead);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = DWORD(fileSize - fileOffset);
		//dwWrote = p_ftp.Write(buffer,lRead);
		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	//CloseHandle(hFile);
	p_source->FileClose();
	p_dest->FileClose();
	//p_ftp.CloseFile();
	/*delete buffer;
	buffer = NULL;*/
	return TRUE;
}

bool D2Xfilecopy::DirUDF2FTP(char *path,char *destroot)
{
	char sourcefile[1024]="";
	char destfile[1024]="";
	char file[100]="";

	VECFILEITEMS item;

	// We must create the dest directory
	if(!p_dest->CreateDirectory(destroot))
		return 0;

	//strcpy(sourcesearch,p_ftp.DelFTP(path));

	if(!p_source->GetDirectory(path, &item))
		return 0;

	// Display each file and ask for the next.
	for(int i=0;i<item.size();i++)
	{
		strcpy(file,item[i].name.c_str());
		strcpy(sourcefile,path);
		strcat(sourcefile,file);
		
		strcpy(destfile,destroot);
		strcat(destfile,file);
		

		// Only do files
		if(item[i].isDirectory)
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
				if((strstr(file,".xbe")) || (strstr(file,".XBE")))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
				
	
				if(!CopyUDF2FTPFile(sourcefile,destfile))
				{
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
	return 1;
}



//////////////////////////////////////////////////////////////////////////////////////
// FTP2UDF

int D2Xfilecopy::FileFTP2UDF(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	//char* ch = strchr(source.item,'/');
	//ch++;

	D2Xff factory;
	p_source = factory.Create(FTP);
	p_dest = factory.Create(UDF);

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		getFatxName(temp2);
		sprintf(temp,"%s%s",dest,temp2);
        stat = CopyFTP2UDFFile(source.item,temp);
	}
	else if(source.type == BROWSE_DIR)
	{
		strcpy(temp,source.item);
		strcat(temp,"/");
		sprintf(temp2,"%s%s",dest,source.name);
		p_utils.addSlash(temp2);
		stat = DirFTP2UDF(temp,temp2);
		p_log.WLog(L"");
		p_log.WLog(L"Copied %d MBytes.",D2Xfilecopy::llValue/1048576);
		p_log.WLog(L"");
	}

	delete p_source;
	p_source = NULL;
	delete p_dest;
	p_dest = NULL;

	return stat;
}

bool D2Xfilecopy::CopyFTP2UDFFile(char* lpcszFile,char* destfile)
{
	#define FTP2UDF_BUFFERSIZE 512
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	//D2Xftp	p_ftp;

	//HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	//if (hFile==NULL)
	if (p_dest->FileOpenWrite(destfile) == 0)
	{
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}

	int fileSize;
	//if(!p_ftp.GetFileSize(lpcszFile,&fileSize))
    // Crappy ftplib needs GetFileSize to be called before FileOpenRead
	if ((fileSize = p_source->GetFileSize(lpcszFile)) == 0)
	{
		p_log.WLog(L"Couldn't determine source file size: %hs",lpcszFile);
		p_dest->FileClose();
		//p_source->FileClose();
		return false;
	}

	//if ((p_ftp.ReadFile(lpcszFile)) != 1)
	if (p_source->FileOpenRead(lpcszFile) == 0)
	{		
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		p_dest->FileClose();
		return FALSE;
	}

	

	int dwBufferSize  = FTP2UDF_BUFFERSIZE;
	BYTE buffer[FTP2UDF_BUFFERSIZE];
	uint64_t fileOffset = 0;

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

		//lRead = p_ftp.Read(buffer,dwBufferSize);
		p_source->FileRead(buffer,dwBufferSize,&lRead);
		if (lRead<=0)
			break;

		if((fileOffset+lRead) > fileSize)
			lRead = DWORD(fileSize - fileOffset);
		//WriteFile(hFile,buffer,lRead,&dwWrote,NULL);
		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	} while ( fileOffset<fileSize );

	//CloseHandle(hFile);
	//p_ftp.CloseFile();

	p_dest->FileClose();
	p_source->FileClose();
	return TRUE;
}

bool D2Xfilecopy::DirFTP2UDF(char *path,char *destroot)
{
	char sourcefile[1024]="";
	char destfile[1024]="";
	char file[100]="";
	char temp[100]="";

	VECFILEITEMS item;
	D2Xtitle p_title;

	// We must create the dest directory
	if(!p_dest->CreateDirectory(destroot))
		return 0;

	//strcpy(sourcesearch,p_ftp.DelFTP(path));

	if(!p_source->GetDirectory(path, &item))
		return 0;

	// Display each file and ask for the next.
	for(int i=0;i<item.size();i++)
	{
		strcpy(file,item[i].name.c_str());
		strcpy(sourcefile,path);
		strcat(sourcefile,file);
		
		strcpy(destfile,destroot);
		strcpy(temp,file);
		getFatxName(file);
		

		if(!strcmp(temp,file))
			strcat(destfile,file);
		else
		{
			p_title.getvalidFilename(destroot,file,"");
			strcat(destfile,file);
			p_log.WLog(L"Renamed %hs to %hs",sourcefile,destfile);
			RENlist.insert(pair<string,string>(sourcefile,destfile));
			++copy_renamed;
		} 

		// Only do files
		if(item[i].isDirectory)
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
				if((strstr(file,".xbe")) || (strstr(file,".XBE")))
				{
					string xbe(destfile);
					XBElist.push_back(xbe);
					D2Xpatcher::mXBECount++;
				}
				
	
				if(!CopyFTP2UDFFile(sourcefile,destfile))
				{
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
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
	DebugOut("Filecopy thread %d OnExit\n",ThreadId());
}

//*************************************************************************************
void D2Xfilecopy::Process()
{
	
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
	case SMB2UDF:
		//dest_type = UDF;
		CopyGeneric(fsource,fdest,D2X_SMB,UDF);
		break;
	case X2SMB:
		//dest_type = D2X_SMB;
		CopyGeneric(fsource,fdest,source_type,D2X_SMB);
		break;
	case X2FTP:
		//dest_type = FTP;
		CopyGeneric(fsource,fdest,source_type,FTP);
		break;
	case UDF:
		//dest_type = UDF;
		CopyGeneric(fsource,fdest,UDF,UDF);
		break;
	case UDF2FTP:
		FileUDF2FTP(fsource,fdest);
		break;
	case FTP2UDF:
		FileFTP2UDF(fsource,fdest);
		break;
	case COPYFAILED:
		CopyFailedGeneric();
		break;
	default:
		FileUDF(fsource,fdest);
		break;
	}
	ftype = UNKNOWN_;
	if(p_source != NULL)
	{
		delete p_source;
		p_source = NULL;
	}
	if(p_dest != NULL)
	{
		delete p_dest;
		p_dest = NULL;
	}
	if(gBuffer != NULL)
	{
		delete gBuffer;
		gBuffer = NULL;
	}
}
