#ifndef D2XFILEFACTORY
#define D2XFILEFACTORY



#include "d2xsettings.h"
#include "filefactory/d2xfile.h"

class D2Xff
{
protected:
	
	

public:
	D2Xff();
	virtual ~D2Xff();
	
	D2Xfile*	Create(int mode);

};

#endif