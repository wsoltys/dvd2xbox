#include "D2Xaudioencoder.h"


D2Xaenc::D2Xaenc()
{
	init2 = 0;
}


D2Xaenc::~D2Xaenc()
{	

}


int D2Xaenc::InitEnc(char* file,int encoding)
{
	mode = encoding;
	switch(mode)
	{
	case D2XAENC_MP3:
		return InitLame(file);
		break;
	case D2XAENC_OGG:
		return InitOgg(file);
		break;
	case D2XAENC_WAV:
		return InitWav(file);
		break;
	default:
		break;
	}
	return 0;
}

int D2Xaenc::EncodeChunk(int nNumBytesRead,BYTE* pbtStream)
{
	switch(mode)
	{
	case D2XAENC_MP3:
		return LameEnc(nNumBytesRead,pbtStream);
		break;
	case D2XAENC_OGG:
		return OggEnc(nNumBytesRead,pbtStream);
		break;
	case D2XAENC_WAV:
		return WavEnc(nNumBytesRead,pbtStream);
		break;
	default:
		break;
	}
	return 0;
}

int D2Xaenc::Close()
{
	switch(mode)
	{
	case D2XAENC_MP3:
		return LameClose();
		break;
	case D2XAENC_OGG:
		return OggClose();
		break;
	case D2XAENC_WAV:
		return WavClose();
		break;
	default:
		break;
	}
	return 0;
}

void D2Xaenc::AddTag(int key,const char* value)
{
	switch(mode)
	{
	case D2XAENC_MP3:
		return AddLameTag(key,value);
		break;
	case D2XAENC_OGG:
		return AddOggTag(key,(char*)value);
		break;
	default:
		break;
	}
}