// Dir.h: interface for the CDir class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISODIR_H__07F2AD1C_3B4F_48DC_80CC_C9132F424B11__INCLUDED_)
#define AFX_ISODIR_H__07F2AD1C_3B4F_48DC_80CC_C9132F424B11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <xtl.h>
#include <vector>
#include <map>
using namespace std;

//namespace XISO9660
//{

class CIsoFile
{
public:
	CIsoFile();
	CIsoFile(int iDirectory, const char* szFileName, DWORD dwLength, DWORD dwStartSector);
	virtual ~CIsoFile();
	void CIsoFile::Dump(int iIdent);

	int		m_iDirectory;
	char	m_szFileName[256];
	DWORD m_dwLength;
	DWORD m_dwStartSector;
};


class CIsoDir  
{
public:
	CIsoDir(int iDir, int iParentDir, const char* szDirName);
	CIsoDir();
	virtual ~CIsoDir();
	bool AddDir(int iDirectory, int iParentDir, const char* szDirName);
	bool AddFile(int iDirectory, const char* szFileName, DWORD dwLength, DWORD dwStartSector);
	bool DirExists(int iDirectory);
	void Dump(int iIdent=1);

	
	bool GetFileInfo(const char* szFileName, DWORD& dwSector, DWORD& dwFileSize);	
	bool GetFileInfo2(const char *szFileName, DWORD &dwSector, DWORD &dwFileSize);
	HANDLE	FindFirstFile(char* lpFileName,  LPWIN32_FIND_DATA lpFindFileData);
	BOOL		FindNextFile(HANDLE hFindFile,  LPWIN32_FIND_DATA lpFindFileData);
private:
	int		strnocasecmp(const char* str1,const char* str2);

	bool  GetNext( LPWIN32_FIND_DATA lpFindFileData);
	int		m_iDirectory;
	int		m_iParentDirectory;
	int		m_iSector;
	char	m_szDirName[256];


	map<int, CIsoDir*> m_vecDirs;	
	typedef map<int, CIsoDir*>::iterator  ivecDirs;

	vector<CIsoFile> m_vecFiles;
	typedef vector<CIsoFile>::iterator ivecFiles;
};
//};
#endif // !defined(AFX_ISODIR_H__07F2AD1C_3B4F_48DC_80CC_C9132F424B11__INCLUDED_)
