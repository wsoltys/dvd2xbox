#include "cdencoder.h"
#include "cdripxlib.h"
#include "..\dvd2xbox\d2xsettings.h"



int CCDEnc::InitLame(char* file)
{
	int     i;
	/* initialize libmp3lame */
    if (NULL == (gf = lame_init())) {
        DPf("fatal error during initialization\n");
        return 1;
    }
	if((outf = fopen(file,"wb+"))==NULL) {
		DPf("Cannot open file: %s",file);
		return CDRIPX_ERR;
	}
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
        DPf("fatal error during initialization\n");
        return i;
    }
	
	DPf("InitLame2 ok");
	return CDRIPX_OK;
}

int CCDEnc::LameEnc(int nNumBytesRead,BYTE* pbtStream)
{


    /* encode */
	int imp3 = lame_encode_buffer_interleaved(gf,(short*)pbtStream,nNumBytesRead/4, mp3buffer, sizeof(mp3buffer));
	if (imp3 < 0) {
        if (imp3 == -1)
            DPf("mp3 buffer is not big enough... \n"); 
        else
            DPf("mp3 internal error:  error code=%i\n", imp3);
        return CDRIPX_ERR;
    }
	if (fwrite(mp3buffer, 1, imp3, outf) != imp3) { 
        DPf("Error writing mp3 output \n");
        return CDRIPX_ERR;
    }
	
	return CDRIPX_OK;
}

int CCDEnc::LameClose()
{
	int imp3 = lame_encode_flush(gf, mp3buffer, sizeof(mp3buffer)); /* may return one more mp3 frame */
	if (imp3 < 0) {
        if (imp3 == -1)
            DPf("mp3 buffer is not big enough... \n");
        else
           DPf("mp3 internal error:  error code=%i\n", imp3);
        return 1;

    }
	fwrite(mp3buffer, 1, imp3, outf); 
	fclose(outf);

	lame_mp3_tags_fid(gf, outf); /* add VBR tags to mp3 file */
	lame_close(gf);
	return CDRIPX_OK;
}

void CCDEnc::AddLameTag(int key,const char* value)
{
	switch(key)
	{
	case lame_artist:
		id3tag_set_artist(gf,value);
		break;
	case lame_title:
		id3tag_set_title(gf,value);
		break;
	case lame_album:
		id3tag_set_album(gf,value);
		break;
	case lame_year:
		id3tag_set_year(gf,value);
		break;
	case lame_comment:
		id3tag_set_comment(gf,value);
		break;
	case lame_track:
		id3tag_set_track(gf,value);
		break;
	default:
		break;

	}

}

