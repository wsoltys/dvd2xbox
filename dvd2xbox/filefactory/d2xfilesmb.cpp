#include "D2Xfilesmb.h"


D2XfileSMB::D2XfileSMB()
{
}

D2XfileSMB::~D2XfileSMB()
{
	p_smb.Close();
}


int D2XfileSMB::FileOpenWrite(char* filename)
{
	p_smb.Close();
	if ((p_smb.Create(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,filename,445,true)) == false)
	{		
		return 0;
	}
	return 1;
}

int D2XfileSMB::FileOpenRead(char* filename)
{
	p_smb.Close();
	if ((p_smb.Open(g_d2xSettings.smbUsername,g_d2xSettings.smbPassword,g_d2xSettings.smbHostname,filename,445,true)) == false)
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

DWORD D2XfileSMB::GetFileSize()
{
	return p_smb.GetLength();
}

int D2XfileSMB::GetDirectory(char* path, VECFILEITEMS &items)
{
	char sourcesearch[1024]="";
	WIN32_FIND_DATA wfd;
	HANDLE hFind;
	items.clear();
	ITEMS temp_item;

	strcpy(sourcesearch,path);
	strcat(sourcesearch,"*");

	// Start the find and check for failure.
	hFind = p_smbdir.FindFirstFile( sourcesearch, &wfd );

	if( INVALID_HANDLE_VALUE == hFind )
	{
	    return 0;
	}
	else
	{
	    // Display each file and ask for the next.
	    do
	    {
			// Only do files
			if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				temp_item.fullpath = string(path);
				temp_item.name = string(wfd.cFileName);
				temp_item.isDirectory = 1;
			}
			else
			{
				temp_item.fullpath = string(path);
				temp_item.name = string(wfd.cFileName);
				temp_item.isDirectory = 0;
			}
			items.push_back(temp_item);
	    }
	    while(p_smbdir.FindNextFile( hFind, &wfd ));

	    // Close the find handle.
	    p_smbdir.FindClose( hFind );
	}
	
	return 1;
}
