#ifndef D2XAUDIORIPPER
#define D2XAUDIORIPPER



// set to 1 if you wanna use the encoder
#define _D2XENCODER	1

#include <xtl.h>
#include <stdio.h>
#include "..\lib\libcdrip\cdrip.h"
#include "d2xaudioencoder.h"
//#include "thread.h"

//using namespace std;

#define BUFFER0	11
#define	BUFFER1	22

#define CD_ERROR	33
#define CD_OK		44
#define CD_DONE		55
#define CD_EMPTY	66

struct cdtoc {
	int	min;
	int	sec;
	int	frame;
};

#if _D2XENCODER
class D2Xcdrip : public D2Xaenc
#else
class D2Xcdrip
#endif
{
protected:
	int status;
	int fillbuffer;
	LONG nBufferSize;
	BYTE* pbtStream;

	/*struct	BUFFERSTRUCT
	{
		BYTE* buffer;
		LONG  numBytes;
		int   status;
	};

    BUFFERSTRUCT	p_buf[2];*/
	


public:
	D2Xcdrip();
	~D2Xcdrip();
	int		Init(int track);
	int		RipChunk();
	int		DeInit();
	int		GetPercentCompleted();
	int		GetNumTocEntries();
	int		GetTrackInfo();

	cdtoc	oCDCon[100];
	cdtoc	CDCon[100];

	/*virtual void		OnStartup();
	virtual void		OnExit();
	virtual void		Process();*/

};

#endif