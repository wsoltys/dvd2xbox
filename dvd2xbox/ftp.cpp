#include "ftp.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <ctype.h>

#include <time.h>
//#include <winsock.h>
//#include <xbsocket.h>
#include <xbsockaddr.h>

#include <direct.h>
//#include <wincon.h>
#include <helper.h>

//#include <signal.h>


#include <conio.h> 



ftp::ftp() {
	Connected = 0;
	hListenSocket = INVALID_SOCKET;
	hControlSocket = INVALID_SOCKET;
	hDataSocket = INVALID_SOCKET;
	bSendPort      = 1;
	ReadCommand = 0;
	bMode=ASCII;

	//InitWinsock();
	
};

ftp::~ftp() {
	
};


/*
 * DoOpen
 * this function is called when the o,open command is issued.
 * it connects to the requested host, and logs the user in
 *
 */
void ftp::DoOpen( char *command)
{
  char *szHost=NULL;  /* remote host */

   /* 
    * do not do anything if we are already connected.
    */
   if( Connected ) {
       DPf_H("Already connected.  Close connection first.\n");
       fflush(stdout);
       return;
   }
   
   /*
    * extract the remote host from command line (if given ).
    * make a copy of the host name with strdup.
    */
   if(!strcmp(command,"open") || !strcmp(command,"o")) {

       DPf_H("Host:"); fgets(szBuffer,1024,stdin);
       (void)strtok(szBuffer,"\n");
       szHost = (char *)strdup(szBuffer);;
   }
   else if( !strncmp(command,"open ",5))
	szHost = strdup(&command[5]);
   else if( !strncmp(command,"o ",2) )
	szHost = strdup(&command[2]);
   else
	szHost = strdup(command);


   DPf_H("Connecting to %s",szHost);
   hControlSocket = ConnectToServer(szHost,"21");

#if (defined(WIN32) || defined(_WIN32) )
   Sleep(1);
#else
   sleep(1);
#endif

   if( hControlSocket != INVALID_SOCKET)  {
     DPf_H("Connected to %s",szHost);
/*#if (defined(WIN32) || defined(_WIN32) )
   
	 sprintf(command,"dftp: Connected to %s ", szHost);

	 //SetConsoleTitle(command);  // set console window title 
#endif*/

     Connected = 1;         /* we ar now connected */
     GetReply();            /* get reply (welcome message) from server */
     //DoLogin((char *)NULL); /* prompt for username and password */
     //DoBinary();            /* default binary mode */
   }
   free(szHost); /* free the strdupped string */
}



/*
 * DoLogin
 * this function logs the user into the remote host.
 * prompts for username and password
 * or parses username and password from command (user <username>:<password>)
 */
void ftp::DoLogin( char *command)
{
 char *User=NULL;
 char *Password=NULL;
 
  if( command && *command)

     User=strdup(&command[5]);

   if( Connected )  {

     /* 
      * ignore leading whitespace
      */
     while(User && (*User == ' ' || *User == '\t') && *User)
	 User++;
	 Password = strchr(User, ':');
	 if (Password) {
		DPf_H("password given\n");
		*Password = '\0';
		Password++;
	 };
     /*
      * if user name was not provided via command line, read it in.
      */
     if(!User || !(*User) ) {
	DPf_H("Login:"); fgets(szUser,20,stdin);
	User = szUser;
	(void)strtok(szUser,"\n");   /* remove '\n' */
     }

     /*
      * send user name & password to server  & get reply message
      */
     sprintf(szBuffer,"USER %s\r\n",User);
     SendControlMsg(szBuffer,strlen(szBuffer));
     GetReply();
	 if (!Password) {
		//GetPassword( szPass );
		Password = szPass;
	 };

     sprintf(szBuffer,"PASS %s\r\n",Password);
     SendControlMsg(szBuffer,strlen(szBuffer));
     GetReply();
   }
   else
      DPf_H("Not Connected.\n");

   free(User); /* free the strdupped string */
}

/*
 * DoClose
 * closes connection to the ftp server
 */
void ftp::DoClose( void )
{
   if( !Connected  ) {
     DPf_H("Not Connected.\n");
    
   }
   else {
	   SendControlMsg("quit\r\n",6);
	   GetReply();
	   CloseControlConnection();
	   hControlSocket = -1;
#if (defined(WIN32) || defined(_WIN32) )
   
	   ////SetConsoleTitle("dftp: Connection closed");
#endif

	   Connected = 0;
   }

}

/*
 * DoList
 * perform directory listing i.e: ls
 */
void ftp::DoList( char *command)
{
   if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
   }

   /*
    * obtain a listening socket
    */
   if( GetListenSocket() < 0) {
       DPf_H("Cannot obtain a listen socket.\n");
       return;
   }
   
   /*
    * parse command
    */
   if( !strcmp(command,"ls") )  {
       sprintf(szBuffer,"NLST\r\n");
   }
   else if( !strcmp(command,"dir") ) 
       sprintf(szBuffer,"LIST\r\n");

   else if( !strncmp(command, "ls ",3)) {
       while( *command == ' ') command++;
       sprintf(szBuffer,"LIST %s\r\n",&command[3]);
   }
   /*
    * send command to server and get response
    */
   DPf_H("Dolist SendCMSG");
   SendControlMsg(szBuffer,strlen(szBuffer));
   memset(szBuffer,0,1024);
   DPf_H("Dolist GetReply");
   GetReply();

   /*
    * accept server's connection
    */
   DPf_H("Dolist AcceptConnection");
   if(AcceptConnection() < 0) {
      DPf_H("Cannot accept connection.\n");
      return;
   }
   CloseListenSocket();       /* close listening socket */

   /*
    * display directory listing.
    */
   /*
   DPf_H("Dolist ReadDataMSG");
   while( ReadDataMsg(szBuffer,1024) > 0) {
       fflush(stdout);
       DPf_H(szBuffer);
       memset(szBuffer,0,1024);
   }
   */
   /*
    * read response
    */
   //DPf_H("Dolist GetReply2");
   //(void)GetReply();

}

/*
 * DoCD
 * chang to another directory on the remote system
 */
void ftp::DoCD( char *command)
{
   char *dir=&command[2];

   if( !Connected ) {
       DPf_H("Not Connected.\n");
       return;
   }

   /*
    * ignore leading whitespace
    */
   while( *dir && (*dir == ' ' || *dir == '\t') ) 
       dir++;

   /*
    * if dir is not specified, read it in
    */
   /*
   if( ! (*dir) ) {
      DPf_H("Remote directory:");
      fgets(szBuffer,1024,stdin);
      (void)strtok(szBuffer,"\n");
      dir = (char *)strdup(szBuffer);
      while( *dir && (*dir) == ' ') 
       dir++;
      if( !(*dir) ) {
	DPf_H("Usage: cd remote-directory\n");
	return;
      }
   }*/
   
   /*
    * send command to server and read response
    */
   sprintf(szBuffer, "CWD %s\r\n",dir);
   SendControlMsg(szBuffer,strlen(szBuffer));
   (void)GetReply();
}

/*
 * DoLCD
 * change directory on the local system
 */
/*
void ftp::DoLCD( char *command)
{

   char *dir = &command[3];

   while(*dir && (*dir == ' ' || *dir == '\t') ) dir++;
*/
   /*
    * if dir is not specified, then print the current dir
    */
/*
   if( ! *dir ) {
      dir = getcwd((char *)NULL,256);
      if( !dir)
	DPf_H("getcwd");
      else
	DPf_H("Current directory is: %s\n",dir);
   }
   else {
      if( chdir(dir) < 0) 
	   if( chdir(dir) < 0) 
	DPf_H("chdir");
      else {
	dir = getcwd((char *)NULL,256);
	if( !dir)
	    DPf_H("getcwd");
	else
	DPf_H("Current directory is: %s\n",dir);
      }
   }
}
*/

/*
 * DoLLS
 * perform local directory listing.  winqvt implements this, but
 * this is not supported by many systems.  it's not really needed
 * since we already have '!' command.  you can just do !ls or !dir.
 */
/*
void ftp::DoLLS( char *command )
{
#if ( !defined(_WIN32) || !defined(WIN32) )
    system("ls");
#else
    system("dir");
#endif
}
*/
/*
 * function to pass commands to the system, ie: dir.
 * this function gets called when '!' is encountered
 */
/*
void DoShellCommand( char *command )
{
  command++;  /* ignore '!' */
/*#if ( !defined(_WIN32) || !defined(WIN32) )

    system(command);
#else
	if( !command || !*command)
		system("cmd");   /* have to fix this for win95 */
//	else                     /* maybe i should use 'start' */
//		system(command); /* programatically determine which */ 
//#endif                           /* system we are on, then make the  */
				 /* appropriate call */
//}


/*
 * DoBinary
 * set file transfer mode to binary
 */
void ftp::DoBinary()
{
  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE I\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   DPf_H("File transfer modes set to binary.\n");
   bMode = BINARY;
}

/*
 * DoAscii
 * set file transfer mode to ascii text
 */
void ftp::DoAscii()
{
  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE A\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   DPf_H("File transfer modes set to ascii.\n");
   bMode = ASCII;

}

/*
 * DoGet
 * retrieve a file from the remote host.  calls GetFile(..)
 */
void ftp::DoGet( char *command)
{

  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
  //(void)strtok(command," ");
  //GetFile(strtok((char *)NULL, " "));
  GetFile(command);
}

/*
 * DoPut
 * send a file to the remote host.  calls PutFile(..)
 */
void ftp::DoPut( char *command )
{
  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
  //(void)strtok(command," ");
  //PutFile(strtok((char *)NULL, " "));
  PutFile(command);
}

/*
 * DoRhelp
 * sends a help command to the server.
 */
void ftp::DoRhelp( char *command )
{
  char *szCommand;

  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
  (void)strtok(command," ");
  szCommand=strtok((char *)NULL, " ");

  if( szCommand && *szCommand )
     sprintf(szBuffer,"HELP %s\r\n",szCommand);
  else 
     sprintf(szBuffer, "HELP\r\n");
  
  SendControlMsg(szBuffer, strlen(szBuffer));
  GetReply();
}

/*
 * retrieves the current directory on the remote host
 */
void ftp::DoPWD()
{

  if( !Connected ) {
      DPf_H("Not Connected.\n");
      return;
  }
  sprintf(szBuffer, "PWD\r\n");
  SendControlMsg(szBuffer,strlen(szBuffer));
  GetReply();
}


void ftp::GetPWD(char* pwd)
{
	int ret;
	int t;

	//memset( pwd, 0, 256 );
	memset( szBuffer, 0, 1024 );
	if( !Connected ) {
		DPf_H("Not Connected.\n");
		return;
	}
	sprintf(szBuffer, "PWD\r\n");
	SendControlMsg(szBuffer,strlen(szBuffer));
	while(!strstr(szBuffer,"257"))
	{
		ret = ReadDataMsg(szBuffer,1024);
		//if(ret <= 0)
		//	break;
		DPf_H("szBuffer: %s",szBuffer);
		//memset( szBuffer, 0, 1024 );
	}
	//GetReply();
	/*
	char *tt;

	tt = strstr( buffer, "257") + 5;
	t = 0;
	while( tt[t] != '"' )
	{
		pwd[t] = tt[t];
		++t;
	}
	pwd[t]= 0;
	DPf_H("ftp: %s",pwd);*/

}





SOCKET ftp::GetListenSocket()

{

    int flag=1,len;
	SOCKET sockfd;

    struct sockaddr_in  serv_addr, TempAddr;

    char *port,*ipaddr;

    char szBuffer[64]={0};



    /*

     * Open a TCP socket (an Internet stream socket).

     */

	//if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd==SOCKET_ERROR)
	{
		DPf_H("socket");

		return INVALID_SOCKET;

    }
	DPf_H("socked opened");


    /*

     * Fill in structure fields for binding

     */

   if( bSendPort ) {

	   DPf_H("build sin");
       bzero((char *) &serv_addr, sizeof(serv_addr));

       serv_addr.sin_family      = AF_INET;

       serv_addr.sin_addr.s_addr = INADDR_ANY;

       serv_addr.sin_port        = htons(0); /* let system choose */

   }

   else {

     /* reuse the control socket then */
	 DPf_H(" getlisten socket setsockopt");
      if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&flag,sizeof(flag)) < 0) {

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	 close(sockfd);

#else

         closesocket(sockfd);

#endif

	 return INVALID_SOCKET;



      }

   }

    /*

     * bind the address to the socket

     */
	DPf_H("bind socket");
    if (bind(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {

	DPf_H("bind");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

	return INVALID_SOCKET;

    }

    

    len = sizeof(serv_addr);

	DPf_H("get socket name");
    if(getsockname(sockfd,(struct sockaddr *)&serv_addr,&len)<0) {

       DPf_H("getsockname");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

       return INVALID_SOCKET;

    }

    len = sizeof(TempAddr);
	DPf_H("get socket name");
    if(getsockname(hControlSocket,(struct sockaddr *)&TempAddr, &len)<0) {

       DPf_H("getsockname");

#if (!defined( _WIN32 ) || !defined( WIN32 ))

	close(sockfd);

#else

        closesocket(sockfd);

#endif

       return INVALID_SOCKET;

    }



    ipaddr = (char *)&TempAddr.sin_addr;

    port  = (char *)&serv_addr.sin_port;

#define  UC(b)  (((int)b)&0xff)

	DWORD dwState;
	do
	{
		dwState = XNetGetTitleXnAddr(&xna);
		Sleep(500);
	} while (dwState==XNET_GET_XNADDR_PENDING);
    sprintf( szBuffer, "PORT %d,%d,%d,%d,%d,%d\r\n", xna.ina.S_un.S_un_b.s_b1, xna.ina.S_un.S_un_b.s_b2, xna.ina.S_un.S_un_b.s_b3, xna.ina.S_un.S_un_b.s_b4, UC(port[0]), UC(port[1]));
	DPf_H("sprintf %s",szBuffer);
	
    /*

     * allow ftp server to connect

     * allow only one server

     */
	DPf_H("listen start");
	while( listen(sockfd,5)==SOCKET_ERROR );
	/*
    if( listen(sockfd, 1) < 0) {

	DPf_H("listen");

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

	close(sockfd);

#else

        closesocket(sockfd);

#endif

	return INVALID_SOCKET;

    }*/
	DPf_H("send control msg");
    SendControlMsg(szBuffer,strlen(szBuffer));
	DPf_H("get reply");
    GetReply();

    hListenSocket = sockfd;
	DPf_H("leave get listen socket");
    return sockfd;

}



SOCKET ftp::AcceptConnection()

{



    struct sockaddr_in cli_addr;

    int clilen = sizeof(cli_addr);

    SOCKET sockfd;


	DPf_H("AcceptConnection accept %d",hListenSocket); 
    sockfd = accept(hListenSocket, (struct sockaddr *)&cli_addr,&clilen);

    if (sockfd == INVALID_SOCKET ) {

        DPf_H("accept");

	return INVALID_SOCKET;

    }
	DPf_H("AcceptConnection accept done"); 

   

    hDataSocket = sockfd;

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

    close(hListenSocket);

#else

    closesocket(hListenSocket);

#endif

	DPf_H("Leave AcceptConnection()"); 
    return sockfd;



}





int ftp::ConnectToServer(char *name, char *port)



{

  //int s;
  SOCKET s;
  unsigned int portnum;



  struct sockaddr_in server;
  

//  struct hostent *hp;



  while( name && *name == ' ') name++;

  if( !name || ! (*name) )

      return INVALID_SOCKET;



  portnum = atoi(port);

  bzero((char *) &server, sizeof(server));



  //if( isdigit(name[0])) {

   server.sin_family      = AF_INET;

   server.sin_addr.s_addr = inet_addr(name);

   server.sin_port        = htons(portnum);

  //}

  //else{ 

	  /*
   if ( (hp = gethostbyname(name)) == NULL)

    {
#if (!defined( _WIN32 ) || !defined( WIN32 ))

      DPf_H("gethostbyname");
#else 
	  DPf_H("gethostbyname: error code: %d\r\n", WSAGetLastError());

#endif
	  return INVALID_SOCKET;

    }

    bcopy(hp->h_addr,(char *) &server.sin_addr,hp->h_length);

    server.sin_family = hp->h_addrtype;

    server.sin_port = htons(portnum);  
*/
   //}/* else */



  /* create socket */
   //WSASetLastError(0);

  if( (s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 1) {
#if (!defined( _WIN32 ) || !defined( WIN32 ))
	  DPf_H("socket");
#else
	  DPf_H("socket: error code: %d\r\n", WSAGetLastError());
#endif

    return INVALID_SOCKET;

  }

  DPf_H("Socket opened");

  int t = 45;
  setsockopt(s,SOL_SOCKET, SO_SNDTIMEO, (const char *)&t,sizeof(int) );
  setsockopt(s,SOL_SOCKET, SO_RCVTIMEO, (const char *)&t,sizeof(int) );
  
  DPf_H("try to connect socket: %s:%d",name,portnum);
  //WSASetLastError(0);
  if (connect(s,(struct sockaddr *)&server, sizeof(server))< 0) {


#if (!defined( _WIN32 ) || !defined( WIN32 ))
	  DPf_H("connect");
#else
	  DPf_H("connect: error code: %d\r\n", WSAGetLastError());
#endif    
	  

    return INVALID_SOCKET;

  }
  DPf_H("Connected to socket");

  
  //setsockopt(s,SOL_SOCKET,SO_LINGER,0,0);

  //setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);

  //setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);
  
  

  hDataSocket = s;

  
  DPf_H("Leave ConnectToServer");
  return s;

}



int ftp::ReadControlMsg( char *szBuffer, int len)

{

  int ret;

   

   if( (ret=recv(hControlSocket,szBuffer,len,0)) <= 0)

       return 0;

   

   return ret;

}



int ftp::SendControlMsg( char *szBuffer, int len)

{

   

   if( send(hControlSocket,szBuffer,len,0) <= 0)

       return 0;

   

   return 1;

}



int ftp::ReadDataMsg( char *szBuffer, int len)

{

   int ret;



   if( (ret=recv(hDataSocket,szBuffer,len,0)) <= 0)

       return 0;

   

   return ret;

}



int ftp::SendDataMsg( char *szBuffer, int len)

{

   

   if( send(hDataSocket,szBuffer,len,0) <= 0)

       return 0;

   

   return 1;

}



void ftp::CloseDataConnection( SOCKET hDataSocket )

{

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

      close(hDataSocket);

#else

      closesocket(hDataSocket);

#endif



      hDataSocket = INVALID_SOCKET;

}



void ftp::CloseControlConnection( void )

{

#if (!defined( _WIN32 ) || !defined( WIN32 )) 

      close(hControlSocket);

#else

      closesocket(hControlSocket);

#endif



      hControlSocket = INVALID_SOCKET;

}


void ftp::CloseListenSocket( void )
{
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
      close(hListenSocket);
#else
      closesocket(hListenSocket);
#endif

      hListenSocket = INVALID_SOCKET;
}


int ftp::CheckControlMsg( char *szPtr, int len)

{

    

    return recv(hControlSocket,szPtr,len,MSG_PEEK);

}







int ftp::GetLine()

{

   int done=0, iRetCode =0, iLen, iBuffLen=0;

   char *szPtr = szBuffer, nCode[3]={0},ch=0;



   while( (iBuffLen < 1024) && 

	  (CheckControlMsg(&ch,1)  > 0) ){

        iLen = ReadControlMsg(&ch,1);

	iBuffLen += iLen;

	*szPtr = ch;

	szPtr += iLen;

	if( ch == '\n' )

	    break;    // we have a line: return

   }

   *(szPtr+1) = (char)0;



   strncpy(nCode, szBuffer, 3);



   return (atoi(nCode));



}



int ftp::GetReply()

{

   int done = 0, iRetCode = 0;

     

   memset(szBuffer,0,1024);

   while(!done ) {

        iRetCode = GetLine();

	(void)strtok(szBuffer,"\r\n");

	puts(szBuffer);


	if( szBuffer[3] != '-' && iRetCode > 0 )

	    done = 1;

	memset(szBuffer,0,1024);

   }

   return iRetCode;

}



/*
int ftp::CheckInput()
{
  int rval, i;
 fd_set readfds, writefds, exceptfds;
 struct timeval timeout;



#if (!defined(WIN32) || !defined(_WIN32) )
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_CLR (fileno(stdin),&readfds);

  timeout.tv_sec = 0 ;                /* 0-second timeout. */
//  timeout.tv_usec = 0 ;               /* 0 microseconds.  */
/*  FD_SET(fileno(stdin),&readfds);

  i=select ( fileno(stdin)+1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */

  /*if (errno == EINTR && i ==-1)  {
 	  return 0;

  }
  return ( FD_ISSET(fileno(stdin),&readfds) );
#else
	   return (kbhit() );
#endif

}*/

int  ftp::CheckFds( char *command)
{
 int rval, i;
 fd_set readfds, writefds, exceptfds;
 struct timeval timeout;
 char *szInput=command;


/*  memset(command,0,1024); */
  memset(szBuffer,0,1024);
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
#if (!defined(WIN32) || !defined(_WIN32) )
  FD_CLR (fileno(stdin),&readfds);
#endif
  if( hControlSocket > 0) 
      FD_CLR (hControlSocket,&readfds);
  timeout.tv_sec = 0 ;                /* 1-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */

#if (!defined( _WIN32 ) || !defined( WIN32 ) )
  FD_SET(fileno(stdin),&readfds);
#endif

  if( hControlSocket > 0) 
      FD_SET(hControlSocket,&readfds);

  i=select ((hControlSocket > 0) ? (hControlSocket+1) : 1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */
if (errno == EINTR && i ==-1)  {
   /*memset(command,0,1024);*/
   return 0;
}


if( (hControlSocket > 0) && FD_ISSET(hControlSocket, &readfds) ) 
{
  if ( ( rval = ReadControlMsg(szBuffer,1024))  > 0)
	  {
	  DPf_H(szBuffer);
	  DPf_H("ftp>");
	  fflush(stdout);
	  return 0;
	  }
  else {
	 DPf_H("\r\nConnection closed by server\r\n");
#if (defined(WIN32) || defined(_WIN32) )

	 //SetConsoleTitle("dftp: Connection closed");
#endif
	 
	 CloseControlConnection();
	 hControlSocket = -1;
	 return 0;
  }

 }

#if (!defined( _WIN32 ) || !defined( WIN32 ) )

   if( FD_ISSET(fileno(stdin),&readfds) )  
       return (ReadCommand = GetUnixInput(command));
   return (ReadCommand = 0);
#else
    
   //return (ReadCommand = GetWin32Input(command));
 return (ReadCommand = 0);

#endif

}


int ftp::InitWinsock()
{

	WSADATA WsaData;

	if( !WSAStartup(0x0101,&WsaData) ) {
		//SetConsoleTitle("dftp: Connection closed");
		return 1;
	}
	else {
	 DPf_H("Winsock cannot be started\r\n");
	 return 0;
	}
	
}

void ftp::CleanUp()
{

	WSACleanup();
}



/*
 * GetPassword
 * read in the user's password.  turn off echoing before reading.
 * then turn echoing back on.
 */
/*
void ftp::GetPassword( char *szPass )
{
     
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
     EchoOff();
     (void)DPf_H("Password:");
     (void)fgets(szPass,PASSWORD_LENGTH,stdin);
     (void)strtok(szPass,"\n");
     EchoOn(stdin,1);
#else
     /*
      * for WIN32 we just use getch() instead of getche()
      */
  /*   int i=0;
     char ch=0;
	 
	 (void)DPf_H("Password:");

     while( (i < PASSWORD_LENGTH) && ( ch != '\r') ) {
	  
	  ch = getch();
	  if( ch != '\r' && ch != '\n')
	     szPass[i++] = ch;
     }
#endif
     (void)DPf_H("\r\n");
}*/

/*
 * GetUnixInput
 * function called to get user input on the UNIX side.  
 */

int ftp::GetUnixInput(char *command)
{
#if (!defined( _WIN32 ) || !defined( WIN32 )) 
	char ch;
	int i;
	
	/*
	 * ignore leading whitespace
	 */
	while( (ch=getchar()) == ' ' || ch == '\t') ;
	
	if( ch != '\n') {
	    command[0] = ch;
	    fgets(&command[1],1024,stdin);
	    strtok(command,"\n");
	    i = strlen(command) - 1;

	    while( i>0 && isspace(command[i]))
	       i--;
	    if( i>= 0)
	       command[i+1] = 0;

	}
	return 1;
#else
       return 0;
#endif
}

/*
 * GetWin32Input
 * called to get input on the WIN32 side.
 */
/*
int ftp::GetWin32Input( char *command)
{
#if (defined( _WIN32 ) || defined( WIN32 )) 
   char ch;
   static int i=0;   
   
   /*
    * i, above, is static because this function can be called
    * many times before '\r' is pressed.  we set i=0 when
    * '\r' is encountered.
    */

/*
   while( kbhit() ) {
      if( (ch=getch()) == 0)
	  getch();          /* ignore */
  /*    else {
	 if( ch == '\r') {
	     command[i] = 0;
	     i = 0;
	     DPf_H("\r\n");
	     i = strlen(command) - 1;

	     /*
	      * ignore trailing whitespace
	      */
	/*     while( i>0 && isspace(command[i]))
		i--;
		 if( i>= 0)
			 command[i+1] = 0;
		 i = 0;

	     return 1;
	 }
	 else if( ch == (char)8 && i > 0) {   /* process backspace */
	  /*   DPf_H("%c %c",8,8);
	     command[i] = 0;
	     i--;
	 }
	 else if( ( ch >= 32) && ( ch <= 126) ) {
	     DPf_H("%c", ch);
	     command[i++] = ch;
	 }
      }
   }
   return 0;
#else
   return 0; 
#endif
}*/


/*
 * GetFile
 * called to retrive a file from remote host
 */
void ftp::GetFile( char *fname)
{
   FILE *fp=NULL;
   int fd, nTotal=0, nBytesRead=0, retval, aborted=0;
   char *abortstr = "ABOR\r\n", ch;

   /*/void (*OldHandler)(int); */

   /*
    * did we get a filename?
    */
   if( !fname || ! (*fname)) {
      DPf_H("No file specified.\n");
      return;
   }

   /*
    * open the file with current mode
    */
   if(! (fp=fopen(fname,(bMode==ASCII) ? "wt" : "wb"))) {
      DPf_H("file open");
      return;
   }

   /*
    * obtain a listen socket
    */
   if( GetListenSocket() < 0) {
       fclose(fp);
       return;
   }
   
   /*
    * send command to server and read response
    */
   sprintf(szBuffer,"RETR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
      fclose(fp);
      return;
   }
   GetReply();
   
   /*
    * accept server connection
    */
   if( AcceptConnection() <= 0) {
       fclose(fp);
       return;
   }
   /* 
    * now get file and store
    */
   

   fd = fileno(fp);

   
   /*/ this did not work :((
    *  so i am taking it out.  well, i am commenting it out
    *  if you figure it out, then use it.  the way i did it
    *  now works well.
   */
/*/   OldHandler = signal(SIGINT,SigHandler);
     retval = setjmp(abortfile);

   if( retval != 0 ) {
      DPf_H("\r\nAborting\r\n");
      fflush(stdout);
      close(fd);
      CloseDataConnection(hDataSocket);
      if( OldHandler)
	  (void)signal(SIGINT,OldHandler);
      ControlCHit = 0;
      GetReply();
      return;
   }
*/

#if (defined(_WIN32) || defined(WIN32))
	
   DPf_H("Press ESC to abort\r\n");
#else
   DPf_H("Type q and hit return to abort\r\n");
#endif
   while( (nBytesRead=ReadDataMsg(szBuffer,1024)) > 0) {
       
	   
	   write(fd,szBuffer,nBytesRead);
	   nTotal+=nBytesRead;
	   DPf_H("%s : %d received\r",fname,nTotal);
	   /*
	   if( CheckInput() ) {
#if (defined(_WIN32) || defined(WIN32))

			if( !(ch = getch()) ) getch();
			if( ch == 27 )
				aborted = 1;
#else
		    ch = getchar();
			if( ch != '\n') {
				while( getchar() != '\n') ;      /* read 'til new line */
		/*	}
			if( ch == 'q') 
				aborted = 1;
#endif
	   }*/
	   
	   /*
	    * did we abort?
	    */
	   if( aborted ) {
		  
		   DPf_H("\r\nAbort: Waiting for server to finish.");
		   SendControlMsg(abortstr,strlen(abortstr));
		   break;
	   }
   }

   if( aborted ) {         // ignore everything if aborted.

	   while( (nBytesRead=ReadDataMsg(szBuffer,1024)) > 0);
	   GetReply();
   }

 /*  (void)signal(SIGINT,OldHandler); */

   DPf_H("\r\n");
   close(fd);
   CloseDataConnection(hDataSocket);
 /*/  ControlCHit = 0; */
   GetReply();

}

/*
 * PutFile
 * called to transfer a file to the remote host using the current
 * file transfer mode.  it's just like GetFile.
 *
 * i have commented out lines that would have helped with trapping
 * ctrl-c because longjmp did not work :((
 * if you figure it out, then uncomment them
 */
 void ftp::PutFile( char *fname)
{
   FILE *fp=NULL;
   int fd, nTotal=0, nBytesRead=0, retval, aborted=0;
   char *abortstr = "ABOR\r\n", ch;

  /* void (*OldHandler)(int); */

   if( !fname || ! (*fname)) {
      DPf_H("No file specified.\n");
      return;
   }
   if(! (fp=fopen(fname,(bMode==ASCII) ? "rt" : "rb"))) {
      DPf_H("file open");
      return;
   }

   if( GetListenSocket() < 0) {
       fclose(fp);
       return;
   }
   
   /*
    * send command to server & read reply
    */
   sprintf(szBuffer,"STOR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
      fclose(fp);
      return;
   }
   GetReply();
   
   /*
    * accept server connection
    */
   if( AcceptConnection() <= 0) {
       fclose(fp);
       return;
   }
   /* 
    * now send file
    */
   
   fd = fileno(fp);
/*
   OldHandler = signal(SIGINT,SigHandler);
   retval = setjmp(abortfile);

   if( retval != 0 ) {
      DPf_H("Aborting\r\n");
      fflush(stdout);
      close(fd);
      CloseDataConnection(hDataSocket);
      if( OldHandler)
	  (void)signal(SIGINT,OldHandler);
      ControlCHit = 0;
      GetReply();
      return;
   }
  */
#if (defined(_WIN32) || defined(WIN32))
	
   DPf_H("Press ESC to abort\r\n");
#else
   DPf_H("Type q and hit return to abort\r\n");
#endif
   while( (nBytesRead=read(fd,szBuffer,1024)) > 0) {
      SendDataMsg(szBuffer,nBytesRead);
      nTotal+=nBytesRead;
      DPf_H("%s : %d sent\r",fname,nTotal);
	  /*
	   if( CheckInput() ) {
#if (defined(_WIN32) || defined(WIN32))

			if( !(ch = getch()) ) getch();
			if( ch == 27 )
				aborted = 1;
#else
			ch = getchar();
			if( ch != '\n') {
				while( getchar() != '\n') ;      /* read 'til new line */
		/*	}
			if( ch == 'q') 
				aborted = 1;
#endif
	   }*/

	   /*
	    * send an abort command to server if we aborted.
	    */
	   if( aborted ) {
		  
		   DPf_H("\r\nAbort: Waiting for server to finish.");
		   SendControlMsg(abortstr,strlen(abortstr));
		   break;
	   }
   }

   /*(void)signal(SIGINT,OldHandler); */

   DPf_H("\r\n");
   /*
    * close data connection
    */
   CloseDataConnection(hDataSocket);
   close(fd);
   GetReply();

}

