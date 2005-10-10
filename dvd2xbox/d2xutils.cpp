#include "d2xutils.h"

//#include <helper.h>
static D3DGAMMARAMP oldramp, flashramp;
#define clamp(x) (x) > 255.f ? 255 : ((x) < 0 ? 0 : (BYTE)(x+0.5f))


D2Xutils::D2Xutils()
{
	driveMappingEx.clear();
	driveMappingEx.insert(pair<char,string>('C', "C:,Harddisk0\\Partition2"));
	driveMappingEx.insert(pair<char,string>('D', "D:,Cdrom0"));
	driveMappingEx.insert(pair<char,string>('E', "E:,Harddisk0\\Partition1"));
	driveMappingEx.insert(pair<char,string>('F', "F:,Harddisk0\\Partition6"));
	driveMappingEx.insert(pair<char,string>('X', "X:,Harddisk0\\Partition3"));
	driveMappingEx.insert(pair<char,string>('Y', "Y:,Harddisk0\\Partition4"));
	driveMappingEx.insert(pair<char,string>('Z', "Z:,Harddisk0\\Partition5"));
	driveMappingEx.insert(pair<char,string>('G', "G:,Harddisk0\\Partition7"));

}

D2Xutils::~D2Xutils()
{

}
void D2Xutils::RemapHomeDir(char* path)
{
	CIoSupport io;
	CStdString	strValue;

	switch (path[0])
	{
	case 'c':
	case 'C':
		strValue = "Harddisk0\\Partition2";
		break;
	case 'd':
	case 'D':
		strValue = "Cdrom0";
		break;
	case 'e':
	case 'E':
		strValue = "Harddisk0\\Partition1";
		break;
	case 'f':
	case 'F':
		strValue = "Harddisk0\\Partition6";
		break;
	case 'g':
	case 'G':
		strValue = "Harddisk0\\Partition7";
		break;
	default:
		break;
	};
	strValue+=CStdString(path+2);
	io.Mount("Q:",(char*)strValue.c_str());
}


ULONG D2Xutils::getTitleID(char* path)
{
	FILE *stream;
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;

	stream  = fopen( path, "rb" );
	if(stream != NULL) {
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,1,sizeof(HC),stream);
		fclose(stream);
		return HC.TitleId;
	}	
	return 0;
}

//int D2Xutils::getXBECert(char* filename)
//{
//	FILE *stream;
//	_XBE_HEADER			xbeheader;
//
//	stream  = fopen( filename, "rb" );
//	if(stream != NULL) {
//		fread(&xbeheader,1,sizeof(xbeheader),stream);
//		fseek(stream,xbeheader.XbeHeaderSize,SEEK_SET);
//		fread(&xbecert,1,sizeof(xbecert),stream);
//		fclose(stream);
//		return 1;
//	}	
//	return 0;
//}

int D2Xutils::getXBECert(char* filename)
{
	FILE *stream;
	_XBE_HEADER			xbeheader;

	/*D2Xff		factory;
	D2Xfile*	p_source;
	DWORD		dwRead;
	p_source = factory.Create(filename);

	if(p_source->GetType() == FTP)
	{
		delete p_source;
		return 0;
	}

	if(p_source->FileOpenRead( filename))
	{
		p_source->FileRead(&xbeheader,sizeof(xbeheader),&dwRead);
		p_source->FileSeek(xbeheader.XbeHeaderSize,SEEK_SET);
		p_source->FileRead(&xbecert,sizeof(xbecert),&dwRead);
		p_source->FileClose();
		delete p_source;
		return 1;
	}
	delete p_source;*/

	stream  = fopen( filename, "rb" );
	if(stream != NULL) {
		fseek(stream,0,SEEK_SET);
		fread(&xbeheader,1,sizeof(xbeheader),stream);
		fseek(stream,xbeheader.XbeHeaderSize,SEEK_SET);
		fread(&xbecert,sizeof(xbecert),1,stream);
		fclose(stream);
		return 1;
	}	
	return 0;
}


bool D2Xutils::writeTitleName(char* path,const WCHAR* title)
{
	FILE *stream;
	_XBE_CERTIFICATE HC; 
	_XBE_HEADER HS;

	stream  = fopen( path, "r+b" );
	if(stream != NULL) {
		fseek(stream,0,SEEK_SET);
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,sizeof(HC),1,stream);
		wcsncpy(HC.TitleName,title,40);
		//HC.GameRegion = 0x80000007;
		//HC.MediaTypes = 0x000002FF;
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fwrite(&HC,sizeof(HC),1,stream);
		fclose(stream);
		return 1;
	}	
	return 0;
}

int D2Xutils::char2byte(char* ch, BYTE* b)
{
	char tmp[100];
	if(strlen(ch)%2)
		return 1;
	int n_byte = strlen(ch)/2;
	for(int c=0;c<n_byte;c++)
	{
		sprintf(tmp,"%c%c",ch[2*c],ch[2*c+1]);
		b[c] = (BYTE)strtoul(tmp,'\0',16);
	}
	return 0;
}

void D2Xutils::str2byte(string item,BYTE* buf)
{
	char buffer[4];
	for(int i=0;i<item.size();i++)
	{
		sprintf(buffer,"%2X",item.substr(i,1).c_str());
		buf[i] = (BYTE)strtoul(buffer,'\0',16);
	}
}

void D2Xutils::str2hex(string item,char* buf)
{
	char buffer[4];
	for(int i=0;i<item.size();i++)
	{
		sprintf(buffer,"%2X",item.substr(i,1).c_str());
		buf[i] = buffer[0];
		buf[i+1] = buffer[1];
		buf[i+2] = '\0';
	}
}


int D2Xutils::findHex(const char* file,char* mtext,int offset)
{
	FILE *stream;
	int read;
	#define blocksize 600
	bool found = false;
	BYTE tmp[blocksize];
	BYTE btext[100];
	if(strlen(mtext)%2)
		return -1;
	int n_byte = strlen(mtext)/2;
	
	if(char2byte(mtext,btext))
		return -1;

	//DebugOut("Checking %s",mtext);

	stream  = fopen( file, "rb" );
	if(stream==NULL)
		return -2;
	while(!found)
	{
		fseek(stream,offset,SEEK_SET);
		read = fread(tmp,1,blocksize,stream);
		if(read<n_byte) break;
		
		int c2=0;
		for(int c=0;c<read;c++)
		{
			if(btext[c2] != tmp[c])
			{
				c2=0;
				continue;
			}
			if(c2 == n_byte-1)
			{
				offset+=c-n_byte+1;
				found = true;
				break;
			}
			c2++;
		}
		if(!found) offset+=(blocksize-n_byte);
	}
	fclose(stream);
	if(found)
		return offset;
	return -1;
}

int D2Xutils::writeHex(const char* file,char* mtext,int offset)
{
	FILE *stream;
	BYTE btext[100];
	int write;

	if(strlen(mtext)%2)
		return -1;
	int n_byte = strlen(mtext)/2;

	if(char2byte(mtext,btext))
		return -1;

	SetFileAttributes(file,FILE_ATTRIBUTE_NORMAL);
	stream  = fopen( file, "r+b" );
	if(stream==NULL)
		return -2;
	fseek(stream,offset,SEEK_SET);
	write = fwrite(btext,1,n_byte,stream);
	if(write != n_byte)
		return -3;

	fclose(stream);
	return 0;

}


void D2Xutils::addSlash(char* source)
{
	if(source[strlen(source)-1] != '\\')
		strcat(source,"\\");
}

void D2Xutils::addSlash2(char* source)
{
	// the real slash. Just to lazy to rename addSlash in addBackslash :-)
	if(source[strlen(source)-1] != '/')
		strcat(source,"/");
}

bool D2Xutils::DelTree(char *path)
{
	char* sourcesearch = new char[1024];
	char* sourcefile = new char[1024];
	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"\\*");

	// Start the find and check for failure.
	hFind = FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
		RemoveDirectory( path );
	    return true;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			strcpy(sourcefile,path);
			strcat(sourcefile,"\\");
			strcat(sourcefile,wfd.cFileName);

			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				//strcat(sourcefile,"\\");
				// Recursion
				if(!DelTree( sourcefile ))
				{
					if(sourcesearch != NULL)
					{
						delete sourcesearch;
						sourcesearch = NULL;
					}
					if(sourcefile != NULL)
					{
						delete sourcefile;
						sourcefile = NULL;
					}
					return false;
				}
				DebugOut("Called deltree with: %hs\n",sourcefile);
			
			}
			else
			{
				// delete file
				DebugOut("Called delfile with: %hs\n",sourcefile);
				if(!DeleteFile(sourcefile))
					return false;
	
			}

	    }
	    while(FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    FindClose( hFind );
	}
	RemoveDirectory( path );
	if(sourcesearch != NULL)
	{
		delete sourcesearch;
		sourcesearch = NULL;
	}
	if(sourcefile != NULL)
	{
		delete sourcefile;
		sourcefile = NULL;
	}
	return true;
}

void D2Xutils::DelPersistentData(ULONG titleID)
{
	char data_dir[32];
	sprintf(data_dir,"e:\\TDATA\\%08x",titleID);
	DelTree(data_dir);
	sprintf(data_dir,"e:\\UDATA\\%08x",titleID);
	DelTree(data_dir);
	return;
}

void D2Xutils::getHomePath(char* path)
{
	CIoSupport p_IO;
	p_IO.GetXbePath(path);
	if(strlen(path) > 2)
	{
		char* p_xbe = strrchr(path,'\\');
		*p_xbe = 0;
	} else
	{
		strcpy(path,"e:\\");
	}
}

// MXM utils

HRESULT D2Xutils::MakePath( LPCTSTR szPath )
{
	HRESULT hr = E_FAIL;

	// Generate path from start to finish!
	CStdString sPath(szPath);

	sPath = PathSlasher( szPath, false );
#if 0
	int iPos = sPath.GetLength()-1;
	// Strip trailing backslash...
	if ( iPos >= 0 && sPath[iPos] == _T('\\') )
	{
		sPath = sPath.Left( iPos );
	}
#endif
	DWORD dwAttr = GetFileAttributes( sPath );

	if ( sPath.GetLength() > 2 )
	{
		if ( dwAttr == (-1))
		{
			// OK, let's step through the process of making a path
			if ( _istalpha( sPath[0] ) && sPath[1] == _T(':') && sPath[2] == _T('\\') )
			{
				// OK so far... let's see if we need to go further:
				if ( sPath.GetLength() > 3 )
				{
					// Call MakePath on parent directory....
					CStdString sParent;
					int iPos = sPath.ReverseFind( _T("\\") );
					if ( iPos >= 2 )
					{
						sParent = sPath.Left( iPos );
						hr = MakePath( sParent );
					}
					else
					{
						// else parent was root...
						hr = S_OK;
					}
					dwAttr = GetFileAttributes( sPath );
					if ( (int)dwAttr == (-1) )
					{
						if ( SUCCEEDED(hr) && CreateDirectory( sPath, NULL ) )
						{
							hr = S_OK;
						}
						else
						{
							hr = E_FAIL;
						}
					}
					else
					{
						if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
						{
							hr = S_OK;
						}
						else
						{
							hr = E_FAIL;
						}
					}
				}
				else
				{
					//Drive root, just say it's "OK"
					hr = S_OK;
				}
			}
			// Else: Fail!
		}
		else if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
		{
			// Already exists... that's fine.
			hr = S_OK;
		}
		// Else, failed. Must be file. We cannot make a path!
	}
	return hr;
}

CStdString D2Xutils::PathSlasher( LPCTSTR szPath, bool bSlashIt )
{
	CStdString sReturn;
	int iLen;

	if ( szPath && (iLen = _tcslen( szPath )) )
	{
		iLen--;
		if ( szPath[iLen] == _T('\\') )
		{
			if ( !bSlashIt )
			{
				sReturn = CStdString( szPath, iLen );
			}
			else
			{
				sReturn = szPath;
			}
		}
		else
		{
			if ( bSlashIt )
			{
				sReturn = szPath;
				sReturn += _T('\\');
			}
			else
			{
				sReturn = szPath;
			}
		}
		if ( !bSlashIt ) // Special case: Root directory MUST be slashed
		{
			if ( sReturn.GetLength() == 2 && sReturn[1] == _T(':') && _istalpha( sReturn[0]) )
			{
				sReturn += _T('\\');
			}
		}
	}
	return sReturn;
}

int D2Xutils::SetMediatype(const char* file,ULONG &mt,char* nmt)
{
	FILE *stream; 
	_XBE_CERTIFICATE HC;
	_XBE_HEADER HS;
	char rnmt[10];

	if(strlen(nmt) != 8)
		return 0;

	sprintf(rnmt,"%c%c%c%c%c%c%c%c",nmt[6],nmt[7],nmt[4],nmt[5],nmt[2],nmt[3],nmt[0],nmt[1]);

	stream  = fopen( file, "rb" );
	if(stream == NULL) 
		return 0;

	fread(&HS,1,sizeof(HS),stream);
	fseek(stream,HS.XbeHeaderSize,SEEK_SET);
	fread(&HC,1,sizeof(HC),stream);
	
	mt = HC.MediaTypes;
	fclose(stream);
	if(writeHex(file,rnmt,(HS.XbeHeaderSize + 156))!=0)
		return 0;
	return (HS.XbeHeaderSize + 156);
}

int D2Xutils::SetGameRegion(const char* file,ULONG &mt,char* nmt)
{
	FILE *stream;
	_XBE_CERTIFICATE HC; 
	_XBE_HEADER HS;

	if(strlen(nmt) != 8)
		return 0;

	stream  = fopen( file, "r+b" );
	if(stream != NULL) {
		fseek(stream,0,SEEK_SET);
		fread(&HS,1,sizeof(HS),stream);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fread(&HC,sizeof(HC),1,stream);
		mt = HC.GameRegion;
		HC.GameRegion = (ULONG)atol(nmt);
		fseek(stream,HS.XbeHeaderSize,SEEK_SET);
		fwrite(&HC,sizeof(HC),1,stream);
		fclose(stream);
		return 1;
	}	
	return 0;
}


// emulated for smb
char* getenv(const char* szKey)
{
	return NULL;
}

//__inline void D2Xutils::getFatxName(char* pattern)
//{
//	int i=0;
//	int fatx=42;
//	char temp[128];
//	strcpy(temp,"\0");
//	while(pattern[i] != NULL)
//	{
//		if(isalnum(pattern[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern[i]))
//		{
//			sprintf(temp,"%s%c",temp,pattern[i]);
//		}
//		++i;
//	}
//	memset(pattern,0,fatx+1);
//	if(strlen(temp) > fatx)
//	{
//		char* c;
//		c = strrchr(temp,'.');
//		if(c)
//		{
//			strncpy(pattern,temp,fatx-strlen(c));
//			strcat(pattern,c);
//		} else {
//			strncpy(pattern,temp,fatx);
//			pattern[fatx] = '\0';
//		}
//	} else {
//		strcpy(pattern,temp);
//	}
//	return;
//}

//__inline void getFatxName(char* pattern)
//{
//	CStdString f_name;
//
//	for(int i=0;i<strlen(pattern);i++)
//	{
//		if(isalnum(pattern[i]) || strchr(" !#$%&'()-.@[]^_`{}~",pattern[i]))
//		{
//			f_name.push_back(pattern[i]);
//		}
//	}
//
//	memset(pattern,0,strlen(pattern)+1);
//	if(f_name.size() > FATX_LENGTH)
//	{
//		char* c;
//		c = strrchr(f_name.c_str(),'.');
//		if(c != 0)
//		{
//			strncpy(pattern,f_name.c_str(),FATX_LENGTH-strlen(c));
//			strcat(pattern,c);
//		} else {
//			strncpy(pattern,f_name.c_str(),FATX_LENGTH);
//			pattern[FATX_LENGTH] = '\0';
//		}
//	} else {
//		strcpy(pattern,f_name.c_str());
//	}
//	return;
//}

void D2Xutils::getFatxNameW(WCHAR* pattern)
{
	char temp[128];
	wsprintf(temp,"%S",pattern);	
	getFatxName(temp);
	swprintf(pattern,L"%S", temp );
	return;
}

bool D2Xutils::isdriveD(char* path)
{
	if(!_strnicmp(path,"d:",2))
		return true;
	else
		return false;
}

bool D2Xutils::getfreeDiskspaceMB(char* drive,char* size)
{
	ULARGE_INTEGER ulFreeAvail;
	if( !GetDiskFreeSpaceEx( drive, NULL, NULL, &ulFreeAvail ) )
		return false;
	sprintf(size,"%6d MB free",(int)(ulFreeAvail.QuadPart/1048576));
	return true;
}

bool D2Xutils::getfreeDiskspaceMB(char* drive,int& size)
{
	ULARGE_INTEGER ulFreeAvail;
	if( !GetDiskFreeSpaceEx( drive, NULL, NULL, &ulFreeAvail ) )
		return false;
	size = int(ulFreeAvail.QuadPart/1048576);
	return true;
}

int D2Xutils::getfreeDiskspaceMB(char* drive)
{
	int isize;
	getfreeDiskspaceMB(drive,isize);
	return isize;
}

void D2Xutils::LaunchXbe(CHAR* szPath, CHAR* szXbe, CHAR* szParameters)
{
	CIoSupport helper;
	char temp[1024];
	if(!_strnicmp(szPath,"e:",2))
	{
		strcpy(temp,"Harddisk0\\Partition1");
		szPath+=2;
		strcat(temp,szPath);
	} else if(!_strnicmp(szPath,"f:",2))
	{
		strcpy(temp,"Harddisk0\\Partition6");
		szPath+=2;
		strcat(temp,szPath);
	} else if(!_strnicmp(szPath,"g:",2))
	{
		strcpy(temp,"Harddisk0\\Partition7");
		szPath+=2;
		strcat(temp,szPath);
	} else if(!_strnicmp(szPath,"d:",2))
	{
		strcpy(temp,"Cdrom0");
		szPath+=2;
		strcat(temp,szPath);
	}
	helper.Unmount("D:");
	helper.Mount("D:",temp);

	//DebugOut("Launching %s %s",temp,szXbe);

	if (szParameters==NULL)
	{
		XLaunchNewImage(szXbe, NULL );
	}
	else
	{
		LAUNCH_DATA LaunchData;
		strcpy((CHAR*)LaunchData.Data,szParameters);

		XLaunchNewImage(szXbe, &LaunchData );
	}
}

int D2Xutils::IsDrivePresent( char* cDrive )
{
	// taken from MXM
	int bReturn = 0;
	ULARGE_INTEGER uFree1, uTotal1, uTotal2;

	CIoSupport		io;
	WIN32_FIND_DATA wfd; 
	HANDLE hFind;
	char path[5];

	strcpy(path,cDrive);
	addSlash(path);
	strcat(path,"*");

	io.Remap((char*)driveMappingEx[toupper(cDrive[0])].c_str());

	hFind = FindFirstFile( path, &wfd );
		
	if( INVALID_HANDLE_VALUE != hFind)
	{
		bReturn = 1;
		FindClose( hFind );
	}
	else if ( GetDiskFreeSpaceEx( cDrive, &uFree1, &uTotal1, &uTotal2 ) ) 
	{
		bReturn = 1;
	}
	else
	{
		// Should do a final check....  
		char szVolume[256];
		char szFileSys[32];
		DWORD dwVolSer, dwMaxCompLen, dwFileSysFlags;
		if ( GetVolumeInformation( cDrive, szVolume, 255, &dwVolSer, &dwMaxCompLen, &dwFileSysFlags, szFileSys, 32 ) )
		{
			bReturn = 1;
		}
	}
	io.Unmount(cDrive);
	return bReturn;
}

void D2Xutils::GetHDDModel(CStdString& strModel)
{
	if(strhdd.empty())
	{
		CHAR TempString1[100];
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		DWORD slen = 0;
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_MASTER;
		hddcommand.IPReg.bCommandReg = IDE_ATA_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
		
		ZeroMemory(TempString1, 100);
		XKHDD::GetIDEModel(hddcommand.DATA_BUFFER, TempString1, &slen);
		strhdd = TempString1;
	}
	strModel = strhdd;

}

void D2Xutils::GetDVDModel(CStdString& strModel)
{
	if(strhdd.empty())
	{
		CHAR TempString1[100];
		XKHDD::ATA_COMMAND_OBJ hddcommand;
		DWORD slen = 0;
		ZeroMemory(&hddcommand, sizeof(XKHDD::ATA_COMMAND_OBJ));
		hddcommand.DATA_BUFFSIZE = 0;
		hddcommand.IPReg.bDriveHeadReg = IDE_DEVICE_SLAVE;
		hddcommand.IPReg.bCommandReg = IDE_ATAPI_IDENTIFY;
		XKHDD::SendATACommand(IDE_PRIMARY_PORT, &hddcommand, IDE_COMMAND_READ);
		
		ZeroMemory(TempString1, 100);
		XKHDD::GetIDEModel(hddcommand.DATA_BUFFER, TempString1, &slen);
		strdvd = TempString1;
	}
	strModel = strdvd;

}

bool D2Xutils::IsEthernetConnected()
{
	if (!(XNetGetEthernetLinkStatus() & XNET_ETHERNET_LINK_ACTIVE))
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////
// XBMC
//

void D2Xutils::SetBrightnessContrastGamma(float Brightness, float Contrast, float Gamma, bool bImmediate)
{
  // calculate ramp
  D3DGAMMARAMP ramp;

  Gamma = 1.0f / Gamma;
  for (int i = 0; i < 256; ++i)
  {
    float f = (powf((float)i / 255.f, Gamma) * Contrast + Brightness) * 255.f;
    ramp.blue[i] = ramp.green[i] = ramp.red[i] = clamp(f);
  }

  // set ramp next v sync
  //g_graphicsContext.Lock();
  g_pd3dDevice->SetGammaRamp(bImmediate ? D3DSGR_IMMEDIATE : 0, &ramp);
  //g_graphicsContext.Unlock();
}

CStdString D2Xutils::GetNextFilename(const char* fn_template, int max)
{
  // Open the file.
  char szName[1024];

  INT i;

  WIN32_FIND_DATA wfd;
  HANDLE hFind;


  if (NULL != strstr(fn_template, "%03d"))
  {
    for (i = 0; i <= max; i++)
    {

      wsprintf(szName, fn_template, i);

      memset(&wfd, 0, sizeof(wfd));
      if ((hFind = FindFirstFile(szName, &wfd)) != INVALID_HANDLE_VALUE)
        FindClose(hFind);
      else
      {
        // FindFirstFile didn't find the file 'szName', return it
        return szName;
      }
    }
  }

  return ""; // no fn generated
}

void D2Xutils::FlashScreen(bool bImmediate, bool bOn)
{
  static bool bInFlash = false;

  if (bInFlash == bOn)
    return ;
  bInFlash = bOn;
  //g_graphicsContext.Lock();
  if (bOn)
  {
    g_pd3dDevice->GetGammaRamp(&flashramp);
    SetBrightnessContrastGamma(0.5f, 1.2f, 2.0f, bImmediate);
  }
  else
    g_pd3dDevice->SetGammaRamp(bImmediate ? D3DSGR_IMMEDIATE : 0, &flashramp);
  //g_graphicsContext.Unlock();
}

void D2Xutils::TakeScreenshot(const char* fn, bool flashScreen)
{
    LPDIRECT3DSURFACE8 lpSurface = NULL;

    //g_graphicsContext.Lock();
    
    // now take screenshot
    g_pd3dDevice->BlockUntilVerticalBlank();
    if (SUCCEEDED(g_pd3dDevice->GetBackBuffer( -1, D3DBACKBUFFER_TYPE_MONO, &lpSurface)))
    {
      if (FAILED(XGWriteSurfaceToFile(lpSurface, fn)))
      {
        DebugOut( "Failed to Generate Screenshot");
      }
      else
      {
        DebugOut( "Screen shot saved as %s", fn);
      }
      lpSurface->Release();
    }
   // g_graphicsContext.Unlock();
    if (flashScreen)
    {
      g_pd3dDevice->BlockUntilVerticalBlank();
      FlashScreen(true, true);
      Sleep(10);
      g_pd3dDevice->BlockUntilVerticalBlank();
      FlashScreen(true, false);
    }
}

void D2Xutils::TakeScreenshot()
{
	char fn[1024];
  
	strcpy(fn, "Q:\\screenshot%03d.bmp");
	strcpy(fn, D2Xutils::GetNextFilename(fn, 999).c_str());

	if (strlen(fn))
	{
		TakeScreenshot(fn, true);
	}
}

void D2Xutils::Unicode2Ansi(const wstring& wstrText,CStdString& strName)
{
  strName="";
  char *pstr=(char*)wstrText.c_str();
  for (int i=0; i < (int)wstrText.size();++i )
  {
    strName += pstr[i*2];
  }
}

// around 50% faster than memcpy
// only worthwhile if the destination buffer is not likely to be read back immediately
// and the number of bytes copied is >16
// somewhat faster if the source and destination are a multiple of 16 bytes apart
void fast_memcpy(void* d, const void* s, unsigned n)
{
	__asm {
		mov edx,n
		mov esi,s
		prefetchnta [esi]
		prefetchnta [esi+32]
		mov edi,d

		// pre align
		mov eax,edi
		mov ecx,16
		and eax,15
		sub ecx,eax
		and ecx,15
		cmp edx,ecx
		jb fmc_exit_main
		sub edx,ecx

		test ecx,ecx
fmc_start_pre:
		jz fmc_exit_pre

		mov al,[esi]
		mov [edi],al

		inc esi
		inc edi
		dec ecx
		jmp fmc_start_pre

fmc_exit_pre:
		mov eax,esi
		and eax,15
		jnz fmc_notaligned

		// main copy, aligned
		mov ecx,edx
		shr ecx,4
fmc_start_main_a:
		jz fmc_exit_main

		prefetchnta [esi+32]
		movaps xmm0,[esi]
		movntps [edi],xmm0

		add esi,16
		add edi,16
		dec ecx
		jmp fmc_start_main_a

fmc_notaligned:
		// main copy, unaligned
		mov ecx,edx
		shr ecx,4
fmc_start_main_u:
		jz fmc_exit_main

		prefetchnta [esi+32]
		movups xmm0,[esi]
		movntps [edi],xmm0

		add esi,16
		add edi,16
		dec ecx
		jmp fmc_start_main_u

fmc_exit_main:

		// post align
		mov ecx,edx
		and ecx,15
fmc_start_post:
		jz fmc_exit_post

		mov al,[esi]
		mov [edi],al

		inc esi
		inc edi
		dec ecx
		jmp fmc_start_post

fmc_exit_post:
	}
}

void fast_memset(void* d, int c, unsigned n)
{
	char __declspec(align(16)) buf[16];

	__asm {
		mov edx,n
		mov edi,d

		// pre align
		mov eax,edi
		mov ecx,16
		and eax,15
		sub ecx,eax
		and ecx,15
		cmp edx,ecx
		jb fms_exit_main
		sub edx,ecx
		mov eax,c

		test ecx,ecx
fms_start_pre:
		jz fms_exit_pre

		mov [edi],al

		inc edi
		dec ecx
		jmp fms_start_pre

fms_exit_pre:
		test al,al
		jz fms_initzero

		// duplicate the value 16 times
		lea esi,buf
		mov [esi],al
		mov [esi+1],al
		mov [esi+2],al
		mov [esi+3],al
		mov eax,[esi]
		mov [esi+4],eax
		mov [esi+8],eax
		mov [esi+12],eax
		movaps xmm0,[esi]
		jmp fms_init_loop

fms_initzero:
		// optimzed set zero
		xorps xmm0,xmm0

fms_init_loop:
		mov ecx,edx
		shr ecx,4

fms_start_main:
		jz fms_exit_main

		movntps [edi],xmm0

		add edi,16
		dec ecx
		jmp fms_start_main

fms_exit_main:

		// post align
		mov ecx,edx
		and ecx,15
fms_start_post:
		jz fms_exit_post

		mov [edi],al

		inc edi
		dec ecx
		jmp fms_start_post

fms_exit_post:
	}
}

void usleep(int t)
{
  LARGE_INTEGER li;
  
  li.QuadPart = (LONGLONG)t * -10;

  // Where possible, Alertable should be set to FALSE and WaitMode should be set to KernelMode,
  // in order to reduce driver complexity. The principal exception to this is when the wait is a long term wait.
  KeDelayExecutionThread(KernelMode, false, &li);
}