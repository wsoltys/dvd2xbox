/* *****************************************************************
 * sha.c
 * (C) Copyright 1994 John Halleck
 * All rights reserved
 *
 * --ABSTRACT-- sha.c
 * This implements the "Secure Hash Algorithm" as per the original
 * NIST specs, with the latest NIST version as an option.
 *
 * --KEYWORDS-- sha.c
 *
 * --CONTENTS-- sha.c
 * Date, Department, Author
 *   20may1994, John Halleck
 * Revision history
 *   For each revision: Date, change summary, authorizing document,
 *   change department, section, author
 *    3nov1999, Corrected padding bug.
 *   20apr1995, change to default context option, John Halleck.
 *   19nov1994, portablility changes, output string code.
 *    1aug1994, Changes per NIST
 *   20may1994, Initial Creation, John Halleck
 * Unit purpose
 *   (What does this do?)
 *   Implements the "Secure Hash Algorithm" as proposed by NSA
 * Unit function
 *   (How does it do it?)
 *   Straight forward reduction of the spec to code.
 * External Units accessed
 *   Name, purpose, access summary
 * Exceptions propagated by this unit
 *   [None]
 * Input Output
 *   Device name, Access type, Access purpose, access summary
 *   [None]
 * Machine-dependencies
 *   Access type, purpose, and justification
 *   Assumes a 32 bit word is availiable.
 * Compiler-dependencies
 *   [None]
 *******************************************************************
 */
#include <string.h>
#include "environ.h"

/* This code is independent of the byte order of the
 * underlying machine.  (Bigendian and Littleendian were
 * bad enough, but when I started to deal with machines like
 * the PDP-11 getting the order right became difficult without
 * a redesign...
 */
 
/* Public part of this unit. */
#include "sha.h"

/*#ifndef NOPROTOTYPES
// Private part of this unit 
void shapad        (sha_context_ptr context);
void shaTransform  (sha_context_ptr context);
void shaTransformCanned                     (void);
#endif*/


#define SHF_LENGTH_PAD  8
#define SHF_PAD_WORDS 2
/* Part of the padding needs to be message length (2  4 byte words) */
#define SHF_PADDING (SHF_BLOCKSIZE - SHF_LENGTH_PAD)
/* Remainder of block to be padded */

/* We have to exercise special care on machines with a natural
 * wordlength >32 characters that we do masking properly
 */
#ifdef BITS32_GREATERTHAN_32
#define MASK32 & 0xFFFFFFFF
#define MASK8 & 0xFF
#else
#define MASK32
#define MASK8
#endif

int debugi; /* DEBUG */

static sha_context canned_context;
/* Place for contextless calls to have context. */



/* Implementation */

/*#ifdef NOPROTOTYPES
void shaInit (context, version)
sha_context_ptr context;
int version;
{
#else*/
void shaInit (sha_context_ptr context, int version) {
//#endif
 int loopindex;

 if (!context) context = &canned_context;
 
 /* Set initial values */
 context->digest[0] = 0x67452301L;
 context->digest[1] = 0xEFCDAB89L;
 context->digest[2] = 0x98BADCFEL;
 context->digest[3] = 0x10325476L;
 context->digest[4] = 0xC3D2E1F0L;

 /* Initialize bit count */
 context->countLo = 0L;
 context->countHi = 0L;
 
 /* Initialize buffer to empty. */
 context->thisword = 0;
 context->thisbyte = 0;
 
 /* Zero out data */
 for (loopindex=0;loopindex<SHF_BLOCKWORDSIZE;loopindex++)
     context->data[loopindex] = 0;

 /* What sort of SHA are we doing? */
 context->kind = version;
 
}

/* 
**************************************************************** 
*/

/* Rotate Left n bits (32 bit) */
#define S(n,X) (((X)<<(n)) | ((X)>>(32-(n))))

/*#ifdef NOPROTOTYPES
static void shaTransform (context)
sha_context_ptr context;
{
#else*/
static void shaTransform (sha_context_ptr context) {
//#endif

 BITS32 W[80], temp;  int i;

/* A buffer of 5 32-bit words */
BITS32 A, B, C, D, E;

#define DG   context->digest

 if (!context) context = &canned_context;

/* Get digest */
 A = DG[0]; B = DG[1]; C = DG[2]; D = DG[3]; E = DG[4];

 /* Copy the data buffer into the work buffer */
 for (i=0; i<SHF_BLOCKWORDSIZE; i++)
     {W[i] = context->data[i];context->data[i] = 0;}
 
/* Expand the 16 words into the SHF_BLOCKSIZE temporary data words */
 for (i=16; i<80; i++) {
     W[i] = W[i-3]^W[i-8]^W[i-14]^W[i-16];
     if (context->kind) W[i] = S(1,W[i]);
 }
 
/* Guts (four sub-rounds) */
#define PRE  temp = W[i] + S(5,A) + E + 
#define POST ; E = D; D = C; C = S(30,B); B = A; A = temp;
 for (i= 0; i<20; i++) {PRE 0x5A827999L + (( B & C ) | (~B & D))  POST}
 for (i=20; i<40; i++) {PRE 0x6ED9EBA1L + (B^C^D)                 POST}
 for (i=40; i<60; i++) {PRE 0x8F1BBCDCL + ((B&C) | (B&D) | (C&D)) POST}
 for (i=60; i<80; i++) {PRE 0xCA62C1D6L + (B^C^D)                 POST}

/* Update digest */
 DG[0] += A; DG[1] += B; DG[2] += C; DG[3] += D; DG[4] += E;

#if BITS32_GREATERTHAN_32
 DG[0] &= 0xFFFFFFFF;
 DG[1] &= 0xFFFFFFFF;
 DG[2] &= 0xFFFFFFFF;
 DG[3] &= 0xFFFFFFFF;
 DG[4] &= 0xFFFFFFFF;
#endif

/* Block is now empty */
 context->thisword = 0;
 context->thisbyte = 0;
}


/* ******************************* */

/*#ifdef NOPROTOTYPES
void shaUpdate (context, buffer, count)
sha_context_ptr context;
BITS8 *buffer;
int count;
{
#else*/
void shaUpdate (sha_context_ptr context, BITS8 *buffer, int count) {
//#endif

 BITS32 thebits; /* current bit pattern being processed */
 int theword; /* Which word in the buffer we are dealing with. */
// int i;
 
 if (!context) context = &canned_context;

 /* Add a potentially 32 bit count to the two word count */
 context->countHi += count >> 29;           /* handle count > 2**29 */
 context->countLo += count & 0x1FFFFFFF;    /* Handle count <=2**29 */
 context->countHi += context->countLo >> 29;/* Handle carry */
 context->countLo &= 0x1FFFFFFF;            /* Clear carry  */

 theword = context->thisword;
 thebits = context->data[theword];

 while (count--) {
   thebits = (thebits << 8) | *buffer++;
   if (++context->thisbyte >= 4) {
      context->data[theword++] = thebits; thebits = 0;
      if (theword >= SHF_BLOCKWORDSIZE) { 
         shaTransform (context);
         theword = 0;
      }
      context->thisbyte = 0;
   }
 }
 context->data[theword] = thebits;
 context->thisword = theword;

 }
/* ************************************ */
/*#ifdef NOPROTOTYPES
void shaStrUpdate  (context, input)
sha_context_ptr context;
char *input;
#else*/
void shaStrUpdate  (sha_context_ptr context, char *input)
//#endif
{shaUpdate (context, (BITS8 *)input, strlen(input));}

/* ************************************ */
/* Pad out a block. */

/*#ifdef NOPROTOTYPES
static void shapad(context)
sha_context_ptr context;
{
#else*/
static void shapad(sha_context_ptr context) {
//#endif
  int loopindex;

  if (!context) context = &canned_context;

  context->data[context->thisword]<<=8;
  context->data[context->thisword] |= 0x80;
  /* pad out the rest of this word */
  switch (context->thisbyte) {
    case 3:                                      ;break;
    case 2: context->data[context->thisword]<<= 8;break;
    case 1: context->data[context->thisword]<<=16;break;
    case 0: context->data[context->thisword]<<=24;break;
  }

  /* and then the rest of the words in the block */
  for (loopindex=context->thisword + 1; loopindex < SHF_BLOCKWORDSIZE;loopindex++)
     context->data[loopindex] = 0;

  /* And note it is now empty */
  context->thisword = 0;  context->thisbyte = 0;
}
/* ************************************ */
/* Convert a word digest to bytes, in a byte order independent manner */

/*#ifdef NOPROTOTYPES
void shaBytes (context, adigest)
sha_context_ptr context;
sha_digest adigest;
{
#else */
void shaBytes (sha_context_ptr context, sha_digest adigest) {
//#endif
int loopindex;

 if (!context) context = &canned_context;

 for (loopindex=0;loopindex<SHF_DIGESTWORDSIZE;loopindex++) {
     *adigest++ = (BITS8) (context->digest[loopindex] >> 24   MASK8);
     *adigest++ = (BITS8) (context->digest[loopindex] >> 16  & (BITS32) 0xFF);
     *adigest++ = (BITS8) (context->digest[loopindex] >>  8  & (BITS32) 0xFF);
     *adigest++ = (BITS8) (context->digest[loopindex]        & (BITS32) 0xFF);
 }

}

/* ************************************ */
/*#ifdef NOPROTOTYPES
void shaString (adigest, outputstring)
sha_digest adigest;
char *outputstring;
{
#else*/
void shaString (sha_digest adigest, char *outputstring) {
//#endif
  char output [17];
  int loop; int abyte;

  strcpy (output, "0123456789ABCDEF");

  for (loop = 0; loop < SHF_DIGESTSIZE; loop++) {
     abyte = *adigest++;
     *outputstring++ = output [ (abyte >> 4) & 0xF ];
     *outputstring++ = output [  abyte       & 0xF ];
  }
  *outputstring = 0;
}
/* ************************************ */

/*#ifdef NOPROTOTYPES
void shaFinal(context, adigest)
sha_context_ptr context;
sha_digest adigest;
{
#else*/
void shaFinal (sha_context_ptr context, sha_digest adigest) {
//#endif
 int loopindex; int bytesused;

 if (!context) context = &canned_context;

 /* bytes used in the buffer */
 bytesused = context->thisword * 4 + context->thisbyte + 2;
   /* +1 for un zerobasing, +1 for the pad character we're about to do */
 
 /* Pad, but with the convention that the 0 pad starts with a single */
 /* one bit. */
 shapad(context);

 /* if we don't have room for the message size, then start a new block */
 if (bytesused > (SHF_BLOCKSIZE - SHF_LENGTH_PAD)) {
    shaTransform(context);
    for (loopindex=0; loopindex<(SHF_BLOCKWORDSIZE-SHF_PAD_WORDS); loopindex++) 
        context->data[loopindex] = 0;
    context->thisword = SHF_BLOCKWORDSIZE;
    context->thisbyte = 0;
 }

 /* Append length in bits, and transform */
 context->data[14] = context->countHi;
 context->data[15] = context->countLo<<3; /* We keep it as a byte count */
 shaTransform (context);

 /* get the digest out to the user. */
 shaBytes (context, adigest);

} /* end routine shaFinal */


/* ************************************************************ */

/*#ifdef NOPROTOTYPES
void shaHash (version, buffer, count, adigest)
int version;
BITS8 *buffer;
int count;
sha_digest adigest;
{
#else*/
void shaHash (int version, BITS8 *buffer, int count, sha_digest adigest) {
//#endif
 sha_context example;

 shaInit   (&example, version);
 shaUpdate (&example, buffer, count);
 shaFinal  (&example, adigest);

}

/* end sha.c **************************************************** */
