#include "D2Xfilefactory.h"
#include "filefactory/d2xfileudf.h"
#include "filefactory/d2xfileiso.h"
#include "filefactory/d2xfiledvd.h"
#include "filefactory/d2xfilesmb.h"


D2Xff::D2Xff()
{
	
}


D2Xff::~D2Xff()
{	

}

D2Xfile* D2Xff::Create(int mode)
{
	switch(mode)
	{
	case GAME:
	case UDF:
		return (D2Xfile*)new D2XfileUDF();
		break;
	case ISO:
		return (D2Xfile*)new D2XfileISO();
		break;
	case DVD:
		return (D2Xfile*)new D2XfileDVD();
		break;
	case D2X_SMB:
		return (D2Xfile*)new D2XfileSMB();
		break;
	default:
		return (D2Xfile*)new D2XfileUDF();
		break;
	};
}
