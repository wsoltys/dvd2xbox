#include "D2Xfilesmb.h"


D2XfileSMB::D2XfileSMB()
{
	
}

D2XfileSMB::~D2XfileSMB()
{
	p_smb.Close();
	smbc_purge();
}

void D2XfileSMB::FormPath(char* path, char* ret_path)
{
	char* f;
	if(!_strnicmp(path,"smb:",4))
		strcpy(ret_path,path+5);
	else
		strcpy(ret_path,path);
	
	while((f = strchr(ret_path,'\\')) != NULL)
	{
		*f = '/';
	}
}

int D2XfileSMB::CreateDirectory(char* name)
{
	char temp[1024];
	char sdir[1024];
	FormPath(name,temp);
	sprintf(sdir,"%s/%s",g_d2xSettings.smbShare,temp);

	if(p_smb.CreateDirectory(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,sdir,445,true) != 0)
		return 0;
	return 1;
}


int D2XfileSMB::FileOpenWrite(char* filename)
{
	char temp[1024];
	char sfile[1024];
	FormPath(filename,temp);
	sprintf(sfile,"%s/%s",g_d2xSettings.smbShare,temp);
	p_smb.Close();
	if ((p_smb.Create(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,sfile,445,true)) == false)
	{		
		return 0;
	}
	return 1;
}

int D2XfileSMB::FileOpenRead(char* filename)
{
	char temp[1024];
	char sfile[1024];
	FormPath(filename,temp);
	sprintf(sfile,"%s/%s",g_d2xSettings.smbShare,temp);
	p_smb.Close();
	if ((p_smb.Open(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,sfile,445,true)) == false)
	{		
		return 0;
	}
	return 1;
}

int D2XfileSMB::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	*dwWrote = p_smb.Write(buffer,dwWrite);
	return 1;
}


int D2XfileSMB::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	*dwRead = p_smb.Read(buffer,dwToRead);
	return 1;
}

int D2XfileSMB::FileClose()
{
	p_smb.Close();
	return 1;
}

DWORD D2XfileSMB::GetFileSize(char* filename)
{
	return p_smb.GetLength();
}


int D2XfileSMB::GetDirectory(char* path, VECFILEITEMS *items)
{
	// modified from XBMC

	items->clear();
	ITEMS temp_item;
	char temp[1024];
	
	FormPath(path,temp);

	char szFileName[1024];
	if (g_d2xSettings.smbPassword && g_d2xSettings.smbUsername)
		sprintf(szFileName,"smb://%s:%s@%s/%s", g_d2xSettings.smbUsername, g_d2xSettings.smbPassword, g_d2xSettings.smbHostname, g_d2xSettings.smbShare);
	else
		sprintf(szFileName,"smb://%s/%s", g_d2xSettings.smbHostname, g_d2xSettings.smbShare);

	p_utils.addSlash2(szFileName);
	strcat(szFileName,temp);

	strRoot = szFileName;

	smb.Init();
	smb.Lock();
	int fd = smbc_opendir(strRoot);
	smb.Unlock();

	if (fd < 0)
	{
		int nt_error;
		if (errno == ENODEV) nt_error = NT_STATUS_INVALID_COMPUTER_NAME;
		else nt_error = map_nt_error_from_unix(errno);

		// write error to logfile
		DebugOut("SMBDirectory->GetDirectory: Unable to open directory : '%s'\nunix_err:'%x' nt_err : '%x' error : '%s'",
			strRoot.c_str(), errno, nt_error, get_friendly_nt_error_msg(nt_error));

		// is we have an 'invalid handle' error we don't display the error
		// because most of the time this means there is no cdrom in the server's
		// cdrom drive.
		/*if (nt_error != 0xc0000008)
		{
			const char* cError = get_friendly_nt_error_msg(nt_error);
			
			CGUIDialogOK* pDialog = (CGUIDialogOK*)m_gWindowManager.GetWindow(WINDOW_DIALOG_OK);
			pDialog->SetHeading(257);
			pDialog->SetLine(0,cError);
			pDialog->SetLine(1,L"");
			pDialog->SetLine(2,L"");

			ThreadMessage tMsg = {TMSG_DIALOG_DOMODAL, WINDOW_DIALOG_OK, m_gWindowManager.GetActiveWindow()};
			g_applicationMessenger.SendMessage(tMsg, false);
		}*/
		return 0;
	}
	else
	{
		struct smbc_dirent* dirEnt;
		CStdString strFile;

		smb.Lock();
		dirEnt = smbc_readdir(fd);
		smb.Unlock();

		while (dirEnt) 
		{
			if (dirEnt->name && strcmp(dirEnt->name,".") && strcmp(dirEnt->name,"..") &&
				 (dirEnt->name[dirEnt->namelen - 2] != '$'))
			{
				unsigned __int64 iSize = 0;
				bool bIsDir = true;
				__int64 lTimeDate = 0;

				// Convert UTF8 to the user defined charset
				//g_charsetConverter.utf8ToStringCharset(dirEnt->name, strFile);
				strFile = dirEnt->name;

				// doing stat on one of these types of shares leaves an open session
				// so just skip them and only stat real dirs / files.
				if( dirEnt->smbc_type != SMBC_IPC_SHARE &&
						dirEnt->smbc_type != SMBC_FILE_SHARE &&
						dirEnt->smbc_type != SMBC_PRINTER_SHARE &&
						dirEnt->smbc_type != SMBC_COMMS_SHARE &&
						dirEnt->smbc_type != SMBC_WORKGROUP &&
						dirEnt->smbc_type != SMBC_SERVER)
				{
					struct __stat64 info;
					CStdString strFullName = strRoot + dirEnt->name; //Make sure we use the authenticated path wich contains any default username

					smb.Lock();
					smbc_stat(strFullName, &info);
					smb.Unlock();

					bIsDir = (info.st_mode & S_IFDIR) ? true : false;
					lTimeDate = info.st_ctime;
					iSize = info.st_size;
				}
				
				FILETIME fileTime,localTime;
				LONGLONG ll = Int32x32To64(lTimeDate & 0xffffffff, 10000000) + 116444736000000000;
				fileTime.dwLowDateTime = (DWORD) (ll & 0xffffffff);
				fileTime.dwHighDateTime = (DWORD)(ll >>32);
				FileTimeToLocalFileTime(&fileTime,&localTime); 

				if (bIsDir)
				{
					/*CFileItem *pItem = new CFileItem(strFile);
					pItem->m_strPath = strRoot;*/

					// needed for network / workgroup browsing
					// skip if root has already a valid domain and type is not a server
					if ((strRoot.find(';') == -1) &&
							(dirEnt->smbc_type == SMBC_SERVER)) 
							/*&& (strRoot.find('@') == -1))*/ //Removed to allow browsing even if a user is specified
					{
						// lenght > 6, which means a workgroup name is specified and we need to
						// remove it. Domain without user is not allowed
						int strLength = strRoot.length();
						if (strLength > 6)
						{
							/*if(CUtil::HasSlashAtEnd(strRoot))
								pItem->m_strPath = "smb://";*/
						}
					}
					/*pItem->m_strPath += dirEnt->name;
					if(!CUtil::HasSlashAtEnd(pItem->m_strPath)) pItem->m_strPath += '/';
					pItem->m_bIsFolder = true;
					FileTimeToSystemTime(&localTime, &pItem->m_stTime);  
					vecCacheItems.push_back(pItem);
					items.push_back(new CFileItem(*pItem));*/

					temp_item.fullpath = strRoot + dirEnt->name;
					temp_item.name = dirEnt->name;
					temp_item.isDirectory = 1;
					items->push_back(temp_item);
				}
				else
				{
					/*CFileItem *pItem = new CFileItem(strFile);
					pItem->m_strPath = strRoot + dirEnt->name;
					pItem->m_bIsFolder = false;
					pItem->m_dwSize = iSize;
					FileTimeToSystemTime(&localTime, &pItem->m_stTime);
		        
					vecCacheItems.push_back(pItem);
					if (IsAllowed(dirEnt->name)) items.push_back(new CFileItem(*pItem));*/

					temp_item.fullpath = strRoot + dirEnt->name;
					temp_item.name = dirEnt->name;
					temp_item.isDirectory = 0;
					items->push_back(temp_item);
				}
			}
			smb.Lock();
			dirEnt = smbc_readdir(fd);
			smb.Unlock();
		}
		smb.Lock();
		smbc_closedir(fd);
		smb.Unlock();
	}


	
	return 1;
}



