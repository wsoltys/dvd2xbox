// Dir.cpp: implementation of the CDir class.
//
//////////////////////////////////////////////////////////////////////
#pragma code_seg( "ISO9660" )
#pragma data_seg( "ISO9660_RW" )
#pragma bss_seg( "ISO9660_RW" )
#pragma const_seg( "ISO9660_RD" )
#pragma comment(linker, "/merge:ISO9660_RW=ISO9660")
#pragma comment(linker, "/merge:ISO9660_RD=ISO9660")

//#include "stdafx.h"
#include "IsoDir.h"
//using namespace XISO9660;
#pragma warning (disable:4018)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIsoFile::CIsoFile()
{
}
CIsoFile::~CIsoFile()
{
}

CIsoFile::CIsoFile(int iDirectory, const char* szFileName, DWORD dwLength, DWORD dwStartSector)
{
	strcpy(m_szFileName,szFileName);
	m_iDirectory		= iDirectory;
	m_dwLength			= dwLength;
	m_dwStartSector	= dwStartSector;
}

void CIsoFile::Dump(int iIdent)
{
	for (int i=0; i < iIdent; i++) printf(" ");
	printf("%s\n", m_szFileName);
}

CIsoDir::CIsoDir()
{
	strcpy(m_szDirName,"");
	m_iDirectory=-1;
	m_iParentDirectory=-1;
}

CIsoDir::~CIsoDir()
{
	ivecDirs iDir;
	iDir=m_vecDirs.begin(); 
	while (iDir != m_vecDirs.end())
	{	
		CIsoDir* dir=iDir->second;
		delete dir;
		iDir=m_vecDirs.erase(iDir);
	}
	m_vecFiles.erase( m_vecFiles.begin(), m_vecFiles.end() );
}

CIsoDir::CIsoDir(int iDir, int iParentDir, const char* szDirName)
{
	m_iDirectory=iDir;
	m_iParentDirectory=iParentDir;
	strcpy(m_szDirName,szDirName);
}



bool CIsoDir::AddFile(int iDirectory, const char *szFileName, DWORD dwLength, DWORD dwStartSector)
{
	if (m_iDirectory==iDirectory||iDirectory==1)
	{		
		CIsoFile file(iDirectory, szFileName, dwLength, dwStartSector);
		m_vecFiles.push_back(file);
		return true;
	}
	ivecDirs iDir;
	for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
	{	
		CIsoDir* dir=iDir->second;
		if (dir->AddFile(iDirectory, szFileName, dwLength, dwStartSector)) return true;
	}
	return false;

}

bool CIsoDir::AddDir(int iDirectory, int iParentDir, const char* szDirName)
{
	ivecDirs iDir;
	if (m_iDirectory==iParentDir||iParentDir==1)
	{
		iDir=m_vecDirs.find(iDirectory);
		if (iDir != m_vecDirs.end()) return true;

		CIsoDir* newdir=new CIsoDir(iDirectory, iParentDir, szDirName);	
		m_vecDirs[iDirectory] = newdir;
		return true;
	}
	for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
	{	
		CIsoDir* dir=iDir->second;
		if (dir->AddDir(iDirectory, iParentDir, szDirName)) return true;
	}
	


	return false;
}

void CIsoDir::Dump(int iIdent)
{
	
	for (int x=0; x < iIdent; x++) printf(" ");
	printf("[%s]\n", m_szDirName);
	ivecFiles iFile;
	for (iFile=m_vecFiles.begin(); iFile != m_vecFiles.end(); ++iFile)
	{	
		CIsoFile& file=*iFile;
		file.Dump(iIdent+2);
	}

	ivecDirs i;
	for (i=m_vecDirs.begin(); i != m_vecDirs.end(); ++i)
	{	
		CIsoDir* dir=i->second;
		dir->Dump(iIdent+2);
	}
	
}

bool CIsoDir::DirExists(int iDirectory)
{
	if (m_iDirectory==iDirectory) return true;
	ivecDirs iDir;
	for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
	{	
		CIsoDir* dir=iDir->second;
		if (dir->DirExists(iDirectory)) return true;
	}
	return false;
}

bool  CIsoDir::GetNext( LPWIN32_FIND_DATA lpFindFileData)
{
	if (lpFindFileData->dwReserved0 < m_vecDirs.size())
	{
		int iPos=0;
		ivecDirs iDir;
		for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
		{	
			CIsoDir* dir=iDir->second;
			if (iPos == lpFindFileData->dwReserved0)
			{
				lpFindFileData->dwReserved0++;
				strcpy(lpFindFileData->cFileName, dir->m_szDirName);
				lpFindFileData->nFileSizeLow=0;
				lpFindFileData->nFileSizeHigh=0;
				lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_READONLY;
				return true;
			}
			iPos++;
		}
	}
	if (lpFindFileData->dwReserved1 < m_vecFiles.size())
	{
		int iPos=0;
		ivecFiles iFile;
		for (iFile=m_vecFiles.begin(); iFile != m_vecFiles.end(); ++iFile)
		{	
			CIsoFile& file=*iFile;
			if (iPos == lpFindFileData->dwReserved1)
			{
				lpFindFileData->dwReserved1++;
				strcpy(lpFindFileData->cFileName, file.m_szFileName);
				lpFindFileData->nFileSizeLow= file.m_dwLength;
				lpFindFileData->nFileSizeHigh=0;
				lpFindFileData->dwFileAttributes=FILE_ATTRIBUTE_READONLY;
				return true;
			}
			iPos++;
		}
	}
	return false;
}

HANDLE CIsoDir::FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData)
{
	char szDir[1024];
	strcpy(szDir,lpFileName);
	char* pNextSubDir=NULL;
	char* pPath=strstr(szDir,"/");
	if (!pPath)
	{
		pPath=strstr(szDir,"\\");
	}
	if (pPath)
	{
		*pPath=0;
		pNextSubDir=++pPath;
	}
	if ( strlen(szDir)==0 || strnocasecmp(szDir,m_szDirName)==0)
	{
		// return all dirs & files in this directory;
		if (pNextSubDir && strlen(pNextSubDir))
		{
			ivecDirs iDir;
			for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
			{	
				CIsoDir* dir=iDir->second;
				HANDLE hHandle=dir->FindFirstFile( pNextSubDir,lpFindFileData);
				if (hHandle!= INVALID_HANDLE_VALUE) return hHandle;
			}
		}
		memset(lpFindFileData,0,sizeof(WIN32_FIND_DATA));
		lpFindFileData->dwReserved0=0;
		lpFindFileData->dwReserved1=0;
		if ( GetNext(lpFindFileData)) return (HANDLE)this;
	}
	else 
	{
		ivecDirs iDir;
		for (iDir=m_vecDirs.begin(); iDir != m_vecDirs.end(); ++iDir)
		{	
			CIsoDir* dir=iDir->second;
			HANDLE hHandle=dir->FindFirstFile( lpFileName,lpFindFileData);
			if (hHandle!= INVALID_HANDLE_VALUE) return hHandle;
		}
	}
	return (HANDLE)INVALID_HANDLE_VALUE;
}

BOOL CIsoDir::FindNextFile(HANDLE hFindFile,  LPWIN32_FIND_DATA lpFindFileData)
{
	CIsoDir* pDir = (CIsoDir*)hFindFile;
	if ( pDir->GetNext(lpFindFileData)) 
	{
		return true;
	}
	return false;
}

bool CIsoDir::GetFileInfo(const char *szFileName, DWORD &dwSector, DWORD &dwFileSize)
{
	char szDir[1024];
	char szFile[1024];
	int i=strlen(szFileName)-1;
	while (i > 0)
	{
		if (szFileName[i] == '/' || szFileName[i]=='\\')
		{
			strcpy(szDir, szFileName);
			szDir[i]=0;
			strcpy(szFile, &szFileName[i+1]);

			WIN32_FIND_DATA wf;
			HANDLE hDir=FindFirstFile(szDir,  &wf);
			if (hDir==INVALID_HANDLE_VALUE) return false;
			CIsoDir* pDir = (CIsoDir*)hDir;
			return pDir->GetFileInfo2(szFile, dwSector, dwFileSize);
		}
		i--;
	}

	return GetFileInfo2(szFileName, dwSector, dwFileSize);
}


bool CIsoDir::GetFileInfo2(const char *szFileName, DWORD &dwSector, DWORD &dwFileSize)
{
	ivecFiles iFile;
	for (iFile=m_vecFiles.begin(); iFile != m_vecFiles.end(); ++iFile)
	{	
		CIsoFile& file=*iFile;
		if (strnocasecmp(file.m_szFileName,szFileName)==0)
		{
			dwSector=file.m_dwStartSector;
			dwFileSize=file.m_dwLength;
			return true;
		}
	}
	return false;
}

int CIsoDir::strnocasecmp(const char* str1,const char* str2)
{
	if ( strlen(str1) != strlen(str2) ) return 1;
	for (int i=0; i < strlen(str1);i++ )
	{
		if (tolower(str1[i]) != tolower(str2[i]) ) return 1;
	}
	return 0;
}
