#ifndef D2XFILECOPY
#define D2XFILECOPY

#include <helper.h>
#include <undocumented.h>
#include "d2xdbrowser.h"
#include "IOSupport.h"
#include "iso9660.h"
#include "thread.h"
#include "..\cdripxlib\cdripxlib.h"
#include "d2xtitle.h"
#include "d2xpatcher.h"
#include "d2xsettings.h"
#include <dvd_reader.h>
#include <stdstring.h>
#include <FileSMB.h>


#define uint64_t   unsigned __int64

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
	);

class D2Xfilecopy : public CThread
{
protected:

	HelperX*				p_help;
	CCDRipX*				p_cdripx;
	D2Xtitle*				p_title;
	D2Xlogger*				p_log;
	D2Xsettings*			p_set;
	HDDBROWSEINFO			fsource;
	int						ftype;
	char					fdest[1024];
	dvd_reader_t*			dvd;

	static char*			excludeFiles;
	static char*			excludeDirs;


	int FileUDF(HDDBROWSEINFO source,char* dest);
	int DirUDF(char *path,char *destroot);
	int FileISO(HDDBROWSEINFO source,char* dest);
	bool CopyISOFile(char* lpcszFile,char* destfile);
	bool DirISO(char *path,char *destroot);
	int FileCDDA(HDDBROWSEINFO source,char* dest);
	int DirCDDA(char* dest);
	int CopyCDDATrack(HDDBROWSEINFO source,char* dest);
	//int FileVOB(HDDBROWSEINFO source,char* dest);
	int CopyVOB(char* sourcefile,char* destfile);
	int FileUDF2SMB(HDDBROWSEINFO source,char* dest);
	bool CopyUDF2SMBFile(char* lpcszFile,char* destfile);
	bool DirUDF2SMB(char *path,char *destroot);

	bool excludeFile(char* string);
	bool excludeDir(char* string);
	
	
public:

	static LONGLONG			llValue;
	static int				i_process;
	static bool				b_finished;
	static WCHAR			c_source[1024];
	static WCHAR			c_dest[1024];
	static float			f_ogg_quality;
	static int				copy_failed;
	static int				copy_ok;
	static int				copy_renamed;

	// smb
	//static char				smbUsername[128];
	//static char				smbPassword[20];
	//static char				smbHostname[128];

	
	D2Xfilecopy();
	~D2Xfilecopy();
	virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();
	void FileCopy(HDDBROWSEINFO source,char* dest,int type);
	//void CancleThread();
	int GetProgress();
	int GetMBytes();

	static void setExcludePatterns(const char* files,const char* dirs);
	
};

#endif