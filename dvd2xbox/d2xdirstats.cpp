#include "D2Xdirstats.h"


D2Xdirstats::D2Xdirstats()
{

}

D2Xdirstats::~D2Xdirstats()
{

}


int D2Xdirstats::save_stats(map<string,string> stats)
{
	HANDLE	hFile;
	DWORD dwWrote;
	string line;

	if(stats.empty())
		return OK;
	hFile = CreateFile( g_d2xSettings.disk_statsPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
	if (hFile==NULL)
		return FAILED_OPEN;
	
	stats_it it;
	it = stats.begin();
	while (it != stats.end() )
	{
		line = string(it->first) + string("::") + string(it->second) + string("\r\n");
		if(WriteFile(hFile,line.c_str(),strlen(line.c_str()),&dwWrote,NULL) == 0)
		{
			CloseHandle(hFile);
			return FAILED_WRITE;
		}
		it++;
	}
	CloseHandle(hFile);
	return OK_WRITE;
}

int D2Xdirstats::load_stats(map<string,string> stats)
{
	FILE*	hFile;
	char buffer[1024];
	char name[1024];
	char size[20];
	memset(buffer,'0',1024);
	hFile = fopen(g_d2xSettings.disk_statsPath,"r");
	if (hFile==NULL)
		return FAILED_OPEN;
	while((fgets(buffer,1024,hFile) != NULL))
	{
		sscanf(buffer,"%[^::]::%[^::]",name,size);
		stats.insert(pair<string,string>(string(name),string(size)));
		memset(buffer,'0',1024);
	}
	fclose(hFile);
	return OK;
}