/*
** Copyright (C) 1999 Albert L. Faber
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef CDDA_INCLUDED
#define CDDA_INCLUDED

#include "Aspi.h"
#include "CDRip.h"
#include "cdromsettings.h"



// Table of Contents




extern SENSEKEY	g_SenseKey;

// Handle to the ASPI libaray
extern HINSTANCE hAspiLib;


#define WAVETYPE	0
#define MP3TYPE		1

#define MP3RUNINTHREAD


// possible values for UNDERSAMPLING: 1, 2, 3, 4, 6, 7, 12, 14, 28, 49 
// Sample Frequency      		      44 22 14 11 8  6  3.7  3           KHz 

#define UNDERSAMPLING	1


class CAspiCD:public CDRomSettings
{
private:
	BYTE			m_bNumHA;
	BYTE			m_SenseData[SENSE_LEN];
	BYTE			m_btLastError;
	int				m_nTimeOut;
	bool			m_bUseNtScsi;
	bool			m_bAvailable;

public:
	CAspiCD();

	BOOL GetStatus();
	BYTE GetDeviceType(BYTE btAdapterID,BYTE btTargetID,BYTE btLunID);
	void GetDiskInfo();
	void BusDeviceReset();
	void GetCDRomDevices();
	void InquiryCommand(BYTE btAdapterID,BYTE btTargetID,BYTE btLunID);
	BOOL EjectCD(BOOL bEject);
	void GetDeviceName(BYTE btAdapterID,BYTE btTargetID,BYTE btLunID,LPSTR lpszDeviceName);
	BYTE Seek(DWORD dwAbsPos);

	void PreventMediaRemoval (BOOL bAudioMode);

	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen);
	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen);
	BYTE IssueScsiCmd(BYTE bFlags,LPBYTE lpcbData,int ncbLen,LPBYTE lpBuffer,int nBufLen,BYTE btAdapterID,BYTE btTargetID,BYTE btLunID);
	BOOL ScsiAbort(SRB_EXECSCSICMD* sp,BYTE btAdapterID);

//	BOOL IsCDRom()		{return (BOOL)(GetDeviceType()==DTC_CDROM);}
	BOOL IsUnitReady();

	CDEX_ERR ReadToc();
	CDEX_ERR ReadCDText(BYTE* pbtBuffer,int nBufferSize,LPINT pnCDTextSize);

	void DumpToc (LPBYTE pbtBuffer,int nSize);
	void RecordTrack(DWORD dwFirstSector,DWORD dwLastSector);
	static UINT RecordThread(void* pThreadParams);

	BOOL ReadCdRomSector( BYTE* pbtReadBuffer, LONG lSector, DWORD dwNumSectors, BOOL bGetC2 );

	BOOL	IsMediaLoaded();
	void	UpdateDeviceParams();
	BOOL	PlayTrack(DWORD dwStartSector,DWORD dwEndSector);
	BYTE	IsAudioPlaying();
	BOOL	StopPlayTrack();
	WORD	GetPeakValue();
	void	SetCDSpeed(int nSpeed=-1);
	void	AutoDetectDriveType();
	void	SetSectorSize(DWORD dwSecSize,BYTE btDensity);
	DWORD	GetSectorSize(BYTE& btDensity);
	void	EnableCdda(BOOL bAudioMode);
	BOOL	IsMMC(LPSTR lpszInfo);
//	BOOL	TestDrive(DRIVETABLE Test,ENDIAN& Endian,CADebug& dbgFile);
	BOOL	RequestSense();
	BYTE	ModeSense(int nPageLen,BYTE* pbtBuffer,int nBufSize);
	BYTE	ModeSelect(int nPageSize,BYTE* pbtBuffer,int nBufSize,BOOL bPageFormat=0);
	BOOL	PauseCD(BOOL bPause);
	BOOL	Scan(DWORD dwSector,BOOL bForeWard);
	BYTE	CurrentPosition(DWORD& dwRelPos,DWORD& dwAbsPos);
	BYTE	ReadSubChannel( BYTE btDataFormat, BYTE* pbtBuffer, int nBufSize, BYTE btTrack=0 );
	BYTE	ReadSubChannelQ( BYTE btDataFormat, BYTE* pbtBuffer, int nBufSize );
	int		GetScsiTimeOut();
	void	SetScsiTimeOut(int nTimeOut);
	void	SetTimeOut(int nMilliSeconds);
	int		GetCDSpeed();
//	void	AllocASPIBuffer(ASPI32BUFF* pASPIBuffer);
//	void	DeAllocASPIBuffer(ASPI32BUFF* pASPIBuffer);
	bool	IsAvailable() const { return m_bAvailable; }
	BYTE	GetSubChannelTrackInfo( int& nReadIndex, int&	nReadTrack, DWORD& dwReadPos );

	void	ScanForC2Errors(	LONG	lStartSector,
								LONG	lEndSector,
								INT&	nErrors,
								INT*	pnErrorSectors,
								INT		nMaxErrors,
								BOOL&	bAbort	);

};



#endif