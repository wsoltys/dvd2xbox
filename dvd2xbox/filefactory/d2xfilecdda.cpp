#include "D2Xfilecdda.h"



D2XfileCDDA::D2XfileCDDA()
{
	pbtStream	= NULL;
	bAbort		= false;
}

D2XfileCDDA::~D2XfileCDDA()
{
	
}

int D2XfileCDDA::DeInit()
{
	CR_CloseRipper();

	if ( CDEX_OK != CR_DeInit() ) {
		OutputDebugString("CR_DeInit failed");
	}
	if(pbtStream != NULL)
	{
		delete[] pbtStream;
		pbtStream = NULL;
	}
	return 1;
}


int D2XfileCDDA::FileOpenWrite(char* filename)
{
	return 0;
}

int D2XfileCDDA::FileOpenRead(char* filename)
{
	int track = atoi(filename);
	LONG nBufferSize = 0;
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

	pbtStream = new BYTE[nBufferSize];
	if(pbtStream == NULL)
	{
		DeInit();
		return 0;
	}
	return 1;
}

int D2XfileCDDA::FileWrite(LPCVOID buffer,DWORD dwWrite,DWORD *dwWrote)
{
	return 0;
}

int D2XfileCDDA::FileRead(LPVOID buffer,DWORD dwToRead,DWORD *dwRead)
{
	nNumBytesRead = 0;
	
	CDEX_ERR ripErr = CR_RipChunk(pbtStream,&nNumBytesRead, bAbort );
	*dwRead = (DWORD)nNumBytesRead;

	if ( CDEX_ERROR == ripErr )
		return 0;
	/*else if ( CDEX_RIPPING_DONE == ripErr )
		ret = CD_DONE;
	else
		ret = CD_OK;*/

	return 1; 
}

int D2XfileCDDA::FileClose()
{
	CR_CloseRipper();
	return 1;
}

DWORD D2XfileCDDA::GetFileSize(char* filename)
{
	return (DWORD)CR_GetPercentCompleted();
	return 0;
}

int D2XfileCDDA::GetDirectory(char* path, VECFILEITEMS *items)
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

	items->clear();
	ITEMS temp_item;
	char temp_name[10]={0};

	for(int i=1;i<=tracks;i++)
	{
		sprintf(temp_name,"Track%02d",i);
		temp_item.fullpath = i;
		temp_item.name = string(temp_name);
		items->push_back(temp_item);
	}

	return 1;
}
