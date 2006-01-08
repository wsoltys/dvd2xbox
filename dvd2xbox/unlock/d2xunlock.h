#ifndef D2XUNLOCK
#define D2XUNLOCK

#include <xtl.h>
#include <undocumented.h>
#include "..\d2xsettings.h"
#include "sha.h"
#include "rc4.h"

// XBOX 1 DVD-Drive unlocker tool v0.1 by The Specialist.


typedef struct _SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER {
	SCSI_PASS_THROUGH_DIRECT	Spt;
	ULONG						Filler;
	BYTE						SenseBuf[32];
} SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER, *PSCSI_PASS_THROUGH_DIRECT_WITH_BUFFER;

typedef struct _TRC4Context {
	BYTE	D[8];
	BYTE	I,J;
} TRC4Context;

class D2Xunlock 
{
protected:

	HANDLE	fd;
	TRC4Context	rc4key;
	BYTE		scsibuffer[2000];
	

public:
	D2Xunlock();
	~D2Xunlock();

	int Unlock();



};


#endif