#ifndef D2XLOGGER
#define D2XLOGGER

#include <xtl.h>

#define MLOG_BUFFER		20	


class D2Xlogger
{
protected:
	
	
public:
	D2Xlogger();
	~D2Xlogger();

	void setLogFilename(char *file);
	void setLogPath(char *file);
	void setLogFile(char *file);
	void enableLog(bool value);
	void WLog(WCHAR *message,...);

	static char				logFilename[1200];
	static int				writeLog;
							
	

};

#endif