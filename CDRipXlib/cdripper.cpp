
#include "cdripper.h"


CCDRipper::CCDRipper()
{
	cdrip = new CDRip();
}

CCDRipper::~CCDRipper()
{
}

CDEX_ERR CCDRipper::Init()
{
	return CR_Init( "test.ini" );
}

CDEX_ERR CCDRipper::OpenRipper(	LONG* plBufferSize,LONG dwStartSector,LONG dwEndSector )
{
	return CR_OpenRipper(	plBufferSize, dwStartSector,dwEndSector );
}

CDEX_ERR CCDRipper::CloseRipper()
{
	return CR_CloseRipper();
}

CDEX_ERR CCDRipper::RipChunk( BYTE* pbtStream,LONG* pNumBytesRead )
{
	BOOL			bAbort=false;
	return CR_RipChunk(pbtStream,pNumBytesRead, bAbort );
}

CDEX_ERR CCDRipper::ReadToc()
{
	return CR_ReadToc();
}

LONG CCDRipper::GetNumTocEntries()
{
	return CR_GetNumTocEntries();
}

TOCENTRY CCDRipper::GetTocEntry(LONG nTocEntry)
{
	return CR_GetTocEntry(nTocEntry);
}

void CCDRipper::GetLastJitterErrorPosition(DWORD& dwStartSector,DWORD& dwEndSector)
{
	return CR_GetLastJitterErrorPosition(dwStartSector,dwEndSector);
}

LONG CCDRipper::GetPercentCompleted()
{
	return CR_GetPercentCompleted();
}

LONG CCDRipper::GetJitterPosition()
{
	return CR_GetJitterPosition();
}

LONG CCDRipper::GetPeakValue()
{
	return CR_GetPeakValue();
}

LONG CCDRipper::GetNumberOfJitterErrors()
{
	return CR_GetNumberOfJitterErrors();
}
