/*
 * Copyright (C) 1999 Derek Fawcus <derek@spider.com>
 *
 * This code may be used under the terms of Version 2 of the GPL,
 * read the file COPYING for details.
 *
 */

/*
 * These routines do some reordering of the supplied data before
 * calling engine() to do the main work.
 *
 * The reordering seems similar to that done by the initial stages of
 * the DES algorithm, in that it looks like it's just been done to
 * try and make software decoding slower.  I'm not sure that it
 * actually adds anything to the security.
 *
 * The nature of the shuffling is that the bits of the supplied
 * parameter 'varient' are reorganised (and some inverted),  and
 * the bytes of the parameter 'challenge' are reorganised.
 *
 * The reorganisation in each routine is different,  and the first
 * (CryptKey1) does not bother of play with the 'varient' parameter.
 *
 * Since this code is only run once per disc change,  I've made the
 * code table driven in order to improve readability.
 *
 * Since these routines are so similar to each other,  one could even
 * abstract them all to one routine supplied a parameter determining
 * the nature of the reordering it has to do.
 */

//#include <inttypes.h>

typedef char int8_t;
typedef unsigned char uint8_t;
typedef unsigned long uint32_t;
#include "css_auth.h"
#include "css_tbl.h"

#pragma warning (disable:4244)
extern uint8_t r_tbl[];

/**
 * We use two LFSR's (seeded from some of the input data uint8_t) to
 * generate two streams of pseudo-random bits.  These two bit streams
 * are then combined by simply adding with carry to generate a final
 * sequence of pseudo-random bits which is stored in the buffer that
 * 'output' points to the end of - len is the size of this buffer.
 *
 * The first LFSR is of degree 25,  and has a polynomial of:
 * x^13 + x^5 + x^4 + x^1 + 1
 *
 * The second LSFR is of degree 17,  and has a (primitive) polynomial of:
 * x^15 + x^1 + 1
 *
 * I don't know if these polynomials are primitive modulo 2,  and thus
 * represent maximal-period LFSR's.
 *
 *
 * Note that we take the output of each LFSR from the new shifted in
 * bit,  not the old shifted out bit.  Thus for ease of use the LFSR's
 * are implemented in bit reversed order.
 *
 */

static void _generate_bits (uint8_t *output, int len,  const byte *s)
{
 	uint32_t lfsr0, lfsr1;
	int val;
	byte o_lfsr0, o_lfsr1;

	/* In order to ensure that the LFSR works we need to ensure that the
	 * initial values are non-zero.  Thus when we initialise them from
	 * the seed,  we ensure that a bit is set.
	 */

	lfsr0 = (s[0] << 17) | (s[1] << 9) | ((s[2] & ~7) << 1) | 8 | (s[2] & 7);

	/*
	 	reverse lfsr0/1 to simplify calculation in loop
	*/
	lfsr0 = (r_tbl[lfsr0&0xff]<<17) | (r_tbl[(lfsr0>>8)&0xff] << 9)
		  | (r_tbl[(lfsr0>>16)&0xff]<<1) |(lfsr0>>24);

	lfsr1 = (r_tbl[s[4]] << 9) | 0x100 | (r_tbl[ s[3]]);

	output++;
	val = 0;

	do {
		o_lfsr0 = (lfsr0 >> 12) ^ (lfsr0 >> 4) ^  (lfsr0 >> 3) ^ lfsr0;

		o_lfsr1 = ((lfsr1 >> 14) & 7) ^ lfsr1;
		o_lfsr1 ^= (o_lfsr1 << 3) ^ (o_lfsr1 << 6);

		lfsr1 = (lfsr1 >> 8) ^ (o_lfsr1 << 9);
		lfsr0 = (lfsr0 >> 8) ^ (o_lfsr0 << 17);

		o_lfsr0 = ~o_lfsr0;
		o_lfsr1 = ~o_lfsr1;

		val += o_lfsr0 + o_lfsr1;
	
		*--output = val & 0xFF;
		val >>= 8;
#if (CSS_DEBUG & 2)
		fprintf(stderr, "lfsr0=%08x lfsr1=%08x\n", lfsr0, lfsr1);
#endif

	} while (--len > 0);
}


/**
 * This encryption engine implements one of 32 variations
 * one the same theme depending upon the choice in the
 * varient parameter (0 - 31).
 *
 * The algorithm itself manipulates a 40 bit input into
 * a 40 bit output.
 * The parameter 'input' is 80 bits.  It consists of
 * the 40 bit input value that is to be encrypted followed
 * by a 40 bit seed value for the pseudo random number
 * generators.
 **/

static __inline void mangle1( uint8_t *bs, uint8_t cse, const uint8_t *ip, uint8_t *op)
{
	int i, term, index;
	for (i = 5, term = 0; --i >= 0; term = ip[i]) {
		index = bs[i] ^ ip[i];
		index = CSSmangle1[index] ^ cse;
		op[i] = CSSmangle2[index] ^ term;
        }	
}


/**
 *
 **/

static __inline void mangle2( uint8_t *bs, uint8_t cse, const uint8_t *ip, uint8_t *op)
{
	int i, term, index;
	for (i = 5, term = 0; --i >= 0; term = ip[i]) {
		index = bs[i] ^ ip[i];
		index = CSSmangle1[index] ^ cse;
		index = CSSmangle2[index] ^ term;
                op[i] = CSSmangle0[index];
        }	
}


/**
 *
 **/

static __inline void engine (int varient, uint8_t const *input, uint8_t *output)
{
	uint8_t cse;
	uint8_t tmp1[5];
	uint8_t tmp2[5];
	uint8_t bits[30];

	int i;

	/* Feed the secret into the input values such that
	 * we alter the seed to the LFSR's used above,  then
	 * generate the bits to play with.
	 */
	for (i=5; --i>=0; )
		tmp1[i] = input[5+i] ^ CSSsecret[i];

	_generate_bits (&bits[29], sizeof bits, &tmp1[0]);

	/* This term is used throughout the following to
	 * select one of 32 different variations on the
	 * algorithm.
	 */
	cse = CSSvarients[varient];

	/* Now the actual blocks doing the encryption.  Each
	 * of these works on 40 bits at a time and are quite
	 * similar.
	 */
	mangle1( &bits[25], cse, input, tmp1);
	tmp1[4] ^= tmp1[0];

	mangle1( &bits[20],  cse, tmp1, tmp2);
	tmp2[4] ^= tmp2[0];

	mangle2( &bits[15],  cse, tmp2, tmp1);
	tmp1[4] ^= tmp1[0];

	mangle2( &bits[10],  cse, tmp1, tmp2);
	tmp2[4] ^= tmp2[0];

	mangle1( &bits[5],  cse, tmp2, tmp1);
	tmp1[4] ^= tmp1[0];
	mangle1( &bits[0],  cse, tmp1, output);
}


/**
 *
 **/

void CryptKey1 (int varient, uint8_t const *challenge, byte *key)
{
	uint8_t perm_challenge[] = {1,3,0,7,5, 2,9,6,4,8};

	uint8_t scratch[10];
	int i;

	for (i=9; i>=0; --i)
		scratch[i] = challenge[perm_challenge[i]];

	engine (varient, scratch, key);
}


/**
 * This shuffles the bits in varient to make perm_varient such that
 *                4 -> !3
 *                3 ->  4
 * varient bits:  2 ->  0  perm_varient bits
 *                1 ->  2
 *                0 -> !1
 **/

void CryptKey2 (int varient, uint8_t const *challenge, byte *key)
{
	static uint8_t perm_challenge[] = {
		0x06,0x01,0x09,0x03,0x08,0x05,0x07,0x04,
		0x00,0x02
	};
	static uint8_t perm_varient[] = {
		0x0a,0x08,0x0e,0x0c,0x0b,0x09,0x0f,0x0d,
		0x1a,0x18,0x1e,0x1c,0x1b,0x19,0x1f,0x1d,
		0x02,0x00,0x06,0x04,0x03,0x01,0x07,0x05,
		0x12,0x10,0x16,0x14,0x13,0x11,0x17,0x15
	};
	uint8_t scratch[10];
	int i;

	for (i=9; i>=0; --i)
		scratch[i] = challenge[perm_challenge[i]];

	engine (perm_varient[varient], scratch, key);
}


/**
 * This shuffles the bits in varient to make perm_varient such that
 *                4 ->  0
 *                3 -> !1
 * varient bits:  2 -> !4  perm_varient bits
 *                1 ->  2
 *                0 ->  3
 **/

void CryptBusKey (int varient, uint8_t const *challenge, byte *key)
{
	static uint8_t perm_challenge[] = {
		0x04,0x00,0x03,0x05,0x07,0x02,0x08,0x06,
		0x01,0x09
	};
	static uint8_t perm_varient[] = {
		0x12,0x1a,0x16,0x1e,0x02,0x0a,0x06,0x0e,
		0x10,0x18,0x14,0x1c,0x00,0x08,0x04,0x0c,
		0x13,0x1b,0x17,0x1f,0x03,0x0b,0x07,0x0f,
		0x11,0x19,0x15,0x1d,0x01,0x09,0x05,0x0d
	};
	uint8_t scratch[10];
	int i;

	for (i=9; i>=0; --i)
		scratch[i] = challenge[perm_challenge[i]];

	engine (perm_varient[varient], scratch, key);
}

