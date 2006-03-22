#ifndef D2XGUISOUNDS
#define D2XGUISOUNDS



#include "d2xsettings.h"
#include <xbutil.h>
#include <map>
#include <dmusici.h>
#include <dmusicfx.h>
//#include <myfactory.h>


class D2Xguisounds
{
protected:
	
	D2Xguisounds();
	static std::auto_ptr<D2Xguisounds> sm_inst;

	map<CStdString,IDirectMusicSegment8*> mapSounds;

	LPDIRECTSOUND8            m_pDSound;          // DirectSound object
	IDirectMusicLoader8*      m_pLoader;          // DM Loader
	IDirectMusicPerformance8* m_pPerformance;     // DM Performance
	IDirectMusicAudioPath8*   m_pAudioPath;       // DM AudioPath
	LPDIRECTSOUNDBUFFER8      m_p3DBuffer;        // 3D buffer from audiopath

	FLOAT                     m_fVolume;          // Current volume
	BOOL					  m_bSound;

public:
	
	static D2Xguisounds* Instance();
	int InitDirectMusic();
	int LoadSound(const CStdString& strFileName,const CStdString& name);
	void PlaySound(const CStdString& name);
	void StopSound(const CStdString& name);
	void DoSoundWork();
	void ReleaseLoader();

};

#endif