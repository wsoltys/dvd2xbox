#include "d2xaudioencoder.h"
//#include "..\dvd2xbox\d2xsettings.h"

int D2Xaenc::InitLame(char* file)
{
	int     i;
	
	/* initialize libmp3lame */
    if (NULL == (gf = lame_init())) {
        OutputDebugString("fatal error during initialization\n");
        return 1;
    }
	if((outf = fopen(file,"wb+"))==NULL) {
		OutputDebugString("Cannot open output file."); 
		return 0;
	}
	//lame_set_preset(gf,STANDARD_FAST); 
	lame_set_asm_optimizations(gf,MMX,1);
	lame_set_asm_optimizations(gf,SSE,1); 
	id3tag_init(gf);
	lame_set_quality(gf,5);
	lame_set_in_samplerate(gf,44100);
	MPEG_mode mode;
	if(g_d2xSettings.mp3_mode == 0)
		mode = STEREO;
	if(g_d2xSettings.mp3_mode == 1)
		mode = JOINT_STEREO;
	lame_set_mode(gf,mode);
	lame_set_brate(gf,g_d2xSettings.mp3_bitrate); 
	/* Now that all the options are set, lame needs to analyze them and
     * set some more internal options and check for problems
     */
    i = lame_init_params(gf);
    if (i < 0) {
        OutputDebugString("fatal error during initialization\n");
        return i;
    }
	buffer = new BYTE[26624];
	if(buffer == NULL)
	{
		OutputDebugString("cannot allocate mp3 buffer");
		return 0;
	}
	OutputDebugString("InitLame ok");
	return 1;
}

int D2Xaenc::LameEnc(int nNumBytesRead,BYTE* pbtStream) 
{


    /* encode */
	int imp3 = lame_encode_buffer_interleaved(gf,(short*)pbtStream,nNumBytesRead/4, buffer, 0);
	if (imp3 < 0) {
        if (imp3 == -1)
            OutputDebugString("mp3 buffer is not big enough..."); 
        else
            //OutputDebugString("mp3 internal error:  error code=%i\n", imp3);
			OutputDebugString("mp3 internal error");
		delete[] buffer;
        return 0;
    }
	if (fwrite(buffer, 1, imp3, outf) != imp3) { 
        OutputDebugString("Error writing mp3 output");
		delete[] buffer;
        return 0;
    }
	
	return 1;
}

int D2Xaenc::LameClose()
{
	int imp3 = lame_encode_flush(gf, buffer, 0); /* may return one more mp3 frame */
	if (imp3 < 0) {
        if (imp3 == -1)
            OutputDebugString("mp3 buffer is not big enough...");
        else
           //OutputDebugString("mp3 internal error:  error code=%i\n", imp3);
		   OutputDebugString("mp3 internal error");
		delete[] buffer;
        return 1;

    }
	fwrite(buffer, 1, imp3, outf); 
	lame_mp3_tags_fid(gf, outf); /* add VBR tags to mp3 file */
	lame_close(gf);
	fclose(outf);
	delete[] buffer;
	return 1;
}

void D2Xaenc::AddLameTag(int key,const char* value)
{
	switch(key)
	{
	case ENC_ARTIST:
		id3tag_set_artist(gf,value);
		break;
	case ENC_TITLE:
		id3tag_set_title(gf,value);
		break;
	case ENC_ALBUM:
		id3tag_set_album(gf,value);
		break;
	case ENC_YEAR:
		id3tag_set_year(gf,value);
		break;
	case ENC_COMMENT:
		id3tag_set_comment(gf,value);
		break;
	case ENC_TRACK:
		id3tag_set_track(gf,value);
		break;
	default:
		break;

	}

}

