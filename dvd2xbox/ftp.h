#ifndef __ftp__
#define __ftp__

#include <io.h>
#include <xbsockaddr.h>

#define DEFAULT_PORT_NUM 21

#define PASSWORD_LENGTH 256

#define EINTR WSAEINTR 

#define bzero(x,y) memset(x,0,y)

#define bcopy(x,y,z) memcpy(y,x,z)

#define close closesocket

#define MSG_PEEK        0x2             /* peek at incoming message */
#define SO_KEEPALIVE    0x0008          /* keep connections alive */


enum {
 LS = 0,
 BINARY,
 ASCII,
 PWD,
 CD,
 OPEN,
 CLOSE,
 QUIT,
 LCD,
 LLS,
 LDIR,
 USER,
 SHELL,
 IGNORE_COMMAND,
 GET,
 PUT,
 HELP,
 RHELP,

 FTP_COMPLETE=1, /* these will be used later */
 FTP_CONTINUE,
 FTP_ERROR
};


class ftp {

public:
	
	ftp();
	~ftp();

	void DoOpen(char *);
	void DoList(char *);
	void DoCD(char *);
	//void DoShellCommand(char *);
	void DoLogin(char *);
	void DoClose(void);
	//void DoLCD( char *);
	void DoPut( char *);
	void DoGet( char *);
	//void DoLLS(char * );
	void DoBinary();
	void DoRhelp( char *);
	void DoAscii();
	void DoPWD();

	int  CheckFds(char *);

	// for dvd2xbox class
	int Connected;     /* flag for connect status */
	int ReadDataMsg( char *szBuffer, int len);
	int GetReply();
	void GetPWD(char* pwd);

private:
	char szBuffer[1025];  /* buffer used to read/write */
	char szUser[20];          /* stores username */
	char szPass[256];         /* stores user password */
	

	
	SOCKET hListenSocket;
	SOCKET hControlSocket;
	SOCKET hDataSocket;	
	XNADDR xna;
	int bSendPort;
	int ReadCommand;
	int bMode;

	
    int GetLine();
	void CleanUp();
	int SendControlMsg(char *, int);
	int SendDataMsg( char *szBuffer, int len);
	int ConnectToServer(char *name, char *port);
	SOCKET GetListenSocket();
	int InitWinsock();
	SOCKET AcceptConnection();
	void CloseControlConnection( void );
	void CloseDataConnection( SOCKET hDataSocket );
	void CloseListenSocket();
	//void GetPassword( char *szPass );
	int GetUnixInput(char *command);
	//int GetWin32Input( char *command);
	void GetFile( char *fname);
	void PutFile( char *fname);
	int ReadControlMsg( char *szBuffer, int len);
	int CheckControlMsg( char *szPtr, int len);
	//int CheckInput();

};

#endif