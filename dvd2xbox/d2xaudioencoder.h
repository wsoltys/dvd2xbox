#ifndef D2XAUDIOENCODER
#define D2XAUDIOENCODER

#include <xtl.h>
#include <string>
#include "d2xsettings.h"
#include "..\lib\liblame\lame.h"
#include "..\lib\libogg\vorbisenc.h"

using namespace std;

#define D2XAENC_MP3		1111
#define D2XAENC_OGG		2222
#define D2XAENC_WAV		3333

#define ENC_ARTIST		1
#define	ENC_TITLE		2
#define ENC_ALBUM		3
#define ENC_YEAR		4
#define ENC_COMMENT		5
#define ENC_TRACK		6

class D2Xaenc
{
protected:
	int		mode;
	FILE*	outf;
	FILE*	OGG;

	BYTE*	buffer;

	// LAME
	lame_global_flags *gf;
	int InitLame(char* file);
	int LameEnc(int nNumBytesRead,BYTE* pbtStream);
	int LameClose();
	void AddLameTag(int key,const char* value);

	// ogg
	vorbis_comment	vc;
	vorbis_info      vi; /* struct that stores all the static vorbis bitstream settings */
	vorbis_dsp_state vd; /* central working state for the packet->PCM decoder */
	vorbis_block     vb; /* local working space for packet->PCM decode */
	ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
	ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
	ogg_packet       op; /* one raw packet of data for decode */

	int InitOgg(char* file);
	int OggEnc(int nNumBytesRead,BYTE* pbtStream);
	int OggClose();
	void AddOggTag(int key,char* value);


public:
	D2Xaenc();
	~D2Xaenc();
	
	int InitEnc(char* file,int encoding);
	int EncodeChunk(int nNumBytesRead,BYTE* pbtStream);
	int Close();
	void AddTag(int key,const char* value);

};

#endif