#include "D2Xfilefat.h"

#include <MemoryUnitManager.h>


D2XfileFAT::D2XfileFAT()
{
	m_bOpened = false;
}

D2XfileFAT::~D2XfileFAT()
{
	if (m_bOpened)
	{
		FileClose();
	}
}


int D2XfileFAT::FileOpenWrite(char* filename, int mode, DWORD size)
{
	CStdString strFile = filename;
	CStdString strFName = "/";
	strFName += strFile.Mid(7);
	strFName.Replace("\\", "/");
	
	char unit = filename[3]-'0';
	if (!g_memoryUnitManager.OpenFile(unit, filename, true, &m_fileInfo))
    return 0;

	m_bOpened = true;
	return 1;
}

int D2XfileFAT::FileOpenRead(char* filename, int mode)
{
	CStdString strFName = "/";
	strFName += filename+7;
	strFName.Replace("\\", "/");
	
	char unit = filename[3]-'0';
	if (!g_memoryUnitManager.OpenFile(unit, strFName, false, &m_fileInfo))
    return 0;

	m_bOpened = true;
	return 1;
}

int D2XfileFAT::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return 0;
}


int D2XfileFAT::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	if (!m_bOpened) return 0;
	*dwRead = g_memoryUnitManager.ReadFile(&m_fileInfo, (unsigned char *)buffer, (long)dwToRead);
	return *dwRead;
}

int D2XfileFAT::FileClose()
{
	m_bOpened = false;
	return 1;
}

DWORD D2XfileFAT::GetFileSize(char* filename)
{
	if (!m_bOpened) return -1;
	return m_fileInfo.filelen;
}

int D2XfileFAT::GetDirectory(char* path, VECFILEITEMS *items)
{
	items->clear();
	CStdString strPath = path;
	strPath.Replace("\\", "/");

	char unit = path[3]-'0';

	if (g_memoryUnitManager.GetDirectory(unit, strPath.Mid(7), items))
		return 1;
	else
		return 0;
    
}

int D2XfileFAT::CreateDirectory(char* name)
{
	return 0;
}

__int64 D2XfileFAT::FileSeek(long offset, int origin)
{
	if (!m_bOpened) return -1;
	__int64 position = offset;
	if (origin == SEEK_CUR)
		position += m_fileInfo.pointer;
	else if (origin == SEEK_END)
		position += m_fileInfo.filelen;
	if (position < 0) position = 0;
	if (position > m_fileInfo.filelen) position = m_fileInfo.filelen;
	g_memoryUnitManager.SeekFile(&m_fileInfo, (unsigned int)position);
	return m_fileInfo.pointer;
}

int D2XfileFAT::DeleteFile(char* filename)
{
	return 0;
}

int D2XfileFAT::DeleteDirectory(char* filename)
{
	return 0;
}

int D2XfileFAT::MoveItem(char* source, char* dest)
{
	return 0;
}

int D2XfileFAT::GetType()
{
	return D2X_FAT;
}