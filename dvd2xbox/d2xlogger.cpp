#include "d2xlogger.h"
#include <stdstring.h>
#include <xtl.h>


char D2Xlogger::logFilename[1200] = "\0";
int D2Xlogger::writeLog = 0;
WCHAR* D2Xlogger::message_log[MLOG_BUFFER];
int D2Xlogger::msgNo = 0;

D2Xlogger::D2Xlogger()
{
}

D2Xlogger::~D2Xlogger()
{
}

void D2Xlogger::resetMsgLog()
{
	for(int loop=0;loop<MLOG_BUFFER;loop++)
	{
		if(message_log[loop])
		{
			delete message_log[loop];
       		message_log[loop]=NULL;
		} else {
			break;
		}
	}
	msgNo=0;
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
	if((writeLog == 0) || (logFilename == NULL))
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

	// First see if we have space otherwise a shift is in order
	if(msgNo>=MLOG_BUFFER)
	{
		// Delete 1st message as it will be lost
		delete message_log[0];
		// Shift all messages up one
		for(int loop=0;loop<MLOG_BUFFER-1;loop++)
		{
			message_log[loop]=message_log[loop+1];
		}
		// Fix at end point to be doubly sure
		msgNo=MLOG_BUFFER-1;
	}

	//Create & copy our new buffer
	message_log[msgNo]=new WCHAR[wcslen(expanded_message)+1];
	wcscpy(message_log[msgNo],expanded_message);

	// Move the message pointer on
	msgNo++;

	FILE *stream;
	char mchar[1024];
	if(!(stream = fopen(logFilename,"a+")))
		return;
	wsprintf(mchar,"%S\n",expanded_message);
	fwrite(mchar,sizeof(char),strlen(mchar),stream);
	fclose(stream);
	return;
}
