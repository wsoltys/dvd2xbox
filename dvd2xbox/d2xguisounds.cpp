#include "D2Xguisounds.h"

std::auto_ptr<D2Xguisounds> D2Xguisounds::sm_inst;

D2Xguisounds::D2Xguisounds()
{
	m_fVolume           = DSBVOLUME_MAX;
	m_bSound			= false;
}


D2Xguisounds* D2Xguisounds::Instance()
{
    if(sm_inst.get() == 0)
    {
		std::auto_ptr<D2Xguisounds> tmp(new D2Xguisounds);
        sm_inst = tmp;
    }
    return sm_inst.get();
}

int D2Xguisounds::InitDirectMusic()
{
	// Create DirectSound object
    if( FAILED( DirectSoundCreate( NULL, &m_pDSound, NULL ) ) )
        return E_FAIL;

	// If the application doesn't care about vertical HRTF positioning,
    // calling DirectSoundUseLightHRTF can save about 60k of memory.
    DirectSoundUseLightHRTF();
    //DirectSoundUseFullHRTF();

    // download the standard DirectSound effects image
    DSEFFECTIMAGELOC EffectLoc;
    EffectLoc.dwI3DL2ReverbIndex = GraphI3DL2_I3DL2Reverb;
    EffectLoc.dwCrosstalkIndex   = GraphXTalk_XTalk;
	if( FAILED( XAudioDownloadEffectsImage( "dmusicfx", 
                                            &EffectLoc, 
                                            XAUDIO_DOWNLOADFX_XBESECTION, 
                                            NULL ) ) )
        return 0;

    // Initialize DMusic
	DirectMusicInitialize();

	// Create DirectMusic loader object
    DirectMusicCreateInstance( CLSID_DirectMusicLoader, NULL, 
                               IID_IDirectMusicLoader8, (VOID**)&m_pLoader );

	// Create DirectMusic performance object
    DirectMusicCreateInstance( CLSID_DirectMusicPerformance, NULL,
                               IID_IDirectMusicPerformance8, (VOID**)&m_pPerformance );

    // Initialize the performance with an ordinary music setup.
    m_pPerformance->InitAudioX( DMUS_APATH_SHARED_STEREOPLUSREVERB , 64, 128, 0 );

	// Get 3D audiopath.
    m_pPerformance->GetDefaultAudioPath( &m_pAudioPath );

    // Max volume for music
    m_pAudioPath->SetVolume( (LONG)m_fVolume, 0 );
    m_pAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, 
                                   DMUS_PATH_BUFFER, 
                                   0, 
                                   GUID_NULL, 
                                   0, 
                                   GUID_NULL, 
                                   (VOID **)&m_p3DBuffer );

	m_bSound = true;
	return 1;
}

int D2Xguisounds::LoadSound(const CStdString& strFileName,const CStdString& name)
{
	map<CStdString,IDirectMusicSegment8*>::iterator iwav;
	iwav = mapSounds.find(name);
	if(iwav != mapSounds.end())
		return 0;

	IDirectMusicSegment8*     m_pSegment;

	m_pLoader->LoadObjectFromFile( CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
		strFileName.c_str(), (VOID **)&m_pSegment );

	m_pSegment->SetRepeats( 0 );

	mapSounds.insert(pair<CStdString,IDirectMusicSegment8*>(name,m_pSegment));

	return 1;
}

void D2Xguisounds::PlaySound(const CStdString& name)
{
	map<CStdString,IDirectMusicSegment8*>::iterator iwav;
	iwav = mapSounds.find(name);
	if(iwav != mapSounds.end())
	{
		m_pPerformance->StopEx( iwav->second , 0, 0 );
		m_pPerformance->PlaySegmentEx( iwav->second, NULL, NULL, 0, 0, 
                                       NULL, NULL, NULL );
	}
}

void D2Xguisounds::StopSound(const CStdString& name)
{
	map<CStdString,IDirectMusicSegment8*>::iterator iwav;
	iwav = mapSounds.find(name);
	if(iwav != mapSounds.end())
	{
		m_pPerformance->StopEx( iwav->second , 0, 0 );
	}
}

void D2Xguisounds::DoSoundWork()
{
	if(m_bSound)
		DirectSoundDoWork();
}

void D2Xguisounds::ReleaseLoader()
{
	m_pLoader->Release();
}