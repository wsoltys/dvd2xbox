/*
 *     ApplyPPF3.c
 *     written by Icarus/Paradox
 *     suggestions and some fixes by <Hu Kares>, thanks.
 *
 *     Applies PPF1.0, PPF2.0 & PPF3.0 Patches (including PPF3.0 Undo support)
 *     Feel free to use this source in and for your own
 *     programms.
 *
 *     Visual C++ is needed in order to compile this source.
 *
 */

#ifndef D2XAPPLYPPF3
#define D2XAPPLYPPF3

#include <xtl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "d2xlogger.h"

class D2Xppf
{
protected:

	//////////////////////////////////////////////////////////////////////
	// Used global variables.
	int ppf, bin;
	char binblock[1024], ppfblock[1024];
	unsigned char ppfmem[512];
	#define APPLY 1
	#define UNDO 2

	D2Xlogger	p_log;

	//////////////////////////////////////////////////////////////////////
	// Used prototypes.
	int		OpenFiles(char* file1, char* file2);
	int		PPFVersion(int ppf);
	void	ApplyPPF1Patch(int ppf, int bin);
	void	ApplyPPF2Patch(int ppf, int bin);
	void	ApplyPPF3Patch(int ppf, int bin, char mode);
	int		ShowFileId(int ppf, int ppfver);
	


public:
	D2Xppf();
	~D2Xppf();
	int		ApplyPPF3(char mode, char *binfile, char *patchfile);
	
	
};

#endif