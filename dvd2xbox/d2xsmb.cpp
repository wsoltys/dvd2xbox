#include "D2Xsmb.h"
#include <xtl.h>



D2Xsmb::D2Xsmb()
{

}

D2Xsmb::~D2Xsmb()
{

}

HANDLE D2Xsmb::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{

	char szFileName[1024];
	if (g_d2xSettings.smbPassword && g_d2xSettings.smbUsername)
		sprintf(szFileName,"smb://%s:%s@%s/%s", g_d2xSettings.smbUsername, g_d2xSettings.smbPassword, g_d2xSettings.smbHostname, lpFileName);
	else
		sprintf(szFileName,"smb://%s/%s", g_d2xSettings.smbHostname, lpFileName);

	strRoot = szFileName;
	/*
	Taken from XBMC's SMBDirectory:

	We accept smb://[[[domain;]user[:password@]]server[/share[/path[/file]]]]

	unsigned int smbc_type; 
		 Type of entity.
	    SMBC_WORKGROUP=1,
	    SMBC_SERVER=2, 
	    SMBC_FILE_SHARE=3,
	    SMBC_PRINTER_SHARE=4,
	    SMBC_COMMS_SHARE=5,
	    SMBC_IPC_SHARE=6,
	    SMBC_DIR=7,
	    SMBC_FILE=8,
	    SMBC_LINK=9,
*/

	// note, samba uses UTF8 strings internal,
	// that's why we have to convert strings and wstrings to UTF8.
	char strUtfPath[1024];

	smb.Init();

	// convert from string to UTF8
	int strLen = convert_string(CH_DOS, CH_UTF8, strRoot, strRoot.length(), strUtfPath, 1024);
	strUtfPath[strLen] = 0;


	fd = smbc_opendir(strUtfPath);


	if (fd < 0)
	{
		int error;
		//if (errno == ENODEV) error = NT_STATUS_INVALID_COMPUTER_NAME;
		//else error = map_nt_error_from_unix(errno);
		
		//const char* cError = get_friendly_nt_error_msg(error);


		return INVALID_HANDLE_VALUE;
	}
	else
	{
		//struct smbc_dirent* dirEnt;
		wchar_t wStrFile[1024];
		char strUtfFile[1024];

		dirEnt = smbc_readdir(fd);
		if (dirEnt->name && strcmp(dirEnt->name,".") && strcmp(dirEnt->name,"..") &&
				 (dirEnt->name[dirEnt->namelen - 2] != '$'))
			{
				unsigned __int64 iSize = 0;
				bool bIsDir = true;
				__int64 lTimeDate = 0;

				// convert from UTF8 to wide string
				strLen = convert_string(CH_UTF8, CH_UCS2, dirEnt->name, dirEnt->namelen, wStrFile, 1024);
				wStrFile[strLen] = 0;

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
					CStdString strFile = strRoot + wStrFile;

					// convert from string to UTF8
					strLen = convert_string(CH_DOS, CH_UTF8, strFile, strFile.length(), strUtfFile, 1024);
					strUtfFile[strLen] = 0;

					smbc_stat(strUtfFile, &info);

					bIsDir = (info.st_mode & S_IFDIR) ? true : false;
					lTimeDate = info.st_ctime;
					iSize = info.st_size;
				}
				
				FILETIME fileTime,localTime;
				LONGLONG ll = Int32x32To64(lTimeDate, 10000000) + 116444736000000000;
				fileTime.dwLowDateTime = (DWORD) ll;
				fileTime.dwHighDateTime = (DWORD)(ll >>32);

				FileTimeToLocalFileTime(&fileTime,&localTime); 

				if (bIsDir)
				{
					/*
					CFileItem *pItem = new CFileItem(wStrFile);
					pItem->m_strPath = strRoot;

					// needed for network / workgroup browsing
					// skip if root has already a valid domain and type is not a server
					if ((strRoot.find(';') == -1) &&
							(dirEnt->smbc_type == SMBC_SERVER) &&
							(strRoot.find('@') == -1))
					{
						// lenght > 6, which means a workgroup name is specified and we need to
						// remove it. Domain without user is not allowed
						int strLength = strRoot.length();
						if (strLength > 6)
						{
							if(CUtil::HasSlashAtEnd(strRoot))
								pItem->m_strPath = "smb://";
						}
					}
					pItem->m_strPath += wStrFile;
					if(!CUtil::HasSlashAtEnd(pItem->m_strPath)) pItem->m_strPath += '/';
					pItem->m_bIsFolder = true;
					FileTimeToSystemTime(&localTime, &pItem->m_stTime);  
					items.push_back(pItem);*/
					lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
					wsprintf(lpFindFileData->cFileName,"%S",wStrFile);
				}
				else
				{
					/*
					if (IsAllowed(wStrFile))
					{
						CFileItem *pItem = new CFileItem(wStrFile);
						pItem->m_strPath = strRoot;
						pItem->m_strPath += wStrFile;
						pItem->m_bIsFolder = false;
						pItem->m_dwSize = iSize;
						FileTimeToSystemTime(&localTime, &pItem->m_stTime);
		        
						items.push_back(pItem);
					}*/
					lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
					wsprintf(lpFindFileData->cFileName,"%S",wStrFile);
				}
			}

	}

	return (HANDLE)this;
}

BOOL D2Xsmb::FindNextFile(HANDLE hFindFile,LPWIN32_FIND_DATA lpFindFileData)
{
	wchar_t wStrFile[1024];
	char strUtfFile[1024];
	dirEnt = smbc_readdir(fd);
	if (dirEnt->name && strcmp(dirEnt->name,".") && strcmp(dirEnt->name,"..") &&
				 (dirEnt->name[dirEnt->namelen - 2] != '$'))
	{
		unsigned __int64 iSize = 0;
		bool bIsDir = true;
		__int64 lTimeDate = 0;

		// convert from UTF8 to wide string
		int strLen = convert_string(CH_UTF8, CH_UCS2, dirEnt->name, dirEnt->namelen, wStrFile, 1024);
		wStrFile[strLen] = 0;

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
			CStdString strFile = strRoot + wStrFile;

			// convert from string to UTF8
			strLen = convert_string(CH_DOS, CH_UTF8, strFile, strFile.length(), strUtfFile, 1024);
			strUtfFile[strLen] = 0;

			smbc_stat(strUtfFile, &info);

			bIsDir = (info.st_mode & S_IFDIR) ? true : false;
			lTimeDate = info.st_ctime;
			iSize = info.st_size;
		}
				
		FILETIME fileTime,localTime;
		LONGLONG ll = Int32x32To64(lTimeDate, 10000000) + 116444736000000000;
		fileTime.dwLowDateTime = (DWORD) ll;
		fileTime.dwHighDateTime = (DWORD)(ll >>32);

		FileTimeToLocalFileTime(&fileTime,&localTime); 

		if (bIsDir)
		{
			/*
			CFileItem *pItem = new CFileItem(wStrFile);
			pItem->m_strPath = strRoot;

			// needed for network / workgroup browsing
			// skip if root has already a valid domain and type is not a server
			if ((strRoot.find(';') == -1) &&
					(dirEnt->smbc_type == SMBC_SERVER) &&
					(strRoot.find('@') == -1))
			{
				// lenght > 6, which means a workgroup name is specified and we need to
				// remove it. Domain without user is not allowed
				int strLength = strRoot.length();
				if (strLength > 6)
				{
					if(CUtil::HasSlashAtEnd(strRoot))
						pItem->m_strPath = "smb://";
				}
			}
			pItem->m_strPath += wStrFile;
			if(!CUtil::HasSlashAtEnd(pItem->m_strPath)) pItem->m_strPath += '/';
			pItem->m_bIsFolder = true;
			FileTimeToSystemTime(&localTime, &pItem->m_stTime);  
			items.push_back(pItem);*/
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			wsprintf(lpFindFileData->cFileName,"%S",wStrFile);
		}
		else
		{
			/*
			if (IsAllowed(wStrFile))
			{
				CFileItem *pItem = new CFileItem(wStrFile);
				pItem->m_strPath = strRoot;
				pItem->m_strPath += wStrFile;
				pItem->m_bIsFolder = false;
				pItem->m_dwSize = iSize;
				FileTimeToSystemTime(&localTime, &pItem->m_stTime);
	       
				items.push_back(pItem);
			}*/
			lpFindFileData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
			wsprintf(lpFindFileData->cFileName,"%S",wStrFile);
		}
		return true;
	}
	else
	{
		return false;
	}
	
}

BOOL D2Xsmb::FindClose(HANDLE hFindFile)
{
	smbc_closedir(fd);
	hFindFile = NULL;
	return true;
}

