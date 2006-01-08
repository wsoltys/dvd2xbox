/* *****************************************************************
 * sha.h
 * © Copyright 1994 John Halleck
 * All rights reserved
 *
 * --ABSTRACT-- sha.h
 * This implements the "Secure Hash Algorithm" as per the original
 * NIST specs, with the latest NIST version as an option.
 *
 * --KEYWORDS-- sha.h
 *
 * --CONTENTS-- sha.h
 * Date, Department, Author
 *    20may1994, John Halleck
 * Revision history
 *    For each revision: Date, change summary, authorizing document,
 *    change department, section, author
 *    20may1994, Initial Creation, John Halleck
 *     1aug1994, Changes per NIST
 *    19nov1994, portablility changes, output string code.
 *   20apr1995, change to default context option, John Halleck.
 * Unit purpose
 *    (What does this do?)
 *    Secure hash function with the NIST modifications, and renamed
 *    routines.
 * External Units accessed
 *    Name, purpose, access summary
 *    [None]
 * Exceptions propagated by this unit
 *    [None]
 * Machine-dependencies
 *    Access type, purpose, and justification
 *    Assumes there is a 32 bit word availiable on the machine.
 * Compiler-dependencies
 *    [None]
 ********************************************************************
 */

/* Random note: This standard was originally proposed to provide a hash function
 * for the Digital Signature Standard that was proposed in August 1991 
 */
 
#ifndef SHF
#define SHF

#ifndef BITS8
#define BITS8  unsigned char
#endif
#ifndef BITS16
#define BITS16 unsigned int
#endif
#ifndef BITS32
#define BITS32 unsigned long
#endif

#ifndef PROTOTYPES
#define NOPROTOTYPES
#endif

#ifndef ARGCARGV
#define NOARGCARGV
#endif

//#include "environ.h"
/* Defines common to everything */

#define SHF_DIGESTSIZE 20
/* Put out a digest of this size in bytes */
#define SHF_DIGESTWORDSIZE 5
/* and words */

#define SHF_BLOCKSIZE  64
#define SHF_BLOCKWORDSIZE 16
/* Process messages in this block size */

/* Structure for storing SHF info */
typedef BITS32 shadigest[SHF_DIGESTWORDSIZE];
typedef BITS8  sha_digest[SHF_DIGESTSIZE];

typedef struct {
  BITS32    data [SHF_BLOCKWORDSIZE];     /* SHS data buffer */
  BITS32    countLo;       /* Count (64 bits in              */
  BITS32    countHi;       /* 2 32 bit words)                */
  shadigest digest;        /* Message digest                 */
  int       kind;          /* Which type of SHA?             */
  int       thisword;      /* Which word are we processing?  */
  int       thisbyte;      /* Which byte in that word?       */
} sha_context, *sha_context_ptr;

//#ifndef NOPROTOTYPES

/* These routines all take a context, so that multiple streams can be
 * handled.  If you pass NULL, then a single package wide canned context
 * will be used.
 */

/* Initialize a stream */
void shaInit       (sha_context_ptr context, int version);
/* Version 0 - Original NIST version (Obsolete) */
/*             (30 January 1995) */
/* Version 1 - lastest NIST modifications    */

/* Add a buffer's worth of characters to the current hash. */
void shaUpdate     (sha_context_ptr context, BITS8 *buffer, int count);

/* Add a string's worth of characters to the current hash. */
void shaStrUpdate  (sha_context_ptr context, char *input);

/* Done, report the final hash. */
void shaFinal      (sha_context_ptr context, sha_digest adigest);

/* And for all you bit twiddlers out there, the machine independent
 * Digest to digest string routines.
 */
 
/* digest from context */
void shaBytes (sha_context_ptr context, sha_digest adigest);
/* string from digest */
void shaString (sha_digest adigest, char *outputstring);

/* hash of a buffer */
/* This returns directly the SHA digest of the characters in a buffer. */
void shaHash (int version, BITS8 *buffer, int count, sha_digest adigest);

/*#else

extern void shaInit            ();
extern void shaUpdate          ();
extern void shaStrUpdate       ();
extern void shaFinal           ();
extern void shaBytes           ();
extern void shaString          ();
extern void shaHash            ();

#endif */
/* NOPROTOTYPES */

#endif
/* SHF */
/* end sha.h ***************************************************** */
