#ifndef D2XACL
#define D2XACL

#include "d2xlogger.h"
#include "d2xutils.h"
#include <iosupport.h>


#define ACL_UNKNOWN			0
#define ACL_HEXREPLACE		1
#define ACL_COPYFILES		2
#define ACL_DELFILES		3
#define ACL_SETMEDIA		3

#define ACL_PATTERNS		  6
#define ACL_PATTERN_LENGTH 1024

class D2Xacl
{
protected:

	D2Xlogger*		p_log;
	D2Xutils*		p_util;
	char*			m_destination;	
	char			m_pattern[ACL_PATTERNS][ACL_PATTERN_LENGTH];
	char			m_currentmask[1024];
	ULONG			m_titleID;
	CIoSupport		p_IO;
	int				m_acltype;

	void			reset();
	void			resetPattern();
	void			FillVars(char* pattern);
	void			HexReplace(char* pattern);
	bool			processSection(char* pattern);
	bool			processFiles(char *path);

	
	
public:
	D2Xacl();
	~D2Xacl();

	bool processACL(char* dest);
	
};

#endif