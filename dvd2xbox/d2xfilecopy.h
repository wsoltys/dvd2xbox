#ifndef D2XFILECOPY
#define D2XFILECOPY

#include <helper.h>
#include <undocumented.h>
//#include "d2xdbrowser.h"
#include "IOSupport.h"
//#include "iso9660.h"
#include "thread.h"
#include "d2xtitle.h"
#include "d2xpatcher.h"
#include "d2xsettings.h"
#include "d2xacl.h"
#include "d2xutils.h"
//#include "..\lib\libdvdread\dvd_reader.h"
#include <string.h>
#include <FileSMB.h>
//#include "d2xftp.h"
#include "d2xcddaripper.h"
#include "d2xfilefactory.h"


#define uint64_t   unsigned __int64

#define GENERIC_BUFFER_SIZE	32768

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

	D2Xlogger				p_log;
	D2Xutils				p_utils;
	D2Xfile*				p_source;
	D2Xfile*				p_dest;
	HDDBROWSEINFO			fsource;
	int						ftype;
	int						source_type;
	int						dest_type;
	int						fail_type;
	char					fdest[1024];

	// generic pre_init
	BYTE		gBuffer[GENERIC_BUFFER_SIZE];
	uint64_t	gFileSize;
	uint64_t	gFileOffset;
	bool		gbResult;
	uint64_t	gnOldPercentage;
	uint64_t	gnNewPercentage;
	DWORD		glRead;
	DWORD		gdwWrote;
	// end generic pre_init 
	

	void	CopyFailedGeneric();
	int		FileUDF(HDDBROWSEINFO source,char* dest); 
	int		DirUDF(char *path,char *destroot);
	bool	CopyUDFFile(char* lpcszFile,char* destfile);
	int		FileDVD(HDDBROWSEINFO source,char* dest);
	int		DirDVD(char *path,char *destroot);
	int		FileISO(HDDBROWSEINFO source,char* dest);
	bool	CopyISOFile(char* lpcszFile,char* destfile);
	bool	DirISO(char *path,char *destroot);
	int		FileCDDA(HDDBROWSEINFO source,char* dest);
	int		DirCDDA(char* dest);
	int		CopyCDDATrack(HDDBROWSEINFO source,char* dest);
	int		CopyCDDATrackLame2(HDDBROWSEINFO source,char* dest);
	int		CopyVOB(char* sourcefile,char* destfile);
	int		FileUDF2SMB(HDDBROWSEINFO source,char* dest);
	bool	CopyUDF2SMBFile(char* lpcszFile,char* destfile);
	bool	DirUDF2SMB(char *path,char *destroot);
	int		CopyVOB2SMB(char* sourcefile,char* destfile);
	int		FileISO2SMB(HDDBROWSEINFO source,char* dest);
	bool	CopyISO2SMBFile(char* lpcszFile,char* destfile);
	bool	DirISO2SMB(char *path,char *destroot);
	int		FileUDF2FTP(HDDBROWSEINFO source,char* dest);
	bool	CopyUDF2FTPFile(char* lpcszFile,char* destfile);
	bool	DirUDF2FTP(char *path,char *destroot);
	int		FileFTP2UDF(HDDBROWSEINFO source,char* dest);
	bool	CopyFTP2UDFFile(char* lpcszFile,char* destfile);
	bool	DirFTP2UDF(char *path,char *destroot);

	bool	CopyFileGeneric(char* source, char* dest);
	int		CopyDirectoryGeneric(char* source, char* dest);
	int		CopyGeneric(HDDBROWSEINFO source, char* dest, int sourcetype, int desttype);

	//bool excludeFile(char* string);
	//bool excludeDir(char* string);
	
	
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
	static vector<string> excludeList;
	typedef vector <string>::iterator iexcludeList;
	static vector<string> XBElist;
	static map<string,string> RENlist;
	static map<string,string> FAILlist;

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
	void CopyFailed(int type);
	//void CancleThread();
	int GetProgress();
	int GetMBytes();

	static void setExcludePatterns(const char* files,const char* dirs);
	
};

#endif