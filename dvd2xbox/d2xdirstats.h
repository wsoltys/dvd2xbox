#ifndef D2XDIRSTATS
#define D2XDIRSTATS

#include <map>
#include <xtl.h>
#include <string>
#include "d2xsettings.h"

using namespace std;

#define FAILED_OPEN		1000
#define FAILED_WRITE	1010
#define OK_WRITE		2000
#define OK				2200

class D2Xdirstats
{
protected:

	map<string,string> current_stats;
	map<string,string> saved_stats;
	map<string,string> new_stats;
	typedef map<string,string>::iterator stats_it;

	int save_stats(map<string,string> stats);
	int load_stats(map<string,string> stats);

public:
	D2Xdirstats();
	~D2Xdirstats();


};

#endif