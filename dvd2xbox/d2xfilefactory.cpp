#include "D2Xfilefactory.h"
#include "filefactory/d2xfileudf.h"
#include "filefactory/d2xfileiso.h"


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
	default:
		return (D2Xfile*)new D2XfileUDF();
		break;
	};
}
