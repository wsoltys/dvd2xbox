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
map<string,string> D2Xfilecopy::FAILlist;


D2Xfilecopy::D2Xfilecopy()
{
	p_source = NULL;
	p_dest = NULL;
	ftype = UNKNOWN_;
	m_bStop = false;
	D2Xfilecopy::i_process = 0;
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
	if(!(p_utils.isdriveD(source.item)))
		ftype = GAME;
	else
        ftype = type;

	if(!strncmp(dest,"ftp:",4) && (ftype == GAME))
		ftype = UDF2FTP;
	else if(!strncmp(source.item,"ftp:",4))
		ftype = FTP2UDF;

	if(!strncmp(source.item,"smb:",4))
		ftype = SMB2UDF;
	else if(!strncmp(dest,"smb:",4))
	{
		ftype = X2SMB;
		if(!(p_utils.isdriveD(source.item)))
			source_type = UDF;
	}


	SetPriority(THREAD_PRIORITY_HIGHEST);
}

void D2Xfilecopy::CopyFailed(int type)
{
	ftype = COPYFAILED;
	fail_type = type;
	llValue = 1;
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
            p_utils.getFatxName(temp2);
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
            p_utils.getFatxName(temp);
		sprintf(temp2,"%s%s",dest,temp);
		p_utils.addSlash(temp2);
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

	//strcpy(sourcesearch,p_ftp.DelFTP(path));

	if(!p_source->GetDirectory(source, &item))
		return 0;

	// Display each file and ask for the next.
	for(int i=0;i<item.size();i++)
	{
		strcpy(file,item[i].name.c_str());
		strcpy(sourcefile,source);
		strcat(sourcefile,file);
		
		strcpy(destfile,dest);
		strcpy(temp,file);
		if(dest_type == UDF)
            p_utils.getFatxName(file);
		

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
				if((strstr(file,".xbe")) || (strstr(file,".XBE")))
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

	if (p_dest->FileOpenWrite(dest) == 0)
	{
		p_log.WLog(L"Couldn't open destination file %hs",dest);
		return FALSE;
	}

	if (p_source->FileOpenRead(source) == 0)
	{		
		p_log.WLog(L"Couldn't open source file %hs",source);
		p_dest->FileClose();
		return FALSE;
	}


	gFileSize   = p_source->GetFileSize();
	gFileOffset = 0;
	gbResult	= false;

	gnOldPercentage = 1;
	gnNewPercentage = 0;
	glRead = 0;
	gdwWrote = 0;

	do
	{
		if (gnNewPercentage!=gnOldPercentage)
		{
			gnOldPercentage = gnNewPercentage;
		}

		gbResult = p_source->FileRead(gBuffer,GENERIC_BUFFER_SIZE,&glRead);

		if (gbResult &&  glRead == 0 ) 
		{ 
		    // We're at the end of the file. 
			break;
		} else if(gbResult == 0)
		{
			p_log.WLog(L"Read error %hs at position %d",source,gFileOffset);
			p_source->FileClose();
			p_dest->FileClose();
			return false;
		}

		p_dest->FileWrite(gBuffer,glRead,&gdwWrote);
		gFileOffset+=glRead;
		D2Xfilecopy::llValue += gdwWrote;

		if(gFileSize > 0)
			gnNewPercentage = ((gFileOffset*100)/gFileSize);
		D2Xfilecopy::i_process = gnNewPercentage;


	} while ( true );

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
		p_utils.getFatxName(temp2);
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
	
				//if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
				if(!CopyUDFFile(sourcefile,destfile))
				{
					DPf_H("can't copy %s to %s",sourcefile,destfile);
					p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
					FAILlist.insert(pair<string,string>(sourcefile,destfile));
					++copy_failed;
					continue;
				} else {
					SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
					p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
					++copy_ok;
				}
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


bool D2Xfilecopy::CopyUDFFile(char* lpcszFile,char* destfile)
{
#define UDFBUFFERSIZE	32768	// 2048*16
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	//HANDLE fh = CreateFile( lpcszFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );

	if (p_source->FileOpenRead(lpcszFile) == 0)
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	//int dwBufferSize  = UDFBUFFERSIZE;
	BYTE buffer[UDFBUFFERSIZE];

	//uint64_t fileSize   = GetFileSize(fh,NULL);
	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t fileOffset = 0;
	bool bResult		= false;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);


	//HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (p_dest->FileOpenWrite(destfile) == 0)
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

	
		//bResult = ReadFile(fh,buffer,UDFBUFFERSIZE,&lRead,NULL);
		bResult = p_source->FileRead(buffer,UDFBUFFERSIZE,&lRead);
		//if (lRead<=0)
		//	break;
		if (bResult &&  lRead == 0 ) 
		{ 
		    // We're at the end of the file. 
			break;
		} else if(bResult == 0)
		{
			p_log.WLog(L"Read error %hs %d",lpcszFile,GetLastError());
			/*CloseHandle(hFile);
			CloseHandle(fh);*/
			p_source->FileClose();
			p_dest->FileClose();
			return false;
		}

		//WriteFile(hFile,buffer,lRead,&dwWrote,NULL);
		p_dest->FileWrite(buffer,lRead,&dwWrote);
		fileOffset+=lRead;
		D2Xfilecopy::llValue += dwWrote;

		if(fileSize > 0)
			nNewPercentage = ((fileOffset*100)/fileSize);
		D2Xfilecopy::i_process = nNewPercentage;

	//} while ( fileOffset<fileSize );
	} while ( true );

	//CloseHandle(hFile);
	//CloseHandle(fh);
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
	map<string,string>::iterator it;
	for(it = FAILlist.begin();it != FAILlist.end();it++)
	{
		if(D2Xfilecopy::CopyUDFFile((char*)it->first.c_str(),(char*)it->second.c_str()))
		{
			p_log.WLog(L"Copied %hs to %hs",it->first.c_str(),it->second.c_str());
			copy_ok++;
			SetFileAttributes(it->second.c_str(),FILE_ATTRIBUTE_NORMAL);
			FAILlist.erase(it);
		}
		else
		{
			p_log.WLog(L"Failed to copy %hs to %hs",it->first.c_str(),it->second.c_str());
		}
	}
	delete p_source;
	p_source = NULL;
	
	delete p_dest;
	p_dest = NULL;
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
		p_utils.getFatxName(temp2);
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
						//DPf_H("copydir %s to %s",sourcefile,destfile);
					}
				//}
				//else 
				//{
				//	
				//	if(!CopyFileEx(sourcefile,destfile,&CopyProgressRoutine,NULL,NULL,NULL))
				//	{
				//		DPf_H("can't copy %s to %s",sourcefile,destfile);
				//		p_log.WLog(L"Failed to copy %hs to %hs",sourcefile,destfile);
				//		copy_failed++;
				//		continue;
				//	} else {
				//		SetFileAttributes(destfile,FILE_ATTRIBUTE_NORMAL);
				//		p_log.WLog(L"Copied %hs to %hs",sourcefile,destfile);
				//		copy_ok++;
				//		//DPf_H("copydir %s to %s",sourcefile,destfile);
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
	
	if(!(p_dest->FileOpenWrite(destfile)))
	{
		p_log.WLog(L"Couldn't create File: %hs",destfile);
		return 0;
	}
	
	if(!(p_source->FileOpenRead(sourcefile)))
	{
		p_log.WLog(L"Could not open file %hs. Maybe 0 byte vob ?",sourcefile);
		p_dest->FileClose();
		return 0;
	}
	
	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t nOldPercentage = 1;
	uint64_t nNewPercentage = 0;
	uint64_t fileOffset = 0;
	DWORD lRead;
	DWORD dwWrote;

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
	// attempt open file
	//iso9660 mISO;
	//HANDLE fh;

	//if ((fh = mISO.OpenFile(lpcszFile)) == INVALID_HANDLE_VALUE)
	if (!(p_source->FileOpenRead(lpcszFile)))
	{		
		DPf_H("Couldn't open file: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return FALSE;
	}

	int dwBufferSize  = 2048*16;
	LPBYTE buffer		= new BYTE[dwBufferSize];
	//uint64_t fileSize   = mISO.GetFileSize();
	uint64_t fileSize   = p_source->GetFileSize();
	uint64_t fileOffset = 0;

	DPf_H("Filesize: %s %d",lpcszFile,fileSize);


	//HANDLE hFile = CreateFile( destfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	//if (hFile==NULL)
	if(!(p_dest->FileOpenWrite(destfile)))
	{
		DPf_H("Couldn't create File: %s",destfile);
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
		p_utils.getFatxName(file);
		

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
		
		DPf_H("Found %d Tracks",mfilescount);
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

/////////////////////////////////////////////////////
// UDF2SMB


int D2Xfilecopy::FileUDF2SMB(HDDBROWSEINFO source,char* dest)
{
	int stat = 0;
	char temp[1024];
	char temp2[1024];
	D2Xff factory;
	p_source = factory.Create(UDF);
	p_dest = factory.Create(D2X_SMB);

	if(source.type == BROWSE_FILE)
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
		DPf_H("copy iso %s to %s",temp,temp2);
		stat = DirUDF2SMB(temp,temp2);
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

bool D2Xfilecopy::CopyUDF2SMBFile(char* lpcszFile,char* destfile)
{
	wsprintfW(D2Xfilecopy::c_source,L"%hs",lpcszFile);
	wsprintfW(D2Xfilecopy::c_dest,L"%hs",destfile);

	if (!(p_dest->FileOpenWrite(destfile)))
	{		
		DebugOut("Couldn't open file: %s",destfile);
		p_log.WLog(L"Couldn't open destination file %hs",destfile);
		return FALSE;
	}

	if(!(p_source->FileOpenRead(lpcszFile)))
	{
		DebugOut("Couldn't open File: %s",lpcszFile);
		p_log.WLog(L"Couldn't open source file %hs",lpcszFile);
		return false;
	}

	int dwBufferSize  = 2048*16;
	//LARGE_INTEGER l_filesize;
	//GetFileSizeEx(hFile,&l_filesize);
	
	LPBYTE buffer		= new BYTE[dwBufferSize];
	//uint64_t fileSize   = l_filesize.QuadPart;
	DWORD fileSize = p_source->GetFileSize();
	uint64_t fileOffset = 0;

	//DPf_H("Filesize: %s %d",lpcszFile,fileSize);

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
		//dwWrote = p_smb.Write(buffer,lRead);
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
	//p_smb.Close();
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
		p_log.WLog(L"Invalid read file handle: %hs",sourcesearch);
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
	/*char* ch = strchr(dest,'\\');
	ch++;
	dest = ch;*/
	D2Xff factory;
	p_source = factory.Create(UDF);
	p_dest = factory.Create(FTP);

	if(source.type == BROWSE_FILE)
	{
		strcpy(temp2,source.name);
		p_utils.getFatxName(temp2);
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


	//if ((p_ftp.CreateFile(destfile)) != 1)

	//HANDLE hFile = CreateFile( lpcszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//if (hFile==NULL)
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
		p_utils.getFatxName(temp2);
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
		p_utils.getFatxName(file);
		

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
	//D2Xfilecopy::excludeDirs = 0;
	//D2Xfilecopy::excludeFiles = 0;
	wsprintfW(D2Xfilecopy::c_source,L"\0");
	wsprintfW(D2Xfilecopy::c_dest,L"\0");
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
	case SMB2UDF:
		dest_type = UDF;
		CopyGeneric(fsource,fdest,D2X_SMB,UDF);
		break;
	case X2SMB:
		dest_type = D2X_SMB;
		CopyGeneric(fsource,fdest,source_type,D2X_SMB);
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

