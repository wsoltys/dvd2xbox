#include "d2xunlock.h"

// XBOX 1 DVD-Drive unlocker tool v0.1 by The Specialist.

D2Xunlock::D2Xunlock()
{

}

D2Xunlock::~D2Xunlock()
{

}

int D2Xunlock::Unlock()
{

	SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER	SPTDW;
	DWORD		Size, Returned;
	int			i,k,l;
	int			chalpos[11];
	BYTE		restable[261];
	BYTE		hash[44];
	BYTE		shaDigest[55];
	/*CStdString	shastring;*/

	ZeroMemory(&SPTDW,sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);

	fd = CreateFile("cdrom0:", GENERIC_READ, 
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);

	if( fd == INVALID_HANDLE_VALUE )
	{
		return -1;
	}

	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; // = SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28;
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x25;  //The first byte of the packet is always the opcode. $25=read capacity
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x00;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x00;
	SPTDW.Spt.Cdb[9] = 0x00;


	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Read capacity io succesful.\n");

	// Next, continu with sending a 'mode sense' ATAPI command
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);

	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; // = SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28;
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x5a; //opcode for 'mode sense'
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x3e; //page code, this informs the drive what this packet is all about. In this case
	SPTDW.Spt.Cdb[3] = 0x00; // it is $3e, which means that this is a 'authentication related' packet'
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c; //this sets a maximum length to the datablock the drive should return
	SPTDW.Spt.Cdb[9] = 0x00;

	if(DeviceIoControl(  fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode sense io succesful\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending 1st mode sense\n");
		CloseHandle(fd);
		return -1;
	}

	//step 3: read dvd struct
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 12;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; //SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 0x664; // control block is $664 bytes long
	//SPTDW.Spt.DataTransferLength = 1636;
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0xAD; //opcode for 'read dvd structure'
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0xff; //adress on disc specified by these 4 bytes
	SPTDW.Spt.Cdb[3] = 0x02;
	SPTDW.Spt.Cdb[4] = 0xfd;
	SPTDW.Spt.Cdb[5] = 0xff;
	SPTDW.Spt.Cdb[6] = 0xfe; //layer number
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x06; //these 2 bytes set the length to $664
	SPTDW.Spt.Cdb[9] = 0x64;
	SPTDW.Spt.Cdb[10] =0x00;
	SPTDW.Spt.Cdb[11] =0xc0; //control code, is used to specify what kind of command this is. $C0 tells the drive that the xbox wants the control block for the xbox partition

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Read DVD structure IO succesful.\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending read dvd struct\n");
		CloseHandle(fd);
		return -1;
	}
		
	//byte 772 in the returned control block should ALWAYS be 1. Byte 773 contains the number
	//of challenge/responses in the challenge/response table}

	if  ((scsibuffer[772] != 1) || (scsibuffer[773] == 0))
	{
		DebugOut("fatal error, invalid host challenge response table\n");
		CloseHandle(fd);
		return -1;
	}

	DebugOut("Number of entries in challenge responste table = %d\n",scsibuffer[773]);

	//Now, the xbox first calculates a SHA1 'digest' for $2c bytes in the control block, starting at $4a3.
	//It uses this digest to calculate a RC4 key to decrypt the table with

	for(i=0;i<=0x2b;i++)
		hash[i] = scsibuffer[i+0x4a3];

	sha_context_ptr shaHash = 0;
	shaInit(shaHash, 1);
	shaUpdate(shaHash, hash, 44);
	shaFinal(shaHash, shaDigest);

	/*shastring="";
	for(i=0;i<=54;i++)
		shastring=shastring+char(shadigest[i]);*/

	//RC4Init(rc4key,shastring);
	rc4_key RC4KeyStruct;
	prepare_key(shaDigest, 56 / 8, &RC4KeyStruct);
	for(i=0;i<=260;i++)
		restable[i]=scsibuffer[774+i];
	//rc4decrypt (restable, restable, 0xFD); //overwrite unencrypted data with encrypted data. Size = $FD

	/* decrypt HostChallengeResponseTable with RC4 key */
    rc4(restable,sizeof(restable) , &RC4KeyStruct);

	 k=0;
	 for(l=0;l<=23;l++)
	 {
		 if(restable[l*11]==1)
		 {
             chalpos[k]=l;
			 k=k+1;
		 }
	 }
	
	 if(k<2)  //there should be at least 2 challenges with identifier=1 to send.
	 {
		 DebugOut("Fatal Error: Not enough usable challenge/responses found in table !\n");
		 CloseHandle(fd);
		 return -1;
	 }

	 //Now we're going to issue the first challenge to the drive. We're only going to send 2 different
	 //challenges -> the last 2 challenges in the table -> k-2 and k-1.
	 //The xbox might send more, but this is at random. 2 is all you need. It however always sends the last
	 //2 challenges

	 //step 4: mode select
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 0; //0 = SCSI_IOCTL_DATA_OUT
	SPTDW.Spt.DataTransferLength = 28;
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x55;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x00;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;

	scsibuffer[1]= 0x1A;  // length of total packet
	scsibuffer[8]= 0x3E; // the 'page code'
	scsibuffer[9]= 0x12; //page length
	scsibuffer[11]=0x01; //should always be 1.
	scsibuffer[13]=0xD1; //disc category and version
	scsibuffer[14]=0x01; //should always be 1.
	scsibuffer[15]=restable[1+chalpos[k-2]*11]; //challenge ID
	scsibuffer[16]=restable[2+chalpos[k-2]*11]; //actual challenge
	scsibuffer[17]=restable[3+chalpos[k-2]*11];
	scsibuffer[18]=restable[4+chalpos[k-2]*11];
	scsibuffer[19]=restable[5+chalpos[k-2]*11];

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode select IO succesful\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending mode select\n");
		CloseHandle(fd);
		return -1;
	}
    
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);

	// step 5: mode sense
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; //SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x5a;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x3e;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode sense io succesful.\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending mode sense\n");
		CloseHandle(fd);
		return -1;
	}

	//step 6: mode select
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 0; //SCSI_IOCTL_DATA_OUT; !!!
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x55;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x00;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;

	scsibuffer[1]= 0x1A;
	scsibuffer[8]= 0x3E;
	scsibuffer[9]= 0x12;
	scsibuffer[11]=0x01;
	scsibuffer[12]=0x01; //this must now be set to 1
	scsibuffer[13]=0xD1;
	scsibuffer[14]=0x01;
	scsibuffer[15]=restable[1+chalpos[k-1]*11];
	scsibuffer[16]=restable[2+chalpos[k-1]*11];
	scsibuffer[17]=restable[3+chalpos[k-1]*11];
	scsibuffer[18]=restable[4+chalpos[k-1]*11];
	scsibuffer[19]=restable[5+chalpos[k-1]*11];

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode select IO succesful.\n");

	// step 7: mode sense
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; //SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x5a;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x3e;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode sense io succesful.\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending mode sense\n");
		CloseHandle(fd);
		return -1;
	}

	//step 8: mode select -> we should now issue the SAME challenge as before, BUT this
	//time indicate partition '1' by setting byte $10 in the packet to 1.
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 0; //SCSI_IOCTL_DATA_OUT; !!!
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x55;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x00;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;
	scsibuffer[1]= 0x1A;
	scsibuffer[8]= 0x3E;
	scsibuffer[9]= 0x12;
	scsibuffer[10]=0x01; //partition should now be set to 1
	scsibuffer[11]=0x01;
	scsibuffer[12]=0x01;
	scsibuffer[13]=0xD1;
	scsibuffer[14]=0x01;
	scsibuffer[15]=restable[1+chalpos[k-1]*11];
	scsibuffer[16]=restable[2+chalpos[k-1]*11];
	scsibuffer[17]=restable[3+chalpos[k-1]*11];
	scsibuffer[18]=restable[4+chalpos[k-1]*11];
	scsibuffer[19]=restable[5+chalpos[k-1]*11];

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode select IO succesful.\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending mode select\n");
		CloseHandle(fd);
		return -1;
	}

	// step 9: mode sense
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; //SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x5a;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x3e;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x1c;
	SPTDW.Spt.Cdb[9] = 0x00;

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Mode sense io succesful.\n");
	else
	{
		DebugOut("Fatal error, IO failure while sending mode sense\n");
		CloseHandle(fd);
		return -1;
	}

	//And finally send the 'Read capacity' command. This is not necessary for authentication,
	//but I did it as a check to see if everything worked out :) }
	  
	ZeroMemory(&SPTDW, sizeof(SPTDW));
	ZeroMemory (&scsibuffer,2000);
	Size = sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_BUFFER);
	SPTDW.Spt.Length             = sizeof(SCSI_PASS_THROUGH_DIRECT);
	SPTDW.Spt.CdbLength          = 10;
	SPTDW.Spt.SenseInfoLength    = 32;
	SPTDW.Spt.DataIn             = 1; //SCSI_IOCTL_DATA_IN;
	SPTDW.Spt.DataTransferLength = 28; //SizeOf(scsibuffer);
	SPTDW.Spt.TimeOutValue       = 120;
	SPTDW.Spt.DataBuffer         = &scsibuffer;
	SPTDW.Spt.SenseInfoOffset    = 48;
	SPTDW.Spt.Cdb[0] = 0x25;
	SPTDW.Spt.Cdb[1] = 0x00;
	SPTDW.Spt.Cdb[2] = 0x00;
	SPTDW.Spt.Cdb[3] = 0x00;
	SPTDW.Spt.Cdb[4] = 0x00;
	SPTDW.Spt.Cdb[5] = 0x00;
	SPTDW.Spt.Cdb[6] = 0x00;
	SPTDW.Spt.Cdb[7] = 0x00;
	SPTDW.Spt.Cdb[8] = 0x00;
	SPTDW.Spt.Cdb[9] = 0x00;

	if(DeviceIoControl( fd, 0x4D014, &SPTDW, Size, &SPTDW, Size, &Returned, NULL))
		DebugOut("Read capacity io succesful.\n");

	if(scsibuffer[1] > 0)
        DebugOut("Reported capacity bigger than standard xbox video partition. Unlocking seems to be succesful !\n");
	else
	{
		DebugOut("Reported small partition size, unlocking probably unsuccesful\n ");
		CloseHandle(fd);
		return -1;
	}

	
	CloseHandle(fd);
	return 1;
}