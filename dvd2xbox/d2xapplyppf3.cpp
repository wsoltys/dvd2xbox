#include "D2Xapplyppf3.h"


D2Xppf::D2Xppf()
{
}


D2Xppf::~D2Xppf()
{	

}


int D2Xppf::ApplyPPF3(char mode, char *binfile, char *patchfile){
	/*p_log.WLog(L"ApplyPPF v3.0 by =Icarus/Paradox= %s", __DATE__);
	if(argc!=4){
		p_log.WLog(L"Usage: ApplyPPF <command> <binfile> <patchfile>");
		p_log.WLog(L"<Commands>");
		p_log.WLog(L"  a : apply PPF1/2/3 patch");
		p_log.WLog(L"  u : undo patch (PPF3 only)");

		p_log.WLog(L"Example: ApplyPPF.exe a game.bin patch.ppf");
		return(0);
	}*/
	int x;
	switch(mode){
			case 'a'	:	if(OpenFiles(binfile, patchfile)) return(0);
							x=PPFVersion(ppf);
							if(x){
								if(x==1){ ApplyPPF1Patch(ppf, bin); break; }
								if(x==2){ ApplyPPF2Patch(ppf, bin); break; }
								if(x==3){ ApplyPPF3Patch(ppf, bin, APPLY); break; }
							} else{ break; }
							break;
			case 'u'	:	if(OpenFiles(binfile, patchfile)) return(0);
							x=PPFVersion(ppf);
							if(x){
								if(x!=3){
									p_log.WLog(L"Undo function is supported by PPF3.0 only");
								} else {
									ApplyPPF3Patch(ppf, bin, UNDO);
								}
							} else{ break; }
							break;

			default		:
							//p_log.WLog(L"Error: unknown command: \"%s\"",argv[1]);
							return(0);
							break;
	}

	_close(bin);
	_close(ppf);
	return(0);
}

//////////////////////////////////////////////////////////////////////
// Applies a PPF1.0 patch.
void D2Xppf::ApplyPPF1Patch(int ppf, int bin){
	char desc[50];
	int pos;
	unsigned int count, seekpos;
	unsigned char anz;


	_lseeki64(ppf, 6,SEEK_SET);  /* Read Desc.line */
	_read(ppf, &desc, 50); desc[50]=0;
	p_log.WLog(L"Patchfile is a PPF1.0 patch. Patch Information:"); 
	p_log.WLog(L"Description : %s",desc);
	p_log.WLog(L"File_id.diz : no");

	p_log.WLog(L"Patching... "); fflush(stdout);
	_lseeki64(ppf, 0, SEEK_END);
	count=_tell(ppf);
	count-=56;
	seekpos=56;
	p_log.WLog(L"Patching ... ");

	do{
		p_log.WLog(L"reading..."); fflush(stdout);
		_lseeki64(ppf, seekpos, SEEK_SET);
		_read(ppf, &pos, 4);
		_read(ppf, &anz, 1);
		_read(ppf, &ppfmem, anz);
		_lseeki64(bin, pos, SEEK_SET);
		p_log.WLog(L"writing..."); fflush(stdout);
		_write(bin, &ppfmem, anz);
		seekpos=seekpos+5+anz;
		count=count-5-anz;
	} while(count!=0);

	p_log.WLog(L"successful.");

}

//////////////////////////////////////////////////////////////////////
// Applies a PPF2.0 patch.
void D2Xppf::ApplyPPF2Patch(int ppf, int bin){
		char desc[50], in;
		unsigned int binlen, obinlen, count, seekpos;
		int idlen, pos;
		unsigned char anz;



		_lseeki64(ppf, 6,SEEK_SET);
		_read(ppf, &desc, 50); desc[50]=0;
		p_log.WLog(L"Patchfile is a PPF2.0 patch. Patch Information:");
		p_log.WLog(L"Description : %s",desc);
		p_log.WLog(L"File_id.diz : ");
		idlen=ShowFileId(ppf, 2);
		if(!idlen) p_log.WLog(L"not available");

		_lseeki64(ppf, 56, SEEK_SET);
		_read(ppf, &obinlen, 4);

        _lseeki64(bin, 0, SEEK_END);
        binlen=_tell(bin);
        if(obinlen!=binlen){
			OutputDebugString("Warning: The size of the bin file isn't correct. continue ... "); //fflush(stdout);
			/*in=getc(stdin);
			if(in!='y'&&in!='Y'){
				p_log.WLog(L"Aborted...");
				return;
			}*/
		}

		//fflush(stdin);
		_lseeki64(ppf, 60, SEEK_SET);
		_read(ppf, &ppfblock, 1024);
		_lseeki64(bin, 0x9320, SEEK_SET);
		_read(bin, &binblock, 1024);
		in=memcmp(ppfblock, binblock, 1024);
		if(in!=0){
			OutputDebugString("Warning: Binblock/Patchvalidation failed. continue ... "); //fflush(stdout);
			/*in=getc(stdin);
			if(in!='y'&&in!='Y'){
				p_log.WLog(L"Aborted...");
				return;
			}*/
		}

		p_log.WLog(L"Start patching PPF ... "); //fflush(stdout);
		_lseeki64(ppf, 0, SEEK_END);
		count=_tell(ppf);
		seekpos=1084;
		count-=1084;
		if(idlen) count-=idlen+38;

        do{
			//p_log.WLog(L"reading..."); fflush(stdout);
			_lseeki64(ppf, seekpos, SEEK_SET);
			_read(ppf, &pos, 4);
			_read(ppf, &anz, 1);
			_read(ppf, &ppfmem, anz);
			_lseeki64(bin, pos, SEEK_SET);
			//p_log.WLog(L"writing..."); fflush(stdout);
			_write(bin, &ppfmem, anz);
			seekpos=seekpos+5+anz;
			count=count-5-anz;
        } while(count!=0);

		p_log.WLog(L"Ok: PPF patching successful.");
}

//////////////////////////////////////////////////////////////////////
// Applies a PPF3.0 patch.
void D2Xppf::ApplyPPF3Patch(int ppf, int bin, char mode){
	unsigned char desc[50], imagetype=0, undo=0, blockcheck=0, in;
	int idlen;
	__int64 offset, count;
	unsigned int seekpos;
	unsigned char anz=0;


	//_lseeki64(ppf, 6,SEEK_SET);  /* Read Desc.line */
	//_read(ppf, &desc, 50); desc[50]=0;
	//p_log.WLog(L"Patchfile is a PPF3.0 patch. Patch Information:");
	//p_log.WLog(L"Description : %s",desc);
	//p_log.WLog(L"File_id.diz : ");

	idlen=ShowFileId(ppf, 3);
	/*if(!idlen) p_log.WLog(L"not available");*/

	_lseeki64(ppf, 56, SEEK_SET);
	_read(ppf, &imagetype, 1);
	_lseeki64(ppf, 57, SEEK_SET);
	_read(ppf, &blockcheck, 1);
	_lseeki64(ppf, 58, SEEK_SET);
	_read(ppf, &undo, 1);

	if(mode==UNDO){
		if(!undo){
			p_log.WLog(L"Error: no undo data available");
			return;
		}
	}

	if(blockcheck){
		fflush(stdin);
		_lseeki64(ppf, 60, SEEK_SET);
		_read(ppf, &ppfblock, 1024);

		if(imagetype){
			_lseeki64(bin, 0x80A0, SEEK_SET);
		} else {
			_lseeki64(bin, 0x9320, SEEK_SET);
		}
		_read(bin, &binblock, 1024);
		in=memcmp(ppfblock, binblock, 1024);
		if(in!=0){
			OutputDebugString("Warning: Binblock/Patchvalidation failed. continue ... "); //fflush(stdout);
			/*in=getc(stdin);
			if(in!='y'&&in!='Y'){
				p_log.WLog(L"Aborted...");
				return;
			}*/
		}
	}

	_lseeki64(ppf, 0, SEEK_END);
	count=_tell(ppf);
	_lseeki64(ppf, 0, SEEK_SET);
	
	if(blockcheck){
		seekpos=1084;
		count-=1084;
	} else {
		seekpos=60;
		count-=60;
	}

	if(idlen) count-=(idlen+18+16+2);
	

	p_log.WLog(L"Start patching PPF ... ");
	_lseeki64(ppf, seekpos, SEEK_SET);
	do{
		//p_log.WLog(L"reading..."); fflush(stdout);
		_read(ppf, &offset, 8);
		_read(ppf, &anz, 1);

		if(mode==APPLY){
			_read(ppf, &ppfmem, anz);
			if(undo) _lseeki64(ppf, anz, SEEK_CUR);
		}

		if(mode==UNDO){
			_lseeki64(ppf, anz, SEEK_CUR);
			_read(ppf, &ppfmem, anz);
		}

		//p_log.WLog(L"writing..."); fflush(stdout);		
		_lseeki64(bin, offset, SEEK_SET);
		_write(bin, &ppfmem, anz);
		count-=(anz+9);
		if(undo) count-=anz;

	} while(count!=0);

	p_log.WLog(L"Ok: patching PPF successful.");

}


//////////////////////////////////////////////////////////////////////
// Shows File_Id.diz of a PPF2.0 / PPF3.0 patch.
// Input: 2 = PPF2.0
// Input: 3 = PPF3.0
// Return 0 = Error/no fileid.
// Return>0 = Length of fileid.
int D2Xppf::ShowFileId(int ppf, int ppfver){
	char buffer2[3073];
	unsigned int idmagic;
	int lenidx=0, idlen=0, orglen=0;


	if(ppfver==2){
		lenidx=4;
	} else {
		lenidx=2;
	}

	_lseeki64(ppf,-(lenidx+4),SEEK_END);
	_read(ppf, &idmagic, 4);
	if(idmagic!='ZID.'){
		return(0);
	} else {
		_lseeki64(ppf,-lenidx,SEEK_END);
		_read(ppf, &idlen, lenidx);
        orglen = idlen;
        if (idlen > 3072) {
			idlen = 3072;
        }
		_lseeki64(ppf,-(lenidx+16+idlen),SEEK_END);
		_read(ppf, &buffer2, idlen);
		buffer2[idlen]=0;
		p_log.WLog(L"available%s",buffer2);
	}

	return(orglen);
}

//////////////////////////////////////////////////////////////////////
// Check what PPF version we have.
// Return: 0 - File is no PPF.
// Return: 1 - File is a PPF1.0
// Return: 2 - File is a PPF2.0
// Return: 3 - File is a PPF3.0
int D2Xppf::PPFVersion(int ppf){
	unsigned int magic;

	_lseeki64(ppf,0,SEEK_SET);
	_read(ppf, &magic, 4);
	switch(magic){
			case '1FPP'		:	return(1);
			case '2FPP'		:	return(2);
			case '3FPP'		:	return(3);
			default			:   p_log.WLog(L"Error: patchfile is no ppf patch"); break;
	}

	return(0);
}


//////////////////////////////////////////////////////////////////////
// Open all needed files.
// Return: 0 - Successful
// Return: 1 - Failed.
int D2Xppf::OpenFiles(char* file1, char* file2){

	bin=_open(file1, _O_BINARY | _O_RDWR);
	if(bin==-1){
		p_log.WLog(L"Error: cannot open file '%s': ",file1); perror("");
		return(1);
	}

	ppf=_open(file2,  _O_RDONLY | _O_BINARY);
	if(ppf==-1){
		p_log.WLog(L"Error: cannot open file '%s': ",file2); perror("");
		_close(bin);
		return(1);
	}

	return(0);
}