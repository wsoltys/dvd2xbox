


#ifndef CDRIPPER_INCLUDED
#define CDRIPPER_INCLUDED

#include "..\cdrip\cdrip.h"


class CCDRipper
{
protected:
public:
	CDEX_ERR		Init();
	CDEX_ERR		OpenRipper(	LONG* plBufferSize,LONG dwStartSector,LONG dwEndSector );
	CDEX_ERR		CloseRipper();
	CDEX_ERR		RipChunk( BYTE* pbtStream,LONG* pNumBytes );
	CDEX_ERR		ReadToc();
	LONG			GetNumTocEntries();
	TOCENTRY		GetTocEntry(LONG nTocEntry);
	void			GetLastJitterErrorPosition(DWORD& dwStartSector,DWORD& dwEndSector);
	LONG			GetPercentCompleted();
	LONG			GetJitterPosition();
	LONG			GetPeakValue();
	LONG			GetNumberOfJitterErrors();
	CDRip			*cdrip;
	CCDRipper();
	~CCDRipper();
};

#endif