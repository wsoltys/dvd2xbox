#ifndef D2XACL
#define D2XACL

#include "d2xlogger.h"
#include "d2xutils.h"
#include <iosupport.h>


#define ACL_UNKNOWN			0
#define ACL_HEXREPLACE		1

class D2Xacl
{
protected:

	D2Xlogger*		p_log;
	D2Xutils*		p_util;
	char*			m_destination;	
	char*			m_pattern[64];
	char*			m_currentpattern;
	char			m_currentmask[1024];
	ULONG			m_titleID;
	CIoSupport		p_IO;
	int				m_acltype;

	void			reset();
	void			resetPattern(int c);
	bool			HexReplace(char* pattern);
	bool			processSection(char* pattern);
	bool			processFiles(char *path);

	
	
public:
	D2Xacl();
	~D2Xacl();

	bool processACL(char* dest);
	
};

#endif