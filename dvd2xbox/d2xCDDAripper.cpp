#include "D2XCDDAripper.h"


D2Xcdrip::D2Xcdrip()
{
	status = 0;
	fillbuffer = BUFFER0;
	pbtStream = NULL;
}

D2Xcdrip::~D2Xcdrip()
{	
	DeInit();
	/*if(pbtStream != NULL)
	{
		delete[] pbtStream;
		pbtStream = NULL;
	}*/
}

int D2Xcdrip::Init(int track)
{
	nBufferSize = 0;
	char debug[1024];

	if ( CDEX_OK != CR_Init( "test.ini" ) ) {
		OutputDebugString("CR_Init failed");
		return 0;
	}

	CDROMPARAMS m_cdParams; 
	// get and set cdrom params
	CR_GetCDROMParameters(&m_cdParams);

	//read around 128k per chunk instead of 0x1a which is the default. This makes the cd reading less noisy.
	m_cdParams.nNumReadSectors = 0x37; 
	CR_SetCDROMParameters(&m_cdParams);


	CR_ReadToc();
	if (CR_OpenRipper(&nBufferSize, CR_GetTocEntry(track-1).dwStartSector, CR_GetTocEntry(track).dwStartSector-1)!=CDEX_OK) {
		OutputDebugString("OpenRipper failed");
		return 0;
	}
	sprintf(debug,"Track %d,Sectors %d",CR_GetTocEntry(track-1).btTrackNumber,CR_GetTocEntry(track).dwStartSector-1-CR_GetTocEntry(track-1).dwStartSector);
	OutputDebugString(debug);

	pbtStream = new BYTE[nBufferSize];
	if(pbtStream == NULL)
	{
		DeInit();
		return 0;
	}

	/*p_buf[0].buffer = new BYTE[nBufferSize];
	p_buf[0].numBytes = 0;
	p_buf[0].status = CD_EMPTY;
	if(p_buf[0].buffer == NULL)
	{
		OutputDebugString("Failed to allocate buffer 0");
		DeInit();
		return 0;
	}

	p_buf[1].buffer = new BYTE[nBufferSize];
	p_buf[1].numBytes = 0;
	p_buf[1].status = CD_EMPTY;
	if(p_buf[1].buffer == NULL)
	{
		OutputDebugString("Failed to allocate buffer 1");
		DeInit();
		return 0;
	}*/
	status = 1;
	return 1;
}

int D2Xcdrip::DeInit()
{
	CR_CloseRipper();

	if ( CDEX_OK != CR_DeInit() ) {
		OutputDebugString("CR_DeInit failed");
		//return 0;
	}
	if(pbtStream != NULL)
	{
		delete[] pbtStream;
		pbtStream = NULL;
	}
	/*if(p_buf[0].buffer != NULL)
	{
		delete[] p_buf[0].buffer;
		p_buf[0].buffer = NULL;
	}
	if(p_buf[1].buffer != NULL)
	{
		delete[] p_buf[1].buffer;
		p_buf[1].buffer = NULL;
	}*/
	status = 0;
	return 1;
}

int	D2Xcdrip::RipChunk()
{
	int ret = CD_ERROR;
	LONG nNumBytesRead = 0;
	if(!status)
		return ret;
	
	BOOL bAbort=false;
	CDEX_ERR ripErr = CR_RipChunk(pbtStream,&nNumBytesRead, bAbort );
	if ( CDEX_ERROR == ripErr )
		return CD_ERROR;
	else if ( CDEX_RIPPING_DONE == ripErr )
		ret = CD_DONE;
	else
		ret = CD_OK;

#if _D2XENCODER
	if(!EncodeChunk(nNumBytesRead,pbtStream))
		return CD_ERROR;
#endif
	//if(fillbuffer == BUFFER0)
	//{
	//	while(p_buf[0].status == CD_EMPTY)
	//		Sleep(50);
	//	//pbtStream = p_buf[0].buffer;
	//	memcpy(pbtStream,p_buf[0].buffer,nBufferSize);
	//	nNumBytesRead = p_buf[0].numBytes;
	//	fillbuffer = BUFFER1;
	//	ret = p_buf[0].status;
	//	p_buf[0].status = CD_EMPTY;
	//}
	//else if(fillbuffer == BUFFER1)
	//{
	//	while(p_buf[1].status == CD_EMPTY)
	//		Sleep(50);
	//	//pbtStream = p_buf[1].buffer;
	//	memcpy(pbtStream,p_buf[1].buffer,nBufferSize);
	//	nNumBytesRead = p_buf[1].numBytes;
	//	fillbuffer = BUFFER0;
	//	ret = p_buf[1].status;
	//	p_buf[1].status = CD_EMPTY;
	//}
	
	return ret;
}

int D2Xcdrip::GetPercentCompleted()
{
	if(!status)
		return 0;
	return CR_GetPercentCompleted();
}

int D2Xcdrip::GetNumTocEntries()
{
	if ( CDEX_OK != CR_Init( "test.ini" ) ) {
		OutputDebugString("CR_Init failed");
		return 0;
	}
	CR_ReadToc();
	int tracks = CR_GetNumTocEntries();
	if ( CDEX_OK != CR_DeInit() ) {
		OutputDebugString("CR_DeInit failed");
		return 0;
	}
	return tracks;
}

int D2Xcdrip::GetTrackInfo()
{
	if ( CDEX_OK != CR_Init( "test.ini" ) ) {
		OutputDebugString("CR_Init failed");
		return 0;
	}
	LONG ntFra =0;
	FLOAT tdur;
	CR_ReadToc();
	int m_nNumTracks =CR_GetNumTocEntries();
	if(m_nNumTracks<1)
		return 0;
	for (int i = 0; i < m_nNumTracks; i++) 
	{
		TOCENTRY TocEntry = CR_GetTocEntry(i);
		TOCENTRY TocEntry2 = CR_GetTocEntry(i+1);

		FLOAT dur = (float)(TocEntry.dwStartSector+150)/60/75;
		oCDCon[i].min = (int) dur;
		oCDCon[i].sec = (int) ((dur - oCDCon[i].min)*60);
		oCDCon[i].frame = (int) ((((dur - oCDCon[i].min)*60)-oCDCon[i].sec)*75);
		//OutputDebugString("Track %2d *** Duration %d:%d.%d ",i+1,oCDCon[i].min,oCDCon[i].sec,oCDCon[i].frame);
		//OutputDebugString("          *** Frames %d ",(oCDCon[i].min*60*75)+(oCDCon[i].sec*75)+oCDCon[i].frame);
		


		LONG sectors = TocEntry2.dwStartSector - TocEntry.dwStartSector;
		dur = (float)sectors/60/75;
		CDCon[i].min = (int) dur;
		CDCon[i].sec = (int) ((dur - CDCon[i].min)*60);
		CDCon[i].frame = (int) ((((dur - CDCon[i].min)*60)-CDCon[i].sec)*75);
		ntFra+=sectors;
	}
	
	tdur = (float)ntFra/60/75;
	CDCon[i+1].min = (int) tdur;
	CDCon[i+1].sec = (int) ((tdur - CDCon[i+1].min)*60);
	CDCon[i+1].frame = (int) ((((tdur - CDCon[i+1].min)*60)-CDCon[i+1].sec)*75);
	
	tdur = (float)(ntFra+150)/60/75;
	oCDCon[i].min = (int) tdur;
	oCDCon[i].sec = (int) ((tdur - oCDCon[i].min)*60);
	oCDCon[i].frame = (int) ((((tdur - oCDCon[i].min)*60)-oCDCon[i].sec)*75);
	//OutputDebugString("Total min %d sek %d frames %d",oCDCon[i].min,oCDCon[i].sec,oCDCon[i].frame);
	if ( CDEX_OK != CR_DeInit() ) {
		OutputDebugString("CR_DeInit failed");
		return 0;
	}
	return 1;
}

//void D2Xcdrip::OnStartup()
//{
//}
//
//void D2Xcdrip::OnExit()
//{
//	 
//}
//
//void D2Xcdrip::Process()
//{
//	BOOL bAbort=false;
//	CDEX_ERR ripErr = CDEX_OK;
//
//	if(status)
//	{
//		do
//		{
//			switch(fillbuffer)
//			{
//			case BUFFER0:
//				if(p_buf[0].status != CD_EMPTY)
//					break;
//				memset(p_buf[0].buffer,0,nBufferSize);
//				ripErr = CR_RipChunk(p_buf[0].buffer,&p_buf[0].numBytes, bAbort );
//				// Check if an error did occur
//				if ( CDEX_ERROR == ripErr )
//					p_buf[0].status = CD_ERROR;
//				else if ( CDEX_RIPPING_DONE == ripErr )
//					p_buf[0].status = CD_DONE;
//				else
//					p_buf[0].status = CD_OK;
//				break;
//			case BUFFER1:
//				if(p_buf[1].status != CD_EMPTY)
//					break;
//				memset(p_buf[1].buffer,0,nBufferSize);
//				ripErr = CR_RipChunk(p_buf[1].buffer,&p_buf[1].numBytes, bAbort );
//				// Check if an error did occur
//				if ( CDEX_ERROR == ripErr )
//					p_buf[1].status = CD_ERROR;
//				else if ( CDEX_RIPPING_DONE == ripErr )
//					p_buf[1].status = CD_DONE;
//				else
//					p_buf[1].status = CD_OK;
//				break;
//			default:
//				break;
//			}
//			//Sleep(50);
//		} while((ripErr != CDEX_RIPPING_DONE) && (ripErr != CDEX_ERROR));
//	}
//}
