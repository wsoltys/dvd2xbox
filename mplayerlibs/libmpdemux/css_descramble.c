/* 
 *  css_descramble.c
 *
 *  Released under the version 2 of the GPL.
 *
 *  Copyright 1999 Derek Fawcus
 *
 *  This file contains functions to descramble CSS encrypted DVD content
 *	
 */

/*
 *  Still in progress:  Remove the use of the bit_reverse[] table by recoding
 *                      the generation of LFSR1.  Finish combining this with
 *                      the css authentication code.
 *
 */

#include <xtl.h>
#include <stdio.h>
#include <string.h>
//#include <inttypes.h>
typedef char int8_t;
typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
#include "css.h"
#include "css_keys.h"

uint8_t r_tbl[256];

static uint8_t csstab1[256]=
{
	0x33,0x73,0x3b,0x26,0x63,0x23,0x6b,0x76,0x3e,0x7e,0x36,0x2b,0x6e,0x2e,0x66,0x7b,
	0xd3,0x93,0xdb,0x06,0x43,0x03,0x4b,0x96,0xde,0x9e,0xd6,0x0b,0x4e,0x0e,0x46,0x9b,
	0x57,0x17,0x5f,0x82,0xc7,0x87,0xcf,0x12,0x5a,0x1a,0x52,0x8f,0xca,0x8a,0xc2,0x1f,
	0xd9,0x99,0xd1,0x00,0x49,0x09,0x41,0x90,0xd8,0x98,0xd0,0x01,0x48,0x08,0x40,0x91,
	0x3d,0x7d,0x35,0x24,0x6d,0x2d,0x65,0x74,0x3c,0x7c,0x34,0x25,0x6c,0x2c,0x64,0x75,
	0xdd,0x9d,0xd5,0x04,0x4d,0x0d,0x45,0x94,0xdc,0x9c,0xd4,0x05,0x4c,0x0c,0x44,0x95,
	0x59,0x19,0x51,0x80,0xc9,0x89,0xc1,0x10,0x58,0x18,0x50,0x81,0xc8,0x88,0xc0,0x11,
	0xd7,0x97,0xdf,0x02,0x47,0x07,0x4f,0x92,0xda,0x9a,0xd2,0x0f,0x4a,0x0a,0x42,0x9f,
	0x53,0x13,0x5b,0x86,0xc3,0x83,0xcb,0x16,0x5e,0x1e,0x56,0x8b,0xce,0x8e,0xc6,0x1b,
	0xb3,0xf3,0xbb,0xa6,0xe3,0xa3,0xeb,0xf6,0xbe,0xfe,0xb6,0xab,0xee,0xae,0xe6,0xfb,
	0x37,0x77,0x3f,0x22,0x67,0x27,0x6f,0x72,0x3a,0x7a,0x32,0x2f,0x6a,0x2a,0x62,0x7f,
	0xb9,0xf9,0xb1,0xa0,0xe9,0xa9,0xe1,0xf0,0xb8,0xf8,0xb0,0xa1,0xe8,0xa8,0xe0,0xf1,
	0x5d,0x1d,0x55,0x84,0xcd,0x8d,0xc5,0x14,0x5c,0x1c,0x54,0x85,0xcc,0x8c,0xc4,0x15,
	0xbd,0xfd,0xb5,0xa4,0xed,0xad,0xe5,0xf4,0xbc,0xfc,0xb4,0xa5,0xec,0xac,0xe4,0xf5,
	0x39,0x79,0x31,0x20,0x69,0x29,0x61,0x70,0x38,0x78,0x30,0x21,0x68,0x28,0x60,0x71,
	0xb7,0xf7,0xbf,0xa2,0xe7,0xa7,0xef,0xf2,0xba,0xfa,0xb2,0xaf,0xea,0xaa,0xe2,0xff
};

#if 0
#if (CSS_DEBUG & 0x01)
#define _CSSPrintDebug(fmt, arg...) (fprintf (stderr, fmt"\n", ##arg);)
#else
#define _CSSPrintDebug(fmt, arg...)
#endif

#if (CSS_DEBUG & 0x10)
#define _CSSPrintDebug2(fmt, arg...) (fprintf (stderr, fmt"\n", ##arg);)
#else
#define _CSSPrintDebug2(fmt, arg...)
#endif
#endif


void css_init_tables (void)
{
	int a[8], b[8];
	int i=0;
	int s;

	for (i=0; i<=255; i++) {
		r_tbl[i] = 0;

		for (s=0; s<=7; s++) {
			a[s] = (i >> s) & 1;
			r_tbl[i] = r_tbl[i] | (a[s] << (7 - s));
		}

#define ND(i,o,s) ((i[o] & i[o+1]) ^ a[s] ^ 1)

		b[0] = ND(a,0,3);
		b[1] = ND(a,4,6);
		b[2] = ND(b,0,5);
		b[3] = ND(b,0,1);
		b[4] = ND(a,0,2);
		b[5] = ND(a,4,7);
		b[6] = ND(b,4,0);
		b[7] = ND(a,4,4);

		//for (s=0; s<=7; s++)
			//p_tbl[i] = p_tbl[i] | (b[s] << (7 - s));
	}
}

/**
 * this function is only used internally when decrypting title key
 **/

static __inline void _CSSKeyGenerate (uint8_t *key, uint8_t *in, uint8_t invert)
{
	int 		i;
	uint8_t 		k[LEN_KEY];
	int val;
	unsigned int	lfsr0, lfsr1;
	uint8_t         	o_lfsr0, o_lfsr1;

	lfsr0 = ((in[4] << 17) | (in[3] << 9) | (in[2] << 1)) + 8 - (in[2] & 7);
       	lfsr0 = (r_tbl[lfsr0&0xff]<<17) | (r_tbl[(lfsr0>>8)&0xff] << 9)
		  | (r_tbl[(lfsr0>>16)&0xff]<<1) |(lfsr0>>24);

	lfsr1 = (r_tbl[ in[0] ] << 9) | 0x100 | r_tbl[ in[1]];

//	_CSSPrintDebug ("CSSKeyGen:\nSEED lfsr0:0x%08x lfsr1: 0x%08x", lfsr0, lfsr1);

	val = 0;
	for (i = 0; i < LEN_KEY; ++i) {

   		o_lfsr0 = (lfsr0 >> 12) ^ (lfsr0 >> 4) ^ (lfsr0 >> 3) ^ lfsr0;

		o_lfsr1 = ((lfsr1 >> 14) & 7) ^ lfsr1;
		o_lfsr1 ^= (o_lfsr1 << 3) ^ (o_lfsr1 << 6);

		lfsr1 = (lfsr1 >> 8) ^ (o_lfsr1 << 9);
		lfsr0 = (lfsr0 >> 8) ^ (o_lfsr0 << 17);

//		_CSSPrintDebug ("lfsr0:0x%08x lfsr1: 0x%08x o_lfsr0:0x%02x o_lfsr1:0x%02x", lfsr0, lfsr1, o_lfsr0, o_lfsr1);

		val += (o_lfsr0 ^ invert) + o_lfsr1;

		k[i] = val & 0xff;
		val >>= 8;
	}

	key[4]=k[4]^csstab1[key[4]]^key[3];
	key[3]=k[3]^csstab1[key[3]]^key[2];
	key[2]=k[2]^csstab1[key[2]]^key[1];
	key[1]=k[1]^csstab1[key[1]]^key[0];
	key[0]=k[0]^csstab1[key[0]]^key[4];

	key[4]=k[4]^csstab1[key[4]]^key[3];
	key[3]=k[3]^csstab1[key[3]]^key[2];
	key[2]=k[2]^csstab1[key[2]]^key[1];
	key[1]=k[1]^csstab1[key[1]]^key[0];
	key[0]=k[0]^csstab1[key[0]];

//	_CSSPrintDebug ("key: %02x %02x %02x %02x %02x", key[0], key[1], key[2], key[3], key[4]);
}


/**
 * this function decrypts a title key with the specified disc key
 *
 * key_title: the unobfuscated title key (XORed with BusKey)
 * key_disc: the unobfuscated disc key (XORed with BusKey)
 *       2048 bytes in length (though only 5 bytes are needed, see below)
 * pkey: array of pointers to player keys and disc key offsets
 *
 *
 * use the result returned in key_title with css_descramble
 **/

/* if != 0, all keys are debuged */
#define TRYALLKEYS 0

int CSSDecryptTitleKey (int8_t *key_title, int8_t *key_disc)
{
	static uint8_t tmp1[6], tmp3[6];
	int i;

#if (TRYALLKEYS)
	uint8_t key_right[LEN_KEY];
	int found_one = 0;
#endif

	for (i=0; playerkeys[i].offset!=-1; ++i) {
		memcpy (tmp1, key_disc + playerkeys[i].offset, LEN_KEY);
		_CSSKeyGenerate (tmp1, playerkeys[i].key, 0);

		memcpy (tmp3, key_disc, LEN_KEY);
		_CSSKeyGenerate (tmp3, tmp1, 0);

		if (memcmp (tmp3, tmp1, LEN_KEY) == 0) {
#if (!TRYALLKEYS)
//			_CSSPrintDebug ("Using Key %3d\n", i+1);
			goto found_key;
#else
//			_CSSPrintDebug ("Valid key[%3d]: %02x %02x %02x %02x %02x",
//				i+1,
//				playerkeys[i].offset, playerkeys[i].key[0], playerkeys[i].key[1],
//				playerkeys[i].key[2], playerkeys[i].key[3], playerkeys[i].key[4]);
			found_one = i + 1;
			memcpy (key_right, tmp1, LEN_KEY);
               } else {
//			_CSSPrintDebug ("Invalid Key[%3d]: [%02x,%02x,%02x,%02x,%02x]\n",
//				i+1,
//				playerkeys[i].offset, playerkeys[i].key[0], playerkeys[i].key[1],
//				playerkeys[i].key[2], playerkeys[i].key[3], playerkeys[i].key[4]);
#endif
		}
	}

#if (TRYALLKEYS)
	if (found_one) {
		memcpy (tmp1, key_right, LEN_KEY);
//		_CSSPrintDebug ("Using Key %d\n", found_one);
		goto found_key;
	}
#endif

//	_CSSPrintDebug ("No suitable Key found.");
	return -1;

found_key:
	memcpy (tmp3, key_title, LEN_KEY);
	_CSSKeyGenerate (tmp3, tmp1, 0xff);
	memcpy (key_title, tmp3, LEN_KEY);

	return 0;
}


/**
 * this function does the actual descrambling
 *
 * buf_sec: encrypted sector (2048 bytes)
 * key: decrypted title key obtained from CSSDecryptTitleKey
 **/

void CSSDescramble (uint8_t *sec, uint8_t *key)
{
        uint8_t		*buf_end = sec + 0x800;
 	int		val;
	unsigned int	lfsr0, lfsr1;
	uint8_t		o_lfsr0, o_lfsr1;
	uint8_t		off;

	off = (sec[13] & 0x07) + 20;

	// return if sector is not scambled
	// 00 ... unencrypted
	// 01 ... CSS (when used for DVDs)
	// 10 ... ??
	// 11 ... CPRM

	if ((sec[off] & 0x30) != 0x10)
		return;

	// remove scramble bits
	sec[off] &= 0xcf;

#define SLT(i,s) ((key[i] ^ sec[0x54 + (i)]) << s)
#define REV(i,s) (r_tbl[(lfsr0>>i)&0xff]<<s)
#define RSV(i,s) (r_tbl[SLT(i,0)]<<s)

	lfsr0 = SLT(4,17) | SLT(3,9) | SLT(2,1);
	lfsr0 = lfsr0 + 8 - (SLT(2,0)&0x07);
	lfsr0 = REV(0,17) | REV(8,9) | REV(16,1) | lfsr0>>24;
	lfsr1 = RSV(0,9) | 0x100 | RSV(1,0);

	sec+=0x80;
	val = 0;
	while (sec != buf_end) {
   		o_lfsr0 = (lfsr0 >> 12) ^ (lfsr0 >> 4) ^  (lfsr0 >> 3) ^ lfsr0;
		o_lfsr1 = ((lfsr1 >> 14) & 7) ^ lfsr1;
		o_lfsr1 ^= (o_lfsr1 << 3) ^ (o_lfsr1 << 6);

		lfsr0 = (lfsr0 >> 8) ^ (o_lfsr0 << 17);
		lfsr1 = (lfsr1 >> 8) ^ (o_lfsr1 << 9);

   		val += o_lfsr0 + (uint8_t)~o_lfsr1;
		*sec++ = csstab1[*sec] ^ (val & 0xff);
		val >>= 8;
	}
}

