#include "d2xlogger.h"
#include "d2xutils.h"
#include <stdstring.h>


char D2Xlogger::logFilename[1200] = "\0";
char D2Xlogger::logPath[1024] = "\0";
bool D2Xlogger::writeLog = false;
HANDLE D2Xlogger::hFile;

D2Xlogger::D2Xlogger()
{
	p_ff = NULL;
}

D2Xlogger::~D2Xlogger()
{
}


void D2Xlogger::setLogFilename(char *file)
{
	strcpy(logFilename,file);
}

void D2Xlogger::setLogPath(char *file)
{
	strcpy(logPath,file);
	D2Xutils p_util;
	p_util.addSlash(logPath);
}

void D2Xlogger::setLogFile(char *file)
{
	sprintf(logFilename,"%s%s",logPath,file);
	if((writeLog == true) && (logFilename != NULL))
	{
		//CloseHandle(hFile);
		hFile = CreateFile( logFilename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL );
		//D2Xff factory;
		//p_ff = factory.Create(UDF);
		if (hFile==NULL)
		//if(p_ff->FileOpenWrite(logFilename) == 0)
		{
			writeLog = false;
			return;
		}
	} else
	{
		CloseHandle(hFile);
		//p_ff->FileClose();
	}
}


void D2Xlogger::enableLog(bool value)
{
	writeLog=value;	
	if(value == false)
	{
		CloseHandle(hFile);
		/*p_ff->FileClose();
		delete p_ff;
		p_ff=NULL;*/
	}
}

void D2Xlogger::WLog(WCHAR *message,...)
{
	
	//DPf_H("calling WriteLog %s",logFilename);
	if(writeLog == false)
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
	char mchar[1024];
	DWORD dwWrote;
	wsprintf(mchar,"%S\r\n",expanded_message);

	WriteFile(hFile,mchar,strlen(mchar),&dwWrote,NULL);
	//p_ff->FileWrite(mchar,strlen(mchar),&dwWrote);

	return;
}
