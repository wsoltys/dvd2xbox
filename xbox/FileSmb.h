// FileSmb.h: interface for the CFileSMB class.

//

//////////////////////////////////////////////////////////////////////



#if !defined(AFX_FILESMB_H__2C4AB5BC_0742_458D_95EA_E9C77BA5663D__INCLUDED_)

#define AFX_FILESMB_H__2C4AB5BC_0742_458D_95EA_E9C77BA5663D__INCLUDED_


#if _MSC_VER > 1000

#pragma once

#endif // _MSC_VER > 1000

#include "IFile.h"
#include "../lib/libsmb/xbLibSmb.h"

class CSMB
{
public:
	CSMB();
	~CSMB();
	void Init();
	void Lock();
	void Unlock();
private:
	bool binitialized;
	CRITICAL_SECTION	m_critSection;
};

extern CSMB smb;

//using namespace XFILE;

//namespace XFILE
//{
	class CFileSMB : public IFile  
	{
	public:
		CFileSMB();
		//CFileSMB(char* ip,char* netm,char* names);
		virtual ~CFileSMB();
		virtual void			Close();
		virtual __int64			Seek(__int64 iFilePosition, int iWhence=SEEK_SET);
		virtual bool			ReadString(char *szLine, int iLineLength);
		virtual unsigned int	Read(void* lpBuf, __int64 uiBufSize);
		//virtual bool			Open(const char* strUserName, const char* strPassword,const char* strHostName, const char* strFileName,int iport, bool bBinary=true);
		virtual bool			Open(const char* strFileName);
		//virtual bool			Create(const char* strUserName, const char* strPassword,const char* strHostName, const char* strFileName,int iport, bool bBinary=true);
		virtual bool			Create(const char* strFileName);
		//virtual int				CreateDirectory(const char* strUserName, const char* strPassword,const char *strHostName, const char *strDirName,int iport, bool bBinary=true);
		virtual int				CreateDirectory(const char *strDirName);
		virtual __int64			GetLength();
		virtual __int64			GetPosition();
		int						Write(const void* lpBuf, __int64 uiBufSize);
		virtual int				Delete(const char* strFileName);


	protected:
		__int64	m_fileSize;
		bool			m_bBinary;
		int				m_fd;
	};
//};

#endif // !defined(AFX_FILESMB_H__2C4AB5BC_0742_458D_95EA_E9C77BA5663D__INCLUDED_)
