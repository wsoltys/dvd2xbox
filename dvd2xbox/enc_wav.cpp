#include "d2xaudioencoder.h"

int D2Xaenc::InitWav(char* file)
{
	m_pSndFile = NULL; 
	memset(&m_wfInfo, 0x00, sizeof(SF_INFO));
	m_wfInfo.channels = 2;
	//m_wfInfo.pcmbitwidth = 16;
	m_wfInfo.frames = 0;		// Number of samples are not know at this point
	m_wfInfo.samplerate = 44100;

	
	// Set format options
	m_wfInfo.format = (SF_FORMAT_WAV|SF_FORMAT_PCM_16);
	
	// Open stream
	if (!(m_pSndFile = sf_open(file,SFM_WRITE,&m_wfInfo)))
	{
		return 0;
	}
	return 1;
}

int D2Xaenc::WavEnc(int nNumBytesRead,BYTE* pbtStream)
{
	sf_write_short(m_pSndFile, (SHORT*)pbtStream, nNumBytesRead/sizeof(SHORT));
	return 1;
}

int D2Xaenc::WavClose()
{
	if (m_pSndFile)
	{
		if (sf_close(m_pSndFile) !=0)
		{
			return 1;
		}
		m_pSndFile = NULL;
	}
	// No Errors
	return 0;       
}