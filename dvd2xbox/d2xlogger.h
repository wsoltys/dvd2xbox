#ifndef D2XLOGGER
#define D2XLOGGER

#include <helper.h>


class D2Xlogger
{
protected:

	
	
public:
	D2Xlogger();
	~D2Xlogger();

	void setLogFilename(char *file);
	void enableLog(bool value);
	void WLog(WCHAR *message,...);
	static char				logFilename[1200];
	static int				writeLog;
	

};

#endif