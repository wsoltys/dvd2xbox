#include "D2Xfilefactory.h"
#include "filefactory/d2xfileudf.h"


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
	default:
		return (D2Xfile*)new D2XfileUDF();
		break;
	};
}
