#define SUN
/* *****************************************************************
 * environ.h
 *   Public domain
 *
 * --ABSTRACT-- environ.h
 * These are the definitions that define the environment that
 * the compile is happening in.
 *
 * --KEYWORDS-- environ.h
 * Portability, standards
 * 
 * --CONTENTS-- environ.h
 * Date, Department, Author
 *    26APR1991, John Halleck
 * Revision history
 *    For each revision: Date, change summary, authorizing document,
 *    change department, section, author
 *    26APR1991, Initial Creation, John Halleck
 *    30APR1991, Added Ultrix.
 *    18OCT1994, Fixed some of the PC defs.
 *    17NOV1994, Added HP.
 *    19dec1994, added NOVOID
 *    20apr1994, added PROTOTYPES, ARGCARGV,
 *               in addition to NOPROTOTYPES
 * Unit purpose
 *    (What does this do?)
 *    Any routines that use non-standard features use these defines
 *    to turn them on.  A unit that does not include this should be
 *    standard.  One that includes this is standard if none of the
 *    tags are uncommented.
 * External Units accessed
 *    Name, purpose, access summary
 *    [None]
 * Exceptions propagated by this unit
 *    [None]
 * Machine-dependencies
 *    Access type, purpose, and justification
 *    [None]
 * Compiler-dependencies
 *    Everything here is machine specific...
 ********************************************************************
 */
#ifndef ENVIRONMENT
#define ENVIRONMENT

/* There are more tags here than may be used in the project that you
 * are compiling.   Rather than have such a file for each project,
 * all of my projects make use of this same one, and a tag may be
 * here if it is used in ANY project that I have.
 */

/* The defaults here are as generic as I can make them.  However, you
 * should set your machine to make sure that things are correct.
 * 
 * *********************************************************************
 * You should check that the BIT8, BIT16, and BIT32 types are correct. *
 ***********************************************************************
 */

/* machine   ---------------------------------------------------
 * If this is being compiled for a Macintosh, define the macro as
 * one of the following
 *
 *  #define MACINTOSH
 *     (Generic Mac)
 *  (No others yet used)
 */

/* or -----------------------------------------------------------
 * if this is being compiled for a PC define the macro as
 * one of the following
 *  #define PC
 *     (Generic PC)
 *  #define PC SPERRY
 *     (Sperry's PC)
 */

/* or -----------------------------------------------------------
 * if this is being compiled for a Sun workstation
 *  #define SUN
 *      (Generic sun)
 *  #define SUN 3
 *      A sun 3/xxx
 *  #define SUN 4
 *      A sun 4 (including SparcStations)
 *  #define SUN SOLARIS
 */

/* or ----------------------------------------------------------- 
 * if this is being compiled for a Univac 1100 series machine.
 * #define UNIVAC
 *      (Generic Univac)
 */

/* or -----------------------------------------------------------
 * if this is being compiled for a DEC machine.
 * #define DEC
 *      (Generic Univac)
 * #define DEC PDP11
 * #define DEC PDP10
 * #define DEC PDP20
 * #define DEC ALPHA
 * #define DEC VAX
 */
 
/* OR ----------------------------------------------------------- 
 * If this is being compiled for an HP machine.
 * #define HP
 *      (Generic HP, whatever that is)
 * #define HP 9000
 */

/* end machines  ------------------------------------------------ */


/* ***** PURE derivation ******  */
/* These define the following:
 * BITS8  unsigned 8 bit quantity.
 * BITS16 unsigned 16 bit quantity.
 * BITS32 unsigned 32 bit quantity.
 * BITS32_GREATERTHAN_32  If BITS32 is in a word physically bigger 
 *                        than 32 bits long. (As on a 36 bit word machine)
 *
 * NOPROTOTYPES  If the machine does not support ANSI C prototypes.
 *   PROTOTYPES  If the machine does     support ANSI C prototypes.
 * NOARGCARGV    if the machine does not support UNIX style argc, argv.
 *   ARGCARGV    if the machine does     support UNIX style argc, argv.
 *
 * NOCONST       if the machine does not support the  const    qualifier
 * NOVOLATILE    if the machine does not support the  volatile qualifier
 * NOVOID        if the machine does not support the  void     data type.
 */

#ifdef PC
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define ARGCARGV
#define PROTOTYPES
#endif

#ifdef UNIVAC
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define BIT32_GREATERTHAN_32
#define ARGCARGV
#define NOPROTOTYPES
#endif /* UNIVAC */

#ifdef SUN
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define ARGCARGV
#define NOPROTOTYPES
#endif /* SUN */

#ifdef DEC
/* ??? */
#endif
#if DEC == VAX
#define VMS
/* Somebody needs to fill these in. */
/* ???????? */
#endif
#if DEC == ALPHA
/* ??? */
#endif
#endif

#ifdef MACINTOSH
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define NOARGCARGV
#define PROTOTYPES
#endif

#ifdef HP
#if HP == 9000
#define HPUX
#endif
#endif

#ifdef HPUX
#define BITS8  unsigned char
#define BITS16 unsigned short
#define BITS32 unsigned long
#define ARGCARGV
#define PROTOTYPES
#endif

/* ----------------- Pure defaults --------------------- */
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

/* ---------------- Housekeeping -------------------------- */

#ifdef  NOVOID
typedef int void; /* we have to declare it as something */
#endif

/* We know how to do standard C */
#ifndef _STDC_
#define NOCONST
#define NOVOLATILE
#else
#ifdef NOPROTOTYPES
#undef NOPROTOTYPES
#define  PROTOTYPES
#endif
#endif

#ifdef  NOCONST
#define const
#endif

#ifdef NOVOLATILE
#define volatile
#endif

/* ENVIRONMENT */
/* end environ.h **************************************************** */
