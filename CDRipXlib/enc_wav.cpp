#include "cdencoder.h"
#include "cdripxlib.h"
#include "..\dvd2xbox\d2xsettings.h"



int CCDEnc::InitWav(char* file)
{
	m_pSndFile = NULL; 
	memset(&m_wfInfo, 0x00, sizeof(SF_INFO));
	m_wfInfo.channels = 2;
	m_wfInfo.pcmbitwidth = 16;
	m_wfInfo.samples = 0;		// Number of samples are not know at this point
	m_wfInfo.samplerate = 44100;

	
	// Set format options
	m_wfInfo.format = (SF_FORMAT_WAV|SF_FORMAT_PCM);
	
	// Open stream
	if (!(m_pSndFile = sf_open_write(file,&m_wfInfo)))
	{
		return CDRIPX_ERR;
	}
	return CDRIPX_OK;
}

int CCDEnc::WavEnc(int nNumBytesRead,BYTE* pbtStream)
{
	return sf_write_short(m_pSndFile, (SHORT*)pbtStream, nNumBytesRead/sizeof(SHORT))*sizeof(SHORT);
}

int CCDEnc::WavClose()
{
	if (m_pSndFile)
	{
		if (sf_close(m_pSndFile) !=0)
		{
			return CDRIPX_ERR;
		}
		m_pSndFile = NULL;
	}
	// No Errors
	return CDRIPX_OK;       
}