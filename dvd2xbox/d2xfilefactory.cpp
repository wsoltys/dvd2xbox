#include "D2Xfilefactory.h"
#include "filefactory/d2xfileudf.h"
#include "filefactory/d2xfileiso.h"
#include "filefactory/d2xfiledvd.h"
#include "filefactory/d2xfilesmb.h"
#include "filefactory/d2xfileftp.h"
//#include "filefactory/d2xfilerar.h"


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
	case VCD:
	case SVCD:
		return (D2Xfile*)new D2XfileISO();
		break;
	case DVD:
		return (D2Xfile*)new D2XfileDVD();
		break;
	case D2X_SMB:
		return (D2Xfile*)new D2XfileSMB();
		break;
	case FTP:
		return (D2Xfile*)new D2XfileFTP();
		break;
	/*case RAR:
		return (D2Xfile*)new D2XfileRAR();
		break;*/
	default:
		return (D2Xfile*)new D2XfileUDF();
		break;
	};
}

D2Xfile* D2Xff::Create(char* path)
{
	if(!_strnicmp(path,"smb:",4))
		return (D2Xfile*)new D2XfileSMB();
	else if(!_strnicmp(path,"ftp:",4))
		return (D2Xfile*)new D2XfileFTP();
	else if(!_strnicmp(path,"d:",2))
	{
		switch(g_d2xSettings.detected_media)
		{
			case GAME:
			case UDF:
				return (D2Xfile*)new D2XfileUDF();
				break;
			case ISO:
			case VCD:
			case SVCD:
				return (D2Xfile*)new D2XfileISO();
				break;
			case DVD:
				return (D2Xfile*)new D2XfileDVD();
				break;
			case D2X_SMB:
				return (D2Xfile*)new D2XfileSMB();
				break;
			case FTP:
				return (D2Xfile*)new D2XfileFTP();
				break;
			/*case RAR:
				return (D2Xfile*)new D2XfileRAR();
				break;*/
			default:
				return (D2Xfile*)new D2XfileUDF();
				break;
		};
	}
	else
		return (D2Xfile*)new D2XfileUDF();
}
