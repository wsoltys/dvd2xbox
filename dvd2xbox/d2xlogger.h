#ifndef D2XLOGGER
#define D2XLOGGER

#include <helper.h>

#define MLOG_BUFFER		20	


class D2Xlogger
{
protected:
	
	
public:
	D2Xlogger();
	~D2Xlogger();

	void setLogFilename(char *file);
	void enableLog(bool value);
	void WLog(WCHAR *message,...);
	void resetMsgLog();
	static char				logFilename[1200];
	static int				writeLog;
	static int				msgNo;
	static WCHAR*			message_log[MLOG_BUFFER];							
	

};

#endif