#include "d2xlogger.h"
#include <stdstring.h>


char D2Xlogger::logFilename[1200] = "\0";
int D2Xlogger::writeLog = 0;

D2Xlogger::D2Xlogger()
{

}

D2Xlogger::~D2Xlogger()
{

}

void D2Xlogger::setLogFilename(char *file)
{
	strcpy(logFilename,file);
}


void D2Xlogger::enableLog(bool value)
{
	writeLog=value;
}

void D2Xlogger::WLog(WCHAR *message,...)
{
	//DPf_H("calling WriteLog %s",logFilename);
	if(logFilename == NULL)
		return;
	WCHAR expanded_message[1024];
	va_list tGlop;
	// Expand the varable argument list into the text buffer
	va_start(tGlop,message);
	if(vswprintf(expanded_message,message,tGlop)==-1)
	{
		// Fatal overflow, lets abort
		return;
	}
	va_end(tGlop);
	FILE *stream;
	char mchar[1024];
	if(!(stream = fopen(logFilename,"a+")))
		return;
	wsprintf(mchar,"%S\n",expanded_message);
	fwrite(mchar,sizeof(char),strlen(mchar),stream);
	fclose(stream);
}
