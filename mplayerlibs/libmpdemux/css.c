
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif
#include "config.h"

#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <string.h>
//#include <inttypes.h>
//#include <time.h>
#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <sys/stat.h>
#include <errno.h>

#include "dvd_util.h"
#include "dvd.h"

#include "css_auth.h"

typedef char int8_t;
typedef unsigned char uint8_t;

#include "css.h"

#ifdef __FreeBSD__
typedef struct dvd_struct   dvd_struct;
typedef struct dvd_authinfo dvd_authinfo;
#endif

typedef struct {
	uint8_t Challenge[10];
	uint8_t Key1[LEN_KEY];
	uint8_t Key2[LEN_KEY];
	uint8_t KeyCheck[LEN_KEY];
	uint8_t variant;
} DiscKeys;

DiscKeys discKeys;

#if 0 //DEBUG
/**
 * Print Functions
 **/

static void _CSSPrintBytes (const uint8_t *val, int len)
{
	const uint8_t *ptr = val;

	for (; len > 0; len--)
		fprintf (stderr, " %02X", *ptr++ & 0xff);
}

#define _CSSPrintDebug(args...) fprintf (stderr, args)
#define _CSSPrintDebugBytes(descr, val, len) fprintf (stderr, descr":\t"); _CSSPrintBytes (val, len); fprintf (stderr, "\n")
#else
#define _CSSPrintBytes(val, len)
#define _CSSPrintDebug(args)
#define _CSSPrintDebugBytes(descr, val, len)
#endif

/**
 * CSSAuthDrive
 * Determine if drive is authentic
 * Return: -1 if failed, 0..32 = variant of CSS
 **/


static int _CSSAuthDrive (const uint8_t *key, DiscKeys *disc)
{
	int i;

	for (i=0; i<LEN_KEY; i++)
		disc->Key1[i] = key[4-i];

	for (i=0; i<32; ++i) {
		CryptKey1 (i, disc->Challenge, disc->KeyCheck);

		if (memcmp (disc->KeyCheck, disc->Key1, LEN_KEY) == 0) {
//			_CSSPrintDebug ("Drive Authentic - using variant %d\n", i);
			disc->variant = i;
			return i;
		}
	}

//	_CSSPrintDebug ("Drive would not Authenticate\n");

	return -1;
}


/**
 * Get Disc Key
 * Input:
 * dvd 	device handle of DVD
 * agid
 * key 	player key
 * 
 * Output:
 * disckey	2048 byte block of disc key XOR player key
 * return 	0=failed, 1 = success
 **/

static int _CSSGetDiscKey (dvd_device_t *dvd, int agid, char *key, char *key_disc)
{
	dvd_struct s;
	int i;

#ifdef __FreeBSD__
	/*
	 * FreeBSD's dvd_struct is not a union as it is with the other
	 * platforms; hence, there are no additional sub-member
	 * dereferences.
	 */

	s.format= DVD_STRUCT_DISCKEY;
	s.agid	= agid;
	memset (s.data, 0, 2048);

	if (ioctl (fd, DVDIOCREADSTRUCTURE, &s) < 0) {
		perror( "Could not read Disc Key");
		return -1;
	}

	for (i=0; i < sizeof (s.data); i++)
		s.data[i] ^= key[4 - (i % LEN_KEY)];

//	_CSSPrintDebugBytes ("Received Disc Key", s.data, 10);

	memcpy (key_disc, s.data, 2048);
#else

	s.type = DVD_STRUCT_DISCKEY;
	s.disckey.agid = agid;
	memset (s.disckey.value, 0, 2048);

	if (DVDReadStruct(dvd, &s)<0) {
		perror ("Could not read Disc Key");
		return -1;
	}

	for (i=0; i<sizeof s.disckey.value; i++)
		s.disckey.value[i] ^= key[4 - (i % LEN_KEY)];

//	_CSSPrintDebugBytes ("Received Disc Key", s.disckey.value, 10);

	memcpy (key_disc, s.disckey.value, 2048);
#endif

	return 0;
}


/**
 * Get Title Key
 * Input:
 * dvd 		device handle of DVD
 * agid
 * key 		player key
 * 
 * Output:
 * key_title	2048 byte block of disc key XOR player key
 * return 		0=failed, 1 = success
 **/

static int _get_title_key (dvd_device_t *dvd, int agid, int lba, char *key, char *key_title)
{
	dvd_authinfo ai;
	int i;

#ifdef __FreeBSD__
	ai.format= DVD_REPORT_TITLE_KEY;
	ai.agid	= agid;
	ai.lba	= lba;
	memset (ai.keychal, 0, 10);

	if (ioctl (fd, DVDIOCREPORTKEY, &ai)) {
		perror ("GetTitleKey failed");
		return -1;
	}

	for (i=0; i<LEN_KEY; i++)
		ai.keychal[i] ^= key[4 - (i%LEN_KEY)];

//	_CSSPrintDebugBytes ("Received Title Key", ai.keychal, LEN_KEY);
//	_CSSPrintDebug (" CPM=%d, CP_SEC=%d, CGMS=%d\n", ai.cpm, ai.cp_sec, ai.cgms);

	memcpy (key_title, ai.keychal, LEN_KEY);
#else
	ai.type = DVD_LU_SEND_TITLE_KEY;

	ai.lstk.agid = agid;
	ai.lstk.lba = lba;

	if (DVDAuth(dvd, &ai)) {
		perror ("GetTitleKey failed");
		return -1;
	}

	for (i=0; i< LEN_KEY; ++i)
		ai.lstk.title_key[i] ^= key[4 - (i%LEN_KEY)];

//	_CSSPrintDebugBytes ("Received Title Key", ai.lstk.title_key, LEN_KEY);
//	_CSSPrintDebug (" CPM=%d, CP_SEC=%d, CGMS=%d\n", ai.lstk.cpm, ai.lstk.cp_sec, ai.lstk.cgms);

	memcpy (key_title, ai.lstk.title_key, LEN_KEY);
#endif

	return 0;
}


/**
 * Get ASF (Authentication Success Flag)
 * dvd 	device handle of DVD
 * Output:
 * return: 0 = failure, 1 = success
 **/

int CSSGetASF (dvd_device_t *dvd)
{
	dvd_authinfo ai;

#ifdef __FreeBSD__
	ai.format= DVD_REPORT_ASF;
	ai.asf	= 0;

	for (ai.agid = 0; ai.agid < 4; ai.agid++) {
		if (!DVDAuth (dvd, &ai)) {
//			_CSSPrintDebug ("%sAuthenticated\n", (ai.asf) ? "" : "not");
			return 0;
		}
	}
#else
	ai.type = DVD_LU_SEND_ASF;
	ai.lsasf.asf = 0;

	for (ai.lsasf.agid = 0; ai.lsasf.agid < 4; ai.lsasf.agid++) {
//		OutputDebugString("CSSGetASF: DVDAuth\n");
		if (!DVDAuth (dvd, &ai)) {
//			OutputDebugString("CSSGetASF: Authenticated\n");
//			_CSSPrintDebug ("%sAuthenticated\n", (ai.lsasf.asf) ? "" : "not");
			return 0;
		}
	}
#endif
	perror ("GetASF");
	return -1;
}


/**
 *
 **/

int _CSSGetAgid (dvd_device_t *dvd, dvd_authinfo *ai)
{
	int tries;
	int rv;

	/* Init sequence, request AGID */
	for (tries = 1; tries < 4; ++tries) {
//		_CSSPrintDebug ("Request AGID [%d]...", tries);
#ifdef __FreeBSD__
		ai->format = DVD_REPORT_AGID;
		ai->agid = 0;
#else
		ai->type = DVD_LU_SEND_AGID;
		ai->lsa.agid = 0;
#endif
		rv = DVDAuth (dvd, ai);

		if (rv != -1) {
			// found key
//			_CSSPrintDebug ("AGID %d\n", ai->agid);
			return tries;
		} else {
//			_CSSPrintDebug ("\n");
		}

		perror ("N/A, invalidating");

#ifdef __FreeBSD__
		ai->format = DVD_INVALIDATE_AGID;
		ai->agid = 0;
#else
		ai->type = DVD_INVALIDATE_AGID;
		ai->lsa.agid = 0;
#endif
		DVDAuth (dvd, ai);
	}

//	_CSSPrintDebug ("Cannot get AGID\n");

	return -1;
}


#ifndef __FreeBSD__
/**
 * Simulation of a non-CSS compliant host (i.e. the authentication fails,
 * but idea is here for a real CSS-compliant authentication scheme).
 **/

static int _CSSAuthHost (dvd_authinfo *ai, DiscKeys *disc)
{
	int i;
static char buffer[256];

	switch (ai->type) {
	/* Host data receive (host changes state) */
	case DVD_LU_SEND_AGID:
		sprintf(buffer, "_CSSAuthHost: DVD_LU_SEND_AGID: %d\n", ai->lsa.agid);
//		OutputDebugString(buffer);
//		_CSSPrintDebug ("AGID %d\n", ai->lsa.agid);

		ai->type = DVD_HOST_SEND_CHALLENGE;
		break;

	case DVD_LU_SEND_KEY1:

//		_CSSPrintDebugBytes ("LU sent key1", ai->lsk.key, LEN_KEY);

		if (_CSSAuthDrive (ai->lsk.key, disc)<0) {
			ai->type = DVD_AUTH_FAILURE;
			return -EINVAL;
		}
		ai->type = DVD_LU_SEND_CHALLENGE;
		break;

	case DVD_LU_SEND_CHALLENGE:
		for (i = 0; i < 10; ++i)
			disc->Challenge[i] = ai->hsc.chal[9-i];

//		_CSSPrintDebugBytes ("LU sent challenge", ai->hsc.chal, 10);

		CryptKey2 (disc->variant, disc->Challenge, disc->Key2);
		ai->type = DVD_HOST_SEND_KEY2;
		break;

	/* Host data send */
	case DVD_HOST_SEND_CHALLENGE:
		for (i = 0; i < 10; ++i)
			ai->hsc.chal[9-i] = disc->Challenge[i];

//		_CSSPrintDebugBytes ("Host sending challenge", ai->hsc.chal, 10);

		/* Returning data, let LU change state */
		break;

	case DVD_HOST_SEND_KEY2:
		for (i = 0; i < LEN_KEY; ++i)
			ai->hsk.key[4-i] = disc->Key2[i];

//		_CSSPrintDebugBytes ("Host sending key 2", &disc->Key2[0], LEN_KEY);

		/* Returning data, let LU change state */
		break;

	default:
		sprintf(buffer, "_CSSAuthHost: Invalid state: %d\n", ai->type);
//		OutputDebugString(buffer);
//		_CSSPrintDebug ("Got invalid state %d\n", ai->type);

		return -EINVAL;
	}

	return 0;
}
#endif


/**
 *
 **/

static char bfr[256];
int CSSAuth (dvd_device_t *dvd, DiscKeys *disc)
{
 	dvd_authinfo ai;
	int i, agid;

//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
	memset (&ai, 0, sizeof (ai));

//	OutputDebugString("CSSAuth: CSSGetASF\n");
//	if (CSSGetASF (dvd))
//		return -1;
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);

//	OutputDebugString("CSSAuth: _CSSGetAgid\n");
	_CSSGetAgid (dvd, &ai);
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);

	for (i=0; i < 10; ++i)
		disc->Challenge[i] = i;

#ifdef __FreeBSD__
	agid = ai.agid;

	/* Get challenge from host */
	ai.format = DVD_SEND_CHALLENGE;
	for (i=0; i<10; ++i)
		ai.keychal[9-i] = disc->Challenge[i];

//	_CSSPrintDebugBytes ("Host sending challenge: ", ai.keychal, 10);

	/* Send challenge to LU */
	if (ioctl (fd, DVDIOCSENDKEY, &ai) < 0) {
//		_CSSPrintDebug ("Send challenge to LU failed\n");
		return -1;
	}

	/* Get key1 from LU */
	ai.format = DVD_REPORT_KEY1;
	if (ioctl (fd, DVDIOCREPORTKEY, &ai) < 0) {
//		_CSSPrintDebug ("Get key1 from LU failed\n");
		return -1;
	}

	/* Send key1 to host */
//	_CSSPrintDebugBytes ("LU sent key1: ", ai.keychal, LEN_KEY);

	if (_CSSAuthDrive( ai.keychal, disc) < 0) {
//		_CSSPrintDebug("Send key1 to host failed\n");
		return -1;
	}

	/* Get challenge from LU */
	ai.format = DVD_REPORT_CHALLENGE;
	if (ioctl(fd, DVDIOCREPORTKEY, &ai) < 0) {
//		_CSSPrintDebug("Get challenge from LU failed\n");
		return -1;
	}

	/* Send challenge to host */
	for (i=0; i<10; ++i)
		disc->Challenge[i] = ai.keychal[9-i];

//	_CSSPrintDebugBytes ("LU sent challenge: ", disc->Challenge, 10);

	CryptKey2(disc->variant, disc->Challenge, disc->Key2);

	/* Get key2 from host */
	ai.format = DVD_SEND_KEY2;
	for (i=0; i<LEN_KEY; ++i)
		ai.keychal[4-i] = disc->Key2[i];

//	_CSSPrintDebugBytes ("Host sending key 2: ", disc->Key2, LEN_KEY);

	/* Send key2 to LU */
	if (ioctl(fd, DVDIOCSENDKEY, &ai) < 0) {
		printf("Send key2 to LU failed (expected)\n");
		return -1;
	}
#else
//	OutputDebugString("CSSAuth: Phase 1\n");
	ai.type = DVD_LU_SEND_AGID;
	/* Send AGID to host */
	if (_CSSAuthHost(&ai, disc) < 0) {
//		_CSSPrintDebug ("Send AGID to host failed\n");
		return -1;
	}
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
//	OutputDebugString("CSSAuth: Phase 2\n");
	/* Get challenge from host */
	if (_CSSAuthHost(&ai, disc) < 0) {
//		_CSSPrintDebug ("Get challenge from host failed\n");
		return -1;
	}
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
//	OutputDebugString("CSSAuth: Phase 3\n");
	agid = ai.lsa.agid;
	/* Send challenge to LU */
	if (DVDAuth(dvd, &ai) < 0) {
//		_CSSPrintDebug ("Send challenge to LU failed\n");
		return -1;
	}
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
//	OutputDebugString("CSSAuth: Phase 4\n");
	/* Get key1 from LU */
	ai.type = DVD_LU_SEND_KEY1;
	if (DVDAuth(dvd, &ai) < 0) {
//		_CSSPrintDebug ("Get key1 from LU failed\n");
		return -1;
	}
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
//	OutputDebugString("CSSAuth: Phase 5\n");
	/* Send key1 to host */
	if (_CSSAuthHost(&ai, disc) < 0) {
//		_CSSPrintDebug ("Send key1 to host failed\n");
		return -1;
	}
//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
//	OutputDebugString("CSSAuth: Phase 6\n");
	/* Get challenge from LU */
	if (DVDAuth(dvd, &ai) < 0) {
//		_CSSPrintDebug ("Get challenge from LU failed\n");
		return -1;
	}
//	OutputDebugString("CSSAuth: Phase 7\n");
	/* Send challenge to host */
	if (_CSSAuthHost (&ai, disc) < 0) {
//		_CSSPrintDebug ("Send challenge to host failed\n");
		return -1;
	}
//	OutputDebugString("CSSAuth: Phase 8\n");
	/* Get key2 from host */
	if (_CSSAuthHost (&ai, disc) < 0) {
//		_CSSPrintDebug ("Get key2 from host failed\n");
		return -1;
	}
//	OutputDebugString("CSSAuth: Phase 9\n");
	/* Send key2 to LU */
	if (DVDAuth(dvd, &ai) < 0) {
//		_CSSPrintDebug ("Send key2 to LU failed (expected)\n");
		return -1;
	}

//	if (ai.type == DVD_AUTH_ESTABLISHED)
//		_CSSPrintDebug ("DVD is authenticated\n");
//	else if (ai.type == DVD_AUTH_FAILURE)
//		_CSSPrintDebug ("DVD authentication failed\n");
#endif
	memcpy (disc->Challenge, disc->Key1, LEN_KEY);
	memcpy (disc->Challenge+LEN_KEY, disc->Key2, LEN_KEY);
	CryptBusKey (disc->variant, disc->Challenge, disc->KeyCheck);

//	_CSSPrintDebugBytes ("Received Session Key", &disc->KeyCheck[0], LEN_KEY);

	return( agid);

}


/**
 * CSSAuthDisc: Authorise disc
 * dvd  DVD device handle
 * key	player key
 **/
	
int CSSAuthDisc (dvd_device_t *dvd, char *key)
{
	int agid;
	DiscKeys *disc = &discKeys;

//	sprintf(bfr, "dvd: %p\n", bfr); OutputDebugString(bfr);
// (re)init tables
        css_init_tables ();

//	OutputDebugString("Callign CSSAuth\n");
	agid = CSSAuth (dvd, disc);

	if (agid < 0)
		return -1;

//	OutputDebugString("Calling CSSGetASF\n");
//	if (CSSGetASF (dvd) < 0)
//		return -1;

//	OutputDebugString("Calling _CSSGetDiscKey\n");
	_CSSGetDiscKey (dvd, agid, disc->KeyCheck, key);

//	OutputDebugString("Calling CSSGetASF\n");
//	if (CSSGetASF (dvd) < 0)
//		return -1;


	return 0;
}


/** 
 * CSSAuthTitle
 * Authorise title from disc
 * dvd	DVD device handle
 * key	disc key
 * lba	logical block address of title	
 **/

int CSSAuthTitle (dvd_device_t *dvd, char *key, int lba)
{
	int agid;
	DiscKeys *disc = &discKeys;

	if ((agid = CSSAuth (dvd, disc)) < 0)
		return -1;

	_get_title_key (dvd, agid, lba, disc->KeyCheck, key);

//	if (CSSGetASF (dvd) < 0)
//		return -1;

	return 0;
}


/**
 * CSSAuthTitlePath
 * Authorise title from disc
 * dvd        DVD device handle
 * key        disc key
 * path pathname of title on disc
 **/

int CSSAuthTitlePath (dvd_device_t *dvd, char *key, char *path)
{
	int lba;

	lba = DVDPath2LBA (dvd, path);
	if (lba == -1)
		return -1;

	return CSSAuthTitle (dvd, key, lba);
}


/**
 * CSSisEncrypted
 * check if disk is encrypted at all
 * dvd		DVD device handle
 **/

int CSSisEncrypted (dvd_device_t *dvd_handle)
{
	dvd_struct dvd;

	dvd.copyright.type = DVD_STRUCT_COPYRIGHT;
	dvd.copyright.layer_num = 0;

	if (DVDReadStruct (dvd_handle, &dvd) < 0) {
//		_CSSPrintDebug ("error using %s\n", __FUNCTION__);
		return -1;
	}

	return dvd.copyright.cpst;      // 0 ... not encrypted
}

